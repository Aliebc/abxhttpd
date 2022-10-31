# Abxhttpd

## 项目简介

abxhttpd是一个采用C++编写的轻量级http服务器，可以做到简单的http协议解析、响应功能，可以用来练习C++或http协议.

## 代码标准版本

C++11

## 支持平台

以下平台已经通过编译和运行测试

· Windows

· MacOS

· Linux(Ubuntu/CentOS/Android)

支持的编译器

GCC/Clang/MSVC

## 第三方库

目前无，核心部分只使用了C++标准库

## 从源码编译

1. 首先进入目录，然后新建一个build目录，然后生成cmake文件

```shell
#进入源码目录
mkdir build
cd build
cmake ..
```

2. 按照生成的文件进行构建
在Windows上，默认生成VS的项目，双击打开项目，进行生成即可。
(如果在Windows上使用MinGW构建,需要在cmake命令后添加 -G "MinGW Makefile")
在MacOS或Linux上，默认生成Unix Makefile, 此时直接键入make命令即可构建

3. 安装项目
   在Windows上，输入cmake -P "./cmake_install.cmake"进行安装;在MacOS或Linux上输入make install安装（可能需要sudo make install）
```shell
# On MacOS or Linux
make
make install
```

4. 卸载项目
   在Windows上，输入cmake -P "./cmake_uninstall.cmake"进行卸载;在MacOS或Linux上输入make uninstall卸载（可能需要sudo make uninstall）
如果需要

5. 构建模块
   Abxhttpd支持构建时加入其他项目，如
   -DABXHTTPD_SHARED=1 构建动态库
   -DABXHTTPD_STATAIC=1 构建静态库
   -DABXHTTPD_SSL=1 构建SSL模块

如果需要对应的选项，在cmake配置时输入，如
```shell
cmake .. -DABXHTTPD_SHARED=1 -DABXHTTPD_SSL=1
```

## 运行方式
   把可执行文件正确加入环境路径后，命令行中输入abxhttpd -V检查是否正确安装
```shell
#如果显示类似下方则代表已经正确安装
$ abxhttpd -V
Abxhttpd version: abxhttpd/2.0.3 (MacOS)
Built by Apple LLVM 14.0.0 (clang-1400.0.29.102)
Build Date: Oct 31 2022 21:00:14
Copyright (C) 2022 The Abxhttpd Group.
```

## 运行示例

最简单的运行示例
```shell
abxhttpd -p 9999 -D /var/www/html
```
即以根目录为/var/www/html在本地的9999端口建立站点
```shell
abxhttpd -p 9999 -D /var/www/html -l a.log -e e.log -d
```
即以根目录为/var/www/html在本地的9999端口建立站点，并把日志输入a.log文件,错误日志输入e.log文件,同时挂为后台程序.
输入abxhttpd -h 进入帮助页面或参阅文档以查看更详细的信息.