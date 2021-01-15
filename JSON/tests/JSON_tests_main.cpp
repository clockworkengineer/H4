//
// Unit Tests: JSON
//
// Description: ISource, IDestination and JNode unit tests for
// JSON class using the Catch2 test framework.
//
// ================
// Test definitions
// =================
#include "JSON_tests.hpp"
// =======================
// JSON class namespace
// =======================
using namespace H4;
// ==========================
// Unit test helper functions
// ==========================
/// <summary>
/// Open a JSON file, read its contents into a string buffer and return
/// the buffer.
/// </summary>
/// <param name="jsonFileName">JSON file name</param>
/// <returns></returns>
std::string readJSONFromFile(const std::string &jsonFileName)
{
  std::ifstream jsonFile;
  jsonFile.open(jsonFileName);
  std::ostringstream jsonFileBuffer;
  jsonFileBuffer << jsonFile.rdbuf();
  return (jsonFileBuffer.str());
}
/// <summary>
/// Verify that an JNodeArray has the correct decoded format.
/// </summary>
/// <param name="jNode">Pointer to JNNodeArray</param>
/// <returns></returns>
void checkArray(JNode *jNode)
{ // Array [\"Dog\",1964,true,null]
  REQUIRE(jNode->nodeType == JSON::JNodeType::array);
  REQUIRE(JNode::refJNodeArray(*jNode).value.size() == 4);
  REQUIRE((*jNode)[0].nodeType == JSON::JNodeType::string);
  REQUIRE((*jNode)[1].nodeType == JSON::JNodeType::number);
  REQUIRE((*jNode)[2].nodeType == JSON::JNodeType::boolean);
  REQUIRE((*jNode)[3].nodeType == JSON::JNodeType::null);
  REQUIRE(JNode::refJNodeString((*jNode)[0]).value == "Dog");
  REQUIRE(JNode::refJNodeNumber((*jNode)[1]).value == "1964");
  REQUIRE(JNode::refJNodeBoolean((*jNode)[2]).value == true);
  REQUIRE(JNode::refJNodeNull((*jNode)[3]).value == nullptr);
}
/// <summary>
/// Verify that an JNodeObject has the correct decoded format.
/// </summary>
/// <param name="jNode">Pointer to JNodeObject</param>
/// <returns></returns>
void checkObject(JNode *jNode)
{ // {\"City\":\"Southampton\",\"Population\":500000}
  REQUIRE(jNode->nodeType == JSON::JNodeType::object);
  REQUIRE(JNode::refJNodeObject(*jNode).value.size() == 2);
  REQUIRE(JNode::refJNodeObject(*jNode).value.count("City") > 0);
  REQUIRE(JNode::refJNodeObject(*jNode).value.count("Population") > 0);
  REQUIRE((*jNode)["City"].nodeType == JSON::JNodeType::string);
  REQUIRE((*jNode)["Population"].nodeType == JSON::JNodeType::number);
  REQUIRE(JNode::refJNodeString((*jNode)["City"]).value == "Southampton");
  REQUIRE(JNode::refJNodeNumber((*jNode)["Population"]).value == "500000");
}
// ==========
// Test cases
// ==========
TEST_CASE("Creation and use of ISource (File) interface.", "[JSON][Decode][ISource]")
{
  SECTION("Create FileSource with testfile001.json.", "[JSON][Decode][ISource]")
  {
    REQUIRE_NOTHROW(FileSource(kGeneratedTorrentFile));
  }
  SECTION("Create FileSource with non existants file.", "[JSON][Decode][ISource]")
  {
    REQUIRE_THROWS_AS(FileSource(kNonExistantJSONFile), std::runtime_error);
    REQUIRE_THROWS_WITH(FileSource(kNonExistantJSONFile), "JSON file input stream failed to open or does not exist.");
  }
  SECTION("Create FileSource with testfile001.json. and positioned on the correct first character", "[JSOND][Decode][ISource]")
  {
    FileSource source = FileSource(kSIngleJSONFile);
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == '{');
  }
  SECTION("Create FileSource with testfile001.json and then check moveToNextByte positions to correct next character", "[JSON][Decode][ISource]")
  {
    FileSource source = FileSource(kSIngleJSONFile);
    source.moveToNextByte();
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == '\n');
  }
  SECTION("Create FileSource with testfile001.json  move past last character, check it and the bytes moved.", "[JSON][Decode][ISource]")
  {
    FileSource source = FileSource(kSIngleJSONFile);
    long length = 0;
    while (source.bytesToDecode())
    {
      source.moveToNextByte();
      length++;
    }
    REQUIRE(length == 583);                      // eof
    REQUIRE(source.currentByte() == (char)0xff); // eof
  }
}
TEST_CASE("Creation and use of ISource (Buffer) interface (buffer contains file testfile001.json).", "[JSON][Decode][ISource]")
{
  std::string buffer = readJSONFromFile(kSIngleJSONFile);
  SECTION("Create BufferSource.", "[JSON][Decode][ISource]")
  {
    REQUIRE_NOTHROW(BufferSource(buffer));
  }
  SECTION("Create BufferSource with empty buffer.", "[JSON][Decode][ISource]")
  {
    REQUIRE_THROWS_AS(BufferSource(""), std::invalid_argument);
    REQUIRE_THROWS_WITH(BufferSource(""), "Empty source buffer passed to be encoded.");
  }
  SECTION("Create BufferSource with testfile001.json and that it is positioned on the correct first character", "[JSON][Decode][ISource]")
  {
    BufferSource source = BufferSource(buffer);
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == '{');
  }
  SECTION("Create BufferSource with testfile001.json and then check moveToNextByte positions to correct next character", "[JSON][Decode][ISource]")
  {
    BufferSource source = BufferSource(buffer);
    source.moveToNextByte();
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == '\n');
  }
  SECTION("Create BufferSource with testfile001.json move past last character, check it and the bytes moved.", "[JSON][Decode][ISource]")
  {
    BufferSource source = BufferSource(buffer);
    long length = 0;
    while (source.bytesToDecode())
    {
      source.moveToNextByte();
      length++;
    }
    REQUIRE(length == 583);                     // eof
    REQUIRE(source.currentByte() == (char)255); // eof
  }
}
TEST_CASE("Creation and use of IDestination (Buffer) interface.", "[JSON][Decode][ISource]")
{
  SECTION("Create BufferDesination.", "[JSON][Encode][IDesination]")
  {
    REQUIRE_NOTHROW(BufferDestination());
  }
  SECTION("Create BufferDestination and get buffer which should be empty.", "[JSON][Encode][IDesination]")
  {
    BufferDestination buffer;
    REQUIRE_FALSE(!buffer.getBuffer().empty());
  }
  SECTION("Create BufferDestination and add one character.", "[JSON][Encode][IDesination]")
  {
    BufferDestination buffer;
    buffer.addBytes("i");
    REQUIRE(buffer.getBuffer().size() == 1);
  }
  SECTION("Create BufferDestination and add an encoded integer and check result.", "[JSON][Encode][IDesination]")
  {
    BufferDestination buffer;
    buffer.addBytes("65767");
    REQUIRE(buffer.getBuffer().size() == 5);
    REQUIRE(buffer.getBuffer() == ("65767"));
  }
}
TEST_CASE("Creation and use of IDestination (File) interface.", "[JSON][Decode][ISource]")
{
  SECTION("Create FileDestination.", "[JSON][Encode][IDesination]")
  {
    std::filesystem::remove(kGeneratedTorrentFile);
    REQUIRE_NOTHROW(FileDestination(kGeneratedTorrentFile));
  }
  SECTION("Create FileDestination when file already exists.", "[JSON][Encode][IDesination]")
  {
    FileDestination file(kGeneratedTorrentFile);
    file = FileDestination(kGeneratedTorrentFile);
    REQUIRE_NOTHROW(FileDestination(kGeneratedTorrentFile));
  }
  SECTION("Create FileDestination and test file exists and should be empty.", "[JSON][Encode][IDesination]")
  {
    std::filesystem::remove(kGeneratedTorrentFile);
    FileDestination file(kGeneratedTorrentFile);
    REQUIRE_FALSE(!std::filesystem::exists(kGeneratedTorrentFile));
    std::filesystem::path filePath(kGeneratedTorrentFile);
    REQUIRE(std::filesystem::file_size(filePath) == 0);
  }
  SECTION("Create FileDestination and add one character.", "[JSON][Encode][IDesination]")
  {
    std::filesystem::remove(kGeneratedTorrentFile);
    FileDestination file(kGeneratedTorrentFile);
    file.addBytes("t");
    std::filesystem::path filePath(kGeneratedTorrentFile);
    REQUIRE(std::filesystem::file_size(filePath) == 1);
  }
  SECTION("Create FileDestination, add an encoded integer and check result.", "[JSON][Encode][IDesination]")
  {
    std::filesystem::remove(kGeneratedTorrentFile);
    FileDestination file(kGeneratedTorrentFile);
    file.addBytes("65767");
    std::filesystem::path filePath(kGeneratedTorrentFile);
    REQUIRE(std::filesystem::file_size(filePath) == 5);
    std::string expected = readJSONFromFile(kGeneratedTorrentFile);
    REQUIRE(expected == "65767");
  }
}