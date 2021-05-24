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
TEST_CASE("Use XML object to parse XML declaration", "[XML][Parse][Declaration]")
{
  std::string xmlString;
  SECTION("Parse XML declaration. ", "[XML]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog.getAttribute("version").value.parsed == "1.0");
    REQUIRE(xml.m_prolog.getAttribute("encoding").value.parsed == "UTF-8");
    REQUIRE(xml.m_prolog.getAttribute("standalone").value.parsed == "no");
  }
  SECTION("Parse XML declaration with unsupported version. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.2\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 1] Unsupported version number 1.2.");
  }
  SECTION("Parse XML declaration with unsupported encoding. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-32\" standalone=\"no\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 1] Unsupported encoding UTF-32 specified.");
  }
  SECTION("Parse XML declaration with invalid standalone value. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"maybe\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 1] Invalid standalone value of 'maybe'.");
  }
  SECTION("Parse XML declaration with extra content after root element. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root></root>extra content.\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 18] Extra content at the end of document.");
  }
  SECTION("Parse version 1.0, encoding == UTF-8, standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?> "
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog.getAttribute("version").value.parsed == "1.0");
    REQUIRE(xml.m_prolog.getAttribute("encoding").value.parsed == "UTF-16");
    REQUIRE(xml.m_prolog.getAttribute("standalone").value.parsed == "yes");
  }
  SECTION("Parse version 1.0, standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" standalone=\"yes\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog.getAttribute("version").value.parsed == "1.0");
    REQUIRE(xml.m_prolog.getAttribute("encoding").value.parsed == "UTF-8");
    REQUIRE(xml.m_prolog.getAttribute("standalone").value.parsed == "yes");
  }
  SECTION("Check declaration contains at least version attribute.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml?> <root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 8] Version missing from declaration.");
  }
  SECTION("Parse empty XML declaration no end tag ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\">\n"
                "<root></root>";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 20] Declaration end tag not found.");
  }
  SECTION("Parse empty XML declaration no root element.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 2] Missing root element.");
  }
  SECTION("Parse empty XML declaration with content before root element.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "content<root></root>";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 2] Content detected before root element.");
  }
  SECTION("Parse empty XML declaration no closing root tag ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 3 Column: 2] Missing closing tag.");
  }
  SECTION("Parse empty XML declaration no closing child tag ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<root><child></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 25] Missing closing tag.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" standalone=\"no\" encoding=\"UTF-8\"?>\n"
                " <root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 1] Incorrect order for version, encoding and standalone attributes.");
  }
  SECTION("Parse XML with declaration but no root element", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 2] Missing root element.");
  }
}
