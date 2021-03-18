libluamapr.so: luamapr.o
	gcc luamapr.o -shared -o libluamapr.so
luamapr.o: luamapr.c
	gcc luamapr.c -I/usr/include/lua5.3/ -c -fPIC -ggdb3
clean:
	rm *.o
	rm *.so

test: libluamapr.so
	lua test.lua

