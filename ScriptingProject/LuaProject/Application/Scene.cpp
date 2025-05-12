#include "Scene.hpp"
#include "System.hpp"
#include <iostream>

int Scene::GetEntityCount()
{
    return static_cast<int>(m_registry.view<entt::entity>().size());
}

int Scene::CreateEntity() {
    auto entity = m_registry.create();
    std::cout << "Entity created with ID: " << static_cast<int>(entity) << std::endl;
    return static_cast<int>(entity);
}

bool Scene::IsEntity(int entity)
{
    return m_registry.valid((entt::entity)entity);
}

void Scene::RemoveEntity(int entity)
{
    if (IsEntity(entity)) {
        m_registry.destroy((entt::entity)entity);
    }
}

Scene::~Scene() {
    for (auto sys : m_systems) {
        delete sys;
    }
    m_systems.clear();
}

void Scene::UpdateSystems(float delta)
{
    for (auto it = m_systems.begin(); it != m_systems.end(); ) {
        if ((*it)->OnUpdate(m_registry, delta)) {
            delete (*it);
            it = m_systems.erase(it); // korrekt: erase returnerar ny giltig iterator
        }
        else {
            ++it; // endast öka om vi INTE raderade
        }
    }

}

Scene* Scene::lua_GetSceneUpValue(lua_State* L) {
    return static_cast<Scene*>(lua_touserdata(L, lua_upvalueindex(1)));
}

int Scene::lua_CreateEntity(lua_State* L) {
    Scene* scene = lua_GetSceneUpValue(L);
    int id = scene->CreateEntity();
    lua_pushinteger(L, id);
    return 1;
}

int Scene::lua_SetComponent(lua_State* L) {
    Scene* scene = lua_GetSceneUpValue(L);
    int entity = lua_tointeger(L, 1);
    std::string type = lua_tostring(L, 2);

    if (type == "health") {
        float value = static_cast<float>(lua_tonumber(L, 3));
        scene->SetComponent<Health>(entity, value);
    }
    else if (type == "poison") {
        float tickDamage = static_cast<float>(lua_tonumber(L, 3));
        scene->SetComponent<Poison>(entity, tickDamage);
    }
    else if (type == "behaviour"){
		if (scene->HasComponents<Behaviour>(entity))
		{
			scene->RemoveComponent<Behaviour>(entity);
		}

        const char* path = lua_tostring(L, 3);
		luaL_dofile(L, path); // Load the Lua script
        lua_pushvalue(L, -1);

		int ref = luaL_ref(L, LUA_REGISTRYINDEX); // Store the reference in the registry
        lua_pushinteger(L, entity);
		lua_setfield(L, -2, "entity"); // Set the entity field in the Lua table

		lua_pushstring(L, path);
		lua_setfield(L, -2, "path"); // Set the path field in the Lua table

        lua_getfield(L, -1, "OnCreate");
        lua_pushvalue(L, -2);
		lua_pcall(L, 1, 0, 0); // Call the OnCreate function
        //int ref = RefAndPushBehaviour(L, entity, path);
		//scene->SetComponent<BehaviourComponent>(entity, path, ref);
        return 1;
    }

    return 0;
}

void Scene::lua_openscene(lua_State* L, Scene* scene) {
    lua_newtable(L);

    luaL_Reg methods[] = {
        {"CreateEntity", lua_CreateEntity},
        {"SetComponent", lua_SetComponent},
        {"GetEntityCount", lua_GetEntityCount},
        {"IsEntity", lua_IsEntity},
        {"RemoveEntity", lua_RemoveEntity},
        {"HasComponent", lua_HasComponent},
        {"GetComponent", lua_GetComponent},
        {"RemoveComponent", lua_RemoveComponent},
        {NULL, NULL}
    };

    lua_pushlightuserdata(L, scene);
    luaL_setfuncs(L, methods, 1);
    lua_setglobal(L, "scene");
}


// Add dummy to force update recognition
void Scene::LuaDummyHook() {}

template<typename... Args>
bool Scene::HasComponents(int entity) {
    return m_registry.all_of<Args...>(
        (entt::entity)entity);
}

