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
TEST_CASE("Use XML object to parse XML declaration", "[XML]")
{
  XML xml;
  SECTION("Parse basic XML declaration. ", "[XML]")
  {
    XNodeRoot xNodeRoot = xml.parse(kXMLDeclaration1);
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
  }
  SECTION("Parse encoding == UTF-16 standalone == yes XML declaration. ", "[XML]")
  {
    XNodeRoot xNodeRoot = xml.parse(kXMLDeclaration2);
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-16");
    REQUIRE(xNodeRoot.standalone == "yes");
  }
}