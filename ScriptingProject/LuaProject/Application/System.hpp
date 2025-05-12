#include <entt/entt.hpp>
#include "Components.hpp"

#include <iostream>
#include <lua.hpp>

inline void DumpError(lua_State* L) {
    // Get the error message at the top of the stack
    const char* msg = lua_tostring(L, -1);
    std::cerr << "[Lua Error] " << (msg ? msg : "Unknown error") << "\n";
    // Pop the error message so the stack is clean
    lua_pop(L, 1);
}

class System {
public:
    virtual bool OnUpdate(entt::registry& registry, float delta) = 0;
    virtual ~System() = default;
};

// --- PoisonSystem ---
class PoisonSystem : public System {
    int m_lifetime;
public:
    PoisonSystem(int lifetime) : m_lifetime(lifetime) {}

    bool OnUpdate(entt::registry& registry, float delta) final
    {
        auto view = registry.view<Health, Poison>();
        view.each([](Health& health, const Poison& poison) {
            health.Value -= poison.TickDamage;
            });
		return (--m_lifetime <= 0);
    }
};
// --- CleanupSystem ---
class CleanupSystem : public System {
public:
    bool OnUpdate(entt::registry& r, float /*dt*/) override {
        auto view = r.view<Health>();
        for (auto entity : view) {
            const auto& h = view.get<Health>(entity);
            if (h.Value <= 0.0f) {
                r.destroy(entity);
            }
        }
        return false;
    }
};

// --- InfoSystem ---
class InfoSystem : public System {
    int m_tick = 0;
public:
    bool OnUpdate(entt::registry& r, float /*dt*/) override {
        auto healthView = r.view<Health>();
        auto poisonView = r.view<Health, Poison>();

        int aliveCount = static_cast<int>(std::distance(healthView.begin(), healthView.end()));
        int poisonedCount = static_cast<int>(std::distance(poisonView.begin(), poisonView.end()));

        std::printf("-- Update %d --\n", m_tick++);
        std::printf("Living entities:   %d\n", aliveCount);
        std::printf("Poisoned entities: %d\n\n", poisonedCount);
        return false;
    }
};

// 13b) Creating a behaviour system

class BehaviourSystem : public System
{
    lua_State* L;

public:
    BehaviourSystem(lua_State* L)
        : L(L)
    {
    }

    bool OnUpdate(entt::registry& registry, float delta) final
    {
        // Retrieve all entities with a Behaviour component
        auto view = registry.view<Behaviour>();

        // Capture L and delta by reference
        view.each([&](Behaviour& script)
            {
                // Retrieve the behaviour table to the top of the stack
                lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);

                // Retrieve the OnUpdate method from the table
                lua_getfield(L, -1, "OnUpdate");

                // Push the table as the first argument to the method
                lua_pushvalue(L, -2);

                // Push delta as the second argument to the method
                lua_pushnumber(L, delta);

                // Call the method (1 arg, 0 results), popping the method and args
                if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                    DumpError(L);
                }

                // Pop the behaviour table from the stack
                lua_pop(L, 1);
            });

        // false -> Do not destroy the system
        return false;
    }
};
