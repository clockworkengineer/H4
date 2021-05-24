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
