#pragma once

#include <mutex>
#include <Print.h>
#include <utility>
#include <vector>

extern "C" {
struct lua_State;
}

class Lua {
public:
    class Arg {
    public:
        enum Type : uint8_t {
            String,
            Integer,
            Float,
        };

        explicit Arg(std::string val) : m_type{String}, m_strVal{std::move(val)} {
        }

        Arg(const char* val) : m_type{String}, m_strVal{val} /* NOLINT(*-explicit-constructor) */ {
        }

        explicit Arg(int val) : m_type{Integer}, m_intVal{val} {
        }

        explicit Arg(float val) : m_type{Float}, m_floatVal{val} {
        }

        Arg(std::string&& val) : m_type{String}, m_strVal{std::move(val)} /* NOLINT(*-explicit-constructor) */ {
        }

        Arg(int&& val) : m_type{Integer}, m_intVal{val} /* NOLINT(*-explicit-constructor) */ {
        }

        Arg(float&& val) : m_type{Float}, m_floatVal{val} /* NOLINT(*-explicit-constructor) */ {
        }

        Arg(const Arg& other) /* NOLINT(*-pro-type-member-init) */
            : m_type(other.m_type) {
            switch (m_type) {
                case String:
                    ::new(&m_strVal) std::string(other.m_strVal);
                    break;
                case Integer:
                    ::new(&m_intVal) int(other.m_intVal);
                    break;
                case Float:
                    ::new(&m_floatVal) float(other.m_floatVal);
                    break;
            }
        }

        Arg(Arg&& other) /* NOLINT(*-pro-type-member-init) */ noexcept {
            m_type = other.m_type;
            switch (m_type) {
                case String:
                    ::new(&m_strVal) std::string(std::move(other.m_strVal));
                    break;
                case Integer:
                    ::new(&m_intVal) int(other.m_intVal);
                    break;
                case Float:
                    ::new(&m_floatVal) float(other.m_floatVal);
                    break;
            }
        }

        ~Arg() {
            if (m_type == String) {
                m_strVal.~basic_string();
            }
        }

        Arg& operator=(const Arg& other) {
            Arg arg = other;
            swap(*this, arg);
            return *this;
        }

        Arg& operator=(Arg&& other) noexcept {
            this->~Arg();
            new(this) Arg(std::move(other));
            return *this;
        }

        friend void swap(Arg& a, Arg& b) noexcept {
            Arg moved_a = std::move(a);
            a.~Arg();
            ::new(&a) Arg(std::move(b));
            b.~Arg();
            ::new(&b) Arg(std::move(moved_a));
        }

        Type getType() const {
            return m_type;
        }

        const std::string& getStrVal() const {
            assert(m_type == String);
            return m_strVal;
        }

        int getIntVal() const {
            assert(m_type == Integer);
            return m_intVal;
        }

        float getFloatVal() const {
            assert(m_type == Float);
            return m_floatVal;
        }

    private:
        Type m_type;

        union {
            std::string m_strVal;
            int m_intVal;
            float m_floatVal;
        };
    };

    explicit Lua();

    void run(const char* script, Print* output = nullptr);

    void load(const char* fileName, Print* output = nullptr);

    void call(const char* func, const std::vector<std::string>& argv, Print* output = nullptr);

    void call(const char* func, const std::vector<Arg>& argv, Print* output = nullptr);

    void addModule(const char* name, int (* loadFunc)(lua_State*));

private:
    lua_State* m_state;
    std::mutex m_stateMutex;
};
