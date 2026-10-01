/* S04 OLED 显示与显存缓冲：字模、页列寻址与位运算
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic oled-gram-demo.c -o oled-gram-demo.exe
 *
 * 目的：把「字模怎么造」「像素怎么进显存」「为什么改一个像素也要整页重发」跑通。
 * 模拟方式：用 OLED_GRAM[页][列] 二维数组当显存，屏幕缩成 32x16（机制与 128x64 相同），
 *           刷新时统计发送的命令数与数据字节数，代替真的走 I2C。
 * 原型：本地 OLED 工程原型（SSD1306，128x64，软件 I2C，PB8=SCL / PB9=SDA）。
 */

#include <stdio.h>
#include <stdint.h>

#define OLED_WIDTH   32                 /* 缩小版屏幕：32 列 */
#define OLED_HEIGHT  16                 /* 16 行 = 2 页 */
#define PAGE_COUNT   (OLED_HEIGHT / 8)

/* ---- 显存缓冲：真实驱动里是 static uint8_t OLED_GRAM[8][128] ---- */
static uint8_t oled_gram[PAGE_COUNT][OLED_WIDTH];

/* ---- 模拟 I2C 总线上的流量（刷新策略的代价看这里） ---- */
static int sent_commands;               /* 控制命令字节数 */
static int sent_bytes;                  /* 显存数据字节数 */

/* 像素三色：与 OLED 示例工程 的 OLED_SetPixelInternal 同一套位运算 */
enum { COLOR_BLACK = 0, COLOR_WHITE = 1, COLOR_XOR = 2 };

static void oled_set_pixel(int16_t x, int16_t y, uint8_t color)
{
    uint8_t mask;

    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT) {
        return;                         /* 越界像素直接丢弃，不报错 */
    }

    mask = (uint8_t)(1u << (y & 7));    /* 页内行号 = y 除以 8 的余数 */
    if (color == COLOR_WHITE) {
        oled_gram[y >> 3][x] |= mask;   /* 点亮：只置这位 */
    } else if (color == COLOR_XOR) {
        oled_gram[y >> 3][x] ^= mask;   /* 翻转：用于闪烁 */
    } else {
        oled_gram[y >> 3][x] &= (uint8_t)~mask; /* 熄灭：只清这位 */
    }
}

/* ---- 造字模：人按行想图形（行位图），SSD1306 按列收字节（列字模） ----
 * rowmap[8]：每行一个字节，最高位是最左列（所见即所得，好画好检查）
 * columns[8]：每列一个字节，最低位是这一列最上面的点（控制器要的格式）
 * 转换本身就是一次双重循环的位搬运。
 */
static void bitmap_to_columns(const uint8_t rowmap[8], uint8_t columns[8])
{
    int row, col;

    for (col = 0; col < 8; col++) {
        columns[col] = 0;
    }
    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            if (rowmap[row] & (uint8_t)(0x80u >> col)) {
                columns[col] |= (uint8_t)(1u << row);
            }
        }
    }
}

/* 把 8x8 位图画到 (x, y)，y 不需要 8 对齐（y=3 这种跨页位置也正确） */
static void draw_bitmap(int16_t x, int16_t y, const uint8_t rowmap[8])
{
    uint8_t columns[8];
    int col, row;

    bitmap_to_columns(rowmap, columns);
    for (col = 0; col < 8; col++) {
        for (row = 0; row < 8; row++) {
            if (columns[col] & (uint8_t)(1u << row)) {
                oled_set_pixel((int16_t)(x + col), (int16_t)(y + row), COLOR_WHITE);
            }
        }
    }
}

/* 整帧刷新：每页先发 3 条定位命令，再把整页 32 字节连发 */
static void oled_update(void)
{
    int page;

    for (page = 0; page < PAGE_COUNT; page++) {
        sent_commands += 3;            /* 0xB0|页 、0x10|列高4位、列低4位 */
        sent_bytes += OLED_WIDTH;      /* 整页数据一口气发完 */
    }
}

static void oled_clear_buffer(void)
{
    int page, col;

    for (page = 0; page < PAGE_COUNT; page++) {
        for (col = 0; col < OLED_WIDTH; col++) {
            oled_gram[page][col] = 0x00;
        }
    }
}

