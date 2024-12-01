# MdrOS 应用程序开发环境

该环境编译出的应用程序仅适用于`CP_Kernel`内核系列的操作系统.

<hr>

* 注意! 该环境只能在Linux系统下使用, 并要求您具备zig cc编译器(或者clang)和nasm汇编器

* `src` - 源码文件夹
* `lib` - 适用于MdrOS的标准库实现, 尽量不要更改该文件夹内的代码
* `xmake.lua` - 构建脚本

> `xmake build` 一键构建命令 \
> 根目录会生成 `app.elf` 该文件即应用程序实体, 将其移动至mdros光盘映像内即可 \
> 然后启动MdrOS, 在终端中输入 `/app.elf` 即可查看运行结果
