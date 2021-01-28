//
// Unit Tests: JSON
//
// Description: ISource, IDestination and JNode index/reference
// unit tests for JSON class using the Catch2 test framework.
//
// ================
// Test definitions
// =================
#include "XML_tests.hpp"
// =======================
// JSON class namespace
// =======================
using namespace H4;
// ==========================
// Unit test helper functions
// ==========================
// ==========
// Test cases
// ==========
TEST_CASE("Use XML object to parse XML declaration", "[XML][Parse][Declaration]")
{
  XML xml;
  SECTION("Parse basic XML declaration. ", "[XML]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?> <root></root>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
  }
  SECTION("Parse version 1.0 encoding == UTF-16 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?> <root></root>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-16");
    REQUIRE(xNodeRoot.standalone == "yes");
  }
  SECTION("Parse version 1.1 encoding == UTF8 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.1\" standalone = \"yes\"?> <root></root>");
    REQUIRE(xNodeRoot.version == "1.1");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "yes");
  }
  SECTION("Parse empty XML declaration. ", "[XML][Parse][Declaration]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml?> <root></root>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
  }
  SECTION("Parse empty XML declaration no end tag ", "[XML][Parse][Declaration]")
  {
    XNodeRoot xNodeRoot;
    REQUIRE_THROWS_AS(xml.parse("<?xml >"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml >"), "XML syntax error detected.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML][Parse][Declaration]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\" standalone = \"no\" encoding = \"UTF-8\"?> <root></root>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\" standalone = \"no\" encoding = \"UTF-8\"?> <root></root>"), "XML syntax error detected.");
  }
}
TEST_CASE("Use XML object to parse declaration and root element", "[XML][Parse][Root]")
{
  XML xml;
  SECTION("Empty root element <contact-info> ", "[XML][Parse]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?> <contact-info> </contact-info>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.elements.size() == 0);
    REQUIRE(xNodeRoot.name == "contact-info");
  }
  SECTION("Empty root element <AddressBook> ", "[XML][Parse][Root]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?> <AddressBook> </AddressBook>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.elements.size() == 0);
    REQUIRE(xNodeRoot.name == "AddressBook");
  }
  SECTION("Root element <Address> and some contents ", "[XML][Parse][Root]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?> <AddressBook><Address>    This is some contents    </Address></AddressBook>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.name == "AddressBook");
    REQUIRE(xNodeRoot.elements.size() == 1);
    REQUIRE(xNodeRoot.elements[0].name == "Address");
    REQUIRE(xNodeRoot.elements[0].contents == "    This is some contents    ");
  }
  SECTION("Root element multiple sibling <Address> elements and contents ", "[XML][Parse][Root]")
  {
    XNodeRoot xNodeRoot = xml.parse(XString("<?xml version = \"1.0\"?><AddressBook><Address>    This is some contents 1   </Address>") +
                                    "<Address>    This is some contents 2   </Address>" +
                                    "<Address>    This is some contents 3   </Address></AddressBook>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.name == "AddressBook");
    REQUIRE(xNodeRoot.elements.size() == 3);
    REQUIRE(xNodeRoot.elements[0].name == "Address");
    REQUIRE(xNodeRoot.elements[0].contents == "    This is some contents 1   ");
    REQUIRE(xNodeRoot.elements[1].name == "Address");
    REQUIRE(xNodeRoot.elements[1].contents == "    This is some contents 2   ");
    REQUIRE(xNodeRoot.elements[2].name == "Address");
    REQUIRE(xNodeRoot.elements[2].contents == "    This is some contents 3   ");
  }
}