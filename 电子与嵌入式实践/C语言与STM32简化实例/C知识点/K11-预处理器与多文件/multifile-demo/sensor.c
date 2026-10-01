/* 传感器模块的实现（自建练习）
 * 这个文件里的 static 标识符只在文件内可见，不会和别的文件撞名。
 */

#include "sensor.h"

/* 全局变量的定义（头文件里是 extern 声明） */
uint32_t g_sensor_read_count = 0;

/* 模块内部状态：static 表示只在本文件可见 */
static uint16_t s_last_raw = 0;

/* 模块内部函数：static 表示只在本文件可调用 */
static uint16_t scale_raw(uint16_t raw)
{
    return (uint16_t)(raw * 2u);       /* 模拟一个换算 */
}

int sensor_init(void)
{
    s_last_raw = 0;
    g_sensor_read_count = 0;
    return SENSOR_OK;
}

int sensor_read(uint16_t *out_value)
{
    if (out_value == NULL) {
        return SENSOR_ERR;             /* 输出型参数必须判空 */
    }

    uint16_t raw = 1234u;              /* 模拟从硬件采到一个原始值 */
    s_last_raw = raw;
    *out_value = scale_raw(raw);
    g_sensor_read_count++;

    return SENSOR_OK;
}
