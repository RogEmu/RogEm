/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** GTE
*/

#ifndef GTE_H_
#define GTE_H_

#include <cstdint>
#include <array>
#include "Instruction.h"
#include "Coprocessor.h"

template <typename size>
struct  Vector3{
	size x;
  	size y;
  	size z;
};

enum class interpolation
{
    DCPL,
    INTPL,
    DPC
};

struct Flag
{
	bool sf;
	uint8_t mx;
	uint8_t v;
	uint8_t cv;
	bool lm;
};

struct Rgbc
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t c;
};

struct Rgbc {
    uint8_t r, g, b, c;
    Rgbc(uint8_t _r = 0, uint8_t _g = 0, uint8_t _b = 0, uint8_t _c = 0)
        : r(_r), g(_g), b(_b), c(_c) {}
};


struct Mat3x3
{
	int16_t r11;
	int16_t r12;
	int16_t r13;
	int16_t r21;
	int16_t r22;
	int16_t r23;
	int16_t r31;
	int16_t r32;
	int16_t r33;
};

/**
 * @class GTE
 * @brief Geometry Transformation Engine (Coprocessor 2) emulation for the PlayStation.
 *
 * The GTE is a fixed-point math coprocessor designed for 3D transformation and lighting
 * operations. This class provides emulated support for key instructions such as RTPS and RTPT.
 */
class GTE : public Coprocessor {
    public:
        GTE();

        void reset();

        /**
         * @brief Executes a GTE instruction.
         * @param opcode 32-bit encoded instruction.
         */
        void execute(uint32_t opcode) override;

        /**
         * @brief Move to Coprocessor Data Register (MTC2).
         * @param reg Register index.
         * @param value Value to store.
         */
        void mtc(uint8_t reg, uint32_t value) override;

        /**
         * @brief Move to Coprocessor Control Register (CTC2).
         * @param reg Register index.
         * @param value Value to store.
         */
        void ctc(uint8_t reg, uint32_t value) override;

        /**
         * @brief Move from Coprocessor Data Register (MFC2).
         * @param reg Register index.
         * @return Value stored in the register.
         */
        uint32_t mfc(uint8_t reg) override;

        /**
         * @brief Move from Coprocessor Control Register (CFC2).
         * @param reg Register index.
         * @return Value stored in the register.
         */
        uint32_t cfc(uint8_t reg) override;

        /**
         * @brief Clamps a value and sets appropriate GTE flag bits if limits are exceeded.
         * @param value The value to clamp.
         * @param limitHigh Upper bound.
         * @param limitLow Lower bound.
         * @param flagBitHigh Bit to set in the FLAG register if out of range and positive.
         * @param flagBitLow Bit to set in the FLAG register if out of range and negative.
         * @return Clamped value.
         */
        template <typename T>
        T clampMAC(int64_t value, T limitHigh, T limitLow, uint32_t flagBitHigh, uint32_t flagBitLow) {
            if (value > limitHigh) {
                m_ctrlReg[31] |= flagBitHigh;
                return limitHigh;
            }
            if (value < limitLow) {
                m_ctrlReg[31] |= flagBitLow;
                return limitLow;
            }
            return static_cast<T>(value);
        }

        static constexpr int32_t IR_LIMIT_LOW = -0x8000;
        static constexpr int32_t IR_LIMIT_HIGH = 0x7FFF;
        static constexpr int32_t IR_LIMIT_MODE = 0;
        static constexpr uint8_t FIFO_LIMIT_LOW = 0x00;
        static constexpr uint8_t FIFO_LIMIT_HIGH = 0xFF;
        static constexpr int64_t MAC_LIMIT = (1LL << 43);

        /**
         * @brief check if mac is overflowing and set flag accordingly
         * @param macIndex mac's index
         * @param value mac value to be tested
         */
        void checkMACOverflow(int macIndex, int64_t value);

        int32_t getDataReg(int index) const { return m_dataReg.at(index); }
        void setDataReg(int index, int32_t value) { m_dataReg.at(index) = value; }

    private:
        // Coprocessor 2 Control Registers (Cop2C)
        std::array<int32_t, 32> m_ctrlReg{};

        // Coprocessor 2 Data Registers (Cop2D)
        std::array<int32_t, 32> m_dataReg{};

        Rgbc m_colorFIFO[3]; // RGB0 = [0], RGB1 = [1], RGB2 = [2]

