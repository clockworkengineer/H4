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
TEST_CASE("Parse XML with internal DTD that contains entity definitions and uses", "[XML][DTD][Parse][Entity]")
{
  std::string xmlString;
  SECTION("XML DTD with entity definitions and uses", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note [\n"
                "<!ENTITY nbsp \"&#xA0;\">\n"
                "<!ENTITY writer \"Writer: Donald Duck.\">\n"
                "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("XML DTD with entity internal definitions and uses. Check translation of entity values", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note [\n"
                "<!ENTITY nbsp \"&#xA0;\">\n"
                "<!ENTITY writer \"Writer: Donald Duck.\">\n"
                "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::internal);
    REQUIRE(XMLNodeRef<XMLNode>(xml.m_prolog[0]).getNodeType() == XMLNodeType::root);
    REQUIRE(xml.m_prolog[0][4].name == "footer");
    REQUIRE(xml.m_prolog[0][4].getContents() == "Writer: Donald Duck.\u00A0Copyright: W3Schools.");
  }
  SECTION("XML DTD with entity and how it deals with entity character expansion case 1)", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note [\n"
                "<!ENTITY example \"<p>An ampersand (&#38;#38;) may be escaped numerically (&#38;#38;#38;) or with a general entity (&amp;amp;).</p>\">]>\n"
                "<note>&example;</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::internal);
    REQUIRE(xml.m_dtd.getEntity("&example;").internal == "<p>An ampersand (&#38;) may be escaped numerically (&#38;#38;) or with a general entity (&amp;amp;).</p>");
    REQUIRE(xml.m_prolog[0][0].name == "p");
    REQUIRE(xml.m_prolog[0][0].getContents() == "An ampersand (&) may be escaped numerically (&#38;) or with a general entity (&amp;).");
  }
  SECTION("XML DTD with entity and how it deals with entity character expansion case 2)", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE test [\n"
                "<!ELEMENT test (#PCDATA) >\n"
                "<!ENTITY % xx '&#37;zz;'>\n"
                "<!ENTITY % zz '&#60;!ENTITY tricky \"error-prone\" >' >\n"
                "%xx;\n"
                "]>\n"
                "<test>This sample shows a &tricky; method.</test>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::internal);
    REQUIRE(xml.m_dtd.getEntity("%xx;").internal == "%zz;");
    REQUIRE(xml.m_dtd.getEntity("%zz;").internal == "<!ENTITY tricky \"error-prone\" >");
    REQUIRE(xml.m_prolog[0].name == "test");
    REQUIRE(xml.m_prolog[0].getContents() == "This sample shows a error-prone method.");
  }
  SECTION("XML DTD with entity and how it deals with entity character expansion case 3)", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ENTITY x \"&lt;\">\n"
                "]>\n"
                "<foo attr=\"&x;\"/>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::internal);
    REQUIRE(xml.m_dtd.getEntity("&x;").internal == "&lt;");
    REQUIRE(XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).getAttributeList().size() == 1);
    XMLAttribute attribute = xml.m_prolog[0].getAttribute("attr");
    REQUIRE(attribute.name == "attr");
    REQUIRE(attribute.value.unparsed == "&x;");
    REQUIRE(attribute.value.parsed == "&lt;");
  }
  // This should throw an error as & ' " < >  not allowed to be assigned to attribute directly (NEED TO FIX)
  SECTION("XML DTD with entity and how it deals with entity character expansion case 4)", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ENTITY x \"&#60;\">\n"
                "]>\n"
                "<foo attr=\"&x;\"/>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 5 Column: 21] Attribute value contains invalid character '<', '\"', ''' or '&'.");
  }
  SECTION("XML DTD with entity used within an entity.", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE author [\n"
                "<!ELEMENT author (#PCDATA)>\n"
                "<!ENTITY email \"josmith@theworldaccordingtojosmith.com\">\n"
                "<!--the following use of a general entity is legal \n"
                "if it is used in the XML document-->\n"
                "<!ENTITY js \"Jo Smith &email;\">\n"
                "]>\n"
                "<author>&js;</author>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::internal);
    REQUIRE(xml.m_dtd.getEntity("&js;").internal == "Jo Smith &email;");
    REQUIRE(xml.m_prolog[0].name == "author");
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog[0].children[0]).getNodeType() == XMLNodeType::entity);
    REQUIRE(XMLNodeRef<XMLNodeEntityReference>(*xml.m_prolog[0].children[0]).getContents() == "Jo Smith josmith@theworldaccordingtojosmith.com");
    REQUIRE(xml.m_prolog[0].getContents() == "Jo Smith josmith@theworldaccordingtojosmith.com");
  }
  SECTION("XML DTD with entity used within an entity with recursion.", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE author [\n"
                "<!ELEMENT author (#PCDATA)>\n"
                "<!ENTITY email \"josmith@theworldaccordingtojosmith.com\">\n"
                "<!--the two entity"
                " statements are infinitely recursive-->\n"
                "<!ENTITY email \"user@user.com &js;\">\n"
                "<!ENTITY js \"Jo Smith &email;\">\n"
                "]>\n"
                "<author>&js;</author>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "DTD Syntax Error: Entity '&js;' contains recursive definition which is not allowed.");
  }
  SECTION("XML DTD with entity that contains a character reference that is parsed straight away.", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE author [\n"
                "<!ELEMENT author (#PCDATA)>\n"
                "<!ENTITY email \"josmith&#x40;theworldaccordingtojosmith.com\">\n"
                "<!ENTITY js \"Jo Smith &email;\">\n"
                "]>\n"
                "<author>&js;</author>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::internal);
    REQUIRE(xml.m_dtd.getEntity("&email;").internal == "josmith@theworldaccordingtojosmith.com");
  }
  SECTION("XML DTD with entity that is defined externally (file user.txt).", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY>\n"
                "<!ENTITY name SYSTEM \"./testData/name.txt\">\n"
                "]>\n"
                "<foo>Hello &name;</foo>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::internal);
    REQUIRE(xml.m_dtd.getRootName() == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_prolog[0].name == "foo");
    REQUIRE(xml.m_prolog[0].getContents() == "Hello John Joe Doe");
  }
  SECTION("XML DTD with entity that is defined externally (file that does not exist).", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY>\n"
                "<!ENTITY name SYSTEM \"./testData/unknown.txt\">\n"
                "]>\n"
                "<foo>Hello &name;</foo>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Entity '&name;' source file './testData/unknown.txt' does not exist.");
  }
  SECTION("XML with internal DTD with parameter entities to parse  (internal cannot appear within tags).", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ENTITY % empty_report \"<!ELEMENT REPORT EMPTY>\">"
                "%empty_report;\n"
                "]>\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("XML with internal DTD with parameter entities to parse and check values (internal cannot appear within tags).", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ENTITY % empty_report \"<!ELEMENT REPORT EMPTY>\">"
                "%empty_report;\n"
                "]>\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[0]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::internal);
    REQUIRE(xml.m_dtd.getRootName() == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_dtd.getRootName() == "REPORT");
    REQUIRE(xml.m_dtd.getEntity("%empty_report;").internal == "<!ELEMENT REPORT EMPTY>");
    REQUIRE(xml.m_dtd.getElement("REPORT").name == "REPORT");
    REQUIRE(xml.m_dtd.getElement("REPORT").content.parsed == "EMPTY");
  }
  SECTION("XML with external DTD with parameter entities to parse.", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<!DOCTYPE REPORT SYSTEM \"./testData/report01.dtd\">\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::external);
  }

  SECTION("XML with external DTD with both types of entities to parse an check values", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<!DOCTYPE REPORT SYSTEM \"./testData/report01.dtd\">"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[0]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::external);
    REQUIRE(xml.m_dtd.getRootName() == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_dtd.getRootName() == "REPORT");
    REQUIRE(xml.m_dtd.getEntity("%contact;").internal == "phone");
    REQUIRE(xml.m_dtd.getEntity("%area;").internal == "name, street, pincode, city");
    REQUIRE(xml.m_dtd.getElement("REPORT").name == "REPORT");
    REQUIRE(xml.m_dtd.getElement("REPORT").content.unparsed == "(residence|apartment|office|shop)*");
    REQUIRE(xml.m_dtd.getElement("residence").name == "residence");
    REQUIRE(xml.m_dtd.getElement("residence").content.unparsed == "(name, street, pincode, city, phone)");
    REQUIRE(xml.m_dtd.getElement("apartment").name == "apartment");
    REQUIRE(xml.m_dtd.getElement("apartment").content.unparsed == "(name, street, pincode, city, phone)");
    REQUIRE(xml.m_dtd.getElement("office").name == "office");
    REQUIRE(xml.m_dtd.getElement("office").content.unparsed == "(name, street, pincode, city, phone)");
    REQUIRE(xml.m_dtd.getElement("shop").name == "shop");
    REQUIRE(xml.m_dtd.getElement("shop").content.unparsed == "(name, street, pincode, city, phone)");
  }
  SECTION("XML DTD with parameter entity within general entity.", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note SYSTEM \"./testData/note002.dtd\">\n"
                "<note>"
                "&signature;"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.getType() == DTD::DTDType::external);
    REQUIRE(XMLNodeRef<XMLNode>(xml.m_prolog[0]).getNodeType() == XMLNodeType::root);
    REQUIRE(xml.m_dtd.getEntity("&signature;").internal == "© 1999 Yoyodyne, Inc. &legal;");
    REQUIRE(xml.m_prolog[0].getContents() == "© 1999 Yoyodyne, Inc. All Rights Reserved.");
  }
}