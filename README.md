# 项目介绍

本项目基于xmake+openocd开发GD32F427单片机

环境：Windows10

# Xmake

windows版本xmake下载

```
https://github.com/xmake-io/xmake/releases
```

详细使用方式见

```
https://xmake.io/#/
```

# 交叉编译工具下载

交叉编译工具**arm-none-eabi-gcc**访问下列网站了解详情

```
https://developer.arm.com/downloads/-/gnu-rm
```

将bin文件夹的路径添加到环境变量中

# 项目文件介绍

- App：包含测试文件，项目源码放这里
- Doc：相关文档
- Libraries：官方库
- User：包含各类bsp，主函数放这里
- GD32F4xx.svd
- link.ld
- xmake.lua

文件详解请打开对应文件，观看注释

# CopyRight

bsp写法参考安富莱

Copyright (C),  www.armfly.com

xmake.lua link.ld GD32F4xx.svd修改于ART-PI DIY开源群群友（QQ1097282916）工程