template<typename T>
T& Scene::GetComponent(int entity) {
    return m_registry.get<T>(
        (entt::entity)entity);
}

template<typename T>
void Scene::SetComponent(int entity, const T& component)
{
    m_registry.emplace_or_replace<T>(
        (entt::entity)entity, component);
}

template<typename T, typename...Args>
void Scene::SetComponent(int entity, Args... args)
{
    T& comp = m_registry.emplace_or_replace<T>(
        (entt::entity)entity, args...);
}

template<typename T>
void Scene::RemoveComponent(int entity)
{
    m_registry.remove<T>(
        (entt::entity)entity);
}


int Scene::lua_GetEntityCount(lua_State* L)
{
    Scene* scene = lua_GetSceneUpValue(L);
    int count = scene->GetEntityCount();
    lua_pushinteger(L, count);
    return 1;
}
int Scene::lua_IsEntity(lua_State* L)
{
    Scene* scene = lua_GetSceneUpValue(L);
    int entity = lua_tointeger(L, 1);
    bool alive = scene->IsEntity(entity);
    lua_pushboolean(L, alive);
    return 1;
}
int Scene::lua_RemoveEntity(lua_State* L)
{
    Scene* scene = lua_GetSceneUpValue(L);
    int entity = lua_tointeger(L, 1);
    scene->RemoveEntity(entity);
    return 0;
}
int Scene::lua_HasComponent(lua_State* L)
{
    Scene* scene = lua_GetSceneUpValue(L);
    int entity = lua_tointeger(L, 1);
    std::string type = lua_tostring(L, 2);

    bool hasComponent = true;

    if (type == "health")
    {
        hasComponent = scene->HasComponents<Health>(entity);
    }
    else if (type == "poison")
    {
        hasComponent = scene->HasComponents<Poison>(entity);
    }
    else if (type == "transform")
    {
        hasComponent = scene->HasComponents<Transform>(entity);
    }
    else
    {
        std::cout << "Unknown component type: " << type << std::endl;
        hasComponent = false;
    }

    lua_pushboolean(L, hasComponent);
    return 1;
}

int Scene::lua_GetComponent(lua_State* L)
{
    Scene* scene = lua_GetSceneUpValue(L);

    if (!lua_isinteger(L, 1) || !lua_isstring(L, 2))
    {
        lua_pushnil(L);
        return 1;
    }

    int entity = lua_tointeger(L, 1);
    std::string type = lua_tostring(L, 2);

    // Sanity check that the entity exist
    if (!scene->IsEntity(entity))
    {
        lua_pushnil(L);
        return 1;
    }

    if (type == "health" && scene->HasComponents<Health>(entity))
    {
        Health& health = scene->GetComponent<Health>(entity);
        lua_pushnumber(L, health.Value); // Maybe push a "component" table?
        return 1;
    }
    else if (type == "poison" && scene->HasComponents<Poison>(entity))
    {
        Poison& poison = scene->GetComponent<Poison>(entity);
        lua_pushnumber(L, poison.TickDamage); // Maybe push a "component" table?
        return 1;
    }
    else if (type == "transform" && scene->HasComponents<Transform>(entity))
    {
        Transform& transform = scene->GetComponent<Transform>(entity);
        lua_pushtransform(L, transform); // from course module M2–M3
        return 1;
    }
    // else if – add more types here

    // Name or component not found
    lua_pushnil(L);
    return 1;
}

int Scene::lua_RemoveComponent(lua_State* L)
{
    Scene* scene = lua_GetSceneUpValue(L);

    int entity = lua_tointeger(L, 1);
    std::string type = lua_tostring(L, 2);

	if (type == "health" && scene->HasComponents<Health>(entity))
	{
		scene->RemoveComponent<Health>(entity);
	}
	else if (type == "poison" && scene->HasComponents<Poison>(entity))
	{
		scene->RemoveComponent<Poison>(entity);
	}
	else if (type == "transform" && scene->HasComponents<Transform>(entity))
	{
		scene->RemoveComponent<Transform>(entity);
	}
    return 0;
}
