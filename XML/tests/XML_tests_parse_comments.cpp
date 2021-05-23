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
TEST_CASE("Parse XML elements with comments", "[XML][Parse][Comments]")
{
  std::string xmlString;
  SECTION("A simple single line comment", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment -->\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Multiple single line comments beifre root tag", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment -->\n"
                " <!-- A single line comment -->\n"
                " <!-- A single line comment -->\n"
                " <root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Multiple comments inside root element and between its children ", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook>\n"
                "<!--Address one -->\n"
                "<Address>    This is some contents 1   </Address>\n"
                "<!--Address two -->\n"
                "<Address>    This is some contents 2   </Address>\n"
                "<!--Address three -->\n"
                "<Address>    This is some contents 3   </Address>\n"
                "</AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog.getAttribute("version").value.parsed == "1.0");
    REQUIRE(xml.m_prolog.getAttribute("encoding").value.parsed == "UTF-8");
    REQUIRE(xml.m_prolog.getAttribute("standalone").value.parsed == "no");
    REQUIRE(xml.m_prolog[0].name == "AddressBook");
    REQUIRE(xml.m_prolog[0].children.size() == 13);
    REQUIRE(XMLNodeRef<XMLNodeComment>(*xml.m_prolog[0].children[1]).comment == "Address one ");
    REQUIRE(xml.m_prolog[0][0].name == "Address");
    REQUIRE(xml.m_prolog[0][0].getContents() == "    This is some contents 1   ");
    REQUIRE(XMLNodeRef<XMLNodeComment>(*xml.m_prolog[0].children[5]).comment == "Address two ");
    REQUIRE(xml.m_prolog[0][1].name == "Address");
    REQUIRE(xml.m_prolog[0][1].getContents() == "    This is some contents 2   ");
    REQUIRE(XMLNodeRef<XMLNodeComment>(*xml.m_prolog[0].children[9]).comment == "Address three ");
    REQUIRE(xml.m_prolog[0][2].name == "Address");
    REQUIRE(xml.m_prolog[0][2].getContents() == "    This is some contents 3   ");
  }
  SECTION("A single comment after root element", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root></root>\n"
                "<!-- A single line comment --> ";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("A simple multi line comment", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment\n"
                " another line\n"
                " another line\n"
                " and another line\n"
                "--> <root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("A simple comment within element content", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root>Test<!-- a simple comment -->Test"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].getContents() == "TestTest");
  }
  SECTION("A simple comment within element contents and content remains intact", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root>Test  <!-- a simple comment -->  Test"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].getContents() == "Test    Test");
  }
  SECTION("A simple single line comment containing -- is illegal", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment-- --> "
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 30] Missing closing '>' for comment line.");
  }
  SECTION("A simple single line comment ending with -- is illegal", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment ---> "
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 31] Missing closing '>' for comment line.");
  }
}