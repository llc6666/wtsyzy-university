---
title: Linux 零基础入门：在临时目录完成一次日志整理
status: public
created: 2026-11-19
tags: [Linux, Bash, 命令行, 学习方法]
---

# Linux 零基础入门：在临时目录完成一次日志整理

第一次用 Linux 终端，先完成一个实际任务：**整理一份日志，筛出警告行，保护副本，再打包交接。** 做完应留下原始日志、警告清单和一个能列出内容的压缩包。每一步都先预测会读写哪个文件，再和屏幕结果对照。

准备一台自己的 Linux 电脑、一个 Linux 虚拟机，或已装好 Linux 发行版的 WSL 环境。以下命令面向 Ubuntu 一类装有 Bash、GNU 常用工具和 grep、tar 的环境。在 Bash 终端中逐段执行；普通用户权限即可，不用 `sudo`，也不需要联网下载练习材料。

## 1. 确认在哪台环境、哪个目录

- 发行版负责把 Linux 内核、系统软件和管理工具包装在一起。用 `cat /etc/os-release` 看发行版信息（有该文件时）；`uname -r` 显示当前内核版本，两者不要混记。
- 终端提供输入输出界面，Shell 负责解析命令。`cd` 会改变当前 Shell 的工作目录，`pwd` 显示当前位置，`ls` 列出文件。相对路径从当前位置解释，以 `/` 开头的路径从根目录解释。

开一个空练习目录。用下面这行建立唯一临时目录，并在成功后进入它：

```bash
lab=$(mktemp -d) && cd "$lab" && pwd
```

应看到系统分配的绝对路径，记下它。**没有输出路径或有错误就停在这里**；后面的相对路径都假定你在刚创建的临时目录中。后续代码块用同一终端，换终端时先回到这条路径。不要在现有项目或系统目录里照搬写文件示例。

## 2. 创建原始日志和工作副本

```bash
mkdir logs
printf '%s\n' 'INFO start' 'WARN slow input' 'INFO retry' 'WARN retry limit' > logs/session.log
cat logs/session.log
cp -i logs/session.log 'session copy.log'
mv -i 'session copy.log' session-copy.log
ls -l logs/session.log session-copy.log
```

预期：`cat` 显示四行；最后同时存在 `logs/session.log` 和 `session-copy.log`。`cp` 复制后留下原件，`mv` 在这里给副本改名。带空格的文件名加引号，Shell 才会把它传成一个参数。`-i` 遇到覆盖时先询问；若全新的练习目录却出现提示，先检查当前位置和文件来源。

## 3. 读内容，再用管道和重定向

```bash
head -n 2 logs/session.log
tail -n 2 logs/session.log
grep -n '^WARN ' session-copy.log
grep '^WARN ' session-copy.log | wc -l
grep '^WARN ' session-copy.log > warnings.txt
cat warnings.txt
```

预期：带行号的输出标出第 2、4 行；计数为 2；`warnings.txt` 只含两条警告。`grep` 按行筛选，`^WARN ` 只匹配以 `WARN ` 开头的行；`|` 把左边的标准输出送给右边，`>` 把输出写到文件，已有同名文件会被截断覆盖。需要追加才用 `>>`。别写 `grep ... session-copy.log > session-copy.log`，Shell 打开输出文件时就可能先清空输入，应该写到不同文件。

排查时先单独看 `grep` 的输出，再接 `wc`。日志读错路径或没有匹配行时，仅看到计数为 0 不能说明程序从未报警。

## 4. 给副本加最小权限

```bash
id
ls -l session-copy.log
chmod u=rw,go= session-copy.log
ls -l session-copy.log
```

普通权限位应显示 `-rw-------` 开头：所有者有读写，组和其他用户的普通权限位为空。`id` 指出当前用户和所属组。这里改的是**自己创建的练习文件**；真实环境碰到权限拒绝，应先核对当前身份、文件所有者和上级目录的访问权限。目录的 `x` 表示可进入和按名称访问，和普通文件的执行位用途不同。不要试图用 `chmod 777` 或 `sudo` 消除所有报错；共享系统还可能有 ACL 或其他额外限制。

## 5. 打包前看清要交付的内容

```bash
tar -czf log-report.tar.gz logs/session.log warnings.txt
tar -tzf log-report.tar.gz
```

预期列表恰好包含 `logs/session.log` 与 `warnings.txt`，不包含工作副本或其他文件。第一条创建归档并用 gzip 压缩；第二条只列清单，不解包。要交付真实日志时，先删掉凭据、用户身份和内部地址等敏感内容，再从必要文件打包。练习文件可留到复盘；要清理，用文件管理器，先确认选中的是刚才记下的临时目录。

## 6. 遇到新任务，先知道查哪个入口

| 接下来要做什么 | 先查什么 | 注意边界 |
| --- | --- | --- |
| 找到命令和参数说明 | `command -v 命令`、`help cd`、`man 命令`，或程序支持的 `--help` | Shell 内建和外部程序的帮助入口不同，极简系统不一定带 man 页。 |
| 看为什么磁盘满了 | `df -h .` 看当前目录所在文件系统；`du -sh .` 看当前目录树的估算占用。 | 一个衡量文件系统空间，一个衡量扫描到的目录项，不能直接当成同一个数。 |
| 查为什么命令找不到 | 先查拼写、`command -v` 和当前环境，再看本发行版的包管理文档。 | Ubuntu 的 APT 说明不适用于所有发行版或路由器。先确认系统和工具版本，再决定是否安装。 |
| 向远程机器传文件或看服务日志 | 先明确权限、目标主机和目标路径，再查该平台提供的 SSH 或服务管理文档。 | 初次上手先练本地文件；真实主机可能要求管理员授权，别沿用别人的账号、路径或配置。 |

## 自查到哪里算完成

- 能在终端里说清当前目录，并从另一个目录判断 `logs/session.log` 会指向哪里。
- 能说清 `grep` 的输入、`|` 两端以及 `>` 会改写哪个文件。
- 能从 `ls -l` 区分所有者、组和其他用户的读写执行位。
- 能用归档清单说明准备交付什么；目录和权限有误时，知道先查看哪条证据。

这些步骤只覆盖普通用户的命令行入门。精简设备的 Shell 和工具可能与 Bash/GNU 不同，应重新核对目标环境；不要把本页当作系统管理、远程配置或设备操作手册。

## 原教程与命令参考

- **完整阅读：**颢天（ChrisKim）的[《只学够用的 Linux：零基础最小入门教程》](https://www.zouht.com/4399.html)，原站标注 CC BY-NC-SA 4.0。原教程还讲编辑器、远程操作和系统管理等场景；本页只链接原站，未复制正文、图像或本地转录。
- **查参数：**[GNU Coreutils 手册](https://www.gnu.org/software/coreutils/manual/)（路径、文件和权限）· [GNU Bash 手册](https://www.gnu.org/software/bash/manual/bash.html)（引号、管道和重定向）· [Ubuntu Server 软件包管理](https://ubuntu.com/server/docs/how-to/software/package-management/)（了解 Ubuntu 的工具与范围）。
- **证据状态：**示例文件、命令组合和预计输出是为本页编写的练习。来源未提供运行验证记录；本轮仅整理，未执行测试。
