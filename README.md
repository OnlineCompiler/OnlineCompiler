# Online Compiler
## 项目简介
### 在线编译器
- 基于 AJAX 技术实现异步提交表单和无刷新更新页面
- 解析 JSON 数据并完成编译相关任务
### Web 服务器
 - 基于 libco 实现高并发 web 服务器
 - 支持 GET/POST 请求和 CGI 运行模式 
## 目录结构
	.
	├── bin
	│   ├── http_server
	│   ├── lib
	│   │   └── libcolib.a
	│   └── solib
	│       └── libcolib.so
	├── build.sh
	├── cgi
	│   ├── cJSON.c
	│   ├── cJSON.h
	│   ├── compiler.c
	│   └── Makefile
	├── conf
	│   └── httpd.conf
	├── http_ctl.sh
	├── log
	│   └── httpd.log
	├── README.md
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
	│   └── Makefile
	├── tmp
	│   ├── 5997124D13F96
	│   └── 5997124D13F96.cpp
	└── wwwroot
	    ├── cgi-bin
	    │   └── compiler
	    ├── compiler.css
	    ├── compiler.js
	    ├── index.html
	    └── jquery.min.js
## 性能测试
[服务器性能测试](https://github.com/OnlineCompiler/OnlineCompiler/wiki/Web%E6%9C%8D%E5%8A%A1%E5%99%A8%E6%80%A7%E8%83%BD%E6%B5%8B%E8%AF%95%E5%AF%B9%E6%AF%94)
## 编译
> bash build.sh

## 运行
> bash http_ctl.sh -s | -t | -r
