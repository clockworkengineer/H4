#ifndef BENCODING_HPP
#define BENCODING_HPP
//
// C++ STL
//
#include <string>
#include <vector>
// =========
// NAMESPACE
// =========
namespace H4
{
    //
    // Source/destination for Bencoded data.
    //
    struct Bencoding
    {
        Bencoding() {}
        Bencoding(const char *bEncodedData)
        {
            while (*bEncodedData != '\0')
            {
                m_Buffer.push_back((std::byte)*bEncodedData);
                bEncodedData++;
            }
        }
        Bencoding(const std::string &bEncodedData)
        {
            for (auto byte : bEncodedData)
            {
                m_Buffer.push_back((std::byte)byte);
            }
        }
        bool operator==(const Bencoding &rhs) const
        {
            return (std::equal(m_Buffer.begin(), m_Buffer.end(), rhs.m_Buffer.begin()));
        }
        std::byte &operator[](int index)
        {
            return m_Buffer[index];
        }
        bool isEmpty() const
        {
            return (m_Buffer.empty());
        }
        std::size_t size() const
        {
            return (m_Buffer.size());
        }
        void add(std::byte byte)
        {
            m_Buffer.push_back(byte);
        }

    private:
        std::vector<std::byte> m_Buffer;
    };
} // namespace H4
#endif /* BENCODING_HPP */