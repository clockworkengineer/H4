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
TEST_CASE("Use XML object to parse XML declaration", "[XML][Parse][Declaration]")
{
  std::string xmlString;
  SECTION("Parse XML declaration. ", "[XML]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "no");
  }
  SECTION("Parse XML declaration with unsupported version. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.2\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 1] Unsupported version number 1.2.");
  }
  SECTION("Parse XML declaration with unsupported encoding. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-32\" standalone=\"no\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 1] Unsupported encoding UTF-32 specified.");
  }
  SECTION("Parse XML declaration with inavlid standalone value. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"maybe\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 1] Invalid standalone value of 'maybe'.");
  }
  SECTION("Parse XML declaration with extra content after root element. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root></root>extra content.\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 18] Extra content at the end of document.");
  }
  SECTION("Parse version 1.0 encoding == UTF-8 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?> "
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-16");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "yes");
  }
  SECTION("Parse version 1.0 encoding == UTF-8 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" standalone=\"yes\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "yes");
  }
  SECTION("Check declaration contains at least version attribute.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml?> <root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 8] Version missing from declaration.");
  }
  SECTION("Parse empty XML declaration no end tag ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\">\n"
                "<root></root>";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 20] Declaration end tag not found.");
  }
  SECTION("Parse empty XML declaration no root element.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 2] Missing root element.");
  }
  SECTION("Parse empty XML declaration with content before root element.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "content<root></root>";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 2] Content detected before root element.");
  }
  SECTION("Parse empty XML declaration no closing root tag ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 3 Column: 2] Missing closing tag.");
  }
  SECTION("Parse empty XML declaration no closing child tag ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<root><child></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 25] Missing closing tag.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" standalone=\"no\" encoding=\"UTF-8\"?>\n"
                " <root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 1] Incorrect order for version, encoding and standalone attributes.");
  }
  SECTION("Parse XML with declaration but no root element", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 2] Missing root element.");
  }
}
TEST_CASE("Checks for tag names", "[XML][Parse][Tags]")
{
  std::string xmlString;
  SECTION("Allow tag names to with alpha numeric characters and '.','_', '-' ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.>\n"
                "</abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Tag names are case sensitive", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook> </addressbook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 21] Missing closing tag.");
  }
  SECTION("Incorrect closing tag ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook> </Address>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 28] Missing closing tag.");
  }
  SECTION("Self closing tag ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<root><AddressBook/></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Tag starts with a '.' ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?> "
                "<.AddressBook> </.AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 39] Invalid name '.AddressBook' encountered.");
  }
  SECTION("Tag starts with a '-' ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?> <-AddressBook> </-AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 39] Invalid name '-AddressBook' encountered.");
  }
  SECTION("Tag starts with a numeric digit", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?> <0AddressBook> </0AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 39] Invalid name '0AddressBook' encountered.");
  }
  SECTION("Tag starts with a xml etc", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                " <xmlAddressBook> </xmlAddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 21] Invalid name 'xmlAddressBook' encountered.");
  }
  SECTION("Tag starts with a XML etc", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?> <XMLAddressBook> </XMLAddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 41] Invalid name 'XMLAddressBook' encountered.");
  }
  SECTION("Tag starts with a Xml etc", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version=\"1.0\"?> <XmlAddressBook> </XmlAddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 41] Invalid name 'XmlAddressBook' encountered.");
  }
}
TEST_CASE("Use XML object to parse declaration, root element and check parsed information ", "[XML][Parse][Root]")
{
  std::string xmlString;
  SECTION("Empty root element <contact-info> ", "[XML][Parse]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<contact-info></contact-info>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "no");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).children.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).name == "contact-info");
  }
  SECTION("Empty root element <AddressBook> ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook> </AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "no");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).children.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).name == "AddressBook");
  }
  SECTION("Root element <AddressBook> and one child <Address> with contents ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook>\n"
                "<Address>    This is some contents    </Address>\n"
                "</AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "no");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).children.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).getContents() == "    This is some contents    ");
  }
  SECTION("Root element <AddressBook> with multiple sibling <Address> elements and contents ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook>\n"
                "<Address>\n"
                "    This is some contents 1   "
                "</Address>\n"
                "<Address>\n"
                "    This is some contents 2   "
                "</Address>\n"
                "<Address>\n"
                "    This is some contents 3   "
                "</Address>\n"
                "</AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "no");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).children.size() == 7);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).getContents() == "\n    This is some contents 1   ");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).getContents() == "\n    This is some contents 2   ");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][2]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][2]).getContents() == "\n    This is some contents 3   ");
  }
}
TEST_CASE("Sample XML files to read and parse.", "[XML][Parse]")
{
  auto testFile = GENERATE(values<std::string>({"./testData/testfile001.xml",
                                                "./testData/testfile002.xml",
                                                "./testData/testfile003.xml",
                                                "./testData/testfile005.xml",
                                                "./testData/testfile007.xml",
                                                "./testData/testfile010.xml",
                                                "./testData/testfile011.xml"}));
  SECTION("Load file into buffer and parse.", "[XML][Parse]")
  {
    BufferSource xmlSource(readXMLFromFileUTF8(testFile));
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse XML from file.", "[XML][Parse]")
  {
    FileSource xmlSource(testFile);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
}
TEST_CASE("Parse XML elements with attached attributes", "[XML][Parse][Attributes]")
{
  std::string xmlString;
  SECTION("Root attribute with one attached attribute number", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook number='15'>\n"
                "</AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "no");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).children.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].name == "number");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].value.parsed == "15");
  }
  SECTION("Root attribute with 3 attached attributes number, away, flat", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                " <AddressBook number='15' away=\"yes\" flat='no'>\n"
                " </AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "no");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).children.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].name == "number");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].value.parsed == "15");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[1].name == "away");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[1].value.parsed == "yes");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[2].name == "flat");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[2].value.parsed == "no");
  }
  SECTION("Empty elements with attributes are allowed.", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook number='15'>\n"
                "</AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Element with duplicate attributes not allowed.", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook number='15' colour='red' number='16'>\n"
                " </AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 54] Attribute defined more than once within start tag.");
  }
}
TEST_CASE("Parse XML elements with comments", "[XML][Parse][Comments]")
{
  std::string xmlString;
  SECTION("A simple single line comment", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment -->\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Multiple single line comments beifre root tag", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment -->\n"
                " <!-- A single line comment -->\n"
                " <!-- A single line comment -->\n"
                " <root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Multiple comments inside root element and between its children ", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook>\n"
                "<!--Address one -->\n"
                "<Address>    This is some contents 1   </Address>\n"
                "<!--Address two -->\n"
                "<Address>    This is some contents 2   </Address>\n"
                "<!--Address three -->\n"
                "<Address>    This is some contents 3   </Address>\n"
                "</AddressBook>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[0].value.parsed == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[1].value.parsed == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog).attributes[2].value.parsed == "no");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).children.size() == 13);
    REQUIRE(XNodeRef<XNodeComment>(*xml.m_prolog[0].children[1]).comment == "Address one ");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).getContents() == "    This is some contents 1   ");
    REQUIRE(XNodeRef<XNodeComment>(*xml.m_prolog[0].children[5]).comment == "Address two ");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).getContents() == "    This is some contents 2   ");
    REQUIRE(XNodeRef<XNodeComment>(*xml.m_prolog[0].children[9]).comment == "Address three ");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][2]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][2]).getContents() == "    This is some contents 3   ");
  }
  SECTION("A single comment after root element", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root></root>\n"
                "<!-- A single line comment --> ";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("A simple multi line comment", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment\n"
                " another line\n"
                " another line\n"
                " and another line\n"
                "--> <root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("A simple comment within element content", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root>Test<!-- a simple comment -->Test"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getContents() == "TestTest");
  }
  SECTION("A simple comment within element contents and content remains intact", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<root>Test  <!-- a simple comment -->  Test"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getContents() == "Test    Test");
  }
  SECTION("A simple single line comment containing -- is illegal", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment-- --> "
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 30] Missing closing '>' for comment line.");
  }
  SECTION("A simple single line comment ending with -- is illegal", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                "<!-- A single line comment ---> "
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 2 Column: 31] Missing closing '>' for comment line.");
  }
}
TEST_CASE("Parse XML with Unicode character in element names, attributes, comments, character data, and processing instructions. ", "[XML][Parse][Unicode]")
{
  std::string xmlString;
  SECTION("Japanese characters", "[XML][Parse][[Unicode]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"utf-8\"?><config><start_text>転送</start_text></config>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Well-formed XML document including Chinese, Armenian and Cyrillic characters", "[XML][Parse][[Unicode]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<俄语 լեզու=\"ռուսերեն\">данные</俄语>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).name == "俄语");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].name == "լեզու");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].value.parsed == "ռուսերեն");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getContents() == "данные");
  }
}
TEST_CASE("Check the pasring of character entities/reference.", "[XML][Parse][Entities]")
{
  std::string xmlString;
  SECTION("Parse entity &amp; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                "<root> &amp; </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getContents() == " & ");
  }
  SECTION("Parse entity &quot; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root> &quot; </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getContents() == " \" ");
  }
  SECTION("Parse entities &apos; &lt; &gt; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root> &apos; &lt; &gt; </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getContents() == " ' < > ");
  }
  SECTION("Parse reference &#x00A5; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root> &#x00A5; </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getContents() == " ¥ ");
  }
  SECTION("Parse reference &#163; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                "<root> &#163; </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getContents() == " £ ");
  }
  SECTION("Parse entity &amp;&quot;&apos;&gt;&lt; in attribute value", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root attr1=\" &amp;&quot;&apos;&gt;&lt; \">\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].name == "attr1");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].value.parsed == " &#x26;\"\'>&#x3C; ");
  }
  SECTION("Parse references &#x00A5;&#163 in attribute value", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root attr1=\" &#x00A5;&#163; \"></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].name == "attr1");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).attributes[0].value.parsed == " ¥£ ");
  }
}
TEST_CASE("Check the parsing of XML containing program instructions", "[XML][Parse][PI]")
{
  std::string xmlString;
  SECTION("Parse XML containing PI after declaration", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse XML containing multiple PI after declaration", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?> "
                "<?display table-view?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse XML containing PI in root section", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<root><?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse XML containing PI after declaration and check values", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
                "<root></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog.children[1]).getNodeType() == XNodeType::pi);
    REQUIRE(XNodeRef<XNodePI>(*xml.m_prolog.children[1]).name == "xml-stylesheet");
    REQUIRE(XNodeRef<XNodePI>(*xml.m_prolog.children[1]).parameters == "href=\"tutorialspointstyle.css\" type=\"text/css\"");
  }
  SECTION("Parse XML containing PI in root section and check values", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
                "<root><?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNode>(*xml.m_prolog[0].children[0]).getNodeType() == XNodeType::pi);
    REQUIRE(XNodeRef<XNodePI>(*xml.m_prolog[0].children[0]).name == "xml-stylesheet");
    REQUIRE(XNodeRef<XNodePI>(*xml.m_prolog[0].children[0]).parameters == "href=\"tutorialspointstyle.css\" type=\"text/css\"");
  }
}
TEST_CASE("Parse CDATA SECTION", "[XML][Parse][CDATA]")
{
  std::string xmlString;
  SECTION("Parse XML root containing CDDATA containing a XML tags", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                " <root>\n"
                "   <![CDATA[<message> Welcome to TutorialsPoint </message>   ]]>   "
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse XML root containing CDDATA containing a XML tags and check contents", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                " <root>\n"
                "   <![CDATA[<message> Welcome to TutorialsPoint </message>]]>   "
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0]).getContents() == "\n   <message> Welcome to TutorialsPoint </message>   ");
    REQUIRE(XNodeRef<XNodeCDATA>(*xml.m_prolog[0].children[1]).cdata == "<message> Welcome to TutorialsPoint </message>");
  }
  SECTION("Parse XML root containing CDDATA containing nested CDATA ", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                " <root>\n"
                "   <![CDATA[< Test test <![CDATA[ Test text ]]> ]]>\n"
                "   </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 3 Column: 40] Nesting of CDATA sections is not allowed.");
  }
  SECTION("Parse XML root containing CDDATA containing ]]> ", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                " <root>\n"
                "   <![CDATA[< Test Test text ]]>  ]]>\n"
                "   </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 4 Column: 1] ']]>' invalid in element content area.");
  }
}
TEST_CASE("Parse UTF-16 encoded files.", "[XML][Parse][UTF16]")
{
  std::string xmlString;
  SECTION("Parse UTF16 encoded file LE ", "[XML][Parse][UTF16]")
  {
    BufferSource xmlSource(readXMLFromFileUTF16("./testData/testfile008.xml"));
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
  SECTION("Parse UTF16 encoded file BE ", "[XML][Parse][UTF16]")
  {
    BufferSource xmlSource(readXMLFromFileUTF16("./testData/testfile009.xml"));
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
}
TEST_CASE("Parse XML with defined namespaces.", "[XML][Parse][Namespace]")
{
  std::string xmlString;
  SECTION("A root document and two namespaces defined in the child two table elements.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root>\n"
                "<h:table xmlns:h=\"http://www.w3.org/TR/html4/\">\n"
                "<h:tr>\n"
                "<h:td>Apples</h:td>\n"
                "<h:td>Bananas</h:td>\n"
                "</h:tr>\n"
                "</h:table>\n"
                "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<f:name>African Coffee Table</f:name>\n"
                "<f:width>80</f:width>\n"
                "<f:length>120</f:length>\n"
                "</f:table>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).name == "h:table");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces[0].name == "h");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces[0].value.parsed == "http://www.w3.org/TR/html4/");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).namespaces.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).name == "f:table");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).namespaces[0].name == "f");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).namespaces[0].value.parsed == "https://www.w3schools.com/furniture");
  }
  SECTION("A root document and two namespaces defined in the root element.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<h:table>\n"
                "<h:tr>\n"
                "<h:td>Apples</h:td>\n"
                "<h:td>Bananas</h:td>\n"
                "</h:tr>\n"
                "</h:table>\n"
                "<f:table>\n"
                "<f:name>African Coffee Table</f:name>\n"
                "<f:width>80</f:width>\n"
                "<f:length>120</f:length>\n"
                "</f:table>\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).name == "h:table");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces.size() == 2);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces[0].name == "h");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces[0].value.parsed == "http://www.w3.org/TR/html4/");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces[1].name == "f");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces[1].value.parsed == "https://www.w3schools.com/furniture");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).name == "f:table");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).namespaces.size() == 2);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).namespaces[0].name == "h");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).namespaces[0].value.parsed == "http://www.w3.org/TR/html4/");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).namespaces[1].name == "f");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][1]).namespaces[1].value.parsed == "https://www.w3schools.com/furniture");
  }
  SECTION("A root document and two namespaces defined in the root element and non-existant namespace g for one of tables.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table><g:table>\n"
                "<g:name>African Coffee Table</g:name><g:width>80</g:width>\n"
                "<g:length>120</g:length></g:table></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 3 Column: 44] Namespace used but not defined.");
  }
  SECTION("A root documement with a default namespace", "[XML][Parse][Namespace]")
  {
    xmlString = "<table xmlns=\"http://www.w3.org/TR/html4/\"><tr><td>Apples</td><td>Bananas</td></tr></table>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).name == "tr");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces[0].name == ":");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog[0][0]).namespaces[0].value.parsed == "http://www.w3.org/TR/html4/");
  }
  SECTION("A root document and two namespaces (the same name) defined in the root element.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:f=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<f:table><f:tr><f:td>Apples</f:td><f:td>Bananas</f:td></f:tr></f:table><f:table>\n"
                "<f:name>African Coffee Table</f:name><f:width>80</f:width>\n"
                "<f:length>120</f:length></f:table></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_THROWS_WITH(xml.parse(), "XML Syntax Error [Line: 1 Column: 94] Attribute defined more than once within start tag.");
  }
  SECTION("A root document defining one namespae tha is overridden by a child", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table>\n"
                "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<f:name>African Coffee Table</f:name><f:width>80</f:width>\n"
                "<f:length>120</f:length></f:table></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    REQUIRE_NOTHROW(xml.parse());
  }
}
TEST_CASE("Use name for accessing elements", "[XML][Access][ByName]")
{
  std::string xmlString;
  SECTION("Address book access", "[XML][Access][ByName]")
  {
    xmlString = "<?xml version=\"1.0\"?>"
                "<AddressBook>"
                "<Address>"
                "Flat A, West Road, Wolverhampton, W1SSX9"
                "</Address>"
                "</AddressBook>";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog["AddressBook"]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog["AddressBook"]["Address"]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.m_prolog["AddressBook"]["Address"]).getContents() == "Flat A, West Road, Wolverhampton, W1SSX9");
  }
}
TEST_CASE("Make sure whitespace is whitespace.", "[XML][Access][ByName]")
{
  std::string xmlString;
  SECTION("Content node only whitespace if it contains ONLY whitespace.", "[XML][Parse][Whitespace]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook>\n"
                "<Address>\n  \n"
                "Flat A, West Road, Wolverhampton, W1SSX9"
                "</Address>"
                "<Address>\n"
                "        "
                "</Address>"
                "<Address>\n"
                "&amp;        "
                "</Address>"
                "<Address>\n"
                "<![CDATA[<message> Welcome to TutorialsPoint </message>   ]]>        "
                "</Address>"
                "<Address>\n"
                "&amp;        "
                "</Address>"
                "</AddressBook>";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource); 
    xml.parse();
    REQUIRE(XNodeRef<XNodeElement>(*xml.m_prolog["AddressBook"].children[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>(*xml.m_prolog["AddressBook"].children[0]).isWhiteSpace == true);
    REQUIRE(XNodeRef<XNodeElement>((*xml.m_prolog["AddressBook"].children[1])[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.m_prolog["AddressBook"].children[1])[0]).isWhiteSpace == false);
    REQUIRE(XNodeRef<XNodeElement>((*xml.m_prolog["AddressBook"].children[2])[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.m_prolog["AddressBook"].children[2])[0]).isWhiteSpace == true);
    REQUIRE(XNodeRef<XNodeElement>((*xml.m_prolog["AddressBook"].children[3])[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.m_prolog["AddressBook"].children[3])[0]).isWhiteSpace == false);
    REQUIRE(XNodeRef<XNodeElement>((*xml.m_prolog["AddressBook"].children[3])[1]).getNodeType() == XNodeType::entity);
    REQUIRE(XNodeRef<XNodeElement>((*xml.m_prolog["AddressBook"].children[3])[2]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.m_prolog["AddressBook"].children[3])[2]).isWhiteSpace == false);
    REQUIRE(XNodeRef<XNodeElement>((*xml.m_prolog["AddressBook"].children[4])[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.m_prolog["AddressBook"].children[4])[0]).isWhiteSpace == false);
    REQUIRE(XNodeRef<XNodeElement>((*xml.m_prolog["AddressBook"].children[4])[1]).getNodeType() == XNodeType::cdata);
    REQUIRE(XNodeRef<XNodeElement>((*xml.m_prolog["AddressBook"].children[4])[2]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.m_prolog["AddressBook"].children[4])[2]).isWhiteSpace == false);
  }
}