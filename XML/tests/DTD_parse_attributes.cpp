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
TEST_CASE("Parse XML DTD with atttributes and check values.", "[XML][Parse][DTD][Attributes]")
{
  std::string xmlString;
  SECTION("XML with internal DTD with attributes to parse ", "[XML][Parse][DTD][Attributes]")
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
  SECTION("XML with internal DTD with attributes to parse and check values", "[XML][Parse][DTD][Attributes]")
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
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.m_name == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_dtd.m_name == "TVSCHEDULE");
    REQUIRE(xml.m_dtd.m_elements["TVSCHEDULE"].name == "TVSCHEDULE");
    REQUIRE(xml.m_dtd.m_elements["CHANNEL"].name == "CHANNEL");
    REQUIRE(xml.m_dtd.m_elements["BANNER"].name == "BANNER");
    REQUIRE(xml.m_dtd.m_elements["DAY"].name == "DAY");
    REQUIRE(xml.m_dtd.m_elements["HOLIDAY"].name == "HOLIDAY");
    REQUIRE(xml.m_dtd.m_elements["DATE"].name == "DATE");
    REQUIRE(xml.m_dtd.m_elements["PROGRAMSLOT"].name == "PROGRAMSLOT");
    REQUIRE(xml.m_dtd.m_elements["TIME"].name == "TIME");
    REQUIRE(xml.m_dtd.m_elements["TITLE"].name == "TITLE");
    REQUIRE(xml.m_dtd.m_elements["DESCRIPTION"].name == "DESCRIPTION");
    REQUIRE(xml.m_dtd.m_elements["TVSCHEDULE"].attributes.size() == 1);
    REQUIRE(xml.m_dtd.m_elements["CHANNEL"].attributes.size() == 1);
    REQUIRE(xml.m_dtd.m_elements["PROGRAMSLOT"].attributes.size() == 1);
    REQUIRE(xml.m_dtd.m_elements["TITLE"].attributes.size() == 2);
    REQUIRE(xml.m_dtd.m_elements["TVSCHEDULE"].attributes[0].name == "NAME");
    REQUIRE(xml.m_dtd.m_elements["CHANNEL"].attributes[0].name == "CHAN");
    REQUIRE(xml.m_dtd.m_elements["PROGRAMSLOT"].attributes[0].name == "VTR");
    REQUIRE(xml.m_dtd.m_elements["TITLE"].attributes[0].name == "RATING");
    REQUIRE(xml.m_dtd.m_elements["TITLE"].attributes[1].name == "LANGUAGE");
    REQUIRE(xml.m_dtd.m_elements["TVSCHEDULE"].attributes[0].type == "CDATA");
    REQUIRE(xml.m_dtd.m_elements["CHANNEL"].attributes[0].type == "CDATA");
    REQUIRE(xml.m_dtd.m_elements["PROGRAMSLOT"].attributes[0].type == "CDATA");
    REQUIRE(xml.m_dtd.m_elements["TITLE"].attributes[0].type == "CDATA");
    REQUIRE(xml.m_dtd.m_elements["TITLE"].attributes[1].type == "CDATA");
    REQUIRE(xml.m_dtd.m_elements["TVSCHEDULE"].attributes[0].value.parsed == "#REQUIRED");
    REQUIRE(xml.m_dtd.m_elements["CHANNEL"].attributes[0].value.parsed == "#REQUIRED");
    REQUIRE(xml.m_dtd.m_elements["PROGRAMSLOT"].attributes[0].value.parsed == "#IMPLIED");
    REQUIRE(xml.m_dtd.m_elements["TITLE"].attributes[0].value.parsed == "#IMPLIED");
    REQUIRE(xml.m_dtd.m_elements["TITLE"].attributes[1].value.parsed == "#IMPLIED");
  }
  SECTION("XML with internal DTD with elements with multiple attributes to parse and check values", "[XML][Parse][DTD][Attributes]")
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
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[0]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.m_name == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_dtd.m_name == "CATALOG");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].name == "PRODUCT");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes.size() == 5);
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[0].name == "NAME");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[0].type == "CDATA");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[0].value.parsed == "#IMPLIED");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[1].name == "CATEGORY");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[1].type == "(HandTool|Table|Shop-Professional)");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[1].value.parsed == "HandTool");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[2].name == "PARTNUM");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[2].type == "CDATA");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[2].value.parsed == "#IMPLIED");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[3].name == "PLANT");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[3].type == "(Pittsburgh|Milwaukee|Chicago)");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[3].value.parsed == "Chicago");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[4].name == "INVENTORY");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[4].type == "(InStock|Backordered|Discontinued)");
    REQUIRE(xml.m_dtd.m_elements["PRODUCT"].attributes[4].value.parsed == "InStock");
    REQUIRE(xml.m_dtd.m_elements["NOTES"].name == "NOTES");
    REQUIRE(xml.m_dtd.m_elements["NOTES"].content.unparsed == "(#PCDATA)");
  }
}
TEST_CASE("Parse XML DTD that contains enumeration attributes with various errors.", "[XML][Parse][DTD][Error][Attributes]")
{
  std::string xmlString;
  SECTION("Parse XML with DTD that cotains a enumeration attribute gender with a default value if 'F'.", "[XML][Parse][DTD][Error][Attributes]")
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
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.m_elements.contains("person") == true);
    REQUIRE(xml.m_dtd.m_elements["person"].attributes.size() == 1);
    REQUIRE(xml.m_dtd.m_elements["person"].attributes[0].name == "gender");
    REQUIRE(xml.m_dtd.m_elements["person"].attributes[0].type == "(M|F)");
    REQUIRE(xml.m_dtd.m_elements["person"].attributes[0].value.parsed == "F");
    REQUIRE(xml.m_dtd.m_name == "queue");
    REQUIRE(xml.m_dtd.m_name == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_prolog[0][0].name == "person");
    REQUIRE(xml.m_prolog[0][0].getAttributeList().size() == 1);
    XMLAttribute attribute = xml.m_prolog[0][0].getAttribute("gender");
    REQUIRE(attribute.name == "gender");
    REQUIRE(attribute.value.parsed == "M");
    REQUIRE(xml.m_prolog[0][1].name == "person");
    REQUIRE(xml.m_prolog[0][1].getAttributeList().size() == 0);
  }
  SECTION("Parse XML with DTD that cotains a enumeration with a syntax error (missing enumeration name).", "[XML][Parse][DTD][Error][Attributes]")
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
  SECTION("Parse XML with DTD that cotains a enumeration with a syntax error (missing end bracket).", "[XML][Parse][DTD][Error][Attributes]")
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
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 7 Column: 39] Missing closing ')' on enumeration attribute type.");
  }
  SECTION("Parse XML with DTD that contains a enumeration with a default value not in enumeration.", "[XML][Parse][DTD][Error][Attributes]")
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
  SECTION("Parse XML with DTD that contains a enumeration with not all values unique.", "[XML][Parse][DTD][Error][Attributes]")
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
  SECTION("Parse XML with DTD that specifies the use of an two different ID attributes for an element.", "[XML][Parse][DTD][Error][Attributes]")
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