#include <iostream>

#ifdef __cplusplus
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#endif //__cplusplus

int main(int argc, char **argv) {
	std::cout << "Initializing the test\n";

	lua_State* L;
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L, "test.lua");
	std::cout << "Final:" << lua_tostring(L, -1) << "\n";
    	lua_close(L);

	std::cout << "TEST FAILD\n";
	exit(-1);
}

