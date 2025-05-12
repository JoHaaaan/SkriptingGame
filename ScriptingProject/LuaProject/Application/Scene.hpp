#pragma once 
#include <entt/entt.hpp>
#include <vector>
#include <lua.hpp>
#include <string>
#include "Components.hpp"
#include "LuaBindings.hpp"

class System;



class Scene 
{
    entt::registry m_registry;
    std::vector<System*> m_systems;

public:
    Scene() = default;
    Scene(lua_State* L) { lua_openscene(L, this); }
    ~Scene();

    int GetEntityCount();
    int CreateEntity();
    bool IsEntity(int entity);
    void RemoveEntity(int entity);

    template <typename... Args>
    bool HasComponents(int entity);

    template <typename T>
    T& GetComponent(int entity);

    template <typename T>
    void SetComponent(int entity, const T&);

    template <typename T, typename... Args>
    void SetComponent(int entity, Args... args);

    template <typename T>
    void RemoveComponent(int entity);

    // CreateSystem
    template <typename T, typename... Args>
    void CreateSystem(Args... args);

    void UpdateSystems(float delta);

    static void lua_openscene(lua_State* L,
        Scene* scene);



    void LuaDummyHook();
private:
    static Scene* lua_GetSceneUpValue(lua_State* L);

    static int lua_CreateEntity(lua_State* L);
    static int lua_SetComponent(lua_State* L);
    static int lua_GetEntityCount(lua_State* L);
    static int lua_IsEntity(lua_State* L);
    static int lua_RemoveEntity(lua_State* L);
	static int lua_HasComponent(lua_State* L);
	static int lua_GetComponent(lua_State* L);
	static int lua_RemoveComponent(lua_State* L);
};

template <typename T, typename... Args>
void Scene::CreateSystem(Args... args) {
    m_systems.emplace_back(new T(args...));
}

