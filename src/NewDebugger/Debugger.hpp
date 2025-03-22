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
    private:
        std::shared_ptr<CPU> m_cpu;
        GLFWwindow* window;
};

#endif /* !DEBUGGER_HPP_ */