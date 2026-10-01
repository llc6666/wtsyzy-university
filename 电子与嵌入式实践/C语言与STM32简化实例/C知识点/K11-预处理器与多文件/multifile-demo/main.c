/* 主程序（自建练习）：只通过 sensor.h 使用模块，看不到模块内部的实现。 */

#include <stdio.h>
#include "sensor.h"

int main(void)
{
    if (sensor_init() != SENSOR_OK) {
        printf("初始化失败\n");
        return 1;
    }

    uint16_t value = 0;
    if (sensor_read(&value) == SENSOR_OK) {
        printf("1) 读到数值：%u\n", value);
    }

    sensor_read(&value);
    sensor_read(&value);
    printf("2) 读取次数（来自另一个文件的全局变量）：%u\n", g_sensor_read_count);

    /* 3. 错误用法：传空指针 */
    int r = sensor_read(NULL);
    printf("3) 传空指针的返回值：%d（非 0 表示失败）\n", r);

    /* 4. 直接引用模块内部的 static 标识符会导致链接错误 */
    printf("4) s_last_raw 与 scale_raw 是 sensor.c 的内部实现，这里访问不到\n");
    printf("   试着取消下面一行的注释再编译，会看到 undefined reference：\n");
    /* printf("%u\n", s_last_raw); */

    return 0;
}
