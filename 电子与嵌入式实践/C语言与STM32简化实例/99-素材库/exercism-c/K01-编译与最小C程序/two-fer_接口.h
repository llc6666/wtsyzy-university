/* two-fer 练习的官方接口（注释已译为中文，标识符保留原文）
 * 来源：exercism/c 仓库 exercises/practice/two-fer/two_fer.h（MIT 许可）
 */

#ifndef TWO_FER_H
#define TWO_FER_H

/* buffer：调用方提供的缓冲区，结果写进这里
 * name：对方的名字；不知道名字时传 NULL */
void two_fer(char *buffer, const char *name);

#endif
