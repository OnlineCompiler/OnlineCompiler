#!/bin/bash

#here文档，自动生成Makefile文件

#httpd
LOCAL_PATH=$(pwd)
SRC=$(ls ${LOCAL_PATH}/*.c | tr '\n' ' ')
OBJ=$(ls ${LOCAL_PATH}/*.c | sed 's/\.c/\.o/g' | tr '\n' ' ')
SERVER_BIN=httpd
LIB="-lpthread"

#主Makefile生成httpd
cat <<EOF > Makefile
${SERVER_BIN}:${OBJ}
	CXX -o \$@ \$^ ${LIB}
%.o:%.c
	CXX -c \$<

.PHONY:clean
clean:
	rm -rf *.o ${SERVER_BIN} release

.PHONY:release
release:${SERVER_BIN}
	mkdir release
	cp ${SERVER_BIN} release/
	cp -rf log release/
	cp -rf conf release/
	cp -rf wwwroot release/
	cp -f http_ctl.sh release/ 
EOF

