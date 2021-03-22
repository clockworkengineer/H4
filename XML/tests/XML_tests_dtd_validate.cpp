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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "Undefined element <date> found.");
  }
  SECTION("XML with an missing <to> tag which voilates the DTD.", "[XML][DTD][Validate]")
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
                "<from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "Invalid <note> element found.");
  }
  SECTION("XML with an empty notes tag which voilates the DTD.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<!ELEMENT notes (note)+>\n"
                "<!ELEMENT note (to,from,heading,body)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<notes>\n"
                "</notes>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "Invalid <notes> element found.");
  }
  SECTION("XML with an empty notes tag which is valid given DTD.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE notes ["
                "<!ELEMENT notes (note)*>\n"
                "<!ELEMENT note (to,from,heading,body)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<notes>\n"
                "</notes>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
  }
  SECTION("XML with an empty notes tag which is valid given DTD (use (note*) instead of (note)* here).", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE notes ["
                "<!ELEMENT notes (note*)>\n"
                "<!ELEMENT note (to,from,heading,body)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<notes>\n"
                "</notes>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
  }
}