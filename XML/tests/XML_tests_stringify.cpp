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
TEST_CASE("Use XML to stringify previously parsed XML.", "[XML][Stringify")
{
    XML xml;
    std::string xmlString;
    SECTION("Stringify XML UTF-8 declaration.", "[XML][Stringify]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML UTF-16 declaration.", "[XML][Stringify]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\"?>"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify to make sure that an LF after the end tag in source XML is mirrored.", "[XML][Stringify]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<root></root>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        std::string actual = xml.stringify(xNodeRoot);
        REQUIRE(actual.back() == 0x0A);
    }
    SECTION("Stringify XML UTF-16 declaration with prolog containing new lines.", "[XML][Stringify]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\"?>\n\n\n\n"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with minimal declaration and self closing root tag.", "[XML][Stringify]")
    {
        xmlString = "<?xml version=\"1.0\"?>"
                    "<AddressBook/>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><AddressBook/>");
    }
    SECTION("Stringify XML with self closing root tag  with attributes.", "[XML][Stringify]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<AddressBook number=\"46\" street=\"green\"/>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with root and child elements plus contents.", "[XML][Stringify]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<AddressBook><Address>Address 1</Address><Address>Address 2</Address></AddressBook>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with attributes on root tag.", "[XML][Stringify]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<AddressBook number=\"46\" type=\"commercial\"></AddressBook>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with Root element <AddressBook> with multiple sibling <Address> elements and contents ", "[XML][Stringify]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
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
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with a single line comment in prolog area.", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<!-- A single line comment -->"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with multiple single line comments in the prolog area.", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<!-- A single line comment -->"
                    "<!-- A single line comment -->"
                    "<!-- A single line comment -->"
                    "<root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with multiple comments inside root element and between its children ", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<AddressBook>"
                    "<!--Address one -->"
                    "<Address>    This is some contents 1   </Address>"
                    "<!--Address two -->"
                    "<Address>    This is some contents 2   </Address>"
                    "<!--Address three -->"
                    "<Address>    This is some contents 3   </Address>"
                    "</AddressBook>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with multiple comments inside child of root element with contents ", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<AddressBook>\n"
                    "<Address>\n"
                    "<!--Address one -->\n"
                    "This is some contents 1\n"
                    "<!--Address two -->\n"
                    "This is some contents 2\n"
                    "<!--Address three -->\n"
                    "This is some contents 3\n"
                    "</Address>\n"
                    "</AddressBook>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with a multi line comment in prolog.", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<!-- A single line comment\n"
                    " another line\n"
                    " another line\n"
                    " and another line\n"
                    "--><root></root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify XML with a multi-line comment within child element with contents.", "[XML][Stringify][Comments]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                    "<root>Test <!-- A single line comment\n"
                    " another line\n"
                    " another line\n"
                    " and another line\n"
                    "--> Test Test</root>";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        BufferDestination xmlDestination;
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
}
TEST_CASE("Stringify XML from a list of example files.", "[XML][Stringify]")
{
    XML xml;
    auto testFile = GENERATE(values<std::string>({"./testData/testfile001.xml",
                                                  "./testData/testfile002.xml",
                                                  "./testData/testfile003.xml",
                                                  "./testData/testfile004.xml",
                                                  "./testData/testfile005.xml",
                                                  "./testData/testfile006.xml",
                                                  "./testData/testfile007.xml",
                                                  "./testData/testfile010.xml",
                                                  "./testData/testfile011.xml"}));
    SECTION("Load file into buffer and parse.", "[XML][Parse]")
    {
        std::string expected = readXMLFromFileUTF8(testFile);
        BufferSource xmlSource(expected);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        REQUIRE(xml.stringify(xNodeRoot) == expected);
    }
}

TEST_CASE("Stringify XML with Unicode character in element names, attributes, comments, character data, and processing instructions. ", "[XML][Stringify][Unicode]")
{
    XML xml;
    std::string xmlString;
    SECTION("Japanese characters", "[XML][Stringify][[Unicode]")
    {
        xmlString = xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                                "<config><start_text>転送</start_text></config>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
    SECTION("Stringify well-formed XML document including Chinese, Armenian and Cyrillic characters", "[XML][Stringify][[Unicode]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                    "<俄语 լեզու=\"ռուսերեն\">данные</俄语>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
}

TEST_CASE("Stringify XML with character entities/reference.", "[XML][Stringify][Entities]")
{
    XML xml;
    std::string xmlString;
    SECTION("Stringify entity &amp; in contents area", "[XML][Stringify][Entities]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                    "<root> &amp; </root>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }

    SECTION("Stringify entity &quot; in contents area", "[XML][Stringify][Entities]")
    {
        xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                    "<root> &quot; </root>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    }
      SECTION("Stringify entities &apos; &lt; &gt; in contents area", "[XML][Stringify][Entities]")
      {
        xmlString="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                    "<root> &apos; &lt; &gt; </root>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
      }
      SECTION("Stringify character reference &#x00A5; in contents area", "[XML][Stringify][Entities]")
      {
        xmlString="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                    "<root> &#x00A5; </root>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
      }
      SECTION("Stringify character reference &#163; in contents area", "[XML][Stringify][Entities]")
      {
        xmlString="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                    "<root> &#163; </root>\n";
        BufferSource xmlSource(xmlString);
        std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
        REQUIRE(xml.stringify(xNodeRoot) == xmlString);
      }
    //   SECTION("Stringify entities &amp;&quot;&apos;&gt;&lt; in attribute value", "[XML][Stringify][Entities]")
    //   {
    //     xmlString="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
    //                 "<root attr1=\" &amp;&quot;&apos;&gt;&lt; \">\n"
    //                 "</root>\n";
    //     BufferSource xmlSource(xmlString);
    //     std::unique_ptr<XNode> xNodeRoot = xml.parse(xmlSource);
    //     REQUIRE(xml.stringify(xNodeRoot) == xmlString);
    //   }
    //   SECTION("Parse references &#x00A5;&#163 in attribute value", "[XML][Parse][Entities]")
    //   {
    //     xmlString="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
    //                 " <root attr1=\" &#x00A5;&#163; \"></root>\n";
    //     BufferSource xmlSource(xmlString);
    //     std::unique_ptr<XNode> xNodeRoot=xml.parse(xmlSource);
    //     REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[1]).attributes.size() == 1);
    //     REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[1]).attributes[0].name == "attr1");
    //     REQUIRE(XNodeRef<XNodeElement>((*xNodeRoot)[1]).attributes[0].value == " ¥£ ");
    //   }
}