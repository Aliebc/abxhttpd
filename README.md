# abxhttpd

## 项目简介

abxhttpd是一个微型的，采用C++编写的http服务器，可以做到简单的http协议解析、响应功能，可以用来练习C++.

## 代码标准版本

目前采用C++11, string、map等均使用STL

## 支持平台

以下平台已经通过编译和运行测试

· Windows

· MacOS

· Linux

· Android

编译器

GCC/Clang 

## 第三方库

目前无

## 已经实现的功能

1. 解析http请求，将对应目录下的文件返回

2. Windows上的GB2312字符集支持（即可以访问中文路径）

3. 后台运行（Linux/MacOS上采用fork函数实现，Windows上采用ShellExecute函数实现）

## 计划中实现的功能

1. 实现ini类的配置文件

2. CGI功能

   3. 代码模块化

## 计划外功能

1. TLS支持

2. gzip压缩

3. WebSocket

4. FastCGI

## 安装方式

1. 从Release中下载对应平台的二进制文件

2. 编译安装
   
   进入目录后
   
   UNIX
   
   ```shell
   g++ -O2 -std=c++11 -I. -o abxhttpd *.cpp Httpd/*.cpp Parser/*.cpp Extension/*.cpp -lpthread
   ```
   
   Windows
   
   ```shell
   x86_64-w64-mingw32-g++ -O3 -std=c++11 -I. -o abxhttpd *.cpp Httpd/*.cpp Parser/*.cpp Extension/*.cpp -lws2_32
   ```



## 运行方式

   进入命令行运行，输入abxhttpd -h 进入帮助页面.

  