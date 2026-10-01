/* 传感器模块的头文件（自建练习）
 * 头文件只放：对外声明、类型、宏。不放函数体，不放变量定义。
 */

#ifndef SENSOR_H          /* include guard：防止被重复包含 */
#define SENSOR_H

#include <stdint.h>

#define SENSOR_OK   0
#define SENSOR_ERR -1

/* 对外可见的函数声明 */
int sensor_init(void);
int sensor_read(uint16_t *out_value);

/* 对外可见的全局变量：这里只是「声明」，定义在 sensor.c */
extern uint32_t g_sensor_read_count;

#endif
