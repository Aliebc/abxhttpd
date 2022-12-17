# Abxhttpd编译&运行文档

## 配置要求

这一部分将列出编译&运行所需的最低配置

### 操作系统要求

目前仅支持在Windows，Linux（包含Android）与MacOS三个操作系统上进行编译

Windows：系统版本不低于Windows Vista/Windows Server 2008

Linux（包含Android）：内核版本不低于2.1.23

MacOS：系统版本不低于10.1（即Mac OS X）

### 硬件要求

处理器：基于x86,x64或arm64的处理器

(注意：强烈不推荐在32位模式下运行，可能导致一些已知的问题)

内存：不低于16MB

### 编译器要求

要求完整支持C++11特性的编译器(MSVC 2015及以后、GCC4.8及以后)

要求支持对应头文件的编译器，并支持微软新一代Windows API

### 已经测试的项目

目前经过测试的编译器有

1. 在Windows上的VS 2019和Clang 15
2. 在Linux上的GCC4.8.5、GCC7.1、GCC8.3、GCC9.4、GCC11.2、GCC12.1、Clang14、Clang15
3. 在MacOS上的Clang13、Clang14、GCC11.2

目前已经明确不能编译的编译器有

1. 在Windows上的TDM-GCC所有版本、MinGW所有版本、VS2012及之前

目前已经经过测试的编译组合有

arm64-macos x64-macos arm64-windows x64-windows x86-windows x64-linux

## CMake配置指令

在CMake配置时，根据自定义指令CMake会自动选择编译的文件

| 编译参数         | 指定值 | 意义                                       |
| ---------------- | ------ | ------------------------------------------ |
| ABXHTTPD_SHARED  | 1      | 构建动态链接库，以便于其他衍生程序可以调用 |
| ABXHTTPD_SSL     | 1      | 启用SSL拓展模块（隐含哈希拓展模块）        |
| ABXHTTPD_SQLITE3 | 1      | 启用SQLite3拓展模块                        |

## 在Windows上编译

注意，由于头文件的支持原因，MinGW不能用于在Windows上的编译

### 使用Clang进行编译

CMake配置指令

```powershell
cmake .. -DABXHTTPD_SHARED=1 -DABXHTTPD_SSL=1 -DABXHTTPD_SQLITE3=1 -DCMAKE_CXX_FLAGS="-O3" -G "MinGW Makefiles" -DCMAKE_EXE_LINKER_FLAGS="-lc++" -DCMAKE_SHARED_LINKER_FLAGS="-lc++"
```

编译指令

```powershell
mingw32-make.exe -j
```

安装指令

```powershell
cmake -P .\cmake_install.cmake
```

### 使用MSVC进行编译

CMake配置指令

```powershell
cmake .. -DABXHTTPD_SHARED=1 -DABXHTTPD_SSL=1 -DABXHTTPD_SQLITE3=1 -DCMAKE_CXX_FLAGS="-O3"
```

编译指令

打开VS工程，进行生成即可

## 在Linux/MacOS上编译

### 使用Clang或GCC进行编译

CMake配置指令

```shell
cmake .. -DABXHTTPD_SHARED=1 -DABXHTTPD_SSL=1 -DABXHTTPD_SQLITE3=1 -DCMAKE_CXX_FLAGS="-O3"
```

编译指令

```shell
make -j
```

安装指令

```shell
make install #可能需要sudo make install
```

## 运行测试用例

```shell
#!/bin/sh

#这里是命令行程序的测试用例,代码测试用例在开发文档的第五部分（开发案例）中

#建立端口为9998，目录为.的网站
abxhttpd -p 9998 -D .

#建立端口为9998，目录为.的网站，使用8个线程
abxhttpd -p 9998 -D . -T8

#建立端口为9998，目录为.的网站，绑定IP只为127.0.0.1
abxhttpd -p 9998 -D . -T8 -b 127.0.0.1

#建立端口为9998，目录为.的网站，绑定IP为::（启用IPv6）
abxhttpd -p 9998 -D . -T8 -b 127.0.0.1 -b ::6

#建立端口为9998，目录为.的网站，并显示建站过程，输出等级为5
abxhttpd -p 9998 -D . -v5

#建立端口为9998，目录为.的网站，日志存放到log.txt，错误日志存放到error.txt
abxhttpd -p 9998 -D . -v5 -l log.txt -e error.txt

#建立端口为9998，目录为.的网站，日志存放到log.txt，后台运行，错误日志存放到error.txt
#并启用SSL，SSL的证书是cert.pem，密钥是key.pem
abxhttpd -p 9998 -D . -v5 -l log.txt -e error.txt -d -c SSLHttpdCore -K cert.pem -k key.pem

#这里的网站都是静态页面，如果需要自定义动态页面，参见开发文档的第五部分开发案例和第七部分类设计进行自编译
```
