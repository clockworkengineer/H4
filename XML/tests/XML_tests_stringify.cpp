//
// Unit Tests: XML
//
// Description:
//
// ================
// Test definitions
// =================
#include "XML_tests.hpp"
// =======================
// JSON class namespace
// =======================
using namespace H4;
// ==========
// Test cases
// ==========
TEST_CASE("Use XML to stringify previously parsed XML.", "[XML][Stringify")
{
    XML xml;
    std::string xmlString;
    SECTION("Stringify XML UTF-8 declaration.", "[XML][Stringify]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot)+"<root></root>" == xmlString);
    }
    SECTION("Stringify XML UTF-16 declaration.", "[XML][Stringify]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"no\"?>"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot)+"<root></root>" == xmlString);
    }
}