// main.cpp
#include "entt/entt.hpp"
#include "lua.hpp"
#include "Components.hpp"

#include "Scene.hpp"
#include "System.hpp"
#include "LuaBindings.hpp"

#include <iostream>
#include <thread>
#include <windows.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <stdlib.h>
#include <time.h>




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





static int PrintTransform(lua_State* L) {
    Transform t = lua_totransform(L, 1);
    std::cout << "[C++] Transform:\n";
    std::cout << "  Position: (" << t.position.X << ", " << t.position.Y << ", " << t.position.Z << ")\n";
    std::cout << "  Rotation: (" << t.rotation.X << ", " << t.rotation.Y << ", " << t.rotation.Z << ")\n";
    std::cout << "  Scale:    (" << t.scale.X << ", " << t.scale.Y << ", " << t.scale.Z << ")\n";
    return 0;
}




// --------------------------- //
//            MAIN             //
// --------------------------- //

int main() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    // Registrera PrintTransform manuellt om du vill testa direkt
    lua_register(L, "PrintTransform", PrintTransform);

    // Skapa Scene och exponera till Lua
    Scene scene(L);
    Scene::lua_openscene(L, &scene);

    // Skapa systemen
    scene.CreateSystem<PoisonSystem>(10);
    scene.CreateSystem<CleanupSystem>();
    scene.CreateSystem<InfoSystem>();

    // Ladda och kör sceneDemo.lua om du vill
    luaL_dofile(L, "sceneDemo.lua");
    std::printf("Living entities:   %d\n", scene.GetEntityCount());

    // Kör 10 uppdateringar (ticks)
    for (int i = 0; i < 10; i++) {
        scene.UpdateSystems(1);
    }

    // Starta interaktiv Lua-terminal
    ConsoleThreadFunction(L);

    lua_close(L);
    return 0;
}
