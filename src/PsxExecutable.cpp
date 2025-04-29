#include "PsxExecutable.hpp"

#include <fstream>

constexpr size_t exeHeaderSize = 2048;

PsxExecutable::PsxExecutable(const std::string &exeFilePath) :
    path(exeFilePath),
    initialPc(0),
    initialGp(0),
    ramDestination(0),
    exeSize(0),
    initialSpBase(0),
    initialSpOffset(0)
{
}

static uint32_t fromLeBytes(uint8_t data[4])
{
    uint32_t b1 = data[0];
    uint32_t b2 = data[1] << 8;
    uint32_t b3 = data[2] << 16;
    uint32_t b4 = data[3] << 24;
    return b4 | b3 | b2 | b1;
}

int PsxExecutable::readExeHeader(std::ifstream &exeFile)
{
    uint8_t buf[exeHeaderSize] {0};

    exeFile.read(reinterpret_cast<char *>(&buf[0]), exeHeaderSize);
    if (exeFile.gcount() != exeHeaderSize) {
        return -1;
    }
    initialPc = fromLeBytes(&buf[0x10]);
    initialGp = fromLeBytes(&buf[0x14]);
    ramDestination = fromLeBytes(&buf[0x18]);
    exeSize = fromLeBytes(&buf[0x1C]);
    initialSpBase = fromLeBytes(&buf[0x30]);
    initialSpOffset = fromLeBytes(&buf[0x34]);
    return 0;
}

bool PsxExecutable::load()
{
    std::ifstream exeFile(path, std::ios::in | std::ios::binary);

    if (!exeFile) {
        return false;
    }
    if (readExeHeader(exeFile) == -1) {
        exeFile.close();
        return false;
    }
    exeData.resize(exeSize, 0);
    exeFile.read(reinterpret_cast<char *>(&exeData[0]), exeSize);

    bool ok = exeFile.gcount() == exeSize;
    exeFile.close();
    return ok;
}
