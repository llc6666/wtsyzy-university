# 第五册：用 sh 把日志筛选写成可调用的脚本

**目标：**接受一个日志路径，输出以 `WARN ` 开头的行数。空结果应记为零；文件读不到时应留下错误，不能记为零。在本地 Linux 的练习目录写脚本，不需要远程设备、联网或管理员权限。

## 1. 先约定输入与输出

- 恰好一个文件路径，允许路径里有空格。`"$1"` 保留一个参数；要原样转交所有参数时用 `"$@"`。
- 标准输出只写计数；参数错误、读取错误写到标准错误。
- 退出状态 `0` 表示成功，`2` 表示用法或输入错误，`1` 表示筛选失败。`grep` 自身的 `1` 是无匹配，本脚本将它转成成功和数字零。
- 文件保存为 LF 换行。用 `sh count-warnings.sh 文件` 时，由调用的 `sh` 解释；直接运行 `./count-warnings.sh` 还要满足执行权限、解释器路径等条件。点命令 `. ./文件` 会在当前 Shell 中执行，不用它跑这份脚本。

## 2. 写下最小实现

将下列内容保存到 `count-warnings.sh`。判断、变量和输出采用可移植的 `sh` 写法，不依赖 Bash 数组或 `[[ ]]`。

```sh
#!/bin/sh
if [ "$#" -ne 1 ]; then
    printf '%s\n' '用法：sh count-warnings.sh 日志文件' >&2
    exit 2
fi
if [ ! -f "$1" ] || [ ! -r "$1" ]; then
    printf '找不到可读的普通文件：%s\n' "$1" >&2
    exit 2
fi

case "$1" in
    /*) input=$1 ;;
    *)  input=./$1 ;;
esac
if count=$(grep -c '^WARN ' "$input"); then
    printf '%s\n' "$count"
else
    rc=$?
    case "$rc" in
        1) printf '0\n' ;;
        *) printf '筛选失败：%s\n' "$1" >&2; exit 1 ;;
    esac
fi
```

`[` 是命令，和各个参数间留空格；判断数字用 `-ne`、`-eq` 等。`if` 使用命令的退出状态，`$?` 只保存最近一条的状态。用 `< "$1"` 从文件读取，路径不作为 `grep` 的参数解析。后续扩展到多个文件时，可以用 `for file in "$@"; do …; done` 逐个处理；从记录文件逐行读取时用 `while IFS= read -r line; do …; done < 文件`，避免把空格和反斜杠吃掉。

## 3. 读者自查三种结果

仍在自己的练习目录，用编辑器准备 `with warn.log`（含 `INFO start` 与两行分别以 `WARN ` 开头的文字）和 `quiet.log`（只含 `INFO start`）。保存为普通文本，记录：

| 调用参数 | 预期标准输出 | 预期退出状态 |
| --- | --- | --- |
| `'with warn.log'` | `2` | `0` |
| `quiet.log` | `0` | `0` |
| 一个未创建的文件名 | 无；错误写到标准错误 | `2` |

每次调用后紧接着读取 `printf '%s\n' "$?"`，中间不插其它命令。需要留档时把正常计数和标准错误分别重定向到不同文件；把调用接到 `tee` 后再读 `$?`，常读到的是 `tee` 的状态。

脚本语法可用 `sh -n count-warnings.sh` 查看；它不能发现路径或逻辑错误。`sh -x` 会执行脚本并输出展开后的参数，只对练习输入使用，别把真实凭据带入跟踪输出。这里列出的数字是根据练习输入推导的预期，未实际运行。

参考：[POSIX Shell 语言](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html)、[POSIX grep](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/grep.html)和[BusyBox 命令手册](https://busybox.net/BusyBox.html)。回到[快通目录](README.md)。
