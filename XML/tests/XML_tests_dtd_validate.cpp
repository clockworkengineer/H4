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
TEST_CASE("Parse XML with various DTD validation issues.", "[XML][DTD][Validate]")
{
  XML xml;
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 8] Element <child1> does not contain just any parsable data.");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 9] <note> element does not conform to the content specication (to,from,heading,body).");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 9] <note> element does not conform to the content specication (to,from,heading,body).");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 10] <notes> element does not conform to the content specication (note)+.");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 8] <album> element does not conform to the content specication ( title, ( songTitle, duration )+ ).");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 8] <album> element does not conform to the content specication ( title, ( songTitle, duration )+ ).");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 23] <class> element does not conform to the content specication ( number, ( instructor | assistant+ ),( credit | noCredit ) ).");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 24] <donutBox> element does not conform to the content specication ( jelly?, lemon*,( ( creme | sugar )+ | glazed ) ).");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 9] Element <child2> is not empty.");
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
    XMLObject xmlObject = xml.parse(xmlSource);
    REQUIRE_NOTHROW(xml.validate(xmlObject));
  }
  // SECTION("XML with a DTD that specifies elements that are marked as having mixed content.", "[XML][DTD][Validate]")
  // {
  //   xmlString = "<?xml version=\"1.0\"?>\n"
  //               "<!-- Fig. B.5 : mixed.xml-->\n"
  //               "<!-- Mixed content type elements -->\n"
  //               "<!DOCTYPE format [\n"
  //               "<!ELEMENT format ( #PCDATA | bold | italic )*>\n"
  //               "<!ELEMENT bold ( #PCDATA )>\n"
  //               "<!ELEMENT italic ( #PCDATA )>\n"
  //               "]>\n"
  //               "<format>\n"
  //               "Book catalog entry:\n"
  //               "<bold>XML</bold>\n"
  //               "<italic>XML How to Program</italic>\n"
  //               "This book carefully explains XML-based systems development."
  //               "</format>";
  //   BufferSource xmlSource(xmlString);
  //   XMLObject xmlObject = xml.parse(xmlSource);
  //   REQUIRE_NOTHROW(xml.validate(xmlObject));
  // }

  // SECTION("XML with a DTD that contains an illegal mixed content specification (uses ',').", "[XML][DTD][Validate]")
  // {
  //   xmlString = "<?xml version=\"1.0\"?>\n"
  //               "<!-- Fig. B.5 : mixed.xml-->\n"
  //               "<!-- Mixed content type elements -->\n"
  //               "<!DOCTYPE format [\n"
  //               "<!ELEMENT format ( #PCDATA | bold , italic )*>\n"
  //               "<!ELEMENT bold ( #PCDATA )>\n"
  //               "<!ELEMENT italic ( #PCDATA )>\n"
  //               "]>\n"
  //               "<format>\n"
  //               "Book catalog entry:\n"
  //               "<bold>XML</bold>\n"
  //               "<italic>XML How to Program</italic>\n"
  //               "This book carefully explains XML-based systems development."
  //               "</format>";
  //   BufferSource xmlSource(xmlString);
  //   XMLObject xmlObject = xml.parse(xmlSource);
  //   REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 9] Invalid mixed content specification.");
  // }

  // SECTION("XML with a DTD that contains an illegal mixed content specification (#PCDATA doesnt come first).", "[XML][DTD][Validate]")
  // {
  //   xmlString = "<?xml version=\"1.0\"?>\n"
  //               "<!-- Fig. B.5 : mixed.xml-->\n"
  //               "<!-- Mixed content type elements -->\n"
  //               "<!DOCTYPE format [\n"
  //               "<!ELEMENT format ( bold | italic | #PCDATA)*>\n"
  //               "<!ELEMENT bold ( #PCDATA )>\n"
  //               "<!ELEMENT italic ( #PCDATA )>\n"
  //               "]>\n"
  //               "<format>\n"
  //               "Book catalog entry:\n"
  //               "<bold>XML</bold>\n"
  //               "<italic>XML How to Program</italic>\n"
  //               "This book carefully explains XML-based systems development."
  //               "</format>";
  //   BufferSource xmlSource(xmlString);
  //   XMLObject xmlObject = xml.parse(xmlSource);
  //   REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 9] Invalid mixed content specification.");
  // }
  //   SECTION("XML with a DTD that contains an illegal mixed content specification (does not end with '*').", "[XML][DTD][Validate]")
  // {
  //   xmlString = "<?xml version=\"1.0\"?>\n"
  //               "<!-- Fig. B.5 : mixed.xml-->\n"
  //               "<!-- Mixed content type elements -->\n"
  //               "<!DOCTYPE format [\n"
  //               "<!ELEMENT format ( #PCDATA | bold | italic )+>\n"
  //               "<!ELEMENT bold ( #PCDATA )>\n"
  //               "<!ELEMENT italic ( #PCDATA )>\n"
  //               "]>\n"
  //               "<format>\n"
  //               "Book catalog entry:\n"
  //               "<bold>XML</bold>\n"
  //               "<italic>XML How to Program</italic>\n"
  //               "This book carefully explains XML-based systems development."
  //               "</format>";
  //   BufferSource xmlSource(xmlString);
  //   XMLObject xmlObject = xml.parse(xmlSource);
  //   REQUIRE_THROWS_WITH(xml.validate(xmlObject), "XML Validation Error [Line: 9] Invalid mixed content specification.");
  // }
}