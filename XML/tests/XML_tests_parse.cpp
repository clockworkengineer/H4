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
  XML xml;
  std::string xmlString;
  SECTION("Parse XML declaration. ", "[XML]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                "<root></root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
  }
  SECTION("Parse version 1.0 encoding == UTF-8 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?> "
                "<root></root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-16");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "yes");
  }
  SECTION("Parse version 1.1 encoding == UTF8 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\" standalone = \"yes\"?>"
                "<root></root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "yes");
  }
  SECTION("Check declaration contains at least version attribute.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml?> <root></root>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Parse empty XML declaration no root tag ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\">";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\" standalone = \"no\" encoding = \"UTF-8\"?>"
                " <root></root>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Parse XML with declaration but no root element", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), std::runtime_error);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "Parse buffer empty before parse complete.");
  }
}
TEST_CASE("Checks for tag names", "[XML][Parse][Tags]")
{
  XML xml;
  std::string xmlString;
  SECTION("Allow tag names to with alpha numeric characters and '.','_', '-' ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.>"
                "</abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Tag names are case sensitive", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook> </addressbook>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Incorrect closing tag ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook> </Address>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Self closing tag ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook/>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Tag starts with a '.', '-' or a numeric digit", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?> "
                "<.AddressBook> </.AddressBook>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\"?> <-AddressBook> </-AddressBook>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\"?> <0AddressBook> </0AddressBook>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Tag starts with a xml/XML/Xml etc", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                " <xmlAddressBook> </xmlAddressBook>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\"?> <XMLAddressBook> </XMLAddressBook>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\"?> <XmlAddressBook> </XmlAddressBook>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
}
TEST_CASE("Use XML object to parse declaration, root element and check parsed information ", "[XML][Parse][Root]")
{
  XML xml;
  std::string xmlString;
  SECTION("Empty root element <contact-info> ", "[XML][Parse]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<contact-info></contact-info>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).elements.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "contact-info");
  }
  SECTION("Empty root element <AddressBook> ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook> </AddressBook>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).elements.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "AddressBook");
  }
  SECTION("Root element <AddressBook> and one child <Address> with contents ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook>"
                "<Address>    This is some contents    </Address>"
                "</AddressBook>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).elements.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).content == "    This is some contents    ");
  }
  SECTION("Root element <AddressBook> with multiple sibling <Address> elements and contents ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook>"
                "<Address>"
                "    This is some contents 1   "
                "</Address>"
                "<Address>"
                "    This is some contents 2   "
                "</Address>"
                "<Address>"
                "    This is some contents 3   "
                "</Address>"
                "</AddressBook>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).elements.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).content == "    This is some contents 1   ");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).content == "    This is some contents 2   ");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][2]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][2]).content == "    This is some contents 3   ");
  }
}
TEST_CASE("Sample XML files to read and parse.", "[XML][Parse]")
{
  XML xml;
  auto testFile = GENERATE(values<std::string>({"./testData/testfile001.xml",
                                                "./testData/testfile002.xml",
                                                "./testData/testfile003.xml",
                                                "./testData/testfile005.xml",
                                                "./testData/testfile007.xml",
                                                "./testData/testfile010.xml"}));
  SECTION("Load file into buffer and parse.", "[XML][Parse]")
  {
    std::string jsonXMLBuffer = readXMLFromFileUTF8(testFile);
    REQUIRE_NOTHROW(xml.parse(jsonXMLBuffer));
  }
}
TEST_CASE("Parse XML elements with attached attributes", "[XML][Parse][Attributes]")
{
  XML xml;
  std::string xmlString;
  SECTION("Root attribute with one attached attribute number", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook number='15'>"
                "</AddressBook>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).elements.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].name == "number");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].value == "15");
  }
  SECTION("Root attribute with 3 attached attributes number, away, flat", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                " <AddressBook number='15' away=\"yes\" flat='no'>"
                " </AddressBook>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).elements.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].name == "number");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].value == "15");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[1].name == "away");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[1].value == "yes");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[2].name == "flat");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[2].value == "no");
  }
  SECTION("Empty elements with attributes are allowed.", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook number='15'>"
                "<AddressBook/>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Root element with duplicate attributes.", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook number='15' colour='red' number='16'>"
                " </AddressBook>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
}
TEST_CASE("Parse XML elements with comments", "[XML][Parse][Comments]")
{
  XML xml;
  std::string xmlString;
  SECTION("A simple single line comment", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                "<!-- A single line comment -->"
                "<root></root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Multiple single line comments beifre root tag", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                "<!-- A single line comment -->"
                " <!-- A single line comment -->"
                " <!-- A single line comment -->"
                " <root></root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Multiple comments inside root element and between its children ", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\"?>"
                "<AddressBook>"
                "<!--Address one -->"
                "<Address>    This is some contents 1   </Address>"
                "<!--Address two -->"
                "<Address>    This is some contents 2   </Address>"
                "<!--Address three -->"
                "<Address>    This is some contents 3   </Address>"
                "</AddressBook>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).elements.size() == 6);
    REQUIRE(XNodeRef<XNodeComment>((*xNodeRoot)[0][0]).comment == "Address one ");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).content == "    This is some contents 1   ");
    REQUIRE(XNodeRef<XNodeComment>((*xNodeRoot)[0][2]).comment == "Address two ");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][3]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][3]).content == "    This is some contents 2   ");
    REQUIRE(XNodeRef<XNodeComment>((*xNodeRoot)[0][4]).comment == "Address three ");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][5]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][5]).content == "    This is some contents 3   ");
  }
  SECTION("A single comment after root element", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                "<root></root>"
                "<!-- A single line comment --> ";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("A simple multi line comment", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                "<!-- A single line comment\n"
                " another line\n"
                " another line\n"
                " and another line\n"
                "--> <root></root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("A simple comment within element content", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                "<root>Test<!-- a simple comment -->Test"
                "</root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == "TestTest");
  }
  SECTION("A simple comment within element contents and content remains intact", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                "<root>Test  <!-- a simple comment -->  Test"
                "</root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == "Test    Test");
  }
  SECTION("A simple single line comment containing -- is illegal", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                "<!-- A single line comment-- --> "
                "<root></root>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                "<!-- A single line comment ---> "
                "<root></root>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
}
TEST_CASE("Parse XML with Unicode character in element names, attributes, comments, character data, and processing instructions. ", "[XML][Parse][Unicode]")
{
  XML xml;
  std::string xmlString;
  SECTION("Japanese characters", "[XML][Parse][[Unicode]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"utf-8\"?><config><start_text>転送</start_text></config>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Well-formed XML document including Chinese, Armenian and Cyrillic characters", "[XML][Parse][[Unicode]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<俄语 լեզու=\"ռուսերեն\">данные</俄语>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "俄语");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].name == "լեզու");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].value == "ռուսերեն");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == "данные");
  }
}
TEST_CASE("Check the pasring of character entities/reference.", "[XML][Parse][Entities]")
{
  XML xml;
  std::string xmlString;
  SECTION("Parse entity &amp; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?> "
                "<root> &amp; </root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " & ");
  }
  SECTION("Parse entity &quot; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>"
                " <root> &quot; </root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " \" ");
  }
  SECTION("Parse entities &apos; &lt; &gt; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>"
                " <root> &apos; &lt; &gt; </root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " ' < > ");
  }
  SECTION("Parse reference &#x00A5; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>"
                " <root> &#x00A5; </root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " ¥ ");
  }
  SECTION("Parse reference &#163; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?> "
                "<root> &#163; </root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " £ ");
  }
  SECTION("Parse entity &amp;&quot;&apos;&gt;&lt; in attribute value", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>"
                " <root attr1=\" &amp;&quot;&apos;&gt;&lt; \">"
                "</root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].name == "attr1");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].value == " &\"'>< ");
  }
  SECTION("Parse references &#x00A5;&#163 in attribute value", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>"
                " <root attr1=\" &#x00A5;&#163; \"></root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].name == "attr1");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].value == " ¥£ ");
  }
}
TEST_CASE("Check the parsing of XML containing program instructions", "[XML][Parse][PI]")
{
  XML xml;
  std::string xmlString;
  SECTION("Parse XML containing PI after declaration", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>"
                "<?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>"
                "<root></root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Parse XML containing multiple PI after declaration", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>"
                "<?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?> "
                "<?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>"
                "  <root></root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Parse XML containing PI in root section", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>"
                "<root><?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>"
                "</root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Parse XML containing PI after declaration and check values", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>"
                "<?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>"
                "<root></root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::pi);
    REQUIRE(XNodeRef<XNodePI>((*xNodeRoot)[0]).name == "xml-stylesheet");
    REQUIRE(XNodeRef<XNodePI>((*xNodeRoot)[0]).parameters == "href = \"tutorialspointstyle.css\" type = \"text/css\"");
  }
  SECTION("Parse XML containing PI in root section and check values", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>"
                "<root><?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>"
                "</root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0][0]).getNodeType() == XNodeType::pi);
    REQUIRE(XNodeRef<XNodePI>((*xNodeRoot)[0][0]).name == "xml-stylesheet");
    REQUIRE(XNodeRef<XNodePI>((*xNodeRoot)[0][0]).parameters == "href = \"tutorialspointstyle.css\" type = \"text/css\"");
  }
}
TEST_CASE("Parse CDATA SECTION", "[XML][Parse][CDATA]")
{
  XML xml;
  std::string xmlString;
  SECTION("Parse XML root containing CDDATA containing a XML tags", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                " <root>"
                "   <![CDATA[<message> Welcome to TutorialsPoint </message>   ]]>   "
                "</root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Parse XML root containing CDDATA containing a XML tags and check contents", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                " <root>"
                "   <![CDATA[<message> Welcome to TutorialsPoint </message>]]>   "
                "</root>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == "   <message> Welcome to TutorialsPoint </message>   ");
    REQUIRE(XNodeRef<XNodeCDATA>((*xNodeRoot)[0][0]).cdata == "<message> Welcome to TutorialsPoint </message>");
  }
  SECTION("Parse XML root containing CDDATA containing nested CDATA ", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                " <root>"
                "   <![CDATA[< Test test <![CDATA[ Test text ]]> ]]>"
                "   </root>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Parse XML root containing CDDATA containing ]]> ", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>"
                " <root>"
                "   <![CDATA[< Test Test text ]]>  ]]>"
                "   </root>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
}
TEST_CASE("Parse UTF-16 encoded files.", "[XML][Parse][UTF16]")
{
  XML xml;
  std::string xmlString;
  SECTION("Parse UTF16 encoded file LE ", "[XML][Parse][UTF16]")
  {
    std::u16string jsonXMLBuffer = readXMLFromFileUTF16("./testData/testfile008.xml");
    REQUIRE_NOTHROW(xml.parse(jsonXMLBuffer));
  }
  SECTION("Parse UTF16 encoded file BE ", "[XML][Parse][UTF16]")
  {
    std::u16string jsonXMLBuffer = readXMLFromFileUTF16("./testData/testfile009.xml");
    REQUIRE_NOTHROW(xml.parse(jsonXMLBuffer));
  }
}
TEST_CASE("Parse XML with defined namespaces.", "[XML][Parse][Namespace]")
{
  XML xml;
  std::string xmlString;
  SECTION("A root document and two namespaces defined in the child two table elements.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root>"
                "<h:table xmlns:h=\"http://www.w3.org/TR/html4/\">"
                "<h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr>"
                "</h:table>"
                "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">"
                "<f:name>African Coffee Table</f:name>"
                "<f:width>80</f:width>"
                "<f:length>120</f:length>"
                "</f:table>"
                "</root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).name == "h:table");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[0].name == "h");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[0].value == "http://www.w3.org/TR/html4/");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).namespaces.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).name == "f:table");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).namespaces[0].name == "f");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).namespaces[0].value == "https://www.w3schools.com/furniture");
  }
  SECTION("A root document and two namespaces defined in the root element.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table>"
                "<f:table><f:name>African Coffee Table</f:name><f:width>80</f:width><f:length>120</f:length></f:table></root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).name == "h:table");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces.size() == 2);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[0].name == "h");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[0].value == "http://www.w3.org/TR/html4/");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[1].name == "f");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[1].value == "https://www.w3schools.com/furniture");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).name == "f:table");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).namespaces.size() == 2);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).namespaces[0].name == "h");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).namespaces[0].value == "http://www.w3.org/TR/html4/");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).namespaces[1].name == "f");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).namespaces[1].value == "https://www.w3schools.com/furniture");
  }
  SECTION("A root document and two namespaces defined in the root element and non-existant namespace g for one of tables.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table><g:table>"
                "<g:name>African Coffee Table</g:name><g:width>80</g:width>"
                "<g:length>120</g:length></g:table></root>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("A root documement with a default namespace", "[XML][Parse][Namespace]")
  {
    xmlString = "<table xmlns=\"http://www.w3.org/TR/html4/\"><tr><td>Apples</td><td>Bananas</td></tr></table>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).name == "tr");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[0].name == ":");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[0].value == "http://www.w3.org/TR/html4/");
  }
  SECTION("A root document and two namespaces (the same name) defined in the root element.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:f=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">"
                "<f:table><f:tr><f:td>Apples</f:td><f:td>Bananas</f:td></f:tr></f:table><f:table>"
                "<f:name>African Coffee Table</f:name><f:width>80</f:width>"
                "<f:length>120</f:length></f:table></root>";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("A root document defining one namespae tha is overridden by a child", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table>"
                "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">"
                "<f:name>African Coffee Table</f:name><f:width>80</f:width>"
                "<f:length>120</f:length></f:table></root>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
}
TEST_CASE("Parse XML with DTD both internal and external", "[XML][Parse][DTD]")
{
  XML xml;
  std::string xmlString;
  SECTION("XML with internal DTD", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\"?><!DOCTYPE note [<!ELEMENT note (to,from,heading,body)>"
                "<!ELEMENT to (#PCDATA)><!ELEMENT from (#PCDATA)>"
                "<!ELEMENT heading (#PCDATA)><!ELEMENT body (#PCDATA)>"
                "]><note><to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend</body></note>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with external (SYSTEM) DTD", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE note SYSTEM \"Note.dtd\">"
                "<note><to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend!</body></note>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with external (PUBLIC) DTD", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE note PUBLIC "
                "\"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
                "<note><to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend!</body></note>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with DTD with !ENTITY definitions and uses", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE note "
                "[<!ENTITY nbsp \"&#xA0;\"><!ENTITY writer \"Writer: Donald Duck.\">"
                "<!ENTITY copyright \"Copyright: W3Schools.\">]>"
                "<note><to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer></note>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with DTD with !ENTITY definitions and uses. Check translation of entity values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE note "
                "[<!ENTITY nbsp \"&#xA0;\"><!ENTITY writer \"Writer: Donald Duck.\">"
                "<!ENTITY copyright \"Copyright: W3Schools.\">]>"
                "<note><to>Tove</to><from>Jani</from><heading>Reminder</heading>"
                "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer></note>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[1]).getNodeType() == XNodeType::element);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[1][4]).name == "footer");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[1][4]).content == "Writer: Donald Duck.\u00A0Copyright: W3Schools.");
  }
  SECTION("XML with internal DTD and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\" ?>"
                "<!DOCTYPE address [   <!ELEMENT address (name,company,phone)>"
                "<!ELEMENT name (#PCDATA)><!ELEMENT company (#PCDATA)>"
                "<!ELEMENT phone (#PCDATA)><!ELEMENT br EMPTY><!ELEMENT footer ANY>]><address><name>Tanmay Patil</name>"
                "<company>TutorialsPoint</company><phone>(011) 123-4567</phone></address>";
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
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE note SYSTEM \"Note.dtd\"><note><to>Tove"
                "</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body></note>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).external.name == "SYSTEM");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).external.value == "Note.dtd");
  }
  SECTION("XML with external URL DTD and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
                "<html></html>";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::dtd);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).name == XNodeRef<XNodeDTD>((*xNodeRoot)[1]).name);
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).external.name == "PUBLIC");
    REQUIRE(XNodeRef<XNodeDTD>((*xNodeRoot)[0]).external.value == "-//W3C//DTD XHTML 1.0 Transitional//EN, http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
  }
  SECTION("XML with internal DTD with attributes", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE TVSCHEDULE [<!ELEMENT TVSCHEDULE (CHANNEL+)><!ELEMENT CHANNEL (BANNER,DAY+)>"
                "<!ELEMENT BANNER (#PCDATA)><!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>"
                "<!ELEMENT HOLIDAY (#PCDATA)><!ELEMENT DATE (#PCDATA)>"
                "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)><!ELEMENT TIME (#PCDATA)>"
                "<!ELEMENT TITLE (#PCDATA)><!ELEMENT DESCRIPTION (#PCDATA)>"
                "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>"
                "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED><!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>"
                "<!ATTLIST TITLE RATING CDATA #IMPLIED><!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>"
                "<TVSCHEDULE></TVSCHEDULE>";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("XML with internal DTD with attributes and check values", "[XML][Parse][DTD]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<!DOCTYPE TVSCHEDULE [<!ELEMENT TVSCHEDULE (CHANNEL+)><!ELEMENT CHANNEL (BANNER,DAY+)>"
                "<!ELEMENT BANNER (#PCDATA)><!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>"
                "<!ELEMENT HOLIDAY (#PCDATA)><!ELEMENT DATE (#PCDATA)>"
                "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)><!ELEMENT TIME (#PCDATA)>"
                "<!ELEMENT TITLE (#PCDATA)><!ELEMENT DESCRIPTION (#PCDATA)>"
                "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>"
                "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED><!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>"
                "<!ATTLIST TITLE RATING CDATA #IMPLIED><!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>"
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
  SECTION("XML with internal DTD with elements with multiple attributes and check values", "[XML][Parse][DTD]")
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
  SECTION("XML with internal DTD with parameter entities.", "[XML][Parse][DTD]")
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
  SECTION("XML with internal DTD with both types of entities an check values", "[XML][Parse][DTD]")
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
  SECTION("XML with internal DTD with !NOTIFCATION to parse.", "[XML][Parse][DTD]")
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
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
}