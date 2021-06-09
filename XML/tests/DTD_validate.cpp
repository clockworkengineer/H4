//
// Unit Tests: XML
//
// Description: Unit tests for XML DTD validation.
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
  std::string xmlString;
  SECTION("XML with a DTD that specifies elements that do not contain parsable data.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "]>\n"
                "<root>\n"
                "<child1>contents</child1>\n"
                "<child1></child1>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 8] Element <child1> does not contain just any parsable data.");
  }
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
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 9] <note> element does not conform to the content specification (to,from,heading,body).");
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
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 9] <note> element does not conform to the content specification (to,from,heading,body).");
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
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 10] <notes> element does not conform to the content specification (note)+.");
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
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
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
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
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
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that does not contain any songs.", "[XML][DTD][Validate]")
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
                "</album>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 8] <album> element does not conform to the content specification ( title, ( songTitle, duration )+ ).");
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
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 8] <album> element does not conform to the content specification ( title, ( songTitle, duration )+ ).");
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
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
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
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that uses '?' (zero or one time) operator and has XML conatins occupied and empty seats.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE plane [\n"
                "<!ELEMENT plane ( seats )>\n"
                "<!ELEMENT seats ( seat + )>\n"
                "<!ELEMENT seat ( passenger ? )>\n"
                "<!ELEMENT passenger (#PCDATA)>\n"
                "]>\n"
                "<plane>\n"
                "<seats>\n"
                "<seat></seat>\n"
                "<seat>\n"
                "<passenger>John Smith</passenger>\n"
                "</seat>\n"
                "</seats>\n"
                "</plane>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that uses '|' (or) operator and has valid XML for it.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE timetable [\n"
                "<!ELEMENT timetable (class)+ >\n"
                "<!ELEMENT class ( number, ( instructor | assistant+ ),( credit | noCredit ) )>\n"
                "<!ELEMENT number (#PCDATA)>\n"
                "<!ELEMENT instructor (#PCDATA)>\n"
                "<!ELEMENT assistant (#PCDATA)>\n"
                "<!ELEMENT credit (#PCDATA)>\n"
                "<!ELEMENT noCredit (#PCDATA)>\n"
                "]>\n"
                "<timetable>\n"
                "<class>\n"
                "<number>123</number>\n"
                "<instructor>Dr. Harvey Deitel</instructor>\n"
                "<credit>4</credit>\n"
                "</class>\n"
                "<class>\n"
                "<number>456</number>\n"
                "<assistant>Tem Nieto</assistant>\n"
                "<assistant>Paul Deitel</assistant>\n"
                "<credit>3</credit>\n"
                "</class>\n"
                "</timetable>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that uses '|' (or) operator and has a class without an assistant/instrucor.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE timetable [\n"
                "<!ELEMENT timetable (class)+ >\n"
                "<!ELEMENT class ( number, ( instructor | assistant+ ),( credit | noCredit ) )>\n"
                "<!ELEMENT number (#PCDATA)>\n"
                "<!ELEMENT instructor (#PCDATA)>\n"
                "<!ELEMENT assistant (#PCDATA)>\n"
                "<!ELEMENT credit (#PCDATA)>\n"
                "<!ELEMENT noCredit (#PCDATA)>\n"
                "]>\n"
                "<timetable>\n"
                "<class>\n"
                "<number>123</number>\n"
                "<instructor>Dr. Harvey Deitel</instructor>\n"
                "<credit>4</credit>\n"
                "</class>\n"
                "<class>\n"
                "<number>456</number>\n"
                "<assistant>Tem Nieto</assistant>\n"
                "<assistant>Paul Deitel</assistant>\n"
                "<credit>3</credit>\n"
                "</class>\n"
                "<class>\n"
                "<number>885</number>\n"
                "<credit>3</credit>\n"
                "</class>\n"
                "</timetable>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 23] <class> element does not conform to the content specification ( number, ( instructor | assistant+ ),( credit | noCredit ) ).");
  }
  SECTION("XML with a DTD that uses all of the content specification operators and has valid XML for it.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE donutDelivery [\n"
                "<!ELEMENT donutDelivery (donutBox)+ >\n"
                "<!ELEMENT donutBox ( jelly?, lemon*,( ( creme | sugar )+ | glazed ) )>\n"
                "<!ELEMENT jelly (#PCDATA)>\n"
                "<!ELEMENT lemon (#PCDATA)>\n"
                "<!ELEMENT creme (#PCDATA)>\n"
                "<!ELEMENT sugar (#PCDATA)>\n"
                "<!ELEMENT glazed (#PCDATA)>\n"
                "]>\n"
                "<donutDelivery>\n"
                "<donutBox>\n"
                "<jelly>grape</jelly>\n"
                "<lemon>half-sour</lemon>\n"
                "<lemon>sour</lemon>\n"
                "<lemon>half-sour</lemon>\n"
                "<glazed>chocolate</glazed>\n"
                "</donutBox>\n"
                "<donutBox>\n"
                "<sugar>semi-sweet</sugar>\n"
                "<creme>whipped</creme>\n"
                "<sugar>sweet</sugar>\n"
                "</donutBox>\n"
                "</donutDelivery>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that uses all of the content specification operators and has XML with an empty box.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE donutDelivery [\n"
                "<!ELEMENT donutDelivery (donutBox)+ >\n"
                "<!ELEMENT donutBox ( jelly?, lemon*,( ( creme | sugar )+ | glazed ) )>\n"
                "<!ELEMENT jelly (#PCDATA)>\n"
                "<!ELEMENT lemon (#PCDATA)>\n"
                "<!ELEMENT creme (#PCDATA)>\n"
                "<!ELEMENT sugar (#PCDATA)>\n"
                "<!ELEMENT glazed (#PCDATA)>\n"
                "]>\n"
                "<donutDelivery>\n"
                "<donutBox>\n"
                "<jelly>grape</jelly>\n"
                "<lemon>half-sour</lemon>\n"
                "<lemon>sour</lemon>\n"
                "<lemon>half-sour</lemon>\n"
                "<glazed>chocolate</glazed>\n"
                "</donutBox>\n"
                "<donutBox>\n"
                "<sugar>semi-sweet</sugar>\n"
                "<creme>whipped</creme>\n"
                "<sugar>sweet</sugar>\n"
                "</donutBox>\n"
                "<donutBox></donutBox>\n"
                "</donutDelivery>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 24] <donutBox> element does not conform to the content specification ( jelly?, lemon*,( ( creme | sugar )+ | glazed ) ).");
  }
  SECTION("XML with a DTD that specifies elements that are empty.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1 | child2)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "<!ELEMENT child2 EMPTY>\n"
                "]>\n"
                "<root>\n"
                "<child1>contents</child1>\n"
                "<child2></child2>\n"
                "<child2/>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that specifies elements that are empty but contain data.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1 | child2)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "<!ELEMENT child2 EMPTY>\n"
                "]>\n"
                "<root>\n"
                "<child1>contents</child1>\n"
                "<child2>contents</child2>\n"
                "<child2/>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 9] Element <child2> is not empty.");
  }
  SECTION("XML with a DTD that specifies elements that are marked as any.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1 | child2)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "<!ELEMENT child2 ANY>\n"
                "<!ELEMENT child3 EMPTY>\n"
                "]>\n"
                "<root>\n"
                "<child1>contents</child1><child2><child3/></child2>\n"
                "<child2/>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that specifies elements that are marked as having mixed content.", "[XML][DTD][Validate]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!-- Fig. B.5 : mixed.xml-->\n"
                "<!-- Mixed content type elements -->\n"
                "<!DOCTYPE format [\n"
                "<!ELEMENT format ( #PCDATA | bold | italic )*>\n"
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
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
}
TEST_CASE("Parse XML with various DTD attribute validation issues.", "[XML][DTD][Validate][Attribute]")
{
  std::string xmlString;
  SECTION("XML with a DTD that specifies a required attribute that is missing.", "[XML][DTD][Validate][Attribute]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1|child2|child3)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "<!ELEMENT child2 (#PCDATA)>\n"
                "<!ELEMENT child3 (#PCDATA)>\n"
                "<!ATTLIST child2 number CDATA #REQUIRED>\n"
                "]>\n"
                "<root>\n"
                "<child1>contents 1</child1>\n"
                "<child2>contents 2</child2>\n"
                "<child3>contents 3</child3>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 11] Required attribute 'number' missing for element <child2>.");
  }
  SECTION("XML with a DTD that specifies a required attribute that is present.", "[XML][DTD][Validate][Attribute]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1|child2|child3)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "<!ELEMENT child2 (#PCDATA)>\n"
                "<!ELEMENT child3 (#PCDATA)>\n"
                "<!ATTLIST child2 number CDATA #REQUIRED>\n"
                "]>\n"
                "<root>\n"
                "<child1>contents 1</child1>\n"
                "<child2 number=\"900000\">contents 2</child2>\n"
                "<child3>contents 3</child3>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that specifies a implied attribute that is present/not present in an element.", "[XML][DTD][Validate][Attribute]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1|child2|child3)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "<!ELEMENT child2 (#PCDATA)>\n"
                "<!ELEMENT child3 (#PCDATA)>\n"
                "<!ATTLIST child2 number CDATA #IMPLIED>\n"
                "]>\n"
                "<root>\n"
                "<child1>contents 1</child1>\n"
                "<child2 number=\"900000\">contents 2</child2>\n"
                "<child2>contents 2</child2>\n"
                "<child3>contents 3</child3>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that specifies a fixed attribute trying to have it to more than one value.", "[XML][DTD][Validate][Attribute]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1|child2|child3)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "<!ELEMENT child2 (#PCDATA)>\n"
                "<!ELEMENT child3 (#PCDATA)>\n"
                "<!ATTLIST child2 number CDATA #FIXED \"2001\">\n"
                "]>\n"
                "<root>\n"
                "<child1>contents 1</child1>\n"
                "<child2 number=\"2001\">contents 2</child2>\n"
                "<child2 number =\"2002\">contents 2</child2>\n"
                "<child3>contents 3</child3>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 12] Element <child2> attribute 'number' is '2002' instead of '2001'.");
  }
  SECTION("XML with a DTD that specifies a fixed element attribute that has not been assigned in the data but should be there for the application. ", "[XML][DTD][Validate][Attribute]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1|child2|child3)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "<!ELEMENT child2 (#PCDATA)>\n"
                "<!ELEMENT child3 (#PCDATA)>\n"
                "<!ATTLIST child2 number CDATA #FIXED \"2001\">\n"
                "]>\n"
                "<root>\n"
                "<child1>contents 1</child1>\n"
                "<child2 number=\"2001\">contents 2</child2>\n"
                "<child2>contents 2</child2>\n"
                "<child3>contents 3</child3>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
    REQUIRE(xml.m_prolog[0].getNodeType() == XMLNodeType::root);
    REQUIRE(xml.m_prolog[0][1].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0][1].getAttribute("number").value.parsed == "2001");
    REQUIRE(xml.m_prolog[0][2].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0][2].getAttribute("number").value.parsed == "2001");
  }
  SECTION("XML with a DTD that specifies an element attribute that has a default value if it is not defined. ", "[XML][DTD][Validate][Attribute]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root [\n"
                "<!ELEMENT root (child1|child2|child3)+ >\n"
                "<!ELEMENT child1 (#PCDATA)>\n"
                "<!ELEMENT child2 (#PCDATA)>\n"
                "<!ELEMENT child3 (#PCDATA)>\n"
                "<!ATTLIST child2 number CDATA \"2001\">\n"
                "]>\n"
                "<root>\n"
                "<child1>contents 1</child1>\n"
                "<child2 number=\"2002\">contents 2</child2>\n"
                "<child2>contents 2</child2>\n"
                "<child3>contents 3</child3>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_NOTHROW(xml.validate());
    REQUIRE(xml.m_prolog[0].getNodeType() == XMLNodeType::root);
    REQUIRE(xml.m_prolog[0][1].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0][1].getAttribute("number").value.parsed == "2002");
    REQUIRE(xml.m_prolog[0][2].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0][2].getAttribute("number").value.parsed == "2001");
  }
  SECTION("Validate XML with DTD that cotains a enumeration attribute gender with a default value if 'F'.", "[XML][Valid][DTD]")
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
    REQUIRE_NOTHROW(xml.validate());
    REQUIRE(XMLNodeRef<XMLNode>(*xml.m_prolog.children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xml.m_dtd.m_elements.find("person") != xml.m_dtd.m_elements.end());
    REQUIRE(xml.m_dtd.m_elements["person"].attributes.size() == 1);
    REQUIRE(xml.m_dtd.m_elements["person"].attributes[0].name == "gender");
    REQUIRE(xml.m_dtd.m_elements["person"].attributes[0].type == "(M|F)");
    REQUIRE(xml.m_dtd.m_elements["person"].attributes[0].value.parsed == "F");
    REQUIRE(xml.m_dtd.m_name == "queue");
    REQUIRE(xml.m_dtd.m_name == XMLNodeRef<XMLNodeElement>(xml.m_prolog[0]).name);
    REQUIRE(xml.m_prolog[0][0].name == "person");
    REQUIRE(xml.m_prolog[0][0].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0][0].getAttribute("gender").value.parsed == "M");
    REQUIRE(xml.m_prolog[0][1].name == "person");
    REQUIRE(xml.m_prolog[0][1].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0][1].getAttribute("gender").value.parsed == "F");
  }
  SECTION("Validate XML with DTD that cotains a enumeration attribute gender that is not valid.", "[XML][Valid][DTD]")
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
                "<person gender=\"B\"><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
                "</queue>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 11] Element <person> attribute 'gender' contains invalid enumeration value 'B'.");
  }
  SECTION("Validate XML with DTD that specifies the use of an ID attribute type that has a duplicate value.", "[XML][Valid][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE collection ["
                "<!ELEMENT collection (item)+>\n"
                "<!ELEMENT item (#PCDATA)>\n"
                "<!ATTLIST item itemID ID #REQUIRED >\n"
                "]>\n"
                "<collection>\n"
                "<item itemID=\"i001\">item descripton</item>\n"
                "<item itemID=\"i002\">item descripton</item>\n"
                "<item itemID=\"i003\">item descripton</item>\n"
                "<item itemID=\"i004\">item descripton</item>\n"
                "<item itemID=\"i001\">item descripton</item>\n"
                "</collection>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 11] Element <item> ID attribute 'itemID' is not unique.");
  }
  SECTION("Validate XML that has an element with an ID attribute value that does not start with a letter, '_' or ':'.", "[XML][Valid][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE collection ["
                "<!ELEMENT collection (item)+>\n"
                "<!ELEMENT item (#PCDATA)>\n"
                "<!ATTLIST item itemID ID #REQUIRED >\n"
                "]>\n"
                "<collection>\n"
                "<item itemID=\"i001\">item descripton</item>\n"
                "<item itemID=\"i002\">item descripton</item>\n"
                "<item itemID=\"i003\">item descripton</item>\n"
                "<item itemID=\"i004\">item descripton</item>\n"
                "<item itemID=\"005\">item descripton</item>\n"
                "<item itemID=\"i006\">item descripton</item>\n"
                "</collection>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 11] Element <item> ID attribute 'itemID' that is invalid.");
  }
  SECTION("Validate XML that has a missing ID referenced  by an IDREF.", "[XML][Valid][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE collection ["
                "<!ELEMENT collection (item|itemOnLoan)+>\n"
                "<!ELEMENT item (#PCDATA)>\n"
                "<!ELEMENT itemOnLoan (#PCDATA)>\n"
                "<!ATTLIST item itemID ID #REQUIRED >\n"
                "<!ATTLIST itemOnLoan itemLoanedID IDREF #IMPLIED >\n"
                "]>\n"
                "<collection>\n"
                "<item itemID=\"i001\">item descripton</item>\n"
                "<item itemID=\"i002\">item descripton</item>\n"
                "<item itemID=\"i003\">item descripton</item>\n"
                "<item itemID=\"i004\">item descripton</item>\n"
                "<item itemID=\"i005\">item descripton</item>\n"
                "<item itemID=\"i006\">item descripton</item>\n"
                "<itemOnLoan itemLoanedID=\"i006\">reason for loan</itemOnLoan>\n"
                "<itemOnLoan itemLoanedID=\"i010\">reason for loan</itemOnLoan>\n"
                "</collection>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 18] IDREF attribute 'i010' does not reference any element with the ID.");
  }
  SECTION("Validate XML that has a an IDREF attribute value that does not start with a letter, '_' or ':'.", "[XML][Valid][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE collection [\n"
                "<!ELEMENT collection (item|itemsOnLoan)+>\n"
                "<!ELEMENT item (#PCDATA)>\n"
                "<!ELEMENT itemsOnLoan (#PCDATA)>\n"
                "<!ATTLIST item itemID ID #REQUIRED >\n"
                "<!ATTLIST itemsOnLoan itemsLoanedIDs IDREFS #IMPLIED >\n"
                "]>\n"
                "<collection>\n"
                "<item itemID=\"i001\">item descripton</item>\n"
                "<item itemID=\"i002\">item descripton</item>\n"
                "<item itemID=\"i003\">item descripton</item>\n"
                "<item itemID=\"i004\">item descripton</item>\n"
                "<item itemID=\"i005\">item descripton</item>\n"
                "<item itemID=\"i006\">item descripton</item>\n"
                "<itemsOnLoan itemsLoanedIDs=\"i006 i003 005\">reason for loan</itemsOnLoan>\n"
                "</collection>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 16] Element <itemsOnLoan> ID attribute 'itemsLoanedIDs' that is invalid.");
  }
  SECTION("Validate XML that has a an IDREF attribute value that does not exist on an element.", "[XML][Valid][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE collection [\n"
                "<!ELEMENT collection (item|itemsOnLoan)+>\n"
                "<!ELEMENT item (#PCDATA)>\n"
                "<!ELEMENT itemsOnLoan (#PCDATA)>\n"
                "<!ATTLIST item itemID ID #REQUIRED >\n"
                "<!ATTLIST itemsOnLoan itemsLoanedIDs IDREFS #IMPLIED >\n"
                "]>\n"
                "<collection>\n"
                "<item itemID=\"i001\">item descripton</item>\n"
                "<item itemID=\"i002\">item descripton</item>\n"
                "<item itemID=\"i003\">item descripton</item>\n"
                "<item itemID=\"i004\">item descripton</item>\n"
                "<item itemID=\"i005\">item descripton</item>\n"
                "<item itemID=\"i006\">item descripton</item>\n"
                "<itemsOnLoan itemsLoanedIDs=\"i006 i003 i008\">reason for loan</itemsOnLoan>\n"
                "</collection>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE_THROWS_WITH(xml.validate(), "XML Validation Error [Line: 18] IDREF attribute 'i008' does not reference any element with the ID.");
  }
}