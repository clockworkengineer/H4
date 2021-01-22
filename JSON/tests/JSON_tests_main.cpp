//
// Unit Tests: JSON
//
// Description: ISource, IDestination and JNode index/reference
// unit tests for JSON class using the Catch2 test framework.
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
  REQUIRE(jNode->nodeType == JNodeType::array);
  REQUIRE(JNodeRef<JNodeArray>(*jNode).size() == 4);
  REQUIRE((*jNode)[0].nodeType == JNodeType::string);
  REQUIRE((*jNode)[1].nodeType == JNodeType::number);
  REQUIRE((*jNode)[2].nodeType == JNodeType::boolean);
  REQUIRE((*jNode)[3].nodeType == JNodeType::null);
  REQUIRE(JNodeRef<JNodeString>((*jNode)[0]).getString() == "Dog");
  REQUIRE(JNodeRef<JNodeNumber>((*jNode)[1]).getNumber() == "1964");
  REQUIRE(JNodeRef<JNodeBoolean>((*jNode)[2]).getBoolean() == true);
  REQUIRE(JNodeRef<JNodeNull>((*jNode)[3]).getNull() == nullptr);
}
/// <summary>
/// Verify that an JNodeObject has the correct decoded format.
/// </summary>
/// <param name="jNode">Pointer to JNodeObject</param>
/// <returns></returns>
void checkObject(JNode *jNode)
{ // {\"City\":\"Southampton\",\"Population\":500000}
  REQUIRE(jNode->nodeType == JNodeType::object);
  REQUIRE(JNodeRef<JNodeObject>(*jNode).size() == 2);
  REQUIRE(JNodeRef<JNodeObject>(*jNode).containsKey("City"));
  REQUIRE(JNodeRef<JNodeObject>(*jNode).containsKey("Population"));
  REQUIRE((*jNode)["City"].nodeType == JNodeType::string);
  REQUIRE((*jNode)["Population"].nodeType == JNodeType::number);
  REQUIRE(JNodeRef<JNodeString>((*jNode)["City"]).getString() == "Southampton");
  REQUIRE(JNodeRef<JNodeNumber>((*jNode)["Population"]).getNumber() == "500000");
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
  SECTION("Create FileSource with non existants file.", "[JSON][Decode][ISource][Exception]")
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
  SECTION("Create BufferSource with empty buffer.", "[JSON][Decode][ISource][Exception]")
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
// Implement these for JSON
TEST_CASE("Use of JNode indexing operators", "[JSON][JNode][Index]")
{
  JSON json;
  std::unique_ptr<JNode> jNode;
  SECTION("Decode dictionary and check its components using indexing", "[JSON][JNode][Index]")
  {
    jNode = json.decodeBuffer("{\"City\":\"Southampton\",\"Population\":500000}");
    checkObject(jNode.get());
    REQUIRE(JNodeRef<JNodeString>((*jNode)["City"]).getString() == "Southampton");
    REQUIRE(JNodeRef<JNodeNumber>((*jNode)["Population"]).getNumber() == "500000");
  }
  SECTION("Decode list and check its components using indexing", "[JSON][JNode][Index]")
  {
    jNode = json.decodeBuffer("[777,9000,\"apples\"]");
    REQUIRE(JNodeRef<JNodeNumber>((*jNode)[0]).getNumber() == "777");
    REQUIRE(JNodeRef<JNodeNumber>((*jNode)[1]).getNumber() == "9000");
    REQUIRE(JNodeRef<JNodeString>((*jNode)[2]).getString() == "apples");
  }
  SECTION("Decode list with embedded dictioanry and check its components using indexing", "[JSON][JNode][Index]")
  {
    jNode = json.decodeBuffer("[777,{\"City\":\"Southampton\",\"Population\":500000},\"apples\"]");
    REQUIRE(JNodeRef<JNodeNumber>((*jNode)[0]).getNumber() == "777");
    REQUIRE(JNodeRef<JNodeString>((*jNode)[1]["City"]).getString() == "Southampton");
    REQUIRE(JNodeRef<JNodeNumber>((*jNode)[1]["Population"]).getNumber() == "500000");
    REQUIRE(JNodeRef<JNodeString>((*jNode)[2]).getString() == "apples");
  }
  SECTION("Decode dictionary and check an invalid key generates exception", "[JSON][JNode][Index]")
  {
    jNode = json.decodeBuffer("{\"City\":\"Southampton\",\"Population\":500000}");
    REQUIRE_THROWS_AS((*jNode)["Cityy"].nodeType == JNodeType::object, std::runtime_error);
    REQUIRE_THROWS_WITH((*jNode)["Cityy"].nodeType == JNodeType::object, "Invalid key used to access object.");
  }
  SECTION("Decode list and check an invalid index generates exception", "[JSON][JNode][Index]")
  {
    jNode = json.decodeBuffer("[777,9000,\"apples\"]");
    REQUIRE_THROWS_AS((*jNode)[3].nodeType == JNodeType::array, std::runtime_error);
    REQUIRE_THROWS_WITH((*jNode)[3].nodeType == JNodeType::array, "Invalid index used to access array.");
  }
}
TEST_CASE("Check JNode reference functions work.", "[JSON][JNode][Reference]")
{
  JSON bEncode;
  std::unique_ptr<JNode> jNode;
  SECTION("Integer reference.", "[JSON][JNode][Reference]")
  {
    jNode = bEncode.decodeBuffer("45500");
    REQUIRE(JNodeRef<JNodeNumber>((*jNode)).getNumber() == "45500");
  }
  SECTION("String reference.", "[JSON][JNode][Reference]")
  {
    jNode = bEncode.decodeBuffer("0123456789");
    REQUIRE(JNodeRef<JNodeString>((*jNode)).getString() == "0123456789");
  }
  SECTION("Array reference.", "[JSON][JNode][Reference]")
  {
    jNode = bEncode.decodeBuffer("[777,9000,\"apples\"]");
    REQUIRE(JNodeRef<JNodeArray>((*jNode)).size() == 3);
    REQUIRE(JNodeRef<JNodeString>((*jNode)[2]).getString() == "apples");
  }
  SECTION("Dictionary reference.", "[JSON][JNode][Reference]")
  {
    jNode = bEncode.decodeBuffer("{\"City\":\"Southampton\",\"Population\":500000}");
    REQUIRE(JNodeRef<JNodeObject>((*jNode)).size() == 2);
    REQUIRE(JNodeRef<JNodeString>((*jNode)["City"]).getString() == "Southampton");
  }
}
TEST_CASE("Check JNodeNumber number conversion", "[JSON][JNode][JNodeNumber]")
{
  JSON json;
  std::unique_ptr<JNode> jNode;
  SECTION("Floating point not converted to Integer", "[JSON][JNode][JNodeNumber]")
  {
    long longValue;
    jNode = json.decodeBuffer("678.8990");
    REQUIRE_FALSE(JNodeRef<JNodeNumber>(*jNode).getInteger(longValue));
  }
  SECTION("Floating point converted to double", "[JSON][JNode][JNodeNumber]")
  {
    double doubleValue;
    jNode = json.decodeBuffer("678.8990");
    REQUIRE_FALSE(!JNodeRef<JNodeNumber>(*jNode).getFloatingPoint(doubleValue));
    REQUIRE(doubleValue == 678.8990);
  }
  SECTION("Integer point converted to Long", "[JSON][JNode][JNodeNumber]")
  {
    long longValue;
    jNode = json.decodeBuffer("78989");
    REQUIRE_FALSE(!JNodeRef<JNodeNumber>(*jNode).getInteger(longValue));
    REQUIRE(longValue == 78989);
  }
  SECTION("Integer point not converted to double", "[JSON][JNode][JNodeNumber]")
  {
    double doubleValue;
    jNode = json.decodeBuffer("78989");
    REQUIRE_FALSE(!JNodeRef<JNodeNumber>(*jNode).getFloatingPoint(doubleValue));
  }
  SECTION("Check  flaoing point with exponent", "[JSON][JNode][JNodeNumber][Exception")
  {
    REQUIRE_NOTHROW(jNode = json.decodeBuffer("78.43e-2"));
  }
  SECTION("Check floaing point with invalid exponent", "[JSON][JNode][JNodeNumber][Exception]")
  {
    REQUIRE_THROWS_AS(jNode = json.decodeBuffer("78.e43e-2"), std::runtime_error);
    REQUIRE_THROWS_WITH(jNode = json.decodeBuffer("78.e43e-2"), "JSON syntax error detected.");
  }
  SECTION("Check floaing point with multiple decimal points", "[JSON][JNode][JNodeNumber][Exception]")
  {
    REQUIRE_THROWS_AS(jNode = json.decodeBuffer("78.5454.545"), std::runtime_error);
    REQUIRE_THROWS_WITH(jNode = json.decodeBuffer("78.5454.545"), "JSON syntax error detected.");
  }
}
TEST_CASE("Check string escape translation", "[JSON][DefaultTranslator]")
{
  JSONTranslator translator;
  SECTION("Translate from escape sequences 'Begin \\uD834\\uDD1E End' and check value", "[JSON][Decode]")
  {
    REQUIRE(translator.fromEscapeSequences("Begin \\uD834\\uDD1E End") == "Begin \U0001D11E End");
  }
  SECTION("Translate from escape sequences 'Begin \\uD834 \\uDD1E End' expect excpetion", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(translator.fromEscapeSequences("Begin \\uD834 \\uDD1E End"), std::runtime_error);
    REQUIRE_THROWS_WITH(translator.fromEscapeSequences("Begin \\uD834 \\uDD1E End"), "JSON syntax error detected.");
  }

  SECTION("Translate from escape sequences 'Begin \\uD834\\u0045 End' expect excpetion", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(translator.fromEscapeSequences("Begin \\uD834\\u0045 End"), std::runtime_error);
    REQUIRE_THROWS_WITH(translator.fromEscapeSequences("Begin \\uD834\\u0045 End"), "JSON syntax error detected.");
  }
  SECTION("Translate from escape sequences 'Begin \\uD834 End' expect excpetion", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(translator.fromEscapeSequences("Begin \\uD834 End"), std::runtime_error);
    REQUIRE_THROWS_WITH(translator.fromEscapeSequences("Begin \\uD834 End"), "JSON syntax error detected.");
  }
    SECTION("Translate from escape sequences 'Begin \\uDD1E End' expect excpetion", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(translator.fromEscapeSequences("Begin \\uDD1E End"), std::runtime_error);
    REQUIRE_THROWS_WITH(translator.fromEscapeSequences("Begin \\uDD1E End"), "JSON syntax error detected.");
  }
  

}