#pragma once
#include "Components.hpp"
#include "lua.hpp"

Vector3 lua_tovector(lua_State* L, int index);
void lua_pushvector(lua_State* L, const Vector3& v);
Transform lua_totransform(lua_State* L, int index);
void lua_pushtransform(lua_State* L, const Transform& t);
