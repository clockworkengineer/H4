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
    xmlString = "<?xml version=\"1.0\"?>"
                "<!DOCTYPE note [<!ELEMENT note (to,from,heading,body)>"
                "<!ELEMENT to (#PCDATA)>"
                "<!ELEMENT from (#PCDATA)>"
                "<!ELEMENT heading (#PCDATA)>"
                "<!ELEMENT body (#PCDATA)>"
                "]>"
                "<note>"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend</body>"
                "</note>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with external (SYSTEM) DTD", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE note SYSTEM \"Note.dtd\">"
                "<note>"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend!</body>"
                "</note>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with external (PUBLIC) DTD", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE note PUBLIC "
                "\"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
                "<note>"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend!</body>"
                "</note>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with DTD with !ENTITY definitions and uses", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE note "
                "[<!ENTITY nbsp \"&#xA0;\">"
                "<!ENTITY writer \"Writer: Donald Duck.\">"
                "<!ENTITY copyright \"Copyright: W3Schools.\">]>"
                "<note>"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>"
                "</note>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with DTD with !ENTITY definitions and uses. Check translation of entity values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE note "
                "[<!ENTITY nbsp \"&#xA0;\">"
                "<!ENTITY writer \"Writer: Donald Duck.\">"
                "<!ENTITY copyright \"Copyright: W3Schools.\">]>"
                "<note>"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>"
                "</note>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[1]).getNodeType() == XNodeType::element);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[1][4]).name == "footer");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[1][4]).content == "Writer: Donald Duck.\u00A0Copyright: W3Schools.");
  }
  SECTION("XML with internal to parse DTD and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\" ?>"
                "<!DOCTYPE address [  "
                "<!ELEMENT address (name,company,phone)>"
                "<!ELEMENT name (#PCDATA)><!ELEMENT company (#PCDATA)>"
                "<!ELEMENT phone (#PCDATA)><!ELEMENT br EMPTY>"
                "<!ELEMENT footer ANY>]>"
                "<address>"
                "<name>Tanmay Patil</name>"
                "<company>TutorialsPoint</company>"
                "<phone>(011) 123-4567"
                "</phone>"
                "</address>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["address"].name == "address");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["address"].content == "(name,company,phone)");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["name"].name == "name");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["name"].content == "(#PCDATA)");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["company"].name == "company");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["company"].content == "(#PCDATA)");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["phone"].name == "phone");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["phone"].content == "(#PCDATA)");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["br"].name == "br");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["br"].content == "EMPTY");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["footer"].name == "footer");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["footer"].content == "ANY");
  }
  SECTION("XML with external file DTD and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE note SYSTEM \"Note.dtd\">"
                "<note>"
                "<to>Tove"
                "</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body>"
                "</note>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).external.name == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).external.value == "Note.dtd");
  }
  SECTION("XML with external URL DTD to parse and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
                " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
                "<html></html>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).external.name == "PUBLIC");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).external.value == "-//W3C//DTD XHTML 1.0 Transitional//EN, http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
  }
  SECTION("XML with internal DTD with attributes to parse ", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE TVSCHEDULE [<!ELEMENT TVSCHEDULE (CHANNEL+)><!ELEMENT CHANNEL (BANNER,DAY+)>"
                "<!ELEMENT BANNER (#PCDATA)><!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>"
                "<!ELEMENT HOLIDAY (#PCDATA)><!ELEMENT DATE (#PCDATA)>"
                "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)>"
                "<!ELEMENT TIME (#PCDATA)>"
                "<!ELEMENT TITLE (#PCDATA)>"
                "<!ELEMENT DESCRIPTION (#PCDATA)>"
                "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>"
                "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED><!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>"
                "<!ATTLIST TITLE RATING CDATA #IMPLIED><!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>"
                "<TVSCHEDULE></TVSCHEDULE>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with internal DTD with attributes to parse and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE TVSCHEDULE [<!ELEMENT TVSCHEDULE (CHANNEL+)>"
                "<!ELEMENT CHANNEL (BANNER,DAY+)>"
                "<!ELEMENT BANNER (#PCDATA)>"
                "<!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>"
                "<!ELEMENT HOLIDAY (#PCDATA)>"
                "<!ELEMENT DATE (#PCDATA)>"
                "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)>"
                "<!ELEMENT TIME (#PCDATA)>"
                "<!ELEMENT TITLE (#PCDATA)>"
                "<!ELEMENT DESCRIPTION (#PCDATA)>"
                "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>"
                "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED>"
                "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>"
                "<!ATTLIST TITLE RATING CDATA #IMPLIED>"
                "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>"
                "<TVSCHEDULE></TVSCHEDULE>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == "TVSCHEDULE");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TVSCHEDULE"].name == "TVSCHEDULE");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["CHANNEL"].name == "CHANNEL");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["BANNER"].name == "BANNER");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["DAY"].name == "DAY");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["HOLIDAY"].name == "HOLIDAY");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["DATE"].name == "DATE");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PROGRAMSLOT"].name == "PROGRAMSLOT");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TIME"].name == "TIME");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TITLE"].name == "TITLE");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["DESCRIPTION"].name == "DESCRIPTION");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TVSCHEDULE"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["CHANNEL"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PROGRAMSLOT"].attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TITLE"].attributes.size() == 2);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TVSCHEDULE"].attributes[0].name == "NAME");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["CHANNEL"].attributes[0].name == "CHAN");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PROGRAMSLOT"].attributes[0].name == "VTR");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TITLE"].attributes[0].name == "RATING");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TITLE"].attributes[1].name == "LANGUAGE");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TVSCHEDULE"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["CHANNEL"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PROGRAMSLOT"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TITLE"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TITLE"].attributes[1].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TVSCHEDULE"].attributes[0].value == "#REQUIRED");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["CHANNEL"].attributes[0].value == "#REQUIRED");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PROGRAMSLOT"].attributes[0].value == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TITLE"].attributes[0].value == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["TITLE"].attributes[1].value == "#IMPLIED");
  }
  SECTION("XML with internal DTD with elements with multiple attributes to parse and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE CATALOG ["
                "<!ENTITY AUTHOR \"John Doe\">"
                "<!ENTITY COMPANY \"JD Power Tools, Inc.\">"
                "<!ENTITY EMAIL \"jd@jd-tools.com\">"
                "<!ELEMENT CATALOG (PRODUCT+)>"
                "<!ELEMENT PRODUCT(SPECIFICATIONS+,OPTIONS?,PRICE+,NOTES?)>"
                "<!ATTLIST PRODUCT NAME CDATA #IMPLIED CATEGORY (HandTool|Table|Shop-Professional) \"HandTool\""
                "PARTNUM CDATA #IMPLIED PLANT (Pittsburgh|Milwaukee|Chicago) \"Chicago\" INVENTORY (InStock|Backordered|Discontinued)"
                " \"InStock\">"
                "<!ELEMENT SPECIFICATIONS (#PCDATA)><!ATTLIST SPECIFICATIONS WEIGHT CDATA #IMPLIED POWER CDATA #IMPLIED>"
                "<!ELEMENT OPTIONS (#PCDATA)>"
                "<!ATTLIST OPTIONS FINISH (Metal|Polished|Matte) \"Matte\" ADAPTER (Included|Optional|NotApplicable)"
                " \"Included\" CASE (HardShell|Soft|NotApplicable) \"HardShell\">"
                "<!ELEMENT PRICE (#PCDATA)><!ATTLIST PRICE MSRP CDATA #IMPLIED"
                "WHOLESALE CDATA #IMPLIED STREET CDATA #IMPLIED SHIPPING CDATA #IMPLIED>"
                "<!ELEMENT NOTES (#PCDATA)> ]>"
                "<CATALOG> </CATALOG>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == "CATALOG");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].name == "PRODUCT");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes.size() == 5);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[0].name == "NAME");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[0].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[0].value == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[1].name == "CATEGORY");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[1].type == "(HandTool|Table|Shop-Professional)");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[1].value == "HandTool");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[2].name == "PARTNUM");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[2].type == "CDATA");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[2].value == "#IMPLIED");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[3].name == "PLANT");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[3].type == "(Pittsburgh|Milwaukee|Chicago)");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[3].value == "Chicago");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[4].name == "INVENTORY");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[4].type == "(InStock|Backordered|Discontinued)");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[4].value == "InStock");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["NOTES"].name == "NOTES");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["NOTES"].content == "(#PCDATA)");
  }
  SECTION("XML with internal DTD with parameter entities to parse.", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT ["
                "<!ELEMENT residence (%area;, %contact;)>"
                "<!ELEMENT apartment (%area;, %contact;)>"
                "<!ELEMENT office (%area;, %contact;)>"
                "<!ELEMENT shop (%area;, %contact;)>"
                "<!ENTITY % area \"name, street, pincode, city\">"
                "<!ENTITY % contact \"phone\"> ]>"
                "<area></area>";
    REQUIRE(xml.parse(xmlString));
  }
  SECTION("XML with internal DTD with both types of entities to parse an check values", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT ["
                "<!ELEMENT residence (%area;, %contact;)>"
                "<!ELEMENT apartment (%area;, %contact;)>"
                "<!ELEMENT office (%area;, %contact;)>"
                "<!ELEMENT shop (%area;, %contact;)>"
                "<!ENTITY asg \"dummy test\">"
                "<!ENTITY % area \"name, street, pincode, city\">"
                "<!ENTITY % contact \"phone\"> ]>"
                "<REPORT></REPORT>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == "REPORT");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).entityMapping["&asg;"] == "dummy test");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).entityMapping["%contact;"] == "phone");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).entityMapping["%area;"] == "name, street, pincode, city");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements.size() == 4);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["residence"].name == "residence");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["residence"].content == "(%area;, %contact;)");
  }
  SECTION("XML with internal DTD with !NOTATION to parse and check values.", "[XML][Parse][DTD]")
  {
    xmlString = "<!DOCTYPE REPORT ["
                "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>"
                "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>"
                "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>"
                "<!ELEMENT SHORTSECT (TITLE,%BODY;)>"
                "<!ELEMENT TITLE %TEXT;>"
                "<!ELEMENT PARA %TEXT;>"
                "<!ELEMENT LIST (ITEM)+>"
                "<!ELEMENT ITEM (%BLOCK;)>"
                "<!ELEMENT CODE (#PCDATA)>"
                "<!ELEMENT KEYWORD (#PCDATA)>"
                "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>"
                "<!ELEMENT GRAPHIC EMPTY>"
                "<!ATTLIST REPORT security (high | medium | low ) \"low\">"
                "<!ATTLIST CODE type CDATA #IMPLIED>"
                "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>"
                "<!ENTITY xml \"Extensible Markup Language\">"
                "<!ENTITY sgml \"Standard Generalized Markup Language\">"
                "<!ENTITY pxa \"Professional XML Authoring\">"
                "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">"
                "<!ENTITY % BLOCK \"(PARA|LIST)+\">"
                "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">"
                "<!NOTATION GIF SYSTEM \"\">"
                "<!NOTATION JPG SYSTEM \"\">"
                "<!NOTATION BMP SYSTEM \"\">"
                "]>"
                "<REPORT> </REPORT>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["GIF"].name == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["GIF"].value == "\"\"");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["JPG"].name == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["JPG"].value == "\"\"");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["BMP"].name == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["BMP"].value == "\"\"");
  }
}