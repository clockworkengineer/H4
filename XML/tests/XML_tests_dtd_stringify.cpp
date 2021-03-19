//
// Unit Tests: XML
//
// Description:
//
// ================
// Test definitions
// ================
#include "XML_tests.hpp"
// ====================
// JSON class namespace
// ====================
using namespace H4;
// ==========
// Test cases
// ==========
TEST_CASE("Stringify XML with DTD both internal and external", "[XML][Stringify][DTD]")
{
    XML xml;
    SECTION("Stringify XML with internal DTD", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
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
                            "</note>\n");
    }
    SECTION("Stringify XML with external (SYSTEM) DTD", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                            "<!DOCTYPE note SYSTEM \"Note.dtd\">\n"
                            "<note>\n"
                            "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                            "<body>Don't forget me this weekend!</body>\n"
                            "</note>\n");
    }
    SECTION("Stringify XML with external (PUBLIC) DTD", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                            "<!DOCTYPE note PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                            "<note>\n"
                            "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                            "<body>Don't forget me this weekend!</body>\n"
                            "</note>\n");
    }
    SECTION("Stringify XML with DTD with !ENTITY definitions and uses", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                            "<!DOCTYPE note [\n"
                            "<!ENTITY nbsp \"&#xA0;\">\n"
                            "<!ENTITY writer \"Writer: Donald Duck.\">\n"
                            "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
                            "<note>\n"
                            "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                            "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
                            "</note>\n");
    }
    SECTION("Stringify XML with external file DTD.", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                            "<!DOCTYPE note SYSTEM \"Note.dtd\">\n"
                            "<note>\n"
                            "<to>Tove"
                            "</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body>\n"
                            "</note>\n");
    }
    SECTION("Stringify XML with external URL DTD.", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                            "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
                            " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                            "<html></html>\n");
    }
    SECTION("Stringify XML with internal DTD with attributes.", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
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
                            "<TVSCHEDULE></TVSCHEDULE>\n");
    }
    SECTION("Stringify XML with internal DTD with parameter entities to parse.", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                            "<!DOCTYPE REPORT [\n"
                            "<!ELEMENT REPORT (residence|apartment|office|shop)*>\n"
                            "<!ELEMENT residence (%area;, %contact;)>\n"
                            "<!ELEMENT apartment (%area;, %contact;)>\n"
                            "<!ELEMENT office (%area;, %contact;)>\n"
                            "<!ELEMENT shop (%area;, %contact;)>\n"
                            "<!ENTITY % area \"name, street, pincode, city\">\n"
                            "<!ENTITY % contact \"phone\"> ]>\n"
                            "<REPORT></REPORT>\n");
    }
    SECTION("Stringify XML with internal DTD with !NOTATION.", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                            "<!DOCTYPE REPORT [\n"
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
                            "<!NOTATION GIF SYSTEM \"\">\n"
                            "<!NOTATION JPG SYSTEM \"\">\n"
                            "<!NOTATION BMP SYSTEM \"\">\n"
                            "]>\n"
                            "<REPORT> </REPORT>\n");
    }
    SECTION("Stringify XML with internal DTD containing comments.", "[XML][Stringify][DTD]")
    {
        checkStringify(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
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
                            "</note>\n");
    }
}