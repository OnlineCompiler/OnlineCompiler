---
title: Online Compiler 
tags: httpd,libco,cgi,ajax,json
---
----------
## 项目简介
### 在线编译器
- 基于 AJAX 技术实现异步提交表单和无刷新更新页面
- 解析 JSON 数据并完成编译相关任务
### web 服务器
 - 基于 libco 实现高并发 web 服务器
 - 支持 GET/POST 请求和 CGI 运行模式 
----------
## 目录结构
	.
	├── bin
	│   ├── http_server
	│   ├── lib
	│   │   └── libcolib.a
	│   └── solib
	│       └── libcolib.so
	├── build.sh
	├── conf
	│   └── httpd.conf
	├── http_ctl.sh
	├── log
	│   └── httpd.log
	├── src
	│   ├── coctx.cpp
	│   ├── coctx.h
	│   ├── coctx_swap.S
	│   ├── co_hook_sys_call.cpp
	│   ├── co.mk
	│   ├── co_routine.cpp
	│   ├── co_routine.h
	│   ├── co_routine_inner.h
	│   ├── httpd.cpp
	│   ├── httpd.h
	│   ├── http_server.cpp
	│   ├── Makefile
	│   └── tags
	└── wwwroot
		└── index.html

## 编译

``` shell
bash build.sh
```
## 运行

``` shell
bash http_ctl.sh -s | -t | -r
```
