#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <list>
#include <memory>
#include <GLFW/glfw3.h>

class CPU;

class Debugger {
    public:
        Debugger(const std::shared_ptr<CPU> &cpu);
        ~Debugger();

        void update();
        void registerTable();
        void MemoryTable();
        void InstructionTable();
    private:
        std::shared_ptr<CPU> m_cpu;
        const std::string* registerNames;
        GLFWwindow* window;
};

#endif /* !DEBUGGER_HPP_ */