        void pushColorFIFO(const Rgbc& color);
        Rgbc getRGB0() const;
        /**
         * @brief Decodes and dispatches the GTE instruction to the appropriate execution method.
         * @param opcode Encoded instruction.
         */
        void decodeAndExecute(uint32_t opcode);

        /*
        * @brief Executes the RTPS instruction (Rotate, Translate, Perspective Single).
        void executeRTPS();

         * @brief Executes the RTPT instruction (Rotate, Translate, Perspective Triple).
         void executeRTPT();
         */

        /**
         * @brief Execute the NCLIP instruction (normal clipping)
         */
        void executeNCLIP();
        void executeSQR(bool sf);
        void executeOP(bool sf);

        /**
         * @brief Third party function to simplify AVSZ3 and AVSZ4. Computes the average
         *        of SZ depths and stores it in OTZ (Ordering Table Z).
         *
         * @param szCount Number of SZ values to include (3 for AVSZ3, 4 for AVSZ4).
         * @param zsfRegister Index of the control register containing the ZSF factor.
         */
        void executeAVSZ(int szCount, int zsfRegister);

        // General Purpose Commands
        /**
         * @brief Executes MVMVA (Multiply vector by Mat3x3 and vector addition) instruction.
         * @param opcode flag used in the instuction are extracted from the opcode
         */
        void executeMVMVA(uint32_t opcode);

        /**
         * @brief Executes AVSZ3 (Average of Z values) instruction.
         */
        void executeAVSZ3();

        /**
         * @brief Executes AVSZ4 (Average of Z values) instruction.
         */
        void executeAVSZ4();

        /**
         * @brief Execute GPF (General purpose Interpolation)
         * @param opcode opcode to extract flag from
         */
        void executeGPF(uint32_t opcode);

        /**
         * @brief Execute GPL (General Interpolation with base)
         * @param opcode opcode to extract flag from
         * @param MAC is a base or not
         */
        void executeGPL(uint32_t opcode, bool base);

        void executeDCPL(uint32_t opcode);
        void executeDPCS(uint32_t opcode);
        void executeDPCT(uint32_t opcode);
        void executeINTPL(uint32_t opcode, Vector3<int32_t> macs);

        // Internal helper functions
        /**
         * @brief Extracts a x, y and z value from vector
         * @param base vector to extract from
		 * @param vector vector to extract to
         */
		void extractVector3(int32_t base, Vector3<int16_t> &vector);

        /**
		 * @brief Extracts translation vector
		 * @param base traslation vector to extract from
		 * @return translation
         */
        Vector3<int32_t> extractTranslation(int32_t base);

        /**
         * @brief Extracts Mat3x3
         * @param base Mat3x3 to extract from
         * @param Mat3x3 Mat3x3 to extract to
         */
        void extractMat3x3(int32_t base, Mat3x3 &Mat3x3);

        /**
         * @brief Extracts a signed 16-bit integer from a 32-bit word.
         * @param value Source word.
         * @param upper Whether to extract the upper 16 bits (true) or lower (false).
         * @return Extracted signed value.
         */

        /**
         * @brief get all the Flags from the op code
         * @param opcode opcode to get flages from
         * @return flag struct with the extracted flags
         */
		Flag getFlags(uint32_t opcode);

        /**
         * @brief get Mat3x3 depending of MX flag
         * @param mx mx flag
         * @return Mat3x3 struct with the extracted Mat3x3
         */
		Mat3x3 getMat3x3FromMX(uint8_t mx);

        /**
         * @brief get Vector depending of V flag
         * @param v v flag
         * @return Vector3 struct with the extracted Mat3x3
         */
		Vector3<int16_t> getVector3FromV(uint8_t v);

        /**
         * @brief Extracts a 16-bit signed integer from a 32-bit packed register.
         * @param value The packed 32-bit value.
         * @param upper True to extract the high 16 bits, false for low 16 bits.
         * @return int16_t The extracted signed value.
         */
        int16_t extractSigned16(uint32_t value, bool upper);

        /**
         * @brief Sets the overflow/underflow bit in the FLAG register if needed.
         * @param value The value to evaluate.
         * @param limitHigh Upper bound.
         * @param limitLow Lower bound.
         * @param flagBit Bit to set if out of range.
         */
        void updateFlags(int32_t value, int limitHigh, int limitLow, uint32_t flagBit);
};

#endif /* !GTE_H_ */
