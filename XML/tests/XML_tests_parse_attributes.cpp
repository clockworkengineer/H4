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
TEST_CASE("Parse XML elements with attached attributes", "[XML][Parse][Attributes]")
{
  std::string xmlString;
  SECTION("Root attribute with one attached attribute number", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook number='15'>\n"
                "</AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog.getAttribute("version").value.parsed == "1.0");
    REQUIRE(xml.m_prolog.getAttribute("encoding").value.parsed == "UTF-8");
    REQUIRE(xml.m_prolog.getAttribute("standalone").value.parsed == "no");
    REQUIRE(xml.m_prolog.children.size() == 3);
    REQUIRE(xml.m_prolog[0].name == "AddressBook");
    REQUIRE(xml.m_prolog[0].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0].getAttribute("number").value.parsed == "15");
  }
  SECTION("Root attribute with 3 attached attributes number, away, flat", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                " <AddressBook number='15' away=\"yes\" flat='no'>\n"
                " </AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog.getAttribute("version").value.parsed == "1.0");
    REQUIRE(xml.m_prolog.getAttribute("encoding").value.parsed == "UTF-8");
    REQUIRE(xml.m_prolog.getAttribute("standalone").value.parsed == "no");
    REQUIRE(xml.m_prolog.children.size() == 3);
    REQUIRE(xml.m_prolog[0].name == "AddressBook");
    REQUIRE(xml.m_prolog[0].getAttributeList().size() == 3);
    REQUIRE(xml.m_prolog[0].getAttribute("number").value.parsed == "15");
    REQUIRE(xml.m_prolog[0].getAttribute("away").value.parsed == "yes");
    REQUIRE(xml.m_prolog[0].getAttribute("flat").value.parsed == "no");
  }
  SECTION("Empty elements with attributes are allowed.", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook number='15'>\n"
                "</AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Element with duplicate attributes not allowed.", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook number='15' colour='red' number='16'>\n"
                " </AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 54] Attribute defined more than once within start tag.");
  }
}