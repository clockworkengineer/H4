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
TEST_CASE("Parse XML with Unicode character in element names, attributes, comments, character data, and processing instructions. ", "[XML][Parse][Unicode]")
{
  std::string xmlString;
  SECTION("Japanese characters", "[XML][Parse][[Unicode]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"utf-8\"?><config><start_text>転送</start_text></config>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Well-formed XML document including Chinese, Armenian and Cyrillic characters", "[XML][Parse][[Unicode]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<俄语 լեզու=\"ռուսերեն\">данные</俄语>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].name == "俄语");
    REQUIRE(xml.m_prolog[0].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0].getAttribute("լեզու").value.parsed == "ռուսերեն");
    REQUIRE(xml.m_prolog[0].getContents() == "данные");
  }
}