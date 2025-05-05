/*
#include <iostream>
#include <thread>
#include <windows.h>
#include <string>
#include <cstdlib> // rand()
#include <ctime>   // time()

#include "lua.hpp"

// --------------------------- //
//         Vector3 Struct      //
// --------------------------- //

struct Vector3
{
    float X, Y, Z;
    Vector3(float x = 0.f, float y = 0.f, float z = 0.f) :
        X(x), Y(y), Z(z) {
    }
};

// --------------------------- //
//      Lua -> Vector3         //
// --------------------------- //

Vector3 lua_tovector(lua_State* L, int index) {
    if (!lua_istable(L, index)) {
        luaL_error(L, "Expected table at index %d", index);
        return Vector3();
    }

    lua_getfield(L, index, "x");
    float x = static_cast<float>(luaL_optnumber(L, -1, 0.0));
    lua_pop(L, 1);

    lua_getfield(L, index, "y");
    float y = static_cast<float>(luaL_optnumber(L, -1, 0.0));
    lua_pop(L, 1);

    lua_getfield(L, index, "z");
    float z = static_cast<float>(luaL_optnumber(L, -1, 0.0));
    lua_pop(L, 1);

    return Vector3(x, y, z);
}

// --------------------------- //
//      Vector3 -> Lua         //
// --------------------------- //

void lua_pushvector(lua_State* L, const Vector3& vector) {
    lua_newtable(L);
    lua_pushnumber(L, vector.X);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, vector.Y);
    lua_setfield(L, -2, "y");

    lua_pushnumber(L, vector.Z);
    lua_setfield(L, -2, "z");
}

// --------------------------- //
//     Lua-bindade funktioner  //
// --------------------------- //

static int PrintVector(lua_State* L) {
    Vector3 vector = lua_tovector(L, 1);
    std::cout << "[C++] Vector(" << vector.X << ", " << vector.Y << ", " << vector.Z << ")" << std::endl;
    return 0;
}

static int RandomVector(lua_State* L) {
    int min = static_cast<int>(luaL_checknumber(L, 1));
    int max = static_cast<int>(luaL_checknumber(L, 2));

    Vector3 vec(
        static_cast<float>(rand() % (max - min + 1) + min),
        static_cast<float>(rand() % (max - min + 1) + min),
        static_cast<float>(rand() % (max - min + 1) + min)
    );

    lua_pushvector(L, vec);
    return 1;
}

// --------------------------- //
//     Hjälpfunktioner         //
// --------------------------- //

void DumpError(lua_State* L) {
    if (lua_gettop(L) && lua_isstring(L, -1)) {
        std::cout << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
}

void ConsoleThreadFunction(lua_State* L) {
    std::string input;
    while (GetConsoleWindow()) {
        std::cout << ">";
        std::getline(std::cin, input);
        if (luaL_dostring(L, input.c_str()) != LUA_OK) {
            DumpError(L);
        }
    }
}

void printStack(lua_State* L) {
    int top = lua_gettop(L);
    std::cout << "-------- STACK BEGIN --------" << std::endl;
    for (int i = top; i >= 1; --i) {
        int type = lua_type(L, i);
        const char* typeName = lua_typename(L, type);
        std::cout << i << " | " << typeName;
        switch (type) {
        case LUA_TSTRING: std::cout << " | " << lua_tostring(L, i); break;
        case LUA_TNUMBER: std::cout << " | " << lua_tonumber(L, i); break;
        case LUA_TBOOLEAN: std::cout << " | " << (lua_toboolean(L, i) ? "true" : "false"); break;
        case LUA_TNIL: std::cout << " | nil"; break;
        default: std::cout << " | [unknown]"; break;
        }
        std::cout << std::endl;
    }
    std::cout << "-------- STACK END ----------" << std::endl;
}

// --------------------------- //
//            MAIN             //
// --------------------------- //

int main() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    srand(static_cast<unsigned int>(time(nullptr)));

    // Registrera funktioner till Lua
    lua_pushcfunction(L, PrintVector);
    lua_setglobal(L, "PrintVector");

    lua_pushcfunction(L, RandomVector);
    lua_setglobal(L, "RandomVector");

    // Starta konsoltråd för Lua-input (frivillig)
    std::thread consoleThread(ConsoleThreadFunction, L);

    // Kör Lua-testfil med vektorer
    if (luaL_dofile(L, "vector-demo.lua") != LUA_OK) {
        DumpError(L);
    }

    consoleThread.join();
    lua_close(L);
    return 0;
}
*/