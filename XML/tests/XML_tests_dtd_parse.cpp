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
  XML xml;
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
    REQUIRE_NOTHROW(xml.parse(xmlSource));
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
    REQUIRE_NOTHROW(xml.parse(xmlSource));
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
    REQUIRE_NOTHROW(xml.parse(xmlSource));
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
    REQUIRE_NOTHROW(xml.parse(xmlSource));
  }
  SECTION("XML with DTD with !ENTITY definitions and uses. Check translation of entity values", "[XML][Parse][DTD]")
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[3]).getNodeType() == XNodeType::root);
    REQUIRE(XNodeRef<XNodeElement>(xmlObject.prolog[3][6]).name == "footer");
    REQUIRE(XNodeRef<XNodeElement>(xmlObject.prolog[3][6]).getContents() == "Writer: Donald Duck.\u00A0Copyright: W3Schools.");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name == XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["address"].name == "address");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["address"].content.unparsed == "(name,company,phone)");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["name"].name == "name");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["name"].content.unparsed == "(#PCDATA)");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["company"].name == "company");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["company"].content.unparsed == "(#PCDATA)");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["phone"].name == "phone");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["phone"].content.unparsed == "(#PCDATA)");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["br"].name == "br");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["br"].content.unparsed == "EMPTY");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["footer"].name == "footer");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["footer"].content.unparsed == "ANY");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name == XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).external.name == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).external.value.parsed == "./testData/note.dtd");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["note"].name == "note");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["note"].content.unparsed == "(to,from,heading,body)");
  }
  SECTION("XML with external URL DTD to parse and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
                " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                "<html></html>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name == XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).external.name == "PUBLIC");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).external.value.parsed == "-//W3C//DTD XHTML 1.0 Transitional//EN, http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
  }
  SECTION("XML with internal DTD with attributes to parse ", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE TVSCHEDULE [\n"
                "<!ELEMENT TVSCHEDULE (CHANNEL+)>\n"
                "<!ELEMENT CHANNEL (BANNER,DAY+)>\n"
                "<!ELEMENT BANNER (#PCDATA)><!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>\n"
                "<!ELEMENT HOLIDAY (#PCDATA)><!ELEMENT DATE (#PCDATA)>\n"
                "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)>\n"
                "<!ELEMENT TIME (#PCDATA)>\n"
                "<!ELEMENT TITLE (#PCDATA)>\n"
                "<!ELEMENT DESCRIPTION (#PCDATA)>\n"
                "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>\n"
                "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED><!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
                "<!ATTLIST TITLE RATING CDATA #IMPLIED><!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
                "<TVSCHEDULE></TVSCHEDULE>\n";
    BufferSource xmlSource(xmlString);
    REQUIRE_NOTHROW(xml.parse(xmlSource));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name == XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name == "TVSCHEDULE");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TVSCHEDULE"].name == "TVSCHEDULE");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["CHANNEL"].name == "CHANNEL");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["BANNER"].name == "BANNER");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["DAY"].name == "DAY");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["HOLIDAY"].name == "HOLIDAY");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["DATE"].name == "DATE");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["PROGRAMSLOT"].name == "PROGRAMSLOT");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TIME"].name == "TIME");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TITLE"].name == "TITLE");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["DESCRIPTION"].name == "DESCRIPTION");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TVSCHEDULE"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["CHANNEL"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["PROGRAMSLOT"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TITLE"].attributes.size() == 2);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TVSCHEDULE"].attributes[0].name == "NAME");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["CHANNEL"].attributes[0].name == "CHAN");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["PROGRAMSLOT"].attributes[0].name == "VTR");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TITLE"].attributes[0].name == "RATING");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TITLE"].attributes[1].name == "LANGUAGE");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TVSCHEDULE"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["CHANNEL"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["PROGRAMSLOT"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TITLE"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TITLE"].attributes[1].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TVSCHEDULE"].attributes[0].value.parsed == "#REQUIRED");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["CHANNEL"].attributes[0].value.parsed == "#REQUIRED");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["PROGRAMSLOT"].attributes[0].value.parsed == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TITLE"].attributes[0].value.parsed == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["TITLE"].attributes[1].value.parsed == "#IMPLIED");
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
                "<!ELEMENT SPECIFICATIONS (#PCDATA)><!ATTLIST SPECIFICATIONS WEIGHT CDATA #IMPLIED POWER CDATA #IMPLIED>\n"
                "<!ELEMENT OPTIONS (#PCDATA)>\n"
                "<!ATTLIST OPTIONS FINISH (Metal|Polished|Matte) \"Matte\" ADAPTER (Included|Optional|NotApplicable)"
                " \"Included\" CASE (HardShell|Soft|NotApplicable) \"HardShell\">\n"
                "<!ELEMENT PRICE (#PCDATA)><!ATTLIST PRICE MSRP CDATA #IMPLIED"
                "WHOLESALE CDATA #IMPLIED STREET CDATA #IMPLIED SHIPPING CDATA #IMPLIED>\n"
                "<!ELEMENT NOTES (#PCDATA)> ]>\n"
                "<CATALOG> </CATALOG>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).name == XNodeRef<XNodeDTD>(xmlObject.prolog[2]).name);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).name == "CATALOG");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].name == "PRODUCT");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes.size() == 5);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[0].name == "NAME");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[0].value.parsed == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[1].name == "CATEGORY");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[1].type == "(HandTool|Table|Shop-Professional)");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[1].value.parsed == "HandTool");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[2].name == "PARTNUM");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[2].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[2].value.parsed == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[3].name == "PLANT");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[3].type == "(Pittsburgh|Milwaukee|Chicago)");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[3].value.parsed == "Chicago");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[4].name == "INVENTORY");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[4].type == "(InStock|Backordered|Discontinued)");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["PRODUCT"].attributes[4].value.parsed == "InStock");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["NOTES"].name == "NOTES");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["NOTES"].content.unparsed == "(#PCDATA)");
  }
  SECTION("XML with internal DTD with parameter entities to parse.", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ELEMENT REPORT (residence|apartment|office|shop)*>\n"
                "<!ELEMENT residence (%area;, %contact;)>\n"
                "<!ELEMENT apartment (%area;, %contact;)>\n"
                "<!ELEMENT office (%area;, %contact;)>\n"
                "<!ELEMENT shop (%area;, %contact;)>\n"
                "<!ENTITY % area \"name, street, pincode, city\">\n"
                "<!ENTITY % contact \"phone\"> ]>\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    REQUIRE_NOTHROW(xml.parse(xmlSource));
  }
  SECTION("XML with internal DTD with both types of entities to parse an check values", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ELEMENT REPORT (residence|apartment|office|shop)*>\n"
                "<!ELEMENT residence (%area;, %contact;)>\n"
                "<!ELEMENT apartment (%area;, %contact;)>\n"
                "<!ELEMENT office (%area;, %contact;)>\n"
                "<!ELEMENT shop (%area;, %contact;)>\n"
                "<!ENTITY asg \"dummy test\">\n"
                "<!ENTITY % area \"name, street, pincode, city\">\n"
                "<!ENTITY % contact \"phone\"> ]>\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).name == XNodeRef<XNodeDTD>(xmlObject.prolog[2]).name);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).name == "REPORT");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).entityMapping["&asg;"] == "dummy test");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).entityMapping["%contact;"] == "phone");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).entityMapping["%area;"] == "name, street, pincode, city");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements.size() == 5);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["residence"].name == "residence");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).elements["residence"].content.unparsed == "(%area;, %contact;)");
  }
  SECTION("XML with internal DTD with !NOTATION to parse and check values.", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\n"
                "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\n"
                "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\n"
                "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\n"
                "<!ELEMENT TITLE %TEXT;>\n"
                "<!ELEMENT PARA %TEXT;>\n"
                "<!ELEMENT LIST (ITEM)+>\n"
                "<!ELEMENT ITEM (%BLOCK;)>\n"
                "<!ELEMENT CODE (#PCDATA)>\n"
                "<!ELEMENT KEYWORD (#PCDATA)>\n"
                "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\n"
                "<!ELEMENT GRAPHIC EMPTY>\n"
                "<!ATTLIST REPORT security (high | medium | low ) \"low\">\n"
                "<!ATTLIST CODE type CDATA #IMPLIED>\n"
                "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\n"
                "<!ENTITY xml \"Extensible Markup Language\">\n"
                "<!ENTITY sgml \"Standard Generalized Markup Language\">\n"
                "<!ENTITY pxa \"Professional XML Authoring\">\n"
                "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\n"
                "<!ENTITY % BLOCK \"(PARA|LIST)+\">\n"
                "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\n"
                "<!NOTATION GIF SYSTEM \"GIF\">\n"
                "<!NOTATION JPG SYSTEM \"JPG\">\n"
                "<!NOTATION BMP SYSTEM \"BMP\">\n"
                "]>\n"
                "<REPORT> </REPORT>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).notations["GIF"].name == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).notations["GIF"].value.parsed == "GIF");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).notations["GIF"].value.unparsed == "GIF");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).notations["JPG"].name == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).notations["JPG"].value.parsed == "JPG");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).notations["JPG"].value.unparsed == "JPG");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).notations["BMP"].name == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).notations["BMP"].value.parsed == "BMP");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[0]).notations["BMP"].value.unparsed == "BMP");
  }
  SECTION("XML with internal DTD containing comments.", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<-- root element is note that contains to, from, heading and body elements -->\n"
                "<!ELEMENT note (to,from,heading,body)>\n"
                "<-- Note to field -->\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<-- Note from field -->\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<-- Note heading field -->\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<-- Note boy field -->\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource xmlSource(xmlString);
    REQUIRE_NOTHROW(xml.parse(xmlSource));
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
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error [Line: 4 Column: 2] Missing '>' terminator.");
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
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error [Line: 15 Column: 2] Missing '>' terminator.");
  }
  SECTION("XML with internal DTD with missing terminating '>' on !ENTITY", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ELEMENT REPORT (residence|apartment|office|shop)*>\n"
                "<!ELEMENT residence (%area;, %contact;)>\n"
                "<!ELEMENT apartment (%area;, %contact;)>\n"
                "<!ELEMENT office (%area;, %contact;)>\n"
                "<!ELEMENT shop (%area;, %contact;)>\n"
                "<!ENTITY % area \"name, street, pincode, city\"\n"
                "<!ENTITY % contact \"phone\"> ]>\n"
                "<REPORT></REPORT>\n";
    BufferSource xmlSource(xmlString);
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error [Line: 8 Column: 2] Missing '>' terminator.");
  }
  SECTION("XML with internal DTD with missing terminating '>' on !NOTATION", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\n"
                "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\n"
                "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\n"
                "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\n"
                "<!ELEMENT TITLE %TEXT;>\n"
                "<!ELEMENT PARA %TEXT;>\n"
                "<!ELEMENT LIST (ITEM)+>\n"
                "<!ELEMENT ITEM (%BLOCK;)>\n"
                "<!ELEMENT CODE (#PCDATA)>\n"
                "<!ELEMENT KEYWORD (#PCDATA)>\n"
                "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\n"
                "<!ELEMENT GRAPHIC EMPTY>\n"
                "<!ATTLIST REPORT security (high | medium | low ) \"low\">\n"
                "<!ATTLIST CODE type CDATA #IMPLIED>\n"
                "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\n"
                "<!ENTITY xml \"Extensible Markup Language\">\n"
                "<!ENTITY sgml \"Standard Generalized Markup Language\">\n"
                "<!ENTITY pxa \"Professional XML Authoring\">\n"
                "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\n"
                "<!ENTITY % BLOCK \"(PARA|LIST)+\">\n"
                "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\n"
                "<!NOTATION GIF SYSTEM \"\"\n"
                "<!NOTATION JPG SYSTEM \"\">\n"
                "<!NOTATION BMP SYSTEM \"\">\n"
                "]>\n"
                "<REPORT> </REPORT>\n";
    BufferSource xmlSource(xmlString);
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error [Line: 24 Column: 2] Missing '>' terminator.");
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
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error: Invalid content region specification for element <format>.");
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
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error: Invalid content region specification for element <format>.");
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
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error: Invalid content region specification for element <format>.");
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
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error: Invalid content region specification for element <note>.");
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
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error: Invalid content region specification for element <note>.");
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
                "<person gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName></person>\n"
                "<person><firstName>Jane</firstName><lastName>Smith</lastName></person>\n"
                "</queue>\n";
    BufferSource xmlSource(xmlString);
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE(XNodeRef<XNode>(xmlObject.prolog[1]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements.contains("person") == true);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["person"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["person"].attributes[0].name == "gender");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["person"].attributes[0].type == "( M | F )");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).elements["person"].attributes[0].value.parsed == "F");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name == "queue");
    REQUIRE(XNodeRef<XNodeDTD>(xmlObject.prolog[1]).name == XNodeRef<XNodeElement>(xmlObject.prolog[3]).name);
    REQUIRE(XNodeRef<XNodeElement>(xmlObject.prolog[3][1]).name == "person");
    REQUIRE(XNodeRef<XNodeElement>(xmlObject.prolog[3][1]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>(xmlObject.prolog[3][1]).attributes[0].name == "gender");
    REQUIRE(XNodeRef<XNodeElement>(xmlObject.prolog[3][1]).attributes[0].value.parsed == "M");
    REQUIRE(XNodeRef<XNodeElement>(xmlObject.prolog[3][3]).name == "person");
    REQUIRE(XNodeRef<XNodeElement>(xmlObject.prolog[3][3]).attributes.size() == 0);
  }
  SECTION("Parse XML with DTD that cotains a enumeration with a syntax error.", "[XML][Parse][DTD]")
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
                "<person gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName></person>\n"
                "<person><firstName>Jane</firstName><lastName>Smith</lastName></person>\n"
                "</queue>\n";
    BufferSource xmlSource(xmlString);
    REQUIRE_THROWS_WITH(xml.parse(xmlSource), "");
  }
}