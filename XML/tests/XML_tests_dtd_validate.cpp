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
TEST_CASE("Parse XML with various DTD validation issues.", "[XML][DTD][Validate]")
{
  XML xml;
  std::string xmlString;
  SECTION("XML with an unedfined element <date> which voilates the DTD.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<!ELEMENT note (to,from,heading,body)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<date>28/12/2019</date>\n"
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error [Line: 11 Column: 1]Undefined element 'date' found.");
  }

}