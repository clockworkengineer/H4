//
// Unit Tests: JSON
//
// Description: Unit tests for JSON class using the Catch2 test framework.
//
// ======================
// Test framework include
// ======================
#include "catch.hpp"
// =========================
// Class under test includes
// =========================
#include "JSON.hpp"
#include "JSONSources.hpp"
#include "JSONDestinations.hpp"
// ============
// STL includes
// ============
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
// ===================
// Unit test constants
// ===================
const char *kGeneratedJSONFile = "./testData/generated.json";
const char *kSIngleJSONFile = "./testData/testfile001.json";
const char *kNonExistantJSONFile = "./testData/doesntexist.json";
// =======================
// JSON class namespace
// =======================
using namespace H4;
// =======================
// Local support functions
// =======================
/// <summary>
///
/// </summary>
/// <param name="aa"></param>
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
///
/// </summary>
/// <param name="aa"></param>
/// <returns></returns>
void checkArray(JNode *jNode)
{ // Array [\"Dog\",1964,true,null]
  JNodeArray *jNodeArray = static_cast<JNodeArray *>(jNode);
  REQUIRE(jNodeArray->nodeType == JSON::JNodeType::array);
  REQUIRE(jNodeArray->value.size() == 4);
  REQUIRE(jNodeArray->value[0]->nodeType == JSON::JNodeType::string);
  REQUIRE(jNodeArray->value[1]->nodeType == JSON::JNodeType::number);
  REQUIRE(jNodeArray->value[2]->nodeType == JSON::JNodeType::boolean);
  REQUIRE(jNodeArray->value[3]->nodeType == JSON::JNodeType::null);
  REQUIRE(static_cast<JNodeString *>(jNodeArray->value[0].get())->value == "Dog");
  REQUIRE(static_cast<JNodeString *>(jNodeArray->value[1].get())->value == "1964");
  REQUIRE(static_cast<JNodeBoolean *>(jNodeArray->value[2].get())->value == true);
  REQUIRE(static_cast<JNodeNull *>(jNodeArray->value[3].get())->value == nullptr);
}
/// <summary>
///
/// </summary>
/// <param name="aa"></param>
/// <returns></returns>
void checkObject(JNode *jNode)
{ // {\"City\":\"Southampton\",\"Population\":500000}
  JNodeObject *jNodeObject = (JNodeObject *)jNode;
  REQUIRE(jNode->nodeType == JSON::JNodeType::object);
  REQUIRE(jNodeObject->value.size() == 2);
  REQUIRE(jNodeObject->value.count("City") > 0);
  REQUIRE(jNodeObject->value.count("Population") > 0);
  REQUIRE(jNodeObject->value["City"]->nodeType == JSON::JNodeType::string);
  REQUIRE(jNodeObject->value["Population"]->nodeType == JSON::JNodeType::number);
  REQUIRE(static_cast<JNodeString *>(jNodeObject->value["City"].get())->value == "Southampton");
  REQUIRE(static_cast<JNodeNumber *>(jNodeObject->value["Population"].get())->value == "500000");
}
// ==========
// Test cases
// ==========
TEST_CASE("Creation and use of JSON for decode of simple types (number, string, boolean, null) ", "[JSON][Decode]")
{
  JSON json;
  std::unique_ptr<JNode> jNode;
  SECTION("Decode an string (example string)", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("\"example string\"");
    REQUIRE(jNode->nodeType == JSON::JNodeType::string);
  }
  SECTION("Decode an number (6767)", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("6767");
    REQUIRE(jNode->nodeType == JSON::JNodeType::number);
  }
  SECTION("Decode an boolean (true)", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("true");
    REQUIRE(jNode->nodeType == JSON::JNodeType::boolean);
  }
  SECTION("Decode an boolean (false)", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("false");
    REQUIRE(jNode->nodeType == JSON::JNodeType::boolean);
  }
  SECTION("Decode an null", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("null");
    REQUIRE(jNode->nodeType == JSON::JNodeType::null);
  }
}
TEST_CASE("Creation and use of JSON for decode of simple types (number, string, boolean, null) and check values.", "[JSON][Decode]")
{
  JSON json;
  std::unique_ptr<JNode> jNode;
  SECTION("Decode an string (example string) and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("\"example string\"");
    REQUIRE(static_cast<JNodeString *>(jNode.get())->value == "example string");
  }
  SECTION("Decode an string (another example string) and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("\"another example string\"");
    REQUIRE(static_cast<JNodeString *>(jNode.get())->value == "another example string");
  }
  SECTION("Decode an number (6767) and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("6767");
    REQUIRE(static_cast<JNodeNumber *>(jNode.get())->value == "6767");
  }
  SECTION("Decode an number (190000) and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("190000");
    REQUIRE(static_cast<JNodeNumber *>(jNode.get())->value == "190000");
  }
  SECTION("Decode an boolean (true) and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("true");
    REQUIRE(static_cast<JNodeBoolean *>(jNode.get())->value == true);
  }
  SECTION("Decode an boolean (false) and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("false");
    REQUIRE(static_cast<JNodeBoolean *>(jNode.get())->value == false);
  }
  SECTION("Decode an null and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("null");
    REQUIRE(static_cast<JNodeNull *>(jNode.get())->value == nullptr);
  }
}
TEST_CASE("Creation and use of JSON for decode of collection types (array, object) ", "[JSON][Decode]")
{
  JSON json;
  std::unique_ptr<JNode> jNode;
  SECTION("Decode an object ({\"name\":\"Robert\",\"Age\":15})", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("{\"name\":\"Robert\",\"Age\":15}");
    REQUIRE(jNode->nodeType == JSON::JNodeType::object);
  }
  SECTION("Decode an array ([ 777,9000,\"apples\"]) ", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("[777,9000,\"apples\"]");
    REQUIRE(jNode->nodeType == JSON::JNodeType::array);
  }
}
TEST_CASE("Creation and use of JSON for decode of collection types (array, object) and check values", "[JSON][Decode]")
{
  JSON json;
  std::unique_ptr<JNode> jNode;
  SECTION("Decode an object {\"name\":\"Robert\",\"Age\":15} and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("{\"Name\":\"Robert\",\"Age\":15}");
    JNodeObject *jNodeObject = (JNodeObject *)jNode.get();
    REQUIRE(jNodeObject->nodeType == JSON::JNodeType::object);
    REQUIRE(jNodeObject->value.size() == 2);
    REQUIRE(jNodeObject->value.count("Name") > 0);
    REQUIRE(jNodeObject->value.count("Age") > 0);
    REQUIRE(jNodeObject->value["Name"]->nodeType == JSON::JNodeType::string);
    REQUIRE(jNodeObject->value["Age"]->nodeType == JSON::JNodeType::number);
    REQUIRE(static_cast<JNodeString *>(jNodeObject->value["Name"].get())->value == "Robert");
    REQUIRE(static_cast<JNodeNumber *>(jNodeObject->value["Age"].get())->value == "15");
  }
  SECTION("Decode an array [777,9000,\"apples\"] and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("[777,9000,\"apples\"]");
    JNodeArray *jNodeArray = (JNodeArray *)jNode.get();
    REQUIRE(jNodeArray->nodeType == JSON::JNodeType::array);
    REQUIRE(jNodeArray->value.size() == 3);
    REQUIRE(jNodeArray->value[0]->nodeType == JSON::JNodeType::number);
    REQUIRE(jNodeArray->value[1]->nodeType == JSON::JNodeType::number);
    REQUIRE(jNodeArray->value[2]->nodeType == JSON::JNodeType::string);
    REQUIRE(static_cast<JNodeNumber *>(jNodeArray->value[0].get())->value == "777");
    REQUIRE(static_cast<JNodeNumber *>(jNodeArray->value[1].get())->value == "9000");
    REQUIRE(static_cast<JNodeString *>(jNodeArray->value[2].get())->value == "apples");
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("{\"City\":\"Southampton\",\"Population\":500000}");
    checkObject(jNode.get());
  }
  SECTION("Decode an array [\"Dog\",1964,true,null ] and check its value", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer("[\"Dog\",1964,true,null]");
    checkArray(jNode.get());
  }
}
TEST_CASE("Creation and use of JSON for decode checking various whitespace characters are ignored.", "[JSON][Decode]")
{
  JSON json;
  std::unique_ptr<JNode> jNode;
  std::string ws = "";
  SECTION("Decode an array [\"Dog\",1964,true,null] with no whitespace.", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} with no whitespace", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
  ws += " ";
  SECTION("Decode an array [\"Dog\",1964,true,null] with whitespace ' '.", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' '.", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
  ws += "\t";
  SECTION("Decode an array [\"Dog\",1964,true,null] with whitespace ' \\t'.", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' \\t'.", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
  ws += "\n";
  SECTION("Decode an array [\"Dog\",1964,true,null] with whitespace ' \\t\\n'.", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' \\t\\n'.", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
  ws += "\r";
  SECTION("Decode an array [\"Dog\",1964,true,null] with whitespace ' \\t\\n\\r'.", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' \\t\\n\\r'.", "[JSON][Decode]")
  {
    jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
}
TEST_CASE("Creation and use of JSON for decode of a list of example JSON files.", "[JSON][Decode]")
{
  auto testFile = GENERATE(values<std::string>({"./testData/testfile001.json",
                                                "./testData/testfile002.json",
                                                "./testData/testfile003.json",
                                                "./testData/testfile004.json"}));
  JSON json;
  SECTION("Decode from buffer", "[JSON][Decode]")
  {
    std::string jsonFileBuffer = readJSONFromFile(testFile);
    REQUIRE_NOTHROW(json.decodeBuffer(jsonFileBuffer));
    std::unique_ptr<JNode> jNode = json.decodeBuffer(jsonFileBuffer);
    REQUIRE(jNode->nodeType == JSON::JNodeType::object);
  }
  SECTION("Decode from file directly", "[JSON][Decode]")
  {
    REQUIRE_NOTHROW(json.decodeFile(testFile));
    std::unique_ptr<JNode> jNode = json.decodeFile(testFile);
    REQUIRE(jNode->nodeType == JSON::JNodeType::object);
  }
}
TEST_CASE("Decode generated exceptions.", "[JSON][Decode][Exceptions]")
{
  // Note: The tests for syntax errors is not exhaustive and more may be added over time.
  JSON json;
  SECTION("Decode passing a empty string", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(json.decodeBuffer(""), std::invalid_argument);
    REQUIRE_THROWS_WITH(json.decodeBuffer(""), "Empty string passed to be decoded.");
  }
  SECTION("Decode file passing a empty file name", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(json.decodeBuffer(""), std::invalid_argument);
    REQUIRE_THROWS_WITH(json.decodeFile(""), "Empty file name passed to be decoded.");
  }
  SECTION("Decode missing terminating '\"' in string", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(json.decodeBuffer("{ \"one\" : \"Apple }"), std::runtime_error);
    REQUIRE_THROWS_WITH(json.decodeBuffer("{ \"one\" : \"Apple }"), "JSON syntax error detected.");
  }
  SECTION("Decode number with starting with invalid character", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(json.decodeBuffer("{ \"one\" : z19034}"), std::runtime_error);
    REQUIRE_THROWS_WITH(json.decodeBuffer("{ \"one\" : z19034 }"), "JSON syntax error detected.");
  }
  SECTION("Decode object with invalid value field (number).", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(json.decodeBuffer("{ \"one\" : 18987u3 }"), std::runtime_error);
    REQUIRE_THROWS_WITH(json.decodeBuffer("{ \"one\" : 18987u3 }"), "JSON syntax error detected.");
  }
  SECTION("Decode object with missing value field.", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(json.decodeBuffer("{ \"one\" : }"), std::runtime_error);
    REQUIRE_THROWS_WITH(json.decodeBuffer("{ \"one\" : }"), "JSON syntax error detected.");
  }
  SECTION("Decode object with missing key field.", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(json.decodeBuffer("{  : 89012 }"), std::runtime_error);
    REQUIRE_THROWS_WITH(json.decodeBuffer("{ : }"), "JSON syntax error detected.");
  }
  SECTION("Decode object with missing closing '}'.", "[JSON][Decode]")
  {
    REQUIRE_THROWS_AS(json.decodeBuffer("{  \"one\" : 18987"), std::runtime_error);
    REQUIRE_THROWS_WITH(json.decodeBuffer("{ \"one\" : 18987 "), "JSON syntax error detected.");
  }
}
TEST_CASE("Creation and use of JSON for encode of simple types (number, string, boolean, null) and check its value", "[JSON][Encode]")
{
  JSON json;
  SECTION("Encode a string (Test string) and check its value", "[JSON][Encode]")
  {
    std::string expected = "\"Test string.\"";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode a string (Test another string) and check its value", "[JSON][Encode]")
  {
    std::string expected = "\"Test another string.\"";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode a boolean (true) and check its value", "[JSON][Encode]")
  {
    std::string expected = "true";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode a boolean (false) and check its value", "[JSON][Encode]")
  {
    std::string expected = "false";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode a number (98345) and check its value", "[JSON][Encode]")
  {
    std::string expected = "98345";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode a number (250000) and check its value", "[JSON][Encode]")
  {
    std::string expected = "25000";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode a null and check its value", "[JSON][Encode]")
  {
    std::string expected = "null";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
}
TEST_CASE("Creation and use of JSON for encode of collection types (object, array) and check its value", "[JSON][Encode]")
{
  JSON json;
  SECTION("Encode an object ({\"Age\":77,\"Name\":\"Rob\"}) and check its value", "[JSON][Encode]")
  {
    std::string expected = "{\"Age\":77,\"Name\":\"Rob\"}";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode an array ([999,\"Time\",null,true] and check its value", "[JSON][Encode]")
  {
    std::string expected = "[999,\"Time\",null,true]";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode an object ({\"City\":\"London\",\"Population\":8000000}) and check its value", "[JSON][Encode]")
  {
    std::string expected = "{\"City\":\"London\",\"Population\":8000000}";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode an array ([true,\"Out of time\",789043e13,true]) and check its value", "[JSON][Encode]")
  {
    std::string expected = "[true,\"Out of time\",789043e13,true]";
    REQUIRE(json.encodeBuffer(json.decodeBuffer(expected)) == expected);
  }
}
TEST_CASE("Encode to a file and check result", "[JSON][Encode][Exceptions]")
{
  JSON json;
  SECTION("Encode object to file and check value", "[JSON][Encode]")
  {
    std::string expected = "{\"City\":\"London\",\"Population\":8000000}";
    if (std::filesystem::exists(kGeneratedJSONFile))
    {
      std::filesystem::remove(kGeneratedJSONFile);
    }
    json.encodeFile(json.decodeBuffer(expected), kGeneratedJSONFile);
    REQUIRE(readJSONFromFile(kGeneratedJSONFile) == expected);
  }
  SECTION("Encode array to file and check value", "[JSON][ENcode]")
  {
    std::string expected = "[999,\"Time\",null,true]";
    if (std::filesystem::exists(kGeneratedJSONFile))
    {
      std::filesystem::remove(kGeneratedJSONFile);
    }
    json.encodeFile(json.decodeBuffer(expected), kGeneratedJSONFile);
    REQUIRE(readJSONFromFile(kGeneratedJSONFile) == expected);
  }
}
TEST_CASE("Encode generated exceptions", "[JSON][Encode][Exceptions]")
{
  JSON json;

  SECTION("Encode passed nullptr", "[JSON][Encode][Exceptions]")
  {
    REQUIRE_THROWS_AS(json.encodeBuffer(std::unique_ptr<JNode>(nullptr)), std::invalid_argument);
    REQUIRE_THROWS_WITH(json.encodeBuffer(std::unique_ptr<JNode>(nullptr)), "Nullptr passed as JNode root to be encoded.");
  }
  SECTION("Encode passed invalid JNode type", "[JSON][Encode][Exceptions]")
  {
    REQUIRE_THROWS_AS(json.encodeBuffer(std::unique_ptr<JNode>(new JNode())), std::runtime_error);
    REQUIRE_THROWS_WITH(json.encodeBuffer(std::unique_ptr<JNode>(new JNode())), "Unknown JNode type encountered during encode.");
  }
  SECTION("Encode file passed invalid JNode type", "[JSON][Encode][Exceptions]")
  {
    REQUIRE_THROWS_AS(json.encodeFile(std::unique_ptr<JNode>(nullptr), kGeneratedJSONFile), std::invalid_argument);
    REQUIRE_THROWS_WITH(json.encodeFile(std::unique_ptr<JNode>(nullptr), kGeneratedJSONFile), "Nullptr passed as JNode root to be encoded.");
  }
  SECTION("Encode file passed empty string for file name", "[JSON][Encode][Exceptions]")
  {
    REQUIRE_THROWS_AS(json.encodeFile(std::unique_ptr<JNode>(new JNode()), ""), std::invalid_argument);
    REQUIRE_THROWS_WITH(json.encodeFile(std::unique_ptr<JNode>(new JNode()), ""), "Empty file name passed to be encoded.");
  }
}
TEST_CASE("Creation and use of JSON for encode of a list of example JSON files.", "[JSON][Encode]")
{
  auto testFile = GENERATE(values<std::string>({"./testData/testfile001.json",
                                                "./testData/testfile002.json",
                                                "./testData/testfile003.json",
                                                "./testData/testfile004.json"}));
  JSON json;
  SECTION("Encode to  buffer and check value", "[JSON][Encode]")
  {
    std::string jsonFileBuffer = readJSONFromFile(testFile);
    REQUIRE(json.encodeBuffer(json.decodeBuffer(jsonFileBuffer)) == json.stripWhiteSpaceBuffer(jsonFileBuffer));
  }
  SECTION("Encode to file and check value", "[JSON][ENcode]")
  {
    if (std::filesystem::exists(kGeneratedJSONFile))
    {
      std::filesystem::remove(kGeneratedJSONFile);
    }
    std::string jsonFileBuffer = readJSONFromFile(testFile);
    json.encodeFile(json.decodeBuffer(jsonFileBuffer), kGeneratedJSONFile);
    REQUIRE(readJSONFromFile(kGeneratedJSONFile) == json.stripWhiteSpaceBuffer(jsonFileBuffer));
  }
}
TEST_CASE("Creation and use of ISource (File) interface.", "[JSON][Decode][ISource]")
{
  SECTION("Create FileSource with testfile001.json.", "[JSON][Decode][ISource]")
  {
    REQUIRE_NOTHROW(FileSource(kGeneratedJSONFile));
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
    REQUIRE(length == 583);                       // eof
    REQUIRE(source.currentByte() == (char) 0xff); // eof
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
    REQUIRE(length == 583);                      // eof
    REQUIRE(source.currentByte() == (char) 255); // eof
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
//   SECTION("Create FileDestination.", "[JSON][Encode][IDesination]")
//   {
//     if (std::filesystem::exists(kGeneratedJSONFile))
//     {
//       std::filesystem::remove(kGeneratedJSONFile);
//     }
//     REQUIRE_NOTHROW(FileDestination(kGeneratedJSONFile));
//   }
//   SECTION("Create FileDestination when file already exists.", "[JSON][Encode][IDesination]")
//   {
//     FileDestination file(kGeneratedJSONFile);
//     if (!std::filesystem::exists(kGeneratedJSONFile))
//     {
//       file = FileDestination(kGeneratedJSONFile);
//     }
//     REQUIRE_NOTHROW(FileDestination(kGeneratedJSONFile));
//   }
//   SECTION("Create FileDestination and test file exists and should be empty.", "[JSON][Encode][IDesination]")
//   {
//     if (std::filesystem::exists(kGeneratedJSONFile))
//     {
//       std::filesystem::remove(kGeneratedJSONFile);
//     }
//     FileDestination file(kGeneratedJSONFile);
//     REQUIRE_FALSE(!std::filesystem::exists(kGeneratedJSONFile));
//     std::filesystem::path filePath(kGeneratedJSONFile);
//     REQUIRE(std::filesystem::file_size(filePath) == 0);
//   }
//   SECTION("Create FileDestination and add one character.", "[JSON][Encode][IDesination]")
//   {
//     if (std::filesystem::exists(kGeneratedJSONFile))
//     {
//       std::filesystem::remove(kGeneratedJSONFile);
//     }
//     FileDestination file(kGeneratedJSONFile);
//     file.addBytes("i");
//     std::filesystem::path filePath(kGeneratedJSONFile);
//     REQUIRE(std::filesystem::file_size(filePath) == 1);
//   }
//   SECTION("Create FileDestination, add an encoded integer and check result.", "[JSON][Encode][IDesination]")
//   {
//     if (std::filesystem::exists(kGeneratedJSONFile))
//     {
//       std::filesystem::remove(kGeneratedJSONFile);
//     }
//     FileDestination file(kGeneratedJSONFile);
//     file.addBytes("i65767e");
//     std::filesystem::path filePath(kGeneratedJSONFile);
//     REQUIRE(std::filesystem::file_size(filePath) == 7);
//     std::ifstream torrentFile{kGeneratedJSONFile};
//     std::ostringstream expected;
//     expected << torrentFile.rdbuf();
//     REQUIRE(expected.str() == "i65767e");
   }