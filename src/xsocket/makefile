# $@ 表示指定输出文件名 
# $+ 当前规则的所有依赖文件，按书写顺序保留重复项 
# -fpic 表示生成 位置无关代码 让代码加载到内存不同位置时仍然能正常运行
libxsocket.so:xtcp.cpp xtcp.h 
	g++ $+ -o $@ -fpic -shared -std=c++11 
	cp *.so /usr/lib