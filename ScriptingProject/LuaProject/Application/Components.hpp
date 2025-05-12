#pragma once
#include <entt/entt.hpp>
#include <cstring>

struct Vector3 {
    float X, Y, Z;
    Vector3(float x = 0.f, float y = 0.f, float z = 0.f)
        : X(x), Y(y), Z(z) {
    }
};

struct Transform {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};

struct Health {
    float Value;
};

struct Poison {
    float TickDamage;
};

struct Behaviour
{
    char ScriptPath[64];
    int LuaTableRef;

    Behaviour(const char* path, int luaRef)
        : LuaTableRef(luaRef)
    {
		memset(ScriptPath, '\0', sizeof(ScriptPath));
        strcpy_s(ScriptPath, path);
    }
};

