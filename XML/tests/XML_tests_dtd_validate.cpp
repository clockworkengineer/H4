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
                "<!DOCTYPE note [\n"
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
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 9] <note> element does not conform to the content specication (to,from,heading,body).");
  }
  SECTION("XML with an missing <to> tag which voilates the DTD.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note [\n"
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
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 9] <note> element does not conform to the content specication (to,from,heading,body).");
  }
  SECTION("XML with an empty notes tag which voilates the DTD.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE notes [\n"
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
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 10] <notes> element does not conform to the content specication (note)+.");
  }
  SECTION("XML with an empty notes tag which is valid given DTD.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE notes [\n"
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
                "<!DOCTYPE notes [\n"
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
  SECTION("XML with a DTD that uses sequence operators and '+' (one or more times).", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE album [\n"
                "<!ELEMENT album ( title, ( songTitle, duration )+ )>\n"
                "<!ELEMENT title (#PCDATA)>\n"
                "<!ELEMENT songTitle (#PCDATA)>\n"
                "<!ELEMENT duration (#PCDATA)>\n"
                "]>\n"
                "<album>\n"
                "<title>XML Classical Hits</title>\n"
                "<songTitle>XML Overture</songTitle>\n"
                "<duration>10</duration>\n"
                "<songTitle>XML Symphony 1.0</songTitle>\n"
                "<duration>54</duration>\n"
                "</album>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
  }
  SECTION("XML with a DTD and XML that has out of sequence duration tag.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE album [\n"
                "<!ELEMENT album ( title, ( songTitle, duration )+ )>\n"
                "<!ELEMENT title (#PCDATA)>\n"
                "<!ELEMENT songTitle (#PCDATA)>\n"
                "<!ELEMENT duration (#PCDATA)>\n"
                "]>\n"
                "<album>\n"
                "<title>XML Classical Hits</title>\n"
                "<songTitle>XML Overture</songTitle>\n"
                "<duration>10</duration>\n"
                "<duration>54</duration>\n"
                "<songTitle>XML Symphony 1.0</songTitle>\n"
                "</album>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 8] <album> element does not conform to the content specication ( title, ( songTitle, duration )+ ).");
  }
  SECTION("XML with a DTD that uses '*' (zero or more times) operator and has XML that complies.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE library [\n"
                "<!ELEMENT library ( book* )>\n"
                "<!ELEMENT book (#PCDATA)>"
                "]>\n"
                "<library>\n"
                "<book>The Wealth of Nations</book>\n"
                "<book>The Iliad</book>\n"
                "<book>The Jungle</book>\n"
                "</library>\n";

    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
  }
  SECTION("XML with a DTD that uses '*' (zero or more times) operator and has XML that does not have a book element.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE library [\n"
                "<!ELEMENT library ( book* )>\n"
                "<!ELEMENT book (#PCDATA)>\n"
                "]>\n"
                "<library>\n"
                "</library>\n";

    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
  }

  //   SECTION("XML with a DTD that uses '?' (zero or onetimes) operator and has XML conatins occupied and empty seats.", "[XML][DTD][Validate]")
  // {
  //   xmlString = "<?xml version=\"1.0\"?>\n"
  //               "<!DOCTYPE plane [\n"
  //               "<!ELEMENT plane ( seats )>\n"
  //               "<!ELEMENT seats ( seat + )>\n"
  //               "<!ELEMENT seat ( passenger ? )>\n"
  //               "<!ELEMENT passenger (#PCDATA)>\n"
  //               "]>\n"
  //               "<plane>\n"
  //               "<seats>\n"
  //               // "<seat>\n"
  //               // "</seat>\n"
  //               "<seat>\n"
  //               "<passenger>John Smith</passenger>\n"
  //               "</seat>\n"
  //               "</seats>\n"
  //               "</plane>\n";

  //   BufferSource xmlSource(xmlString);
  //   XMLObject xmlObject = xml.parse(xmlSource);
  //   REQUIRE_NOTHROW(xml.validate(xmlObject));
  // }
}