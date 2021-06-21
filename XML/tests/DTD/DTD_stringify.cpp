//
// Unit Tests: XML
//
// Description: Unit tests for DTD stringification.
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
TEST_CASE("Stringify XML with DTD both internal and external", "[XML][DTD][Stringify]")
{
    SECTION("Stringify XML with internal DTD", "[XML][DTD][Stringify]")
    {
        checkStringify("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
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
    SECTION("Stringify XML with external (SYSTEM) DTD", "[XML][DTD][Stringify]")
    {
        checkStringify("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                       "<!DOCTYPE note SYSTEM \"./testData/note001.dtd\">\n"
                       "<note>\n"
                       "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                       "<body>Don't forget me this weekend!</body>\n"
                       "</note>\n");
    }
    SECTION("Stringify XML with external (PUBLIC) DTD", "[XML][DTD][Stringify]")
    {
        checkStringify("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                       "<!DOCTYPE note PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                       "<note>\n"
                       "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                       "<body>Don't forget me this weekend!</body>\n"
                       "</note>\n");
    }
    SECTION("Stringify XML with DTD with entity definitions and uses", "[XML][DTD][Stringify]")
    {
        checkStringify("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                       "<!DOCTYPE note [\n"
                       "<!ENTITY nbsp \"&#xA0;\">\n"
                       "<!ENTITY writer \"Writer: Donald Duck.\">\n"
                       "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
                       "<note>\n"
                       "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                       "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
                       "</note>\n"); 
    }
    SECTION("Stringify XML with external file DTD.", "[XML][DTD][Stringify]")
    {
        checkStringify("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                       "<!DOCTYPE note SYSTEM \"./testData/note001.dtd\">\n"
                       "<note>\n"
                       "<to>Tove"
                       "</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body>\n"
                       "</note>\n");
    }
    SECTION("Stringify XML with external URL DTD.", "[XML][DTD][Stringify]")
    {
        checkStringify("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                       "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
                       " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                       "<html></html>\n");
    }
    SECTION("Stringify XML with internal DTD with attributes.", "[XML][DTD][Stringify]")
    {
        checkStringify("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
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
                       "<TVSCHEDULE></TVSCHEDULE>\n");
    }
    SECTION("Stringify XML with internal DTD containing comments.", "[XML][DTD][Stringify]")
    {
        checkStringify("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
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
                       "</note>\n");
    }
}