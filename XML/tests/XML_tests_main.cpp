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
TEST_CASE("Use XML object to parse XML declaration", "[XML][Parse]")
{
  XML xml;
  SECTION("Parse basic XML declaration. ", "[XML]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\" ?>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
  }
  SECTION("Parse version 1.0 encoding == UTF-16 standalone == yes XML declaration. ", "[XML][Parse]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\" ?>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-16");
    REQUIRE(xNodeRoot.standalone == "yes");
  }
  SECTION("Parse version 1.1 encoding == UTF8 standalone == yes XML declaration. ", "[XML][Parse]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.1\" standalone = \"yes\" ?>");
    REQUIRE(xNodeRoot.version == "1.1");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "yes");
  }
  SECTION("Parse empty XML declaration. ", "[XML][Parse]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml ?>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
  }
  SECTION("Parse empty XML declaration no end tag ", "[XML][Parse]")
  {
    XNodeRoot xNodeRoot;
    REQUIRE_THROWS_AS(xml.parse("<?xml >"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml >"), "XML syntax error detected.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML][Parse]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\" standalone = \"no\" encoding = \"UTF-8\"?>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\" standalone = \"no\" encoding = \"UTF-8\"?>"), "XML syntax error detected.");
  }
}
TEST_CASE("Use XML object to parse declaration and root element", "[XML][Parse]")
{
  XML xml;
  SECTION("Empty root element ", "[XML][Parse]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?><contact-info> </contact-info>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.elements.size()==1);
  }
}