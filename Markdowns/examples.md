# Abxhttpd开发案例

## 基本代码结构

建议用户直接复制一份DefaultHttpdCore，然后进行修改。

示例文档中, 文件名为example.cpp

示例代码都只给出Handl部分，替换基本结构中的Handl即可

编译如下：

```shell
$ g++ -std=c++11 -O2 example.cpp -o example -lxhttpd
```

运行如下

```shell
$ ./example
```

```cpp
#include <abxhttpd/abxhttpd.H>
#include <iostream>

using namespace abxhttpd;
using namespace std;

void Handl(const HttpResponse & Response,const HttpRequest & Request, void * args)
{
	/*你自己的代码在这里*/
	DefaultHttpH(Response,Request,args); //现在是默认处理方法
}

int main(){
	HttpdSettingList setl={0}; //初始化设置列表
	setl.Http_S.Path="."; //网站根目录设为当前目录
	setl.Socket_S.Port=8889; //设置网站端口
	HttpdCore xcore=DefaultHttpdCore; //复制一份Httpd核心
	xcore.Handler=Handl; //把处理函数换成自己的
	try{
        	Httpd testc(xcore,setl); //初始化一个Httpd服务器
        	testc.start(); //启动
    	}catch (const abxhttpd::BasicException & e){ //捕捉异常
        	cerr << e.what() <<endl;
        	getchar(); //防止立即退出了
	}
}
```

## Hello World! （write方法）

实际情况：访问"/"时，网页显示Hello World!

访问其他，按默认处理(访问根目录下的)

```cpp
void Handl(const HttpResponse & Response,const HttpRequest & Request, void * args)
{
	if(Request.path()=="/"){
		Response.write("Hello World!");
	}else{
		DefaultHttpH(Response,Request,args); //现在是默认处理方法
	}
}
```

## 禁止访问cat! （抛出异常）

需求：访问"/cat"时，网页显示403 Forbidden

访问其他，按默认处理(访问根目录下的)

```cpp
void Handl(const HttpResponse & Response,const HttpRequest & Request, void * args)
{
	if(Request.path()=="/cat"){
		throw HttpException(403); //抛出403异常！
	}else{
		DefaultHttpH(Response,Request,args); //现在是默认处理方法
	}
}
```

## 实现一个统计人数的计数器（Session使用）

需求：用户访问"/count"时，展示他访问的次数和访问的总人数

访问其他，按默认处理(访问根目录下的)

```cpp
void Handl(const HttpResponse & Response,const HttpRequest & Request, void * args)
{
	if(Request.path()=="count"){
	if(Request.session("count")->null()){
            Request.session("count")->wrap(new int(0)); //用户第一次访问，记录为0
        }
        if(Request.global("count")->null()){
            Request.global("count")->wrap(new int(0)); //初始化全局变量
        }
        ++Request.sessionA<int>("count"); //用户访问次数+1
        ++Request.globalA<int>("count"); //全部用户访问次数+1
        Response.write("Times you explore this page: ");
        Response.writeA(Request.sessionA<int>("count")); //输出用户访问的次数
        Response.write("<br>Times all people explore this page: ");
        Response.writeA(Request.globalA<int>("count")); //输出所有用户访问的次数
	}else{
		DefaultHttpH(Response,Request,args); //现在是默认处理方法
	}
}
```

## 网页重定向（location方法）

需求：用户访问"/h1"页面,跳转到"/h2"页面

访问其他，按默认处理(访问根目录下的)

```cpp
void Handl(const HttpResponse & Response,const HttpRequest & Request, void * args)
{
	if(Request.path()=="/h1"){
		Response.location("/h2"); //重定向！
	}else{
		DefaultHttpH(Response,Request,args); //现在是默认处理方法
	}
}
```

## request方法处理GET请求(高级)

需求: 用户访问"/myname?name=xxx"时输出"Your username is xxx"

访问其他，按默认处理(访问根目录下的)

```cpp
void Handl(const HttpResponse & Response,const HttpRequest & Request, void * args)
{
	if(Request.path()=="/myname"){
		Response.write("Your username is ");
		Response.write(Request.request("name")); //输出用户的名字
	}else{
		DefaultHttpH(Response,Request,args); //现在是默认处理方法
	}
}

```
