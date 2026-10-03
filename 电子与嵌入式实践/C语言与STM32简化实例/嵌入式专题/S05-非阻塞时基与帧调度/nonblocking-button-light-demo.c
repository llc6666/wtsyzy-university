/* Independent C17 PC simulation, not an STM32 driver or a hardware test.
 * Button: sampled every 5 ms, observed stable window 20 ms.
 * Light input: sampled every 100 ms; missed samples are not replayed.
 * raw_pressed and raw_dark are normalized logical states, not GPIO levels.
 */
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks;
#define CHECK(condition) do { \
    ++checks; \
    if (!(condition)) { \
        fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

typedef struct {
    uint32_t last;
    uint32_t period;
} periodic_t;

/* Contract: period > 0; elapsed real ticks since last < 2^32.
 * Run at most once per check, preserve phase, skip stale samples.
 */
static bool periodic_due(periodic_t *task, uint32_t now, uint32_t *missed)
{
    const uint32_t elapsed = (uint32_t)(now - task->last);
    *missed = 0;
    if (task->period == 0 || elapsed < task->period) {
        return false;
    }
    const uint32_t periods = elapsed / task->period;
    task->last += periods * task->period;
    *missed = periods - 1;
    return true;
}

typedef struct {
    bool candidate;
    bool stable;
    uint32_t candidate_since;
} debounce_t;

/* Returns +1 for press, -1 for release, 0 for no confirmed edge.
 * Same sampled level for 20 ms does not prove the signal never changed
 * between samples. That boundary also applies to physical debouncing.
 */
static int debounce_feed(debounce_t *button, bool raw, uint32_t now)
{
    if (raw != button->candidate) {
        button->candidate = raw;
        button->candidate_since = now;
    } else if (button->candidate != button->stable &&
               (uint32_t)(now - button->candidate_since) >= UINT32_C(20)) {
        button->stable = button->candidate;
        return button->stable ? 1 : -1;
    }
    return 0;
}

typedef struct {
    periodic_t button_task;
    periodic_t light_task;
    debounce_t button;
    bool led;
    bool buzzer;
    uint32_t presses;
    uint32_t button_samples;
    uint32_t light_samples;
    uint32_t missed_button;
    uint32_t missed_light;
} app_t;

/* Startup contract for this demonstration: button known released.
 * A real board must define its startup-held-button policy explicitly.
 */
static app_t app_init(uint32_t now)
{
    app_t app = {0};
    app.button_task = (periodic_t){now, UINT32_C(5)};
    app.light_task = (periodic_t){now, UINT32_C(100)};
    app.button.candidate_since = now;
    return app;
}

static void app_step(app_t *app, uint32_t now, bool raw_pressed, bool raw_dark)
{
    uint32_t missed;
    if (periodic_due(&app->button_task, now, &missed)) {
        ++app->button_samples;
        app->missed_button += missed;
        if (missed != 0) {
            /* Missing observations cannot establish a stable window. */
            app->button.candidate = raw_pressed;
            app->button.candidate_since = now;
        } else if (debounce_feed(&app->button, raw_pressed, now) == 1) {
            app->led = !app->led;
            ++app->presses;
        }
    }
    if (periodic_due(&app->light_task, now, &missed)) {
        ++app->light_samples;
        app->missed_light += missed;
        app->buzzer = raw_dark;
    }
}

static bool bouncing_button(uint32_t t)
{
    if (t < 10) return false;
    if (t < 30) return ((t / 5) % 2) == 0;
    if (t < 150) return true;
    if (t < 170) return ((t / 5) % 2) != 0;
    if (t < 230) return false;
    if (t < 240) return ((t / 5) % 2) == 0;
    return t < 350;
}

static void test_waveform(uint32_t start)
{
    app_t app = app_init(start);
    for (uint32_t t = 0; t <= 400; ++t) {
        app_step(&app, start + t, bouncing_button(t), t >= 55 && t < 175);
        CHECK(app.presses == (t < 50 ? 0U : (t < 260 ? 1U : 2U)));
        CHECK(app.led == (t >= 50 && t < 260));
        CHECK(app.buzzer == (t >= 100 && t < 200));
    }
    CHECK(app.button_samples == 80);
    CHECK(app.light_samples == 4);
    CHECK(app.missed_button == 0 && app.missed_light == 0);
    CHECK(!app.button.stable);
}

/* Independent reference: five identical observations spaced 5 ms apart
 * establish the 20-ms observed window. Enumerate all 12-observation inputs.
 */
static void test_debounce_exhaustive(void)
{
    for (unsigned mask = 0; mask < 4096; ++mask) {
        debounce_t button = {false, false, 0};
        bool previous = false;
        bool reference_stable = false;
        unsigned run = 0;
        for (unsigned i = 0; i < 12; ++i) {
            const bool raw = ((mask >> i) & 1U) != 0;
            run = raw == previous ? run + 1 : 1;
            previous = raw;
            int reference_edge = 0;
            if (run >= 5 && raw != reference_stable) {
                reference_stable = raw;
                reference_edge = raw ? 1 : -1;
            }
            CHECK(debounce_feed(&button, raw, (i + 1) * UINT32_C(5)) == reference_edge);
            CHECK(button.stable == reference_stable);
        }
    }
}

static void test_period_and_wrap(void)
{
    uint32_t missed;
    periodic_t task = {0, 100};
    CHECK(!periodic_due(&task, 99, &missed));
    CHECK(periodic_due(&task, 100, &missed) && missed == 0);
    CHECK(periodic_due(&task, 450, &missed) && missed == 2 && task.last == 400);
    CHECK(!periodic_due(&task, 450, &missed));
    CHECK(periodic_due(&task, 500, &missed) && missed == 0);
    task = (periodic_t){UINT32_C(0xfffffff0), 20};
    CHECK(!periodic_due(&task, 3, &missed));
    CHECK(periodic_due(&task, 4, &missed) && missed == 0 && task.last == 4);
    task = (periodic_t){0, 0};
    CHECK(!periodic_due(&task, 100, &missed));
}

static void test_overload_and_short_pulse(void)
{
    app_t app = app_init(0);
    app_step(&app, 450, true, true);
    CHECK(app.presses == 0 && app.missed_button == 89);
    CHECK(app.buzzer && app.light_samples == 1 && app.missed_light == 3);
    app_step(&app, 475, true, true);
    CHECK(app.presses == 0); /* Another missing window resets observation. */
    for (uint32_t t = 480; t <= 495; t += 5) {
        app_step(&app, t, true, true);
    }
    CHECK(app.presses == 1);
    app = app_init(0);
    for (uint32_t t = 0; t <= 100; ++t) {
        app_step(&app, t, t >= 11 && t <= 14, false);
    }
    CHECK(app.presses == 0); /* An unsampled 4-ms pulse is deliberately missed. */
}

int main(void)
{
    test_period_and_wrap();
    test_waveform(0);
    for (uint32_t offset = 0; offset < 1024; ++offset) {
        test_waveform(UINT32_MAX - offset);
    }
    puts("PASS waveform starts=1025 press_events=2050");
    test_debounce_exhaustive();
    puts("PASS debounce sequences=4096 observations=49152");
    test_overload_and_short_pulse();
    puts("PASS wrap, phase, skipped samples, overload, short-pulse boundary");
    printf("PASS checks=%u\n", checks);
    return EXIT_SUCCESS;
}
