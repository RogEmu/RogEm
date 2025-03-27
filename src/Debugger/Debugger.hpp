#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <list>
#include <memory>
#include <GLFW/glfw3.h>

struct CPU;

class Debugger
{
    public:
        Debugger(const std::shared_ptr<CPU> &cpu);
        ~Debugger();

        void update();
        void registerTable();
        void MemoryTable();
        void InstructionTable();

        bool isRunning() const;
        bool isPaused() const;
        float getSimSpeed() const;

    private:
        int initGFLW();
        int initImGUi();

    private:
        std::shared_ptr<CPU> m_cpu;
        GLFWwindow* window;
        bool m_running;

        bool m_systemPaused;
        float m_simSpeed;
};

#endif /* !DEBUGGER_HPP_ */
