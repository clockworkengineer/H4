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
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML UTF-16 declaration.", "[XML][Stringify]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"no\"?>"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with minimal declaration and self closing root tag.", "[XML][Stringify]")
    {
        xmlString = "<?xml version = \"1.0\"?>"
                    "<AddressBook/>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?><AddressBook/>");
    }
    SECTION("Stringify XML with root and child elements plus contents.", "[XML][Stringify]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<AddressBook><Address>Address 1</Address><Address>Address 2</Address></AddressBook>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with attributes on root tag.", "[XML][Stringify]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<AddressBook number = \"46\" type = \"commercial\"></AddressBook>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with Root element <AddressBook> with multiple sibling <Address> elements and contents ", "[XML][Stringify]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<AddressBook>"
                    "<Address>"
                    "    This is some contents 1   "
                    "</Address>"
                    "<Address>"
                    "    This is some contents 2   "
                    "</Address>"
                    "<Address>"
                    "    This is some contents 3   "
                    "</Address>"
                    "</AddressBook>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with a single line comment in prolog area.", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<!-- A single line comment -->"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with multiple single line comments in the prolog area.", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<!-- A single line comment -->"
                    "<!-- A single line comment -->"
                    "<!-- A single line comment -->"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with multiple comments inside root element and between its children ", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<AddressBook>"
                    "<!--Address one -->"
                    "<Address>    This is some contents 1   </Address>"
                    "<!--Address two -->"
                    "<Address>    This is some contents 2   </Address>"
                    "<!--Address three -->"
                    "<Address>    This is some contents 3   </Address>"
                    "</AddressBook>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with multiple comments inside child of root element with contents ", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<AddressBook>\n"
                    "<Address>\n"
                    "<!--Address one -->\n"
                    "This is some contents 1\n"
                    "<!--Address two -->\n"
                    "This is some contents 2\n"
                    "<!--Address three -->\n"
                    "This is some contents 3\n"
                    "</Address>\n"
                    "</AddressBook>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with a multi line comment in prolog.", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<!-- A single line comment\n"
                    " another line\n"
                    " another line\n"
                    " and another line\n"
                    "--><root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with a multiline comment within child element with contents.", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                    "<root>Test <!-- A single line comment\n"
                    " another line\n"
                    " another line\n"
                    " and another line\n"
                    "--> Test Test</root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
}
