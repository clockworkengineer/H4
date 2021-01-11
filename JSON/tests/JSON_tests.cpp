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
// =======================
// JSON class namespace
// =======================
using namespace H4;
// =======================
// Local support functions
// =======================
bool compareFiles(const std::string &fileName1, const std::string &fileName2)
{
  std::ifstream file1(fileName1, std::ifstream::binary | std::ifstream::ate);
  std::ifstream file2(fileName2, std::ifstream::binary | std::ifstream::ate);
  if (file1.fail() || file2.fail())
  {
    return false; //file problem
  }
  if (file1.tellg() != file2.tellg())
  {
    return false; //size mismatch
  }
  //seek back to beginning and use std::equal to compare contents
  file1.seekg(0, std::ifstream::beg);
  file2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(file1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(file2.rdbuf()));
}
void checkArray(JNode *jNode)
{
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
void checkObject(JNode *jNode)
{
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
    std::ifstream jsonFile;
    jsonFile.open(testFile);
    std::ostringstream buffer;
    buffer << jsonFile.rdbuf();
    REQUIRE_NOTHROW(json.decodeBuffer(buffer.str()));
    std::unique_ptr<JNode> jNode = json.decodeBuffer(buffer.str());
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