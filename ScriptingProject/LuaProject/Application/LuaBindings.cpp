#include "LuaBindings.hpp"

Vector3 lua_tovector(lua_State* L, int index) {
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
    lua_pushnumber(L, v.X); lua_setfield(L, -2, "x");
    lua_pushnumber(L, v.Y); lua_setfield(L, -2, "y");
    lua_pushnumber(L, v.Z); lua_setfield(L, -2, "z");
}

Transform lua_totransform(lua_State* L, int index) {
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
    lua_pushvector(L, t.position); lua_setfield(L, -2, "position");
    lua_pushvector(L, t.rotation); lua_setfield(L, -2, "rotation");
    lua_pushvector(L, t.scale);    lua_setfield(L, -2, "scale");
}
