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
TEST_CASE("Parse CDATA SECTION", "[XML][Parse][CDATA]")
{
  std::string xmlString;
  SECTION("Parse XML root containing CDDATA containing a XML tags", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                " <root>\n"
                "   <![CDATA[<message> Welcome to TutorialsPoint </message>   ]]>   "
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse XML root containing CDDATA containing a XML tags and check contents", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                " <root>\n"
                "   <![CDATA[<message> Welcome to TutorialsPoint </message>]]>   "
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].getContents() == "\n   <message> Welcome to TutorialsPoint </message>   ");
    REQUIRE(XMLNodeRef<XMLNodeCDATA>(*xml.m_prolog[0].children[1]).cdata == "<message> Welcome to TutorialsPoint </message>");
  }
  SECTION("Parse XML root containing CDDATA containing nested CDATA ", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                " <root>\n"
                "   <![CDATA[< Test test <![CDATA[ Test text ]]> ]]>\n"
                "   </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 3 Column: 40] Nesting of CDATA sections is not allowed.");
  }
  SECTION("Parse XML root containing CDDATA containing ]]> ", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                " <root>\n"
                "   <![CDATA[< Test Test text ]]>  ]]>\n"
                "   </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 4 Column: 1] ']]>' invalid in element content area.");
  }
}