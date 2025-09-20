#include <Arduino.h>
#include "os/Lua.h"

#include <lua.hpp>

Print* globalOutput = nullptr;

extern "C" {
static int lua_print(lua_State* L) {
    if (!globalOutput) {
        return 0;
    }

    int n = lua_gettop(L); /* number of arguments */
    int i;
    lua_getglobal(L, "tostring");
    for (i = 1; i <= n; i++) {
        const char* s;
        size_t l;
        lua_pushvalue(L, -1); /* function to be called */
        lua_pushvalue(L, i); /* value to print */
        lua_call(L, 1, 1);
        s = lua_tolstring(L, -1, &l); /* get result */
        if (s == nullptr)
            return luaL_error(L, "'tostring' must return a string to 'print'");
        if (i > 1) globalOutput->write("\t");
        globalOutput->write(s);
        lua_pop(L, 1); /* pop result */
    }
    globalOutput->println();
    return 0;
}

static int lua_millis(lua_State* L) {
    lua_pushnumber(L, millis());
    return 1;
}

static int get_pin(lua_State* L, int additional_arguments) {
    int n = lua_gettop(L); /* number of arguments */
    if (n != 1 + additional_arguments) {
        return luaL_error(L, "invalid number of arguments");
    }
    int pin = luaL_checkinteger(L, 1);
    if (pin != 0) {
        luaL_error(L, "pin not available");
    }
    return pin;
}

static int lua_pinmode(lua_State* L) {
    int pin = get_pin(L, 1);
    auto mode = luaL_checkstring(L, 2);
    if (strncmp(mode, "output", 6) == 0) {
        pinMode(pin, OUTPUT);
    } else if (strncmp(mode, "input_pullup", 12) == 0) {
        pinMode(pin, INPUT_PULLUP);
    } else {
        pinMode(pin, INPUT);
    }
    return 0;
}

static int lua_readpin(lua_State* L) {
    int pin = get_pin(L, 0);
    lua_pushinteger(L, digitalRead(pin));
    return 1;
}

static const luaL_Reg pinlib[] = {
    {"mode", lua_pinmode},
    {"read", lua_readpin},
    {nullptr, nullptr}
};


/*
** Open math library
*/
LUAMOD_API int luaopen_pin(lua_State* L) {
    luaL_newlib(L, pinlib);
    return 1;
}
}

Lua::Lua() : m_state{luaL_newstate()} {
    luaL_requiref(m_state, "_G", luaopen_base, 1);
    lua_pop(m_state, 1);

    luaL_requiref(m_state, LUA_TABLIBNAME, luaopen_table, 1);
    lua_pop(m_state, 1);

    luaL_requiref(m_state, LUA_MATHLIBNAME, luaopen_math, 1);
    lua_pop(m_state, 1);

    luaL_requiref(m_state, LUA_COLIBNAME, luaopen_coroutine, 1);
    lua_pop(m_state, 1);

    lua_register(m_state, "print", lua_print);
    lua_register(m_state, "millis", lua_millis);

    addModule("pin", &luaopen_pin);

    std::string seedCode = std::string{"math.randomseed("} + std::to_string(esp_random() / 2) + ")";
    run(seedCode.c_str());
}

void Lua::run(const char* script, Print* output) {
    std::unique_lock<std::mutex> stateLock{m_stateMutex};
    globalOutput = output ? output : &Serial;
    if (luaL_dostring(m_state, script) != LUA_OK) {
        globalOutput->println(lua_tostring(m_state, -1));
        lua_pop(m_state, 1);
    }
    globalOutput = nullptr;
}

void Lua::load(const char* fileName, Print* output) {
    std::unique_lock<std::mutex> stateLock{m_stateMutex};
    globalOutput = output ? output : &Serial;
    if (luaL_dofile(m_state, fileName) != LUA_OK) {
        globalOutput->println(lua_tostring(m_state, -1));
        lua_pop(m_state, 1);
    }
    globalOutput = nullptr;
}

void Lua::call(const char* func, const std::vector<std::string>& argv, Print* output) {
    std::unique_lock<std::mutex> stateLock{m_stateMutex};
    globalOutput = output ? output : &Serial;
    lua_getglobal(m_state, func);
    for (auto& arg: argv) {
        lua_pushstring(m_state, arg.c_str());
    }
    if (lua_pcall(m_state, argv.size(), 0, 0) != LUA_OK) {
        globalOutput->printf("error calling '%s': %s\r\n", func, lua_tostring(m_state, -1));
        lua_pop(m_state, 1);
    }
    globalOutput = nullptr;
}

void Lua::call(const char* func, const std::vector<Arg>& argv, Print* output) {
    std::unique_lock<std::mutex> stateLock{m_stateMutex};
    globalOutput = output ? output : &Serial;
    lua_getglobal(m_state, func);
    for (auto& arg: argv) {
        switch (arg.getType()) {
            case Arg::String:
                lua_pushstring(m_state, arg.getStrVal().c_str());
                break;
            case Arg::Integer:
                lua_pushinteger(m_state, arg.getIntVal());
                break;
            case Arg::Float:
                lua_pushnumber(m_state, arg.getFloatVal());
                break;
        }
    }
    if (lua_pcall(m_state, argv.size(), 0, 0) != LUA_OK) {
        globalOutput->printf("error calling '%s': %s\r\n", func, lua_tostring(m_state, -1));
        lua_pop(m_state, 1);
    }
    globalOutput = nullptr;
}

void Lua::addModule(const char* name, int (* loadFunc)(lua_State*)) {
    std::unique_lock<std::mutex> stateLock{m_stateMutex};
    luaL_requiref(m_state, name, loadFunc, 1);
    lua_pop(m_state, 1);
}
