//
// Unit Tests: XML
//
// Description: Unit tests for DTD parsing.
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
TEST_CASE("Parse XML with DTD both internal/external", "[XML][Parse][DTD]")
{
  std::string xmlString;
  SECTION("XML with internal DTD", "[XML][Parse][DTD]")
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
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("XML with external (SYSTEM) DTD", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note SYSTEM \"./testData/note.dtd\">\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend!</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("XML with external (PUBLIC) DTD", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend!</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }

  SECTION("XML with external DTD with !NOTATION to parse and check values.", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT SYSTEM \"./testData/report02.dtd\">"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[0]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.m_notations["GIF"].type == "SYSTEM");
    REQUIRE(xml.m_dtd.m_notations["GIF"].systemID == "GIF");
    REQUIRE(xml.m_dtd.m_notations["JPG"].type == "SYSTEM");
    REQUIRE(xml.m_dtd.m_notations["JPG"].systemID == "JPG");
    REQUIRE(xml.m_dtd.m_notations["BMP"].type == "SYSTEM");
    REQUIRE(xml.m_dtd.m_notations["BMP"].systemID == "BMP");
  }
  SECTION("XML with internal DTD containing comments.", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<!-- root element is note that contains to, from, heading and body elements -->\n"
                "<!ELEMENT note (to,from,heading,body)>\n"
                "<!-- Note to field -->\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!-- Note from field -->\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!-- Note heading field -->\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!-- Note boy field -->\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
}
TEST_CASE("Parse XML DTD and check values.", "[XML][Parse][DTD]")
{
  std::string xmlString;
  SECTION("XML with internal to parse DTD and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n"
                "<!DOCTYPE address [\n"
                "<!ELEMENT address (name,company,phone)>\n"
                "<!ELEMENT name (#PCDATA)><!ELEMENT company (#PCDATA)>\n"
                "<!ELEMENT phone (#PCDATA)><!ELEMENT br EMPTY>\n"
                "<!ELEMENT footer ANY>]>\n"
                "<address>\n"
                "<name>Tanmay Patil</name>\n"
                "<company>TutorialsPoint</company>\n"
                "<phone>(011) 123-4567"
                "</phone>\n"
                "</address>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.m_name == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_dtd.m_elements["address"].name == "address");
    REQUIRE(xml.m_dtd.m_elements["address"].content.unparsed == "(name,company,phone)");
    REQUIRE(xml.m_dtd.m_elements["name"].name == "name");
    REQUIRE(xml.m_dtd.m_elements["name"].content.unparsed == "(#PCDATA)");
    REQUIRE(xml.m_dtd.m_elements["company"].name == "company");
    REQUIRE(xml.m_dtd.m_elements["company"].content.unparsed == "(#PCDATA)");
    REQUIRE(xml.m_dtd.m_elements["phone"].name == "phone");
    REQUIRE(xml.m_dtd.m_elements["phone"].content.unparsed == "(#PCDATA)");
    REQUIRE(xml.m_dtd.m_elements["br"].name == "br");
    REQUIRE(xml.m_dtd.m_elements["br"].content.unparsed == "EMPTY");
    REQUIRE(xml.m_dtd.m_elements["footer"].name == "footer");
    REQUIRE(xml.m_dtd.m_elements["footer"].content.unparsed == "ANY");
  }
  SECTION("XML with external file DTD and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note SYSTEM \"./testData/note.dtd\">\n"
                "<note>\n"
                "<to>Tove"
                "</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.m_name == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_dtd.m_external.type == "SYSTEM");
    REQUIRE(xml.m_dtd.m_external.systemID == "./testData/note.dtd");
    REQUIRE(xml.m_dtd.m_elements["note"].name == "note");
    REQUIRE(xml.m_dtd.m_elements["note"].content.unparsed == "(to,from,heading,body)");
  }
  SECTION("XML with external URL DTD to parse and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
                " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                "<html></html>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.m_name == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_dtd.m_external.type == "PUBLIC");
    REQUIRE(xml.m_dtd.m_external.systemID == "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
    REQUIRE(xml.m_dtd.m_external.publicID == "-//W3C//DTD XHTML 1.0 Transitional//EN");
  }
}
TEST_CASE("Parse XML DTD with various element content specification errors.", "[XML][Parse][DTD][Error]")
{
  std::string xmlString;
  SECTION("XML with a DTD that contains an illegal mixed content specification (#PCDATA doesnt come first).", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!-- Fig. B.5 : mixed.xml-->\n"
                "<!-- Mixed content type elements -->\n"
                "<!DOCTYPE format [\n"
                "<!ELEMENT format ( bold | italic | #PCDATA)*>\n"
                "<!ELEMENT bold ( #PCDATA )>\n"
                "<!ELEMENT italic ( #PCDATA )>\n"
                "]>\n"
                "<format>\n"
                "Book catalog entry:\n"
                "<bold>XML</bold>\n"
                "<italic>XML How to Program</italic>\n"
                "This book carefully explains XML-based systems development."
                "</format>";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Invalid content region specification for element <format>.");
  }
  SECTION("XML with a DTD that contains an illegal mixed content specification (does not end with '*').", "[XML][Parse][DTD][Error]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!-- Fig. B.5 : mixed.xml-->\n"
                "<!-- Mixed content type elements -->\n"
                "<!DOCTYPE format [\n"
                "<!ELEMENT format ( #PCDATA | bold | italic )+>\n"
                "<!ELEMENT bold ( #PCDATA )>\n"
                "<!ELEMENT italic ( #PCDATA )>\n"
                "]>\n"
                "<format>\n"
                "Book catalog entry:\n"
                "<bold>XML</bold>\n"
                "<italic>XML How to Program</italic>\n"
                "This book carefully explains XML-based systems development."
                "</format>";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Invalid content region specification for element <format>.");
  }
  SECTION("Parse XML with DTD that cotains a content specification in error (missing ',').", "[XML][Parse][DTD][Error]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<!ELEMENT note (to,from,heading body)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Invalid content region specification for element <note>.");
  }
  SECTION("Parse XML with DTD that cotains a content specification in error (missing element name).", "[XML][Parse][DTD][Error]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<!ELEMENT note (to,from,heading,)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Invalid content region specification for element <note>.");
  }
  SECTION("XML with a DTD that contains an illegal mixed content specification (uses ',').", "[XML][Parse][DTD][Error]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!-- Fig. B.5 : mixed.xml-->\n"
                "<!-- Mixed content type elements -->\n"
                "<!DOCTYPE format [\n"
                "<!ELEMENT format ( #PCDATA | bold , italic )*>\n"
                "<!ELEMENT bold ( #PCDATA )>\n"
                "<!ELEMENT italic ( #PCDATA )>\n"
                "]>\n"
                "<format>\n"
                "Book catalog entry:\n"
                "<bold>XML</bold>\n"
                "<italic>XML How to Program</italic>\n"
                "This book carefully explains XML-based systems development."
                "</format>";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Invalid content region specification for element <format>.");
  }
}