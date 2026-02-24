/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** StateBuffer
*/

#ifndef STATEBUFFER_HPP_
#define STATEBUFFER_HPP_

#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <stdexcept>

class StateBuffer
{
    public:
        StateBuffer() : m_cursor(0) {}
        ~StateBuffer() = default;

        void write(const void *data, size_t size)
        {
            const uint8_t *bytes = reinterpret_cast<const uint8_t *>(data);
            m_data.insert(m_data.end(), bytes, bytes + size);
        }

        void read(void *data, size_t size)
        {
            if (m_cursor + size > m_data.size())
                throw std::runtime_error("StateBuffer: read past end of buffer");
            std::memcpy(data, m_data.data() + m_cursor, size);
            m_cursor += size;
        }

        template<typename T>
        void write(const T &val)
        {
            write(&val, sizeof(T));
        }

        template<typename T>
        void read(T &val)
        {
            read(&val, sizeof(T));
        }

        void writeVec(const std::vector<uint8_t> &vec)
        {
            uint32_t size = static_cast<uint32_t>(vec.size());
            write(size);
            if (size > 0)
                write(vec.data(), size);
        }

        void readVec(std::vector<uint8_t> &vec)
        {
            uint32_t size = 0;
            read(size);
            vec.resize(size);
            if (size > 0)
                read(vec.data(), size);
        }

        void writeString(const std::string &str)
        {
            uint32_t size = static_cast<uint32_t>(str.size());
            write(size);
            if (size > 0)
                write(str.data(), size);
        }

        void readString(std::string &str)
        {
            uint32_t size = 0;
            read(size);
            str.resize(size);
            if (size > 0)
                read(&str[0], size);
        }

        const std::vector<uint8_t> &data() const { return m_data; }
        size_t size() const { return m_data.size(); }

        void setData(std::vector<uint8_t> &&data)
        {
            m_data = std::move(data);
            m_cursor = 0;
        }

        void resetCursor() { m_cursor = 0; }

    private:
        std::vector<uint8_t> m_data;
        size_t m_cursor;
};

#endif /* !STATEBUFFER_HPP_ */
