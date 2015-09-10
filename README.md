# Goo-OS [![Build Status](https://travis-ci.org/xuzhenglun/Goo-OS.svg?branch=master)](https://travis-ci.org/xuzhenglun/Goo-OS)

Goo-OS是一个玩具级的系统，是阅读《30天自制操作系统》的读书笔记。

---
 1. 与书本不同的是，尽可能的使用了GNU的工具链。熟悉了GCC内联汇编，objdump,objcopy 等工具的配置参数。侧面了解了编译过程中的步骤和原理，计算机系统运行原理和步骤。
 2. 采用GDB联系QEMU追踪调试，熟悉了GDB的使用和操作。
 3. 熟悉了Git、Travis-ci等开源工具和网站的使用。

---

因为使用了GNU工具链，应用程序的格式为RAW Binary，无法从中找出数据段覆盖到内存正确的位置，直接解析ELF头工作量感觉偏大，暂无好的解决方法。

