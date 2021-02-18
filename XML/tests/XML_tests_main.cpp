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
// XML class namespace
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
std::string readXMLFromFileUTF8(const std::string &xmlFileName)
{
  std::ifstream xmlFile;
  xmlFile.open(xmlFileName);
  std::ostringstream xmlFileBuffer;
  xmlFileBuffer << xmlFile.rdbuf();
  return (xmlFileBuffer.str());
}
std::u16string readXMLFromFileUTF16(const std::string &xmlFileName)
{
  std::ifstream fin(xmlFileName, std::ios::binary);
  fin.seekg(0, std::ios::end);
  size_t size = (size_t)fin.tellg();
  fin.seekg(0, std::ios::beg);
  std::u16string u16;
  char ch[2];
  for (int index = 0; index < (int)size / 2; index++)
  {
    fin.read(ch, 2);
    u16 += ((char16_t)(ch[0]) << 8) | ch[1];
  }
  return (u16);
}
// ==========
// Test cases
// ==========
TEST_CASE("Creation and use of ISource (File) interface.", "[XML][Parse][ISource]")
{
  SECTION("Create FileSource with testfile001.xml.", "[XML][Parse][ISource]")
  {
    REQUIRE_NOTHROW(FileSource(kGeneratedXMLFile));
  }
  SECTION("Create FileSource with non existants file.", "[XML][Parse][ISource][Exception]")
  {
    REQUIRE_THROWS_AS(FileSource(kNonExistantXMLFile), std::runtime_error);
    REQUIRE_THROWS_WITH(FileSource(kNonExistantXMLFile), "XML file input stream failed to open or does not exist.");
  }
  SECTION("Create FileSource with testfile001.xml. and positioned on the correct first character", "[XML][Parse][ISource]")
  {
    FileSource source(kSingleXMLFile);
    REQUIRE_FALSE(!source.more());
    REQUIRE((char)source.current() == '<');
  }
  SECTION("Create FileSource with testfile001.xml and then check more positions to correct next character", "[XML][Parse][ISource]")
  {
    FileSource source(kSingleXMLFile);
    source.next();
    REQUIRE_FALSE(!source.more());
    REQUIRE((char)source.current() == 'C');
  }
  SECTION("Create FileSource with testfile001.xml  move past last character, check it and the bytes moved.", "[XML][Parse][ISource]")
  {
    FileSource source(kSingleXMLFile);
    long length = 0;
    while (source.more())
    {
      source.next();
      length++;
    }
    REQUIRE(length == 8697);                   // eof
    REQUIRE(source.current() == (XChar)EOF); // eof
  }
}
TEST_CASE("Creation and use of ISource (Buffer) interface (buffer contains file testfile001.xml).", "[XML][Parse][ISource]")
{
  std::string buffer = readXMLFromFileUTF8(kSingleXMLFile);
  SECTION("Create BufferSource.", "[XML][Parse][ISource]")
  {
    REQUIRE_NOTHROW(BufferSource(buffer));
  }
  SECTION("Create BufferSource with empty buffer.", "[XML][Parse][ISource][Exception]")
  {
    REQUIRE_THROWS_AS(BufferSource(""), std::invalid_argument);
    REQUIRE_THROWS_WITH(BufferSource(""), "Empty source buffer passed to be parsed.");
  }
  SECTION("Create BufferSource with testfile001.xml and that it is positioned on the correct first character", "[XML][Parse][ISource]")
  {
    BufferSource source(buffer);
    REQUIRE_FALSE(!source.more());
    REQUIRE((char)source.current() == '<');
  }
  SECTION("Create BufferSource with testfile001.xml and then check more positions to correct next character", "[XML][Parse][ISource]")
  {
    BufferSource source(buffer);
    source.next();
    REQUIRE_FALSE(!source.more());
    REQUIRE((char)source.current() == 'C');
  }
  SECTION("Create BufferSource with testfile001.xml move past last character, check it and the bytes moved.", "[XML][Parse][ISource]")
  {
    BufferSource source(buffer);
    long length = 0;
    while (source.more())
    {
      source.next();
      length++;
    }
    REQUIRE(length == 8697);                     // eof
    REQUIRE(source.current() == (XChar)EOF); // eof
  }
}
TEST_CASE("Creation and use of IDestination (Buffer) interface.", "[XML][Parse][ISource]")
{
  SECTION("Create BufferDesination.", "[XML][Stringify][IDesination]")
  {
    REQUIRE_NOTHROW(BufferDestination());
  }
  SECTION("Create BufferDestination and get buffer which should be empty.", "[XML][Stringify][IDesination]")
  {
    BufferDestination buffer;
    REQUIRE_FALSE(!buffer.getBuffer().empty());
  }
  SECTION("Create BufferDestination and add one character.", "[XML][Stringify][IDesination]")
  {
    BufferDestination buffer;
    buffer.add("i");
    REQUIRE(buffer.getBuffer().size() == 1);
  }
  SECTION("Create BufferDestination and add an stringified integer and check result.", "[XML][Stringify][IDesination]")
  {
    BufferDestination buffer;
    buffer.add("65767");
    REQUIRE(buffer.getBuffer().size() == 5);
    REQUIRE(buffer.getBuffer() == (U"65767"));
  }
}
TEST_CASE("Creation and use of IDestination (File) interface.", "[XML][Parse][ISource]")
{
  SECTION("Create FileDestination.", "[XML][Stringify][IDesination]")
  {
    std::filesystem::remove(kGeneratedXMLFile);
    REQUIRE_NOTHROW(FileDestination(kGeneratedXMLFile));
  }
  SECTION("Create FileDestination when file already exists.", "[XML][Stringify][IDesination]")
  {
    FileDestination file(kGeneratedXMLFile);
    file = FileDestination(kGeneratedXMLFile);
    REQUIRE_NOTHROW(FileDestination(kGeneratedXMLFile));
  }
  SECTION("Create FileDestination and test file exists and should be empty.", "[XML][Stringify][IDesination]")
  {
    std::filesystem::remove(kGeneratedXMLFile);
    FileDestination file(kGeneratedXMLFile);
    REQUIRE_FALSE(!std::filesystem::exists(kGeneratedXMLFile));
    std::filesystem::path filePath(kGeneratedXMLFile);
    REQUIRE(std::filesystem::file_size(filePath) == 0);
  }
  SECTION("Create FileDestination and add one character.", "[XML][Stringify][IDesination]")
  {
    std::filesystem::remove(kGeneratedXMLFile);
    FileDestination file(kGeneratedXMLFile);
    file.add("t");
    std::filesystem::path filePath(kGeneratedXMLFile);
    REQUIRE(std::filesystem::file_size(filePath) == 1);
  }
  SECTION("Create FileDestination, add an stringified integer and check result.", "[XML][Stringify][IDesination]")
  {
    std::filesystem::remove(kGeneratedXMLFile);
    FileDestination file(kGeneratedXMLFile);
    file.add("65767");
    std::filesystem::path filePath(kGeneratedXMLFile);
    REQUIRE(std::filesystem::file_size(filePath) == 5);
    std::string expected = readXMLFromFileUTF8 (kGeneratedXMLFile);
    REQUIRE(expected == "65767");
  }
}