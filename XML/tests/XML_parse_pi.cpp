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
TEST_CASE("Check the parsing of XML containing program instructions", "[XML][Parse][PI]")
{
  std::string xmlString;
  SECTION("Parse XML containing PI after declaration", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse XML containing multiple PI after declaration", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?> "
                "<?display table-view?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse XML containing PI in root section", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<root><?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse XML containing PI after declaration and check values", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::pi);
    REQUIRE(XMLNodeRef<XMLNodePI>(*xml.m_prolog.children[1]).name == "xml-stylesheet");
    REQUIRE(XMLNodeRef<XMLNodePI>(*xml.m_prolog.children[1]).parameters == "href=\"tutorialspointstyle.css\" type=\"text/css\"");
  }
  SECTION("Parse XML containing PI in root section and check values", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<root><?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog[0].children[0]).getNodeType() == XMLNodeType::pi);
    REQUIRE(XMLNodeRef<XMLNodePI>(*xml.m_prolog[0].children[0]).name == "xml-stylesheet");
    REQUIRE(XMLNodeRef<XMLNodePI>(*xml.m_prolog[0].children[0]).parameters == "href=\"tutorialspointstyle.css\" type=\"text/css\"");
  }
}