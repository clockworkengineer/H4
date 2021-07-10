#ifndef DTDELEMENT_HPP
#define DTDELEMENT_HPP
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
        // XML DTD attribute types. Note only one type bit and
        // value bit should be set for each attribute and that
        // some combinations like id and fixed are illegal and
        // are reported as so by the parser.
        //
        enum DTDAttributeType : uint16_t
        {
            // Types bits 0 - 9
            cdata = (0x1 << 0),
            enumeration = (0x1 << 1),
            id = (0x1 << 2),
            idref = (0x1 << 3),
            idrefs = (0x1 << 4),
            nmtoken = (0x1 << 5),
            nmtokens = (0x1 << 6),
            entity = (0x1 << 7),
            entities = (0x1 << 8),
            notation = (0x1 << 9),
            // Value bits 10 - 13
            normal = (0x1 << 10),
            required = (0x1 << 11),
            implied = (0x1 << 12),
            fixed = (0x1 << 13)
        };
        //
        // XML DTD attribute definition
        //
        struct DTDAttribute
        {
            std::string name;
            uint16_t type;
            std::string enumeration;
            XMLValue value;
        };
        //
        // XML DTD element definition
        //
        struct DTDElement
        {
            DTDElement() {}
            DTDElement(const std::string &name, const XMLValue &content) : name(name), content(content) {}
            std::string name;
            bool idAttributePresent = false;
            XMLValue content;
            std::vector<DTDAttribute> attributes;
        };
} // namespace H4
#endif /* DTDELEMENT_HPP */
