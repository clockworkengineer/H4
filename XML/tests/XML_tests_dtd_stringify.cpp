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
TEST_CASE("Stringify XML with DTD both internal and external", "[XML][Stringify][DTD]")
{
    XML xml;
    std::string xmlString;
    SECTION("Stringify XML with internal DTD", "[XML][Stringify][DTD]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
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
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }

    SECTION("Stringify XML with external (SYSTEM) DTD", "[XML][Stringify][DTD]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<!DOCTYPE note SYSTEM \"Note.dtd\">\n"
                    "<note>\n"
                    "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                    "<body>Don't forget me this weekend!</body>\n"
                    "</note>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with external (PUBLIC) DTD", "[XML][Stringify][DTD]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<!DOCTYPE note PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                    "<note>\n"
                    "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                    "<body>Don't forget me this weekend!</body>\n"
                    "</note>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with DTD with !ENTITY definitions and uses", "[XML][Stringify][DTD]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<!DOCTYPE note [\n"
                    "<!ENTITY nbsp \"&#xA0;\">\n"
                    "<!ENTITY writer \"Writer: Donald Duck.\">\n"
                    "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
                    "<note>\n"
                    "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                    "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
                    "</note>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }

    SECTION("Stringify XML with external file DTD.", "[XML][Stringify][DTD]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<!DOCTYPE note SYSTEM \"Note.dtd\">\n"
                    "<note>\n"
                    "<to>Tove"
                    "</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body>\n"
                    "</note>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with external URL DTD.", "[XML][Stringify][DTD]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
                    " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                    "<html></html>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with internal DTD with attributes.", "[XML][Stringify][DTD]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<!DOCTYPE TVSCHEDULE [\n"
                    "<!ELEMENT TVSCHEDULE (CHANNEL+)><!ELEMENT CHANNEL (BANNER,DAY+)>\n"
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
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    //   SECTION("XML with internal DTD with attributes to parse and check values", "[XML][Parse][DTD]")
    //   {
    //     xmlString="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    //                 "<!DOCTYPE TVSCHEDULE [\n"
    //                 "<!ELEMENT TVSCHEDULE (CHANNEL+)>\n"
    //                 "<!ELEMENT CHANNEL (BANNER,DAY+)>\n"
    //                 "<!ELEMENT BANNER (#PCDATA)>\n"
    //                 "<!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>\n"
    //                 "<!ELEMENT HOLIDAY (#PCDATA)>\n"
    //                 "<!ELEMENT DATE (#PCDATA)>\n"
    //                 "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)>\n"
    //                 "<!ELEMENT TIME (#PCDATA)>\n"
    //                 "<!ELEMENT TITLE (#PCDATA)>\n"
    //                 "<!ELEMENT DESCRIPTION (#PCDATA)>\n"
    //                 "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>\n"
    //                 "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED>\n"
    //                 "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
    //                 "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
    //                 "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
    //                 "<TVSCHEDULE></TVSCHEDULE>\n";
    //     BufferSource xmlSource(xmlString);
    //     std::unique_ptr<XNode> xNodeRoot=xml.parse(xmlSource);
    //     REQUIRE(XNodeRef<XNode>((*xNodeRoot)[1]).getNodeType() == XNodeType::dtd);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name == "TVSCHEDULE");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TVSCHEDULE"].name == "TVSCHEDULE");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["CHANNEL"].name == "CHANNEL");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["BANNER"].name == "BANNER");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["DAY"].name == "DAY");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["HOLIDAY"].name == "HOLIDAY");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["DATE"].name == "DATE");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["PROGRAMSLOT"].name == "PROGRAMSLOT");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TIME"].name == "TIME");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TITLE"].name == "TITLE");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["DESCRIPTION"].name == "DESCRIPTION");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TVSCHEDULE"].attributes.size() == 1);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["CHANNEL"].attributes.size() == 1);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["PROGRAMSLOT"].attributes.size() == 1);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TITLE"].attributes.size() == 2);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TVSCHEDULE"].attributes[0].name == "NAME");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["CHANNEL"].attributes[0].name == "CHAN");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["PROGRAMSLOT"].attributes[0].name == "VTR");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TITLE"].attributes[0].name == "RATING");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TITLE"].attributes[1].name == "LANGUAGE");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TVSCHEDULE"].attributes[0].type == "CDATA");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["CHANNEL"].attributes[0].type == "CDATA");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["PROGRAMSLOT"].attributes[0].type == "CDATA");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TITLE"].attributes[0].type == "CDATA");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TITLE"].attributes[1].type == "CDATA");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TVSCHEDULE"].attributes[0].value == "#REQUIRED");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["CHANNEL"].attributes[0].value == "#REQUIRED");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["PROGRAMSLOT"].attributes[0].value == "#IMPLIED");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TITLE"].attributes[0].value == "#IMPLIED");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[1]).elements["TITLE"].attributes[1].value == "#IMPLIED");
    //   }
    //   SECTION("XML with internal DTD with elements with multiple attributes to parse and check values", "[XML][Parse][DTD]")
    //   {
    //     xmlString="<!DOCTYPE CATALOG [\n"
    //                 "<!ENTITY AUTHOR \"John Doe\">\n"
    //                 "<!ENTITY COMPANY \"JD Power Tools, Inc.\">\n"
    //                 "<!ENTITY EMAIL \"jd@jd-tools.com\">\n"
    //                 "<!ELEMENT CATALOG (PRODUCT+)>\n"
    //                 "<!ELEMENT PRODUCT(SPECIFICATIONS+,OPTIONS?,PRICE+,NOTES?)>\n"
    //                 "<!ATTLIST PRODUCT NAME CDATA #IMPLIED CATEGORY (HandTool|Table|Shop-Professional) \"HandTool\""
    //                 "PARTNUM CDATA #IMPLIED PLANT (Pittsburgh|Milwaukee|Chicago) \"Chicago\" INVENTORY (InStock|Backordered|Discontinued)"
    //                 " \"InStock\">\n"
    //                 "<!ELEMENT SPECIFICATIONS (#PCDATA)><!ATTLIST SPECIFICATIONS WEIGHT CDATA #IMPLIED POWER CDATA #IMPLIED>\n"
    //                 "<!ELEMENT OPTIONS (#PCDATA)>\n"
    //                 "<!ATTLIST OPTIONS FINISH (Metal|Polished|Matte) \"Matte\" ADAPTER (Included|Optional|NotApplicable)"
    //                 " \"Included\" CASE (HardShell|Soft|NotApplicable) \"HardShell\">\n"
    //                 "<!ELEMENT PRICE (#PCDATA)><!ATTLIST PRICE MSRP CDATA #IMPLIED"
    //                 "WHOLESALE CDATA #IMPLIED STREET CDATA #IMPLIED SHIPPING CDATA #IMPLIED>\n"
    //                 "<!ELEMENT NOTES (#PCDATA)> ]>\n"
    //                 "<CATALOG> </CATALOG>\n";
    //     BufferSource xmlSource(xmlString);
    //     std::unique_ptr<XNode> xNodeRoot=xml.parse(xmlSource);
    //     REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[2]).name);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == "CATALOG");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].name == "PRODUCT");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes.size() == 5);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[0].name == "NAME");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[0].type == "CDATA");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[0].value == "#IMPLIED");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[1].name == "CATEGORY");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[1].type == "(HandTool|Table|Shop-Professional)");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[1].value == "HandTool");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[2].name == "PARTNUM");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[2].type == "CDATA");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[2].value == "#IMPLIED");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[3].name == "PLANT");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[3].type == "(Pittsburgh|Milwaukee|Chicago)");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[3].value == "Chicago");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[4].name == "INVENTORY");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[4].type == "(InStock|Backordered|Discontinued)");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["PRODUCT"].attributes[4].value == "InStock");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["NOTES"].name == "NOTES");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["NOTES"].content == "(#PCDATA)");
    //   }
    //   SECTION("XML with internal DTD with parameter entities to parse.", "[XML][Parse][DTD]")
    //   {
    //     xmlString="<!DOCTYPE REPORT [\n"
    //                 "<!ELEMENT residence (%area;, %contact;)>\n"
    //                 "<!ELEMENT apartment (%area;, %contact;)>\n"
    //                 "<!ELEMENT office (%area;, %contact;)>\n"
    //                 "<!ELEMENT shop (%area;, %contact;)>\n"
    //                 "<!ENTITY % area \"name, street, pincode, city\">\n"
    //                 "<!ENTITY % contact \"phone\"> ]>\n"
    //                 "<area></area>\n";
    //     BufferSource xmlSource(xmlString);
    //     REQUIRE(xml.parse(xmlSource));
    //   }
    //   SECTION("XML with internal DTD with both types of entities to parse an check values", "[XML][Parse][DTD]")
    //   {
    //     xmlString="<!DOCTYPE REPORT [\n"
    //                 "<!ELEMENT residence (%area;, %contact;)>\n"
    //                 "<!ELEMENT apartment (%area;, %contact;)>\n"
    //                 "<!ELEMENT office (%area;, %contact;)>\n"
    //                 "<!ELEMENT shop (%area;, %contact;)>\n"
    //                 "<!ENTITY asg \"dummy test\">\n"
    //                 "<!ENTITY % area \"name, street, pincode, city\">\n"
    //                 "<!ENTITY % contact \"phone\"> ]>\n"
    //                 "<REPORT></REPORT>\n";
    //     BufferSource xmlSource(xmlString);
    //     std::unique_ptr<XNode> xNodeRoot=xml.parse(xmlSource);
    //     REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[2]).name);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == "REPORT");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).entityMapping["&asg;"] == "dummy test");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).entityMapping["%contact;"] == "phone");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).entityMapping["%area;"] == "name, street, pincode, city");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements.size() == 4);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["residence"].name == "residence");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).elements["residence"].content == "(%area;, %contact;)");
    //   }
    //   SECTION("XML with internal DTD with !NOTATION to parse and check values.", "[XML][Parse][DTD]")
    //   {
    //     xmlString="<!DOCTYPE REPORT [\n"
    //                 "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\n"
    //                 "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\n"
    //                 "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\n"
    //                 "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\n"
    //                 "<!ELEMENT TITLE %TEXT;>\n"
    //                 "<!ELEMENT PARA %TEXT;>\n"
    //                 "<!ELEMENT LIST (ITEM)+>\n"
    //                 "<!ELEMENT ITEM (%BLOCK;)>\n"
    //                 "<!ELEMENT CODE (#PCDATA)>\n"
    //                 "<!ELEMENT KEYWORD (#PCDATA)>\n"
    //                 "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\n"
    //                 "<!ELEMENT GRAPHIC EMPTY>\n"
    //                 "<!ATTLIST REPORT security (high | medium | low ) \"low\">\n"
    //                 "<!ATTLIST CODE type CDATA #IMPLIED>\n"
    //                 "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\n"
    //                 "<!ENTITY xml \"Extensible Markup Language\">\n"
    //                 "<!ENTITY sgml \"Standard Generalized Markup Language\">\n"
    //                 "<!ENTITY pxa \"Professional XML Authoring\">\n"
    //                 "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\n"
    //                 "<!ENTITY % BLOCK \"(PARA|LIST)+\">\n"
    //                 "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\n"
    //                 "<!NOTATION GIF SYSTEM \"\">\n"
    //                 "<!NOTATION JPG SYSTEM \"\">\n"
    //                 "<!NOTATION BMP SYSTEM \"\">\n"
    //                 "]>\n"
    //                 "<REPORT> </REPORT>\n";
    //     BufferSource xmlSource(xmlString);
    //     std::unique_ptr<XNode> xNodeRoot=xml.parse(xmlSource);
    //     REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["GIF"].name == "SYSTEM");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["GIF"].value.parsed == "\"\"");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["JPG"].name == "SYSTEM");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["JPG"].value.parsed == "\"\"");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["BMP"].name == "SYSTEM");
    //     REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).notations["BMP"].value.parsed == "\"\"");
    //   }
}