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
    REQUIRE(xml.m_prolog[0].getContents() == " & ");
  }
  SECTION("Parse entity &quot; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root> &quot; </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].getContents() == " \" ");
  }
  SECTION("Parse entities &apos; &lt; &gt; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root> &apos; &lt; &gt; </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].getContents() == " ' < > ");
  }
  SECTION("Parse reference &#x00A5; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root> &#x00A5; </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].getContents() == " ¥ ");
  }
  SECTION("Parse reference &#163; in contents area", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                "<root> &#163; </root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].getContents() == " £ ");
  }
  SECTION("Parse entity &amp;&quot;&apos;&gt;&lt; in attribute value", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root attr1=\" &amp;&quot;&apos;&gt;&lt; \">\n"
                "</root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0].getAttribute("attr1").value.parsed == " &#x26;&#x22;&#x27;&#x3E;&#x3C; ");
  }
  SECTION("Parse references &#x00A5;&#163 in attribute value", "[XML][Parse][Entities]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
                " <root attr1=\" &#x00A5;&#163; \"></root>\n";
    BufferSource xmlSource(xmlString);
    XML xml(xmlSource);
    xml.parse();
    REQUIRE(xml.m_prolog[0].getAttributeList().size() == 1);
    REQUIRE(xml.m_prolog[0].getAttribute("attr1").value.parsed == " ¥£ ");
  }
}