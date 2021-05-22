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
TEST_CASE("Parse XML with DTD both internal and external", "[XML][Parse][DTD]")
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
  SECTION("XML with DTD with !ENTITY definitions and uses", "[XML][Parse][DTD]")
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
  SECTION("XML with DTD with !ENTITY internal definitions and uses. Check translation of entity values", "[XML][Parse][DTD]")
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
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNode>(xml.m_prolog[0]).getNodeType() == XNodeType::root);
    REQUIRE(xml.m_prolog[0][4].name == "footer");
    REQUIRE(xml.m_prolog[0][4].getContents() == "Writer: Donald Duck.\u00A0Copyright: W3Schools.");
  }
  SECTION("XML with DTD with !ENTITY and how it deals with entity character expansion case 1)", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note [\n"
                "<!ENTITY example \"<p>An ampersand (&#38;#38;) may be escaped numerically (&#38;#38;#38;) or with a general entity (&amp;amp;).</p>\">]>\n"
                "<note>&example;</note>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(xml.m_dtd.m_entityMapping["&example;"].internal == "<p>An ampersand (&#38;) may be escaped numerically (&#38;#38;) or with a general entity (&amp;amp;).</p>");
    REQUIRE(xml.m_prolog[0][0].name == "p");
    REQUIRE(xml.m_prolog[0][0].getContents() == "An ampersand (&) may be escaped numerically (&#38;) or with a general entity (&amp;).");
  }
  SECTION("XML with DTD with !ENTITY and how it deals with entity character expansion case 2)", "[XML][Parse][DTD]")
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
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(xml.m_dtd.m_entityMapping["%xx;"].internal == "%zz;");
    REQUIRE(xml.m_dtd.m_entityMapping["%zz;"].internal == "<!ENTITY tricky \"error-prone\" >");
    REQUIRE(xml.m_prolog[0].name == "test");
    REQUIRE(xml.m_prolog[0].getContents() == "This sample shows a error-prone method.");
  }
  SECTION("XML with DTD with !ENTITY and how it deals with entity character expansion case 3)", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ENTITY x \"&lt;\">\n"
                "]>\n"
                "<foo attr=\"&x;\"/>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(xml.m_dtd.m_entityMapping["&x;"].internal == "&lt;");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getAttributeList().size() == 1);
    XAttribute attribute = xml.m_prolog[0].getAttribute("attr");
    REQUIRE(attribute.name == "attr");
    REQUIRE(attribute.value.unparsed == "&x;");
    REQUIRE(attribute.value.parsed == "&lt;");
  }
  // This should throw an error as & ' " < >  not allowed to be assigned to attribute directly (NEED TO FIX)
  SECTION("XML with DTD with !ENTITY and how it deals with entity character expansion case 4)", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ENTITY x \"&#60;\">\n"
                "]>\n"
                "<foo attr=\"&x;\"/>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 5 Column: 21] Attribute value contains invalid character '<', '\"', ''' or '&'.");
    // REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    // REQUIRE(xml.m_dtd.m_entityMapping["&x;"].internal == "<");
    // REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes.size() == 1);
    // REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].name == "attr");
    // REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].value.unparsed == "&x;");
    // REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].value.parsed == "<");
  }
  SECTION("XML with DTD with entity used within an entity.", "[XML][Parse][DTD]")
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
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(xml.m_dtd.m_entityMapping["&js;"].internal == "Jo Smith &email;");
    REQUIRE(xml.m_prolog[0].name == "author");
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog[0].children[0]).getNodeType() == XNodeType::entity);
    REQUIRE(XNodeRef<XNodeEntityReference>(*xml.m_prolog[0].children[0]).getContents() == "Jo Smith josmith@theworldaccordingtojosmith.com");
    REQUIRE(xml.m_prolog[0].getContents() == "Jo Smith josmith@theworldaccordingtojosmith.com");
  }
  SECTION("XML with DTD with entity used within an entity with recursion.", "[XML][Parse][DTD]")
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
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Entity '&js;' contains recursive definition which is not allowed.");
  }
  SECTION("XML with DTD with entity that contains a character reference that is parsed straight away.", "[XML][Parse][DTD]")
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
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(xml.m_dtd.m_entityMapping["&email;"].internal == "josmith@theworldaccordingtojosmith.com");
  }
  SECTION("XML with DTD with entity that is defined externally (file user.txt).", "[XML][Parse][DTD]")
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
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).name == XNodeRef<XNodeDTD>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_prolog[0].name == "foo");
    REQUIRE(xml.m_prolog[0].getContents() == "Hello John Joe Doe");
  }
  SECTION("XML with DTD with entity that is defined externally (file that does not exist).", "[XML][Parse][DTD]")
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
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).name == XNodeRef<XNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["address"].name == "address");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["address"].content.unparsed == "(name,company,phone)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["name"].name == "name");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["name"].content.unparsed == "(#PCDATA)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["company"].name == "company");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["company"].content.unparsed == "(#PCDATA)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["phone"].name == "phone");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["phone"].content.unparsed == "(#PCDATA)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["br"].name == "br");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["br"].content.unparsed == "EMPTY");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["footer"].name == "footer");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["footer"].content.unparsed == "ANY");
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
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).name == XNodeRef<XNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->external.type == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->external.systemID == "./testData/note.dtd");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["note"].name == "note");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["note"].content.unparsed == "(to,from,heading,body)");
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
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).name == XNodeRef<XNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->external.type == "PUBLIC");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->external.systemID == "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->external.publicID == "-//W3C//DTD XHTML 1.0 Transitional//EN");
  }
  SECTION("XML with internal DTD with attributes to parse ", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE TVSCHEDULE [\n"
                "<!ELEMENT TVSCHEDULE (CHANNEL+)>\n"
                "<!ELEMENT CHANNEL (BANNER,DAY+)>\n"
                "<!ELEMENT BANNER (#PCDATA)>\n"
                "<!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>\n"
                "<!ELEMENT HOLIDAY (#PCDATA)>\n"
                "<!ELEMENT DATE (#PCDATA)>\n"
                "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)>\n"
                "<!ELEMENT TIME (#PCDATA)>\n"
                "<!ELEMENT TITLE (#PCDATA)>\n"
                "<!ELEMENT DESCRIPTION (#PCDATA)>\n"
                "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>\n"
                "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED>\n"
                "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
                "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
                "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
                "<TVSCHEDULE></TVSCHEDULE>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("XML with internal DTD with attributes to parse and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE TVSCHEDULE [\n"
                "<!ELEMENT TVSCHEDULE (CHANNEL+)>\n"
                "<!ELEMENT CHANNEL (BANNER,DAY+)>\n"
                "<!ELEMENT BANNER (#PCDATA)>\n"
                "<!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>\n"
                "<!ELEMENT HOLIDAY (#PCDATA)>\n"
                "<!ELEMENT DATE (#PCDATA)>\n"
                "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)>\n"
                "<!ELEMENT TIME (#PCDATA)>\n"
                "<!ELEMENT TITLE (#PCDATA)>\n"
                "<!ELEMENT DESCRIPTION (#PCDATA)>\n"
                "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>\n"
                "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED>\n"
                "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
                "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
                "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
                "<TVSCHEDULE></TVSCHEDULE>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).name == XNodeRef<XNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).name == "TVSCHEDULE");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TVSCHEDULE"].name == "TVSCHEDULE");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["CHANNEL"].name == "CHANNEL");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["BANNER"].name == "BANNER");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["DAY"].name == "DAY");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["HOLIDAY"].name == "HOLIDAY");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["DATE"].name == "DATE");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["PROGRAMSLOT"].name == "PROGRAMSLOT");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TIME"].name == "TIME");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TITLE"].name == "TITLE");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["DESCRIPTION"].name == "DESCRIPTION");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TVSCHEDULE"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["CHANNEL"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["PROGRAMSLOT"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TITLE"].attributes.size() == 2);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TVSCHEDULE"].attributes[0].name == "NAME");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["CHANNEL"].attributes[0].name == "CHAN");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["PROGRAMSLOT"].attributes[0].name == "VTR");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TITLE"].attributes[0].name == "RATING");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TITLE"].attributes[1].name == "LANGUAGE");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TVSCHEDULE"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["CHANNEL"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["PROGRAMSLOT"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TITLE"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TITLE"].attributes[1].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TVSCHEDULE"].attributes[0].value.parsed == "#REQUIRED");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["CHANNEL"].attributes[0].value.parsed == "#REQUIRED");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["PROGRAMSLOT"].attributes[0].value.parsed == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TITLE"].attributes[0].value.parsed == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["TITLE"].attributes[1].value.parsed == "#IMPLIED");
  }
  SECTION("XML with internal DTD with elements with multiple attributes to parse and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE CATALOG [\n"
                "<!ENTITY AUTHOR \"John Doe\">\n"
                "<!ENTITY COMPANY \"JD Power Tools, Inc.\">\n"
                "<!ENTITY EMAIL \"jd@jd-tools.com\">\n"
                "<!ELEMENT CATALOG (PRODUCT+)>\n"
                "<!ELEMENT PRODUCT(SPECIFICATIONS+,OPTIONS?,PRICE+,NOTES?)>\n"
                "<!ATTLIST PRODUCT NAME CDATA #IMPLIED CATEGORY (HandTool|Table|Shop-Professional) \"HandTool\""
                "PARTNUM CDATA #IMPLIED PLANT (Pittsburgh|Milwaukee|Chicago) \"Chicago\" INVENTORY (InStock|Backordered|Discontinued)"
                " \"InStock\">\n"
                "<!ELEMENT SPECIFICATIONS (#PCDATA)>\n"
                "<!ATTLIST SPECIFICATIONS WEIGHT CDATA #IMPLIED POWER CDATA #IMPLIED>\n"
                "<!ELEMENT OPTIONS (#PCDATA)>\n"
                "<!ATTLIST OPTIONS FINISH (Metal|Polished|Matte) \"Matte\" ADAPTER (Included|Optional|NotApplicable)"
                " \"Included\" CASE (HardShell|Soft|NotApplicable) \"HardShell\">\n"
                "<!ELEMENT PRICE (#PCDATA)>\n"
                "<!ATTLIST PRICE MSRP CDATA #IMPLIED"
                "WHOLESALE CDATA #IMPLIED STREET CDATA #IMPLIED SHIPPING CDATA #IMPLIED>\n"
                "<!ELEMENT NOTES (#PCDATA)> ]>\n"
                "<CATALOG> </CATALOG>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).name == XNodeRef<XNodeElement>(*xml.m_prolog.children[0]).name);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).name == "CATALOG");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].name == "PRODUCT");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes.size() == 5);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[0].name == "NAME");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[0].value.parsed == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[1].name == "CATEGORY");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[1].type == "(HandTool|Table|Shop-Professional)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[1].value.parsed == "HandTool");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[2].name == "PARTNUM");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[2].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[2].value.parsed == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[3].name == "PLANT");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[3].type == "(Pittsburgh|Milwaukee|Chicago)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[3].value.parsed == "Chicago");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[4].name == "INVENTORY");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[4].type == "(InStock|Backordered|Discontinued)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["PRODUCT"].attributes[4].value.parsed == "InStock");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["NOTES"].name == "NOTES");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["NOTES"].content.unparsed == "(#PCDATA)");
  }
  SECTION("XML with internal DTD with parameter entities to parse  (internal cannot appear within tags).", "[XML][Parse][DTD]")
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
  SECTION("XML with internal DTD with parameter entities to parse and check values (internal cannot appear within tags).", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ENTITY % empty_report \"<!ELEMENT REPORT EMPTY>\">"
                "%empty_report;\n"
                "]>\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).name == XNodeRef<XNodeDTD>(xml.m_prolog[0]).name);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).name == "REPORT");
    REQUIRE(xml.m_dtd.m_entityMapping["%empty_report;"].internal == "<!ELEMENT REPORT EMPTY>");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["REPORT"].name == "REPORT");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["REPORT"].content.parsed == "EMPTY");
  }
  SECTION("XML with external DTD with parameter entities to parse.", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT SYSTEM \"./testData/report.dtd\">\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }

  SECTION("XML with external DTD with both types of entities to parse an check values", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT SYSTEM \"./testData/report01.dtd\">"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).name == XNodeRef<XNodeElement>(*xml.m_prolog.children[0]).name);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).name == "REPORT");
    REQUIRE(xml.m_dtd.m_entityMapping["%contact;"].internal == "phone");
    REQUIRE(xml.m_dtd.m_entityMapping["%area;"].internal == "name, street, pincode, city");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements.size() == 5);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["REPORT"].name == "REPORT");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["REPORT"].content.unparsed == "(residence|apartment|office|shop)*");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["residence"].name == "residence");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["residence"].content.unparsed == "(name, street, pincode, city, phone)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["apartment"].name == "apartment");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["apartment"].content.unparsed == "(name, street, pincode, city, phone)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["office"].name == "office");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["office"].content.unparsed == "(name, street, pincode, city, phone)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["shop"].name == "shop");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->elements["shop"].content.unparsed == "(name, street, pincode, city, phone)");
  }
  SECTION("XML with external DTD with !NOTATION to parse and check values.", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT SYSTEM \"./testData/report02.dtd\">"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->notations["GIF"].type == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->notations["GIF"].systemID == "GIF");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->notations["JPG"].type == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->notations["JPG"].systemID == "JPG");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->notations["BMP"].type == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[0]).dtd->notations["BMP"].systemID == "BMP");
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
  SECTION("XML with internal DTD with missing terminating '>' on !ELEMENT", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<!ELEMENT note (to,from,heading,body)>\n"
                "<!ELEMENT to (#PCDATA)\n"
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
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 4 Column: 2] Missing '>' terminator.");
  }
  SECTION("XML with internal DTD with missing terminating '>' on !ATTLIST", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE TVSCHEDULE [\n"
                "<!ELEMENT TVSCHEDULE (CHANNEL+)>\n"
                "<!ELEMENT CHANNEL (BANNER,DAY+)>\n"
                "<!ELEMENT BANNER (#PCDATA)>\n"
                "<!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>\n"
                "<!ELEMENT HOLIDAY (#PCDATA)>\n"
                "<!ELEMENT DATE (#PCDATA)>\n"
                "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)>\n"
                "<!ELEMENT TIME (#PCDATA)>\n"
                "<!ELEMENT TITLE (#PCDATA)>\n"
                "<!ELEMENT DESCRIPTION (#PCDATA)>\n"
                "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>\n"
                "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED\n"
                "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
                "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
                "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
                "<TVSCHEDULE></TVSCHEDULE>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 15 Column: 2] Missing '>' terminator.");
  }
  SECTION("XML with internal DTD with missing terminating '>' on !ENTITY", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ELEMENT REPORT (paragraph)*>\n"
                "<!ELEMENT paragraph (#PCDATA)>\n"
                "<!ENTITY contact \"0893-456334534\" ]>\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 4 Column: 36] Missing '>' terminator.");
  }
  SECTION("XML with internal DTD with missing terminating '>' on !NOTATION", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ELEMENT REPORT (paragraph)*>\n"
                "<!ELEMENT paragraph (#PCDATA)>\n"
                "<!ENTITY contact \"0893-456334534\">\n"
                "<!NOTATION GIF SYSTEM \"\"\n"
                "<!NOTATION JPG SYSTEM \"\">\n"
                "<!NOTATION BMP SYSTEM \"\">\n"
                "]>\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 6 Column: 2] Missing '>' terminator.");
  }
  SECTION("XML with a DTD that contains an illegal mixed content specification (uses ',').", "[XML][Parse][DTD]")
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
  SECTION("XML with a DTD that contains an illegal mixed content specification (does not end with '*').", "[XML][Parse][DTD]")
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
  SECTION("Parse XML with DTD that cotains a content specification in error (missing ',').", "[XML][Parse][DTD]")
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
  SECTION("Parse XML with DTD that cotains a content specification in error (missing element name).", "[XML][Parse][DTD]")
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
  SECTION("Parse XML with DTD that cotains a enumeration attribute gender with a default value if 'F'.", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE queue ["
                "<!ELEMENT queue (person)+>\n"
                "<!ELEMENT person (firstName, lastName, nationality)>\n"
                "<!ELEMENT firstName (#PCDATA)>\n"
                "<!ELEMENT lastName (#PCDATA)>\n"
                "<!ELEMENT nationality (#PCDATA)>\n"
                "<!ATTLIST person gender ( M | F ) \"F\">\n"
                "]>\n"
                "<queue>\n"
                "<person gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></person>\n"
                "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
                "</queue>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements.contains("person") == true);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["person"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["person"].attributes[0].name == "gender");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["person"].attributes[0].type == "(M|F)");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).dtd->elements["person"].attributes[0].value.parsed == "F");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).name == "queue");
    REQUIRE(XNodeRef<XNodeDTD>(*xml.m_prolog.children[1]).name == XNodeRef<XNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_prolog[0][0].name == "person");
    REQUIRE(xml.m_prolog[0][0].getAttributeList().size() == 1);
    XAttribute attribute = xml.m_prolog[0][0].getAttribute("gender");
    REQUIRE(attribute.name == "gender");
    REQUIRE(attribute.value.parsed == "M");
    REQUIRE(xml.m_prolog[0][1].name == "person");
    REQUIRE(xml.m_prolog[0][1].getAttributeList().size() == 0);
  }
  SECTION("Parse XML with DTD that cotains a enumeration with a syntax error (missing enumeration name).", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE queue ["
                "<!ELEMENT queue (person)+>\n"
                "<!ELEMENT person (firstName, lastName, nationality)>\n"
                "<!ELEMENT firstName (#PCDATA)>\n"
                "<!ELEMENT lastName (#PCDATA)>\n"
                "<!ELEMENT nationality (#PCDATA)>\n"
                "<!ATTLIST person gender ( M | ) \"F\">\n"
                "]>\n"
                "<queue>\n"
                "<person gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></person>\n"
                "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
                "</queue>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 7 Column: 36] Invalid name '' encountered.");
  }
  SECTION("Parse XML with DTD that cotains a enumeration with a syntax error (missing end bracket).", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE queue ["
                "<!ELEMENT queue (person)+>\n"
                "<!ELEMENT person (firstName, lastName, nationality)>\n"
                "<!ELEMENT firstName (#PCDATA)>\n"
                "<!ELEMENT lastName (#PCDATA)>\n"
                "<!ELEMENT nationality (#PCDATA)>\n"
                "<!ATTLIST person gender ( M | F  \"F\">\n"
                "]>\n"
                "<queue>\n"
                "<person gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></person>\n"
                "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
                "</queue>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 7 Column: 39] Invalid attribute type specified.");
  }
  SECTION("Parse XML with DTD that contains a enumeration with a default value not in enumeration.", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE queue ["
                "<!ELEMENT queue (person)+>\n"
                "<!ELEMENT person (firstName, lastName, nationality)>\n"
                "<!ELEMENT firstName (#PCDATA)>\n"
                "<!ELEMENT lastName (#PCDATA)>\n"
                "<!ELEMENT nationality (#PCDATA)>\n"
                "<!ATTLIST person gender ( M | F ) \"D\">\n"
                "]>\n"
                "<queue>\n"
                "<person gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></person>\n"
                "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
                "</queue>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Default value 'D' for enumeration attribute 'gender' is invalid.");
  }
  SECTION("Parse XML with DTD that contains a enumeration with not all values unique.", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE queue ["
                "<!ELEMENT queue (person)+>\n"
                "<!ELEMENT person (firstName, lastName, nationality)>\n"
                "<!ELEMENT firstName (#PCDATA)>\n"
                "<!ELEMENT lastName (#PCDATA)>\n"
                "<!ELEMENT nationality (#PCDATA)>\n"
                "<!ATTLIST person gender ( M | F | F) \"M\">\n"
                "]>\n"
                "<queue>\n"
                "<person gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></person>\n"
                "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
                "</queue>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Enumerator value 'F' for attribute 'gender' occurs more than once in its definition.");
  }
  SECTION("Parse XML with DTD that specifies the use of an two different ID attributes for an element.", "[XML][Valid][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE collection ["
                "<!ELEMENT collection (item)+>\n"
                "<!ELEMENT item (#PCDATA)>\n"
                "<!ATTLIST item itemID1 ID #REQUIRED >\n"
                "<!ATTLIST item itemID2 ID #REQUIRED >\n"
                "]>\n"
                "<collection>\n"
                "<item itemID1=\"i001\" itemID2=\"id001\">item descripton</item>\n"
                "<item itemID1=\"i002\" itemID2=\"id002\">item descripton</item>\n"
                "<item itemID1=\"i003\" itemID2=\"id003\">item descripton</item>\n"
                "<item itemID1=\"i004\" itemID2=\"id004\">item descripton</item>\n"
                "<item itemID1=\"i005\" itemID2=\"id005\">item descripton</item>\n"
                "</collection>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error: Element <item> has more than one ID attribute.");
  }
}