/* 把显存打印成 ASCII：亮像素打 #，暗像素打 .，正好是屏幕本身 */
static void oled_render_ascii(const char *title)
{
    int col, bit;

    printf("%s\n", title);
    for (bit = 0; bit < 8; bit++) {                 /* 上半屏：页 0 */
        printf("  ");
        for (col = 0; col < OLED_WIDTH; col++) {
            printf("%c", (oled_gram[0][col] & (1u << bit)) ? '#' : '.');
        }
        printf("\n");
    }
    for (bit = 0; bit < 8; bit++) {                 /* 下半屏：页 1 */
        printf("  ");
        for (col = 0; col < OLED_WIDTH; col++) {
            printf("%c", (oled_gram[1][col] & (1u << bit)) ? '#' : '.');
        }
        printf("\n");
    }
}

/* 对图形本身做 XOR：翻一次图形消失，翻两次回来——闪烁就是这么做的 */
static void xor_bitmap(int16_t x, int16_t y, const uint8_t rowmap[8])
{
    uint8_t columns[8];
    int col, row;

    bitmap_to_columns(rowmap, columns);
    for (col = 0; col < 8; col++) {
        for (row = 0; row < 8; row++) {
            if (columns[col] & (uint8_t)(1u << row)) {
                oled_set_pixel((int16_t)(x + col), (int16_t)(y + row), COLOR_XOR);
            }
        }
    }
}

int main(void)
{
    /* 8x8 爱心，按行画：0=灭 1=亮，直接对着形状读 */
    static const uint8_t heart_rows[8] = {
        0x66, /* 01100110  ..##..##.. */
        0xFF, /* 11111111  ######## */
        0xFF, /* 11111111  ######## */
        0xFF, /* 11111111  ######## */
        0x7E, /* 01111110  .######. */
        0x3C, /* 00111100  ..####.. */
        0x18, /* 00011000  ...##... */
        0x00, /* 00000000  ........ */
    };
    /* 8x8 字母 H：两竖一横 */
    static const uint8_t letter_h_rows[8] = {
        0x42, /* 01000010  .#....#. */
        0x42, 0x42,
        0x7E, /* 01111110  .######. */
        0x7E,
        0x42, 0x42, 0x42,
    };
    int i;

    printf("== 1. 画一个爱心（x=2, y=1，故意跨页）和字母 H（x=20, y=8）==\n");
    draw_bitmap(2, 1, heart_rows);
    draw_bitmap(20, 8, letter_h_rows);
    oled_update();
    oled_render_ascii("刷新后屏幕：");

    printf("\n[统计] 命令 %d 字节，显存数据 %d 字节（2 页 x 32 列）\n",
           sent_commands, sent_bytes);

    printf("\n== 2. 越界像素被丢弃 ==\n");
    printf("往 (-1, 5)、(32, 0)、(10, 16) 画点：\n");
    oled_set_pixel(-1, 5, COLOR_WHITE);
    oled_set_pixel(32, 0, COLOR_WHITE);
    oled_set_pixel(10, 16, COLOR_WHITE);
    printf("显存未变（左上角/右上角/底行外均无点），程序不崩溃。\n");

    printf("\n== 3. XOR 翻转 = 闪烁的基础，翻两次回到原样 ==\n");
    xor_bitmap(2, 1, heart_rows);      /* 只对图形本身的像素取反 */
    oled_render_ascii("翻转一次（爱心消失，只剩 H）：");
    xor_bitmap(2, 1, heart_rows);
    oled_render_ascii("再翻一次（爱心回来）：");

    printf("\n== 4. 改一个像素也要整页重发 ==\n");
    sent_commands = 0;
    sent_bytes = 0;
    oled_set_pixel(15, 3, COLOR_WHITE); /* 只点一个点 */
    oled_update();
    printf("只改 1 个像素，仍发送：命令 %d 字节，数据 %d 字节\n",
           sent_commands, sent_bytes);
    printf("-> 所以动画的标准做法：先在显存里把整帧画完，再 update 一次。\n");

    printf("\n== 5. 清屏重画后，单独擦一个 8x8 的左半 4 列（&= ~mask）==\n");
    oled_clear_buffer();
    draw_bitmap(2, 1, heart_rows);
    draw_bitmap(20, 8, letter_h_rows);
    for (i = 0; i < 4; i++) {          /* 只擦 x=2..5 这 4 列，右半留着 */
        int row;
        for (row = 1; row < 9; row++) {
            oled_set_pixel((int16_t)(2 + i), (int16_t)row, COLOR_BLACK);
        }
    }
    oled_render_ascii("擦掉爱心左半后的屏幕：");

    return 0;
}
