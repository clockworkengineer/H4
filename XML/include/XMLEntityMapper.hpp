#ifndef XMLENTITYMAPPER_HPP
#define XMLENTITYMAPPER_HPP
//
// XML Sources
//
#include "XMLSources.hpp"
// =========
// NAMESPACE
// =========
namespace H4
{
    //
    // XML Entity mapper
    //
    class XMLEntityMapper : public IXMLEntityMapper
    {
    public:
        XMLEntityMapper()
        {
            m_enityMappings["&amp;"].internal = "&#x26;";
            m_enityMappings["&quot;"].internal = "&#x22;";
            m_enityMappings["&apos;"].internal = "&#x27;";
            m_enityMappings["&lt;"].internal = "&#x3C;";
            m_enityMappings["&gt;"].internal = "&#x3E;";
        }
        void add(const std::string &entityName, XMLEntityMapping &entityMapping)
        {
            m_enityMappings[entityName] = entityMapping;
        }
        XMLEntityMapping &get(const std::string &entityName)
        {
            return (m_enityMappings[entityName]);
        }
        void remove(const std::string &entityName)
        {
            m_enityMappings.erase(entityName);
        }
        bool isPresent(const std::string &entityName)
        {
            return (m_enityMappings.count(entityName) != 0);
        }
        std::unordered_map<std::string, XMLEntityMapping> &getList()
        {
            return (m_enityMappings);
        }
        void map(XMLValue &entityReference)
        {
            if (isPresent(entityReference.unparsed))
            {
                if (!get(entityReference.unparsed).internal.empty())
                {
                    entityReference.parsed = get(entityReference.unparsed).internal;
                }
                else
                {
                    if (std::filesystem::exists(get(entityReference.unparsed).external.systemID))
                    {
                        FileSource entitySource(get(entityReference.unparsed).external.systemID);
                        entityReference.parsed = "";
                        while (entitySource.more())
                        {
                            entityReference.parsed += entitySource.current_to_bytes();
                            entitySource.next();
                        }
                    }
                    else
                    {
                        throw XMLSyntaxError("Entity '" + entityReference.unparsed + "' source file '" + get(entityReference.unparsed).external.systemID + "' does not exist.");
                    }
                }
            }
        }
        std::string translate(const std::string &toTranslate, char type = '%')
        {
            std::string translated = toTranslate;
            bool matchFound;
            do
            {
                matchFound = false;
                for (auto entity : getList())
                {
                    if (entity.first[0] == type)
                    {
                        size_t pos = translated.find(entity.first);
                        if (pos != std::string::npos)
                        {
                            translated.replace(pos, entity.first.length(), entity.second.internal);
                            matchFound = true;
                        }
                    }
                }
            } while (matchFound);
            return (translated);
        }

    private:
        std::unordered_map<std::string, XMLEntityMapping> m_enityMappings;
    };
} // namespace H4
#endif /* XMLENTITYMAPPER_HPP */
