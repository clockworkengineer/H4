#ifndef JSONSTINATIONS_HPP
#define JSONSTINATIONS_HPP
namespace H4
{
    //
    // Destination classes for stringification.
    //
    class BufferDestination : public JSON::IDestination
    {
    public:
        BufferDestination()
        {
        }
        void addBytes(std::string bytes)
        {
            for (auto b : bytes)
            {
                m_stringifyBuffer.push_back(b);
            }
        }
        std::string getBuffer()
        {
            return (m_stringifyBuffer);
        }
    private:
        std::string m_stringifyBuffer;
    };
    class FileDestination : public JSON::IDestination
    {
    public:
        FileDestination(std::string desinationFileName)
        {
            m_destination.open(desinationFileName.c_str(), std::ios_base::binary);
            if (!m_destination.is_open())
            {
                throw std::runtime_error("JSON file output stream failed to open or could not be created.");
            }
        }
        void addBytes(std::string bytes)
        {
            m_destination.write(bytes.c_str(), bytes.length());
            m_destination.flush();
        }
    private:
        std::ofstream m_destination;
    };
} // namespace H4
#endif /* JSONSTINATIONS_HPP */