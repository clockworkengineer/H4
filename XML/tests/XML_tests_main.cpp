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
// ==========================
// Unit test helper functions
// ==========================
/// <summary>
/// Open a XML file, read its contents into a string buffer and return
/// the buffer.
/// </summary>
/// <param name="xmlFileName">XML file name</param>
/// <returns></returns>
std::string readXMLFromFile(const std::string &xmlFileName)
{
  std::ifstream xmlFile;
  xmlFile.open(xmlFileName);
  std::ostringstream xmlFileBuffer;
  xmlFileBuffer << xmlFile.rdbuf();
  return (xmlFileBuffer.str());
}
// ==========
// Test cases
// ==========
TEST_CASE("Use XML object to parse XML declaration", "[XML][Parse][Declaration]")
{
  XML xml;
  SECTION("Parse XML declaration. ", "[XML]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?> <root></root>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
  }
  SECTION("Parse version 1.0 encoding == UTF-8 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?> <root></root>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-16");
    REQUIRE(xNodeRoot.standalone == "yes");
  }
  SECTION("Parse version 1.1 encoding == UTF8 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\" standalone = \"yes\"?> <root></root>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "yes");
  }
  SECTION("Check declaration contains at least version attribute.", "[XML][Parse][Declaration]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml?> <root></root>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml?> <root></root>"), "XML syntax error detected.");
  }
  SECTION("Parse empty XML declaration no root tag ", "[XML][Parse][Declaration]")
  {
    XNodeRoot xNodeRoot;
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\">"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\">"), "XML syntax error detected.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML][Parse][Declaration]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\" standalone = \"no\" encoding = \"UTF-8\"?> <root></root>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\" standalone = \"no\" encoding = \"UTF-8\"?> <root></root>"), "XML syntax error detected.");
  }
  SECTION("Parse XML with declaration but no root element", "[XML][Parse][Declaration]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"), std::runtime_error);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"), "Parse buffer empty before parse complete.");
  }
}
TEST_CASE("Checks for tag names", "[XML][Parse][Tags]")
{
  XML xml;
  SECTION("Allow tag names to with alpha numeric characters and '.','_', '-' ", "[XML][Parse][Tags]")
  {
    REQUIRE_NOTHROW(xml.parse("<?xml version = \"1.0\"?> <abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.></abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.>"));
  }
  SECTION("Tag names are case sensitive", "[XML][Parse][Tags]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\"?> <AddressBook> </addressbook>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\"?> <AddressBook> </addressbook>"), "XML syntax error detected.");
  }
  SECTION("Incorrect closing tag ", "[XML][Parse][Tags]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\"?> <AddressBook> </Address>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\"?> <AddressBook> </Address>"), "XML syntax error detected.");
  }
  SECTION("Self closing tag ", "[XML][Parse][Tags]")
  {
    REQUIRE_NOTHROW(xml.parse("<?xml version = \"1.0\"?> <AddressBook/>"));
  }
  SECTION("Tag starts with a '.', '-' or a numeric digit", "[XML][Parse][Tags]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\"?> <.AddressBook> </.AddressBook>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\"?> <.AddressBook> </.AddressBook>"), "XML syntax error detected.");
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\"?> <-AddressBook> </-AddressBook>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\"?> <-AddressBook> </-AddressBook>"), "XML syntax error detected.");
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\"?> <0AddressBook> </0AddressBook>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\"?> <0AddressBook> </0AddressBook>"), "XML syntax error detected.");
  }
  SECTION("Tag starts with a xml/XML/Xml etc", "[XML][Parse][Tags]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\"?> <xmlAddressBook> </xmlAddressBook>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\"?> <xmlAddressBook> </xmlAddressBook>"), "XML syntax error detected.");
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\"?> <XMLAddressBook> </XMLAddressBook>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\"?> <XMLAddressBook> </XMLAddressBook>"), "XML syntax error detected.");
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\"?> <XmlAddressBook> </XmlAddressBook>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\"?> <XmlAddressBook> </XmlAddressBook>"), "XML syntax error detected.");
  }
}
TEST_CASE("Use XML object to parse declaration, root element and check parsed information ", "[XML][Parse][Root]")
{
  XML xml;
  SECTION("Empty root element <contact-info> ", "[XML][Parse]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?> <contact-info> </contact-info>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.elements.size() == 0);
    REQUIRE(xNodeRoot.name == "contact-info");
  }
  SECTION("Empty root element <AddressBook> ", "[XML][Parse][Root]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?> <AddressBook> </AddressBook>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.elements.size() == 0);
    REQUIRE(xNodeRoot.name == "AddressBook");
  }
  SECTION("Root element <AddressBook> and one child <Address> with contents ", "[XML][Parse][Root]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?> <AddressBook><Address>    This is some contents    </Address></AddressBook>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.name == "AddressBook");
    REQUIRE(xNodeRoot.elements.size() == 1);
    REQUIRE(xNodeRoot.elements[0].name == "Address");
    REQUIRE(xNodeRoot.elements[0].contents == "    This is some contents    ");
  }
  SECTION("Root element <AddressBook> with multiple sibling <Address> elements and contents ", "[XML][Parse][Root]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?><AddressBook><Address>"
                                    "    This is some contents 1   </Address>"
                                    "<Address>    This is some contents 2   </Address>"
                                    "<Address>    This is some contents 3   </Address></AddressBook>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.name == "AddressBook");
    REQUIRE(xNodeRoot.elements.size() == 3);
    REQUIRE(xNodeRoot.elements[0].name == "Address");
    REQUIRE(xNodeRoot.elements[0].contents == "    This is some contents 1   ");
    REQUIRE(xNodeRoot.elements[1].name == "Address");
    REQUIRE(xNodeRoot.elements[1].contents == "    This is some contents 2   ");
    REQUIRE(xNodeRoot.elements[2].name == "Address");
    REQUIRE(xNodeRoot.elements[2].contents == "    This is some contents 3   ");
  }
}
TEST_CASE("Sample XML files to read and parse.", "[XML][Parse]")
{
  XML xml;
  auto testFile = GENERATE(values<std::string>({"./testData/testfile001.xml",
                                                "./testData/testfile002.xml",
                                                "./testData/testfile003.xml",
                                                "./testData/testfile005.xml",
                                                "./testData/testfile007.xml"}));
  SECTION("Load file into buffer and parse.", "[XML][Parse]")
  {
    std::string jsonXMLBuffer = readXMLFromFile(testFile);
    REQUIRE_NOTHROW(xml.parse(jsonXMLBuffer));
  }
}
TEST_CASE("Parse XML elements with attached attributes", "[XML][Parse][Attributes]")
{
  XML xml;
  SECTION("Root attribute with one attached attribute number", "[XML][Parse][[Attributes]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?> <AddressBook number='15'> </AddressBook>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.elements.size() == 0);
    REQUIRE(xNodeRoot.name == "AddressBook");
    REQUIRE(xNodeRoot.attributes.size() == 1);
    REQUIRE(xNodeRoot.attributes[0].name == "number");
    REQUIRE(xNodeRoot.attributes[0].value == "15");
  }
  SECTION("Root attribute with 3 attached attributes number, away, flat", "[XML][Parse][[Attributes]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?> <AddressBook number='15' away=\"yes\" flat='no'> </AddressBook>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.elements.size() == 0);
    REQUIRE(xNodeRoot.name == "AddressBook");
    REQUIRE(xNodeRoot.attributes.size() == 3);
    REQUIRE(xNodeRoot.attributes[0].name == "number");
    REQUIRE(xNodeRoot.attributes[0].value == "15");
    REQUIRE(xNodeRoot.attributes[1].name == "away");
    REQUIRE(xNodeRoot.attributes[1].value == "yes");
    REQUIRE(xNodeRoot.attributes[2].name == "flat");
    REQUIRE(xNodeRoot.attributes[2].value == "no");
  }
  SECTION("Empty elements with attributes are allowed.", "[XML][Parse][[Attributes]")
  {
    REQUIRE_NOTHROW(xml.parse("<?xml version = \"1.0\"?> <AddressBook number='15'><AddressBook/>"));
    REQUIRE_NOTHROW(xml.parse("<?xml version = \"1.0\"?> <AddressBook number='15'/>"));
  }
  SECTION("Root element with duplicate attributes.", "[XML][Parse][[Attributes]")
  {
    REQUIRE_THROWS_AS(xml.parse("<?xml version = \"1.0\"?> <AddressBook number='15' colour='red' number='16'> </AddressBook>"), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse("<?xml version = \"1.0\"?> <AddressBook number='15' colour='red' number='16'> </AddressBook>"), "XML syntax error detected.");
  }
}
TEST_CASE("Parse XML elements with comments", "[XML][Parse][Comments]")
{
  XML xml;
  SECTION("A simple single line comment", "[XML][Parse][[Comments]")
  {
    REQUIRE_NOTHROW(xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                              "<!-- A single line comment --> <root></root>"));
  }
  SECTION("Multiple comments inside root element and between its children ", "[XML][Parse][[Comments]")
  {
    XNodeRoot xNodeRoot = xml.parse("<?xml version = \"1.0\"?><AddressBook><!--Address one -->"
                                    "<Address>    This is some contents 1   </Address><!--Address two -->"
                                    "<Address>    This is some contents 2   </Address><!--Address three -->"
                                    "<Address>    This is some contents 3   </Address></AddressBook>");
    REQUIRE(xNodeRoot.version == "1.0");
    REQUIRE(xNodeRoot.encoding == "UTF-8");
    REQUIRE(xNodeRoot.standalone == "no");
    REQUIRE(xNodeRoot.name == "AddressBook");
    REQUIRE(xNodeRoot.elements.size() == 3);
    REQUIRE(xNodeRoot.elements[0].name == "Address");
    REQUIRE(xNodeRoot.elements[0].contents == "    This is some contents 1   ");
    REQUIRE(xNodeRoot.elements[1].name == "Address");
    REQUIRE(xNodeRoot.elements[1].contents == "    This is some contents 2   ");
    REQUIRE(xNodeRoot.elements[2].name == "Address");
    REQUIRE(xNodeRoot.elements[2].contents == "    This is some contents 3   ");
  }
  SECTION("A single comment after root element", "[XML][Parse][[Comments]")
  {
    REQUIRE_NOTHROW(xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                              "<root></root><!-- A single line comment --> "));
  }
  SECTION("A simple multi line comment", "[XML][Parse][[Comments]")
  {
    REQUIRE_NOTHROW(xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                              "<!-- A single line comment\n"
                              " another line\n"
                              " another line\n"
                              " and another line\n"
                              "--> <root></root>"));
  }
  SECTION("A simple comment within element content", "[XML][Parse][[Comments]")
  {
    REQUIRE_NOTHROW(xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?><root>Test<!-- a simple comment -->Test</root>"));
    XNodeRoot XNodeRoot = xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?><root>Test<!-- a simple comment -->Test</root>");
  }
  SECTION("A simple comment within element contents and content remains intact", "[XML][Parse][[Comments]")
  {
    XNodeRoot XNodeRoot = xml.parse("<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?><root>Test  <!-- a simple comment -->  Test</root>");
    REQUIRE(XNodeRoot.contents == "Test    Test");
  }
}
TEST_CASE("Parse XML with Unicode character in element names, attributes, comments, character data, and processing instructions. ", "[XML][Parse][Unicode]")
{
  XML xml;
  SECTION("Japanese characters", "[XML][Parse][[Unicode]")
  {
    REQUIRE_NOTHROW(xml.parse("<?xml version=\"1.0\" encoding=\"utf-8\"?><config><start_text>転送</start_text></config>"));
  }
  SECTION("Well-formed XML document including Chinese, Armenian and Cyrillic characters", "[XML][Parse][[Unicode]")
  {
    std::string xmlString{"<?xml version=\"1.0\" encoding=\"UTF-8\"?><俄语 լեզու=\"ռուսերեն\">данные</俄语>"};
    REQUIRE_NOTHROW(xml.parse(xmlString));
    XNodeRoot xNodeRoot = xml.parse(xmlString);
    REQUIRE(xNodeRoot.name == "俄语");
    REQUIRE(xNodeRoot.attributes.size() == 1);
    REQUIRE(xNodeRoot.attributes[0].name == "լեզու");
    REQUIRE(xNodeRoot.attributes[0].value == "ռուսերեն");
    REQUIRE(xNodeRoot.contents=="данные");
  }
}