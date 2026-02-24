#ifndef GPUCOMMAND_HPP_
#define GPUCOMMAND_HPP_

#include <cstdint>

class StateBuffer;

class GPUParamArray
{
    public:
        GPUParamArray() {
            m_headPtr = 0;
        }

        uint8_t size() const {
            return m_headPtr;
        }

        void addParam(uint32_t param) {
            if (m_headPtr >= 32) {
                return;
            }
            m_data[m_headPtr] = param;
            m_headPtr++;
        }

        const uint32_t *data() const {
            return m_data;
        }

        void clear() {
            m_headPtr = 0;
        }

        void serialize(StateBuffer &buf) const;
        void deserialize(StateBuffer &buf);

    private:
        uint32_t m_data[32];
        uint8_t m_headPtr;
};

enum class GPUCommandType
{
    None = -1,
    NOP,
    DrawPolygon,
    DrawLine,
    DrawRectangle,
    VramVramCopy,
    CpuVramCopy,
    VramCpuCopy,
    Env,
    // Misc commands
    ClearCache,
    QuickRectFill
};

enum class GPURectSize : uint8_t
{
    Variable = 0,
    Size_1x1,
    Size_8x8,
    Size_16x16
};

struct GPUCommandFlags
{
    bool shaded;
    bool textured;
    uint8_t nbVertices;
    bool semiTransparent;
    bool rawTexture;
    bool polyline;
    GPURectSize rectFlag;
    struct RectSize {
        uint8_t width;
        uint8_t height;
    } rectSize;
    struct DestCoords {
        uint16_t x;
        uint16_t y;
    } destCoords;
    struct SourceCoords {
        uint16_t x;
        uint16_t y;
    } sourceCoords;
};

class GPUCommand
{
    public:
        GPUCommand();
        ~GPUCommand();

        void set(uint32_t cmd);
        GPUCommandType type() const { return m_type; }
        void reset();

        void addParam(uint32_t param);
        const GPUParamArray &params() const { return m_params; }
        const GPUCommandFlags &flags() const { return m_flags; }
        int expectedParams() { return m_nbExpectedParams; }

        void serialize(StateBuffer &buf) const;
        void deserialize(StateBuffer &buf);

    private:
        void parseRawCommand(uint32_t cmd);
        void parseCommandType(uint32_t cmd);

    private:
        GPUCommandType m_type;
        GPUCommandFlags m_flags;
        GPUParamArray m_params;
        int m_nbExpectedParams;
};

#endif /* !GPUCOMMAND_HPP_ */
