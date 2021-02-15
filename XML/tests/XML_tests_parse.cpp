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
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                "<root></root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
  }
  SECTION("Parse version 1.0 encoding == UTF-8 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?> "
                "<root></root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-16");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "yes");
  }
  SECTION("Parse version 1.1 encoding == UTF8 standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\" standalone = \"yes\"?>\n"
                "<root></root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "yes");
  }
  SECTION("Check declaration contains at least version attribute.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml?> <root></root>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Parse empty XML declaration no root tag ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\">\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\" standalone = \"no\" encoding = \"UTF-8\"?>\n"
                " <root></root>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Parse XML with declaration but no root element", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n";
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
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.>\n"
                "</abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Tag names are case sensitive", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<AddressBook> </addressbook>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Incorrect closing tag ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<AddressBook> </Address>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Self closing tag ", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<AddressBook/>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Tag starts with a '.', '-' or a numeric digit", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?> "
                "<.AddressBook> </.AddressBook>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\"?> <-AddressBook> </-AddressBook>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\"?> <0AddressBook> </0AddressBook>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Tag starts with a xml/XML/Xml etc", "[XML][Parse][Tags]")
  {
    xmlString = "<?xml version = \"1.0\"?>\n"
                " <xmlAddressBook> </xmlAddressBook>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\"?> <XMLAddressBook> </XMLAddressBook>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\"?> <XmlAddressBook> </XmlAddressBook>\n";
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
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<contact-info></contact-info>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).elements.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "contact-info");
  }
  SECTION("Empty root element <AddressBook> ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<AddressBook> </AddressBook>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).elements.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "AddressBook");
  }
  SECTION("Root element <AddressBook> and one child <Address> with contents ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<AddressBook>\n"
                "<Address>    This is some contents    </Address>\n"
                "</AddressBook>\n";
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
    xmlString = "<?xml version = \"1.0\"?>\n"
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
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[0].value == "1.0");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[1].value == "UTF-8");
    REQUIRE(XNodeRef<XNodeElement>(*xNodeRoot).attributes[2].value == "no");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).name == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).elements.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).content == "\n    This is some contents 1   ");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][1]).content == "\n    This is some contents 2   ");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][2]).name == "Address");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][2]).content == "\n    This is some contents 3   ");
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
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<AddressBook number='15'>\n"
                "</AddressBook>\n";
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
    xmlString = "<?xml version = \"1.0\"?>\n"
                " <AddressBook number='15' away=\"yes\" flat='no'>\n"
                " </AddressBook>\n";
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
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<AddressBook number='15'>\n"
                "<AddressBook/>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Root element with duplicate attributes.", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<AddressBook number='15' colour='red' number='16'>\n"
                " </AddressBook>\n";
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
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                "<!-- A single line comment -->\n"
                "<root></root>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Multiple single line comments beifre root tag", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                "<!-- A single line comment -->\n"
                " <!-- A single line comment -->\n"
                " <!-- A single line comment -->\n"
                " <root></root>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Multiple comments inside root element and between its children ", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\"?>\n"
                "<AddressBook>\n"
                "<!--Address one -->\n"
                "<Address>    This is some contents 1   </Address>\n"
                "<!--Address two -->\n"
                "<Address>    This is some contents 2   </Address>\n"
                "<!--Address three -->\n"
                "<Address>    This is some contents 3   </Address>\n"
                "</AddressBook>\n";
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
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                "<root></root>\n"
                "<!-- A single line comment --> ";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("A simple multi line comment", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                "<!-- A single line comment\n"
                " another line\n"
                " another line\n"
                " and another line\n"
                "--> <root></root>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("A simple comment within element content", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                "<root>Test<!-- a simple comment -->Test"
                "</root>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == "TestTest");
  }
  SECTION("A simple comment within element contents and content remains intact", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                "<root>Test  <!-- a simple comment -->  Test"
                "</root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == "Test    Test");
  }
  SECTION("A simple single line comment containing -- is illegal", "[XML][Parse][[Comments]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                "<!-- A single line comment-- --> "
                "<root></root>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                "<!-- A single line comment ---> "
                "<root></root>\n";
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
    xmlString = "<?xml version=\"1.0\" encoding=\"utf-8\"?><config><start_text>転送</start_text></config>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Well-formed XML document including Chinese, Armenian and Cyrillic characters", "[XML][Parse][[Unicode]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<俄语 լեզու=\"ռուսերեն\">данные</俄语>\n";
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
                "<root> &amp; </root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " & ");
  }
  SECTION("Parse entity &quot; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>\n"
                " <root> &quot; </root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " \" ");
  }
  SECTION("Parse entities &apos; &lt; &gt; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>\n"
                " <root> &apos; &lt; &gt; </root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " ' < > ");
  }
  SECTION("Parse reference &#x00A5; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>\n"
                " <root> &#x00A5; </root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " ¥ ");
  }
  SECTION("Parse reference &#163; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?> "
                "<root> &#163; </root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == " £ ");
  }
  SECTION("Parse entity &amp;&quot;&apos;&gt;&lt; in attribute value", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>\n"
                " <root attr1=\" &amp;&quot;&apos;&gt;&lt; \">\n"
                "</root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].name == "attr1");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).attributes[0].value == " &\"'>< ");
  }
  SECTION("Parse references &#x00A5;&#163 in attribute value", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"yes\"?>\n"
                " <root attr1=\" &#x00A5;&#163; \"></root>\n";
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
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>\n"
                "<?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>\n"
                "<root></root>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Parse XML containing multiple PI after declaration", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>\n"
                "<?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?> "
                "<?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>\n"
                "  <root></root>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Parse XML containing PI in root section", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>\n"
                "<root><?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>\n"
                "</root>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Parse XML containing PI after declaration and check values", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>\n"
                "<?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>\n"
                "<root></root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNode>((*xNodeRoot)[0]).getNodeType() == XNodeType::pi);
    REQUIRE(XNodeRef<XNodePI>((*xNodeRoot)[0]).name == "xml-stylesheet");
    REQUIRE(XNodeRef<XNodePI>((*xNodeRoot)[0]).parameters == "href = \"tutorialspointstyle.css\" type = \"text/css\"");
  }
  SECTION("Parse XML containing PI in root section and check values", "[XML][Parse][PI]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\"?>\n"
                "<root><?xml-stylesheet href = \"tutorialspointstyle.css\" type = \"text/css\"?>\n"
                "</root>\n";
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
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                " <root>\n"
                "   <![CDATA[<message> Welcome to TutorialsPoint </message>   ]]>   "
                "</root>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
  SECTION("Parse XML root containing CDDATA containing a XML tags and check contents", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                " <root>\n"
                "   <![CDATA[<message> Welcome to TutorialsPoint </message>]]>   "
                "</root>\n";
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0]).content == "\n   <message> Welcome to TutorialsPoint </message>   ");
    REQUIRE(XNodeRef<XNodeCDATA>((*xNodeRoot)[0][0]).cdata == "<message> Welcome to TutorialsPoint </message>");
  }
  SECTION("Parse XML root containing CDDATA containing nested CDATA ", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                " <root>\n"
                "   <![CDATA[< Test test <![CDATA[ Test text ]]> ]]>\n"
                "   </root>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("Parse XML root containing CDDATA containing ]]> ", "[XML][Parse][CDATA]")
  {
    xmlString = "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\"?>\n"
                " <root>\n"
                "   <![CDATA[< Test Test text ]]>  ]]>\n"
                "   </root>\n";
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
    xmlString = "<root>\n"
                "<h:table xmlns:h=\"http://www.w3.org/TR/html4/\">\n"
                "<h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr>\n"
                "</h:table>\n"
                "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<f:name>African Coffee Table</f:name>\n"
                "<f:width>80</f:width>\n"
                "<f:length>120</f:length>\n"
                "</f:table>\n"
                "</root>\n";
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
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table>\n"
                "<f:table><f:name>African Coffee Table</f:name><f:width>80</f:width><f:length>120</f:length></f:table></root>\n";
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
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table><g:table>\n"
                "<g:name>African Coffee Table</g:name><g:width>80</g:width>\n"
                "<g:length>120</g:length></g:table></root>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("A root documement with a default namespace", "[XML][Parse][Namespace]")
  {
    xmlString = "<table xmlns=\"http://www.w3.org/TR/html4/\"><tr><td>Apples</td><td>Bananas</td></tr></table>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
    std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlString);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).name == "tr");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces.size() == 1);
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[0].name == ":");
    REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[0][0]).namespaces[0].value == "http://www.w3.org/TR/html4/");
  }
  SECTION("A root document and two namespaces (the same name) defined in the root element.", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:f=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<f:table><f:tr><f:td>Apples</f:td><f:td>Bananas</f:td></f:tr></f:table><f:table>\n"
                "<f:name>African Coffee Table</f:name><f:width>80</f:width>\n"
                "<f:length>120</f:length></f:table></root>\n";
    REQUIRE_THROWS_AS(xml.parse(xmlString), XML::SyntaxError);
    REQUIRE_THROWS_WITH(xml.parse(xmlString), "XML syntax error detected.");
  }
  SECTION("A root document defining one namespae tha is overridden by a child", "[XML][Parse][Namespace]")
  {
    xmlString = "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table>\n"
                "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">\n"
                "<f:name>African Coffee Table</f:name><f:width>80</f:width>\n"
                "<f:length>120</f:length></f:table></root>\n";
    REQUIRE_NOTHROW(xml.parse(xmlString));
  }
}