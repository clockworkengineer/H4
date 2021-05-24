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