/*
 * This file is part of the luamapr library (https://github.com/jordanvrtanoski/lua-map-reduce.git)
 * Copyright (C) 2021 Jordan Vrtanoski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * string_repeat( int n, const char * s ) {
	size_t slen = strlen(s);
	char * dest = malloc(n*slen+1);

	int i; char * p;
	for ( i=0, p = dest; i < n; ++i, p += slen ) {
		memcpy(p, s, slen);
	}
	*p = '\0';
	return dest;
}

void debug_stack(lua_State *L, const char *message, int n) {
	printf("Stack [%s]: ", message);
	for (int i = n; i > 0; i--) {
		printf("%s[%i(%i)], ",
				lua_typename(L, lua_type(L, -i)),
				-i,
				lua_absindex(L, -i));
	}
	printf("\n");
}

static int l_foreach(lua_State *L) {
	if (lua_gettop(L) != 2) {
		return luaL_error(L, "expecting exactly 2 arguments");
	}
	lua_rotate(L, 2, 2);                           // Swith the placed between the table and the function for more conviniance
	luaL_checktype(L, 2, LUA_TTABLE);
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_pushnil(L);                                // Add extra space for the first lua_next to pop
	int loop=1, indent=0, count=0;
	do {
		if ( lua_next(L,-2) != 0 ) {
			if (lua_istable(L,-1)) {
				lua_pushnil(L);        // Start iterating this sub-table
				indent+=1;
			} else {
				lua_pushnil(L);
				lua_pushnil(L);
				lua_pushnil(L);        // Make space for the function call
				lua_copy(L, 1, -3);    // Place function on -3 place on stack
				lua_copy(L, -5, -2);   // Copy key
				lua_copy(L, -4, -1);   // Copy value
				lua_call(L, 2, 0);     // Call the function
				count++;

				lua_pop(L,1);          // Remove the value from stack to prepare for lua_next()
			}
		} else {
			indent-=1;
			lua_pop(L,1);
			if (!lua_istable(L, -2)) {
				loop = 0;              // No more tables on stack, breaking the loop
			}
		}
	} while (loop);
	lua_pop(L,2);                                  // Pop the arguments from the stack
	lua_pushinteger(L,count);
	return 1;
}


static int l_map(lua_State *L) {
	if (lua_gettop(L) != 2) {
		return luaL_error(L, "expecting exactly 2 arguments");
	}
	lua_rotate(L, 2, 2);                           // Swith the placed between the table and the function for more conviniance
	luaL_checktype(L, 2, LUA_TTABLE);
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_pushnil(L);                                // Add extra space for the first lua_next to pop
	int loop=1, indent=0, count=0;
	do {
		if ( lua_next(L,-2) != 0 ) {
			if (lua_istable(L,-1)) {
				lua_pushnil(L);        // Start iterating this sub-table
				indent+=1;
			} else {
				lua_pushnil(L);
				lua_pushnil(L);
				lua_pushnil(L);
				lua_copy(L, 1, -3);    // Place function on -3 place on stack
				lua_copy(L, -5, -2);   // Copy key
				lua_copy(L, -4, -1);   // Copy value
				lua_call(L, 2, 1);     // Call the function
				lua_copy(L,-3,-2);
				lua_settable(L,-4);
				count++;
			}
		} else {
			indent-=1;
			lua_pop(L,1);
			if (!lua_istable(L, -2)) { 
				loop = 0;              // No more tables on stack, breaking the loop
			}
		}
	} while (loop);
	lua_pop(L,1);                                  // Pop the function form the stack
	lua_pushinteger(L,count);                      // Return the number of non-table values visited
	return 1;
}

static int l_reduce(lua_State *L) {
	if (lua_gettop(L) != 3) {
		return luaL_error(L, "expecting exactly 3 arguments");
	}
	lua_rotate(L, 2, 3);                           // Put accumulator at the end
	lua_rotate(L, 3, 2);                           // Switch the place of table and function
	luaL_checktype(L, 3, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	lua_pushnil(L);                                // Add extra space for the first lua_next to pop
	int loop=1, indent=0, count=0;
	do {
		if ( lua_next(L,-2) != 0 ) {
			if (lua_istable(L,-1)) {
				lua_pushnil(L);        // Start iterating this sub-table
				indent+=1;
			} else {
				lua_pushnil(L);
				lua_pushnil(L);
				lua_pushnil(L);
				lua_pushnil(L);        // Make 4 empty places for the function call

				lua_copy(L, 2, -4);    // Place function on -4 place on stack
				lua_copy(L, 1, -3);    // Place the accumulator on -3
				lua_copy(L, -6, -2);   // Copy key
				lua_copy(L, -5, -1);   // Copy value
				lua_call(L, 3, 1);     // Call the function

				lua_copy(L,-1,1);      // Copy the result back to accumulator
				lua_pop(L,2);          // clean the result and value, leave the key for next iteration
				count++;
			}
		} else {
			indent-=1;
			lua_pop(L,1);                  // get the last key out of the stack
			if (!lua_istable(L, -2)) { 
				loop = 0;              // No more tables on stack, breaking the loop
			}
		}
	} while (loop);
	lua_pop(L,1);                                  // Pop the function (argument #2) from the stack and leve accumulator as return value

	return 1;
}

static int l_preattyprint(lua_State *L) {
	if (lua_gettop(L) != 1) {
		return luaL_error(L, "expecting exactly 1 arguments");
	}
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_pushnil(L);                               // Add extra space for the first lua_next to pop
	int loop=1, indent=2;
	printf("{\n");
	do {
		if ( lua_next(L,-2) != 0 ) {
			if (lua_istable(L,-1)) {
				printf("%s%s = {\n",
						string_repeat(indent, " "),
						lua_tostring(L, -2));
				lua_pushnil(L); // Start iterating this sub-table
				indent+=2;
			} else {
				if (lua_type(L, -1) == LUA_TSTRING) {
					printf("%s%s -> \"%s\"\n", 
							string_repeat(indent, " "),
							lua_tostring(L, -2), 
							lua_tostring(L, -1));
				} else {
					printf("%s%s -> %s\n", 
							string_repeat(indent, " "),
							lua_tostring(L, -2), 
							lua_tostring(L, -1));
				}

				lua_pop(L,1);
			}
		} else {
			printf("%s}\n", string_repeat(indent, " "));
			if (lua_getmetatable(L,-1)) {
				printf("%s^--- Metatable detected\n", string_repeat(indent, " "));
				lua_pop(L,1);
			}
			indent-=2;
			lua_pop(L,1);
			if (!lua_istable(L, -2)) {
				loop = 0; // No more tables on stack, breaking the loop
			}
		}
	} while (loop);
	lua_pop(L,1);
	lua_pushnumber(L,0);
	return 1;
}


int luaopen_libluamapr(lua_State* L)
{
	lua_register(L, "preattyprint", l_preattyprint);
	lua_register(L, "foreach", l_foreach);
	lua_register(L, "map", l_map);
	lua_register(L, "reduce", l_reduce);
	return 1;
}

