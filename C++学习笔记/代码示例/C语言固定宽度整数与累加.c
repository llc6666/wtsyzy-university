#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    const uint16_t samples[] = {60000u, 60000u, 60000u, 60000u};
    uint16_t narrow_total = 0u;
    uint32_t wide_total = 0u;

    for (size_t i = 0; i < sizeof samples / sizeof samples[0]; ++i) {
        narrow_total = (uint16_t)(narrow_total + samples[i]);
        wide_total += samples[i];
    }

    printf("16-bit total = %" PRIu16 "\n", narrow_total);
    printf("32-bit total = %" PRIu32 "\n", wide_total);
    return 0;
}
