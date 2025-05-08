// main.cpp
#include "entt/entt.hpp"
#include "lua.hpp"
#include <iostream>
#include <thread>
#include <windows.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <stdlib.h>
#include <time.h>



// --------------------------- //
//         Vector3 Struct      //
// --------------------------- //

struct Vector3 {
    float X, Y, Z;
    Vector3(float x = 0.f, float y = 0.f, float z = 0.f) : X(x), Y(y), Z(z) {}
};
void ConsoleThreadFunction(lua_State* L) {
    std::string input;
    while (true) {
        std::cout << "Lua> ";
        std::getline(std::cin, input);

        if (input == "exit") break;

        if (luaL_dostring(L, input.c_str()) != LUA_OK) {
            std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
        }
    }
}

struct Transform {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};

// --------------------------- //
//      Lua <-> Vector3        //
// --------------------------- //

Vector3 lua_tovector(lua_State* L, int index) {
    if (!lua_istable(L, index))
        luaL_error(L, "Expected vector table at index %d", index);

    Vector3 v;

    lua_getfield(L, index, "x");
    v.X = static_cast<float>(luaL_optnumber(L, -1, 0.0));
    lua_pop(L, 1);

    lua_getfield(L, index, "y");
    v.Y = static_cast<float>(luaL_optnumber(L, -1, 0.0));
    lua_pop(L, 1);

    lua_getfield(L, index, "z");
    v.Z = static_cast<float>(luaL_optnumber(L, -1, 0.0));
    lua_pop(L, 1);

    return v;
}

void lua_pushvector(lua_State* L, const Vector3& v) {
    lua_newtable(L);

    lua_pushnumber(L, v.X);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, v.Y);
    lua_setfield(L, -2, "y");

    lua_pushnumber(L, v.Z);
    lua_setfield(L, -2, "z");
}

// --------------------------- //
//      Lua <-> Transform      //
// --------------------------- //

Transform lua_totransform(lua_State* L, int index) {
    if (!lua_istable(L, index))
        luaL_error(L, "Expected transform table at index %d", index);

    Transform t;

    lua_getfield(L, index, "position");
    t.position = lua_tovector(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "rotation");
    t.rotation = lua_tovector(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "scale");
    t.scale = lua_tovector(L, -1);
    lua_pop(L, 1);

    return t;
}

void lua_pushtransform(lua_State* L, const Transform& t) {
    lua_newtable(L);

    lua_pushvector(L, t.position);
    lua_setfield(L, -2, "position");

    lua_pushvector(L, t.rotation);
    lua_setfield(L, -2, "rotation");

    lua_pushvector(L, t.scale);
    lua_setfield(L, -2, "scale");
}

static int PrintTransform(lua_State* L) {
    Transform t = lua_totransform(L, 1);
    std::cout << "[C++] Transform:\n";
    std::cout << "  Position: (" << t.position.X << ", " << t.position.Y << ", " << t.position.Z << ")\n";
    std::cout << "  Rotation: (" << t.rotation.X << ", " << t.rotation.Y << ", " << t.rotation.Z << ")\n";
    std::cout << "  Scale:    (" << t.scale.X << ", " << t.scale.Y << ", " << t.scale.Z << ")\n";
    return 0;
}

struct Health {
	float Value;
};

struct Poison {
	float TickDamage;
};

// --------------------------- //
//            MAIN             //
// --------------------------- //

int main() {
    entt::registry registry;

    srand(time(NULL));
    for (int i = 0; i < 100; ++i) {
        auto entity = registry.create();
        registry.emplace<Health>(entity, 100.0f);
        float tickDamage = static_cast<float>(rand() % 10 + 1);

        registry.emplace<Poison>(entity, tickDamage);
    }

    int iterations = 0;
    while (!registry.view<Health>().empty())
    {
        {
            auto view = registry.view<Health, Poison>();
            view.each([](Health& health, Poison& poison) {
                health.Value -= poison.TickDamage;
            });
        }
        {
            auto view = registry.view<Health>();
            view.each([&](entt::entity entity, const Health& health) {
                if (health.Value <= 0.f) 
                {
                    registry.destroy(entity);
                } 
            });
        }
        iterations++;
        std::cout << "Iteration" << iterations
		    << ", entities alive: " << registry.view<entt::entity>().size()
            << std::endl;
    }

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);


    luaL_dofile(L, "vector-test.lua");

    ConsoleThreadFunction(L);

    lua_close(L);
    return 0;
}
