#ifndef GPUCOMMAND_HPP_
#define GPUCOMMAND_HPP_

#include <cstdint>

enum class CommandType : uint8_t
{
    None = 0,
    DrawPolygon = 1,
    DrawLine,
    DrawRectangle,
    VramVramCopy,
    CpuVramCopy,
    VramCpuCopy
};

class ParameterArray
{
    public:
        ParameterArray()
        {
            m_headPtr = 0;
        }

        uint8_t size() const
        {
            return m_headPtr;
        }

        void addParam(uint32_t param)
        {
            if (m_headPtr >= 32) {
                return;
            }
            m_data[m_headPtr] = param;
            m_headPtr++;
        }

        const uint32_t *data() const
        {
            return m_data;
        }

        void clear()
        {
            m_headPtr = 0;
        }

    private:
        uint32_t m_data[32];
        uint8_t m_headPtr;
};

class GPUCommand
{
    public:
        GPUCommand();
        ~GPUCommand();

        void setCommand(uint32_t cmd);
        CommandType command() const;
        void reset();
        uint32_t raw() const;

        void addParam(uint32_t param);
        const uint32_t *params() const;
        uint8_t nbParams() const;

    private:
        CommandType m_type;
        uint32_t m_rawCmd;
        ParameterArray m_params;
};

#endif /* !GPUCOMMAND_HPP_ */
