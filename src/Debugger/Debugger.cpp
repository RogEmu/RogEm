#include <iostream>

#ifdef _WIN32
    #include <Windows.h>
#endif // _WIN32

#include <GL/gl.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <fmt/format.h>
#

#include "Debugger.hpp"
#include "CPU.h"
#include "Disassembler.h"

Debugger::Debugger(const std::shared_ptr<CPU> &cpu) :
    m_cpu(cpu),
    m_systemPaused(false),
    m_simSpeed(1.0f)
{
}

Debugger::~Debugger()
{
}

void Debugger::registerTable()
{
    auto tableFlags = ImGuiTableFlags_Borders
                    | ImGuiTableFlags_RowBg;
    ImGui::Begin("Registers");
    if(ImGui::BeginTable("Registers", 2, tableFlags))
    {
        ImGui::TableSetupColumn("Register");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();
        for (int i = 0; i < NB_GPR; i++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", Disassembler::getRegisterName(i).c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%08X", m_cpu->m_registers[i]);
        }
        const char* extraRegisters[] = {"PC", "HI", "LO"};
        uint32_t extraValues[] = {m_cpu->m_pc, m_cpu->m_hi, m_cpu->m_lo};

        for (int i = 0; i < 3; i++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", extraRegisters[i]);
            ImGui::TableNextColumn();
            ImGui::Text("%08X", extraValues[i]);
        }

        //COP0 Registers
        ImGui::TableNextColumn();
        ImGui::Text("COP0-SR");
        ImGui::TableNextColumn();
        ImGui::Text("%08X", m_cpu->m_cop0Reg[12]);
        ImGui::TableNextColumn();
        ImGui::Text("COP0-CAUSE");
        ImGui::TableNextColumn();
        ImGui::Text("%08X", m_cpu->m_cop0Reg[13]);

        ImGui::EndTable();
    }
    ImGui::End();
}

void Debugger::MemoryTable()
{
    auto tableFlags = ImGuiTableFlags_RowBg
                    | ImGuiTableFlags_BordersH
                    | ImGuiTableFlags_SizingFixedFit;
    ImGui::Begin("Memory", nullptr);
    char startAddrStr[16] = "";
    ImGui::InputText("Start Address", startAddrStr, 16, ImGuiInputTextFlags_CharsHexadecimal);
    if (ImGui::BeginTable("Memory", 18, tableFlags)) {
        ImGui::TableSetupColumn("Address");
        for (int i = 0; i < 16; i++)
        {
            ImGui::TableSetupColumn(fmt::format("{:02X}", i).c_str());
        }
        ImGui::TableSetupColumn("ASCII");
        ImGui::TableHeadersRow();


        uint32_t startAddr = 0;
        if (strlen(startAddrStr) > 0)
        {
            startAddr = std::stoul(startAddrStr, 0, 16);
        }
        uint32_t size = 0x1fe;
        for (uint32_t i = startAddr; i < startAddr + size; i += 16) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%08X", i);

            std::string asciiStr;
            for (int j = 0; j < 16; j++) {
                uint8_t byte = m_cpu->m_bus->loadByte(i + j);
                ImGui::TableNextColumn();
                ImGui::Text("%02X", byte);

                char c = byte > 126 || byte < 32 ? '.' : byte;
                asciiStr += c;
            }
            ImGui::TableNextColumn();
            ImGui::Text("%s", asciiStr.c_str());
        }

        ImGui::EndTable();
    }
    ImGui::End();
}

void Debugger::InstructionTable()
{
    ImGui::Begin("Instructions");

    ImGui::BeginGroup();
    ImGui::Checkbox("Pause", &m_systemPaused);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##CPU_Step_Fw", ImGuiDir_Right) && m_systemPaused)
    {
        m_cpu->step();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        m_cpu->reset();
    }
    ImGui::EndGroup();
    ImGui::SliderFloat("Simulation Speed", &m_simSpeed, 0.00001f, 1.0f, "%.5f");

    auto tableFlags = ImGuiTableFlags_RowBg
                    | ImGuiTableFlags_Borders;

    if (ImGui::BeginTable("Instructions", 1, tableFlags)) {
        ImGui::TableSetupColumn("Instructions");
        ImGui::TableHeadersRow();

        int nbInstructions = 32;
        int half = nbInstructions / 2;

        for (int i = -half; i < half; i++) {
            ImGui::TableNextColumn();
            uint32_t pc = m_cpu->m_pc + i * 4;
            Instruction instr = {.raw = m_cpu->m_bus->loadWord(pc)};
            auto str = Disassembler::disassemble(pc, instr);
            if (pc == m_cpu->m_pc)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
                ImGui::Text("%s", str.c_str());
                ImGui::PopStyleColor();
            } else
            {
                ImGui::Text("%s", str.c_str());
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

bool Debugger::isPaused() const
{
    return m_systemPaused;
}

float Debugger::getSimSpeed() const
{
    return m_simSpeed;
}

void Debugger::update()
{
    registerTable();
    MemoryTable();
    InstructionTable();
}
