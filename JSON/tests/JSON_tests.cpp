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
  REQUIRE(((JNode *)jNode)->nodeType == JSON::JNodeType::array);
  REQUIRE(((JNodeArray *)jNode)->value.size() == 4);
  REQUIRE(((JNode *)((JNodeArray *)jNode)->value[0].get())->nodeType == JSON::JNodeType::string);
  REQUIRE(((JNode *)((JNodeArray *)jNode)->value[1].get())->nodeType == JSON::JNodeType::number);
  REQUIRE(((JNode *)((JNodeArray *)jNode)->value[2].get())->nodeType == JSON::JNodeType::boolean);
  REQUIRE(((JNode *)((JNodeArray *)jNode)->value[3].get())->nodeType == JSON::JNodeType::null);
  REQUIRE(((JNodeNumber *)((JNodeArray *)jNode)->value[0].get())->value == "Dog");
  REQUIRE(((JNodeString *)((JNodeArray *)jNode)->value[1].get())->value == "1964");
  REQUIRE(((JNodeBoolean *)((JNodeArray *)jNode)->value[2].get())->value == true);
  REQUIRE(((JNodeNull *)((JNodeArray *)jNode)->value[2].get())->value == nullptr);
}
void checkObject(JNode *jNode)
{
  REQUIRE(((JNode *)jNode)->nodeType == JSON::JNodeType::object);
  REQUIRE(((JNodeObject *)jNode)->value.size() == 2);
  REQUIRE(((JNodeObject *)jNode)->value.count("City") > 0);
  REQUIRE(((JNodeObject *)jNode)->value.count("Population") > 0);
  REQUIRE(((JNode *)((JNodeObject *)jNode)->value["City"].get())->nodeType == JSON::JNodeType::string);
  REQUIRE(((JNode *)((JNodeObject *)jNode)->value["Population"].get())->nodeType == JSON::JNodeType::number);
  REQUIRE(((JNodeString *)((JNodeObject *)jNode)->value["City"].get())->value == "Southampton");
  REQUIRE(((JNodeNumber *)((JNodeObject *)jNode)->value["Population"].get())->value == "500000");
}
// ==========
// Test cases
// ==========
TEST_CASE("Creation and use of JSON for decode of simple types (number, string, boolean, null) ", "[JSON][Decode]")
{
  JSON json;
  SECTION("Decode an string", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("\"example string\"");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::string);
  }
  SECTION("Decode an number", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("6767");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::number);
  }
  SECTION("Decode an boolean (true)", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("true");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::boolean);
  }
  SECTION("Decode an boolean (false)", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("false");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::boolean);
  }
  SECTION("Decode an null", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("null");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::null);
  }
}
TEST_CASE("Creation and use of JSON for decode of simple types (number, string, boolean, null) and check values.", "[JSON][Decode]")
{
  JSON json;
  SECTION("Decode an string and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("\"example string\"");
    REQUIRE(((JNodeString *)jNode.get())->value == "example string");
  }
  SECTION("Decode an string and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("\"another example string\"");
    REQUIRE(((JNodeString *)jNode.get())->value == "another example string");
  }
  SECTION("Decode an number and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("6767");
    REQUIRE(((JNodeNumber *)jNode.get())->value == "6767");
  }
  SECTION("Decode an number and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("190000");
    REQUIRE(((JNodeNumber *)jNode.get())->value == "190000");
  }
  SECTION("Decode an boolean (true) and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("true");
    REQUIRE(((JNodeBoolean *)jNode.get())->value == true);
  }
  SECTION("Decode an boolean (false) and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("false");
    REQUIRE(((JNodeBoolean *)jNode.get())->value == false);
  }
  SECTION("Decode an null and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("null");
    REQUIRE(((JNodeNull *)jNode.get())->value == nullptr);
  }
}
TEST_CASE("Creation and use of JSON for decode of collection types (array, object) ", "[JSON][Decode]")
{
  JSON json;
  SECTION("Decode an object", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("{ \"name\" : \"Robert\", \"Age\" : 15}");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::object);
  }
  SECTION("Decode an array", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("[ 777, 9000, \"apples\"]");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::array);
  }
}
TEST_CASE("Creation and use of JSON for decode of collection types (array, object) and check values", "[JSON][Decode]")
{
  JSON json;
  SECTION("Decode an object { \"name\" : \"Robert\", \"Age\" : 15} and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("{ \"Name\" : \"Robert\", \"Age\" : 15}");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::object);
    REQUIRE(((JNodeObject *)jNode.get())->value.size() == 2);
    REQUIRE(((JNodeObject *)jNode.get())->value.count("Name") > 0);
    REQUIRE(((JNodeObject *)jNode.get())->value.count("Age") > 0);
    REQUIRE(((JNode *)((JNodeObject *)jNode.get())->value["Name"].get())->nodeType == JSON::JNodeType::string);
    REQUIRE(((JNode *)((JNodeObject *)jNode.get())->value["Age"].get())->nodeType == JSON::JNodeType::number);
    REQUIRE(((JNodeString *)((JNodeObject *)jNode.get())->value["Name"].get())->value == "Robert");
    REQUIRE(((JNodeNumber *)((JNodeObject *)jNode.get())->value["Age"].get())->value == "15");
  }
  SECTION("Decode an array [ 777, 9000, \"apples\"] and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("[ 777, 9000, \"apples\"]");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::array);
    REQUIRE(((JNodeArray *)jNode.get())->value.size() == 3);
    REQUIRE(((JNode *)((JNodeArray *)jNode.get())->value[0].get())->nodeType == JSON::JNodeType::number);
    REQUIRE(((JNode *)((JNodeArray *)jNode.get())->value[1].get())->nodeType == JSON::JNodeType::number);
    REQUIRE(((JNode *)((JNodeArray *)jNode.get())->value[2].get())->nodeType == JSON::JNodeType::string);
    REQUIRE(((JNodeNumber *)((JNodeArray *)jNode.get())->value[0].get())->value == "777");
    REQUIRE(((JNodeNumber *)((JNodeArray *)jNode.get())->value[1].get())->value == "9000");
    REQUIRE(((JNodeString *)((JNodeArray *)jNode.get())->value[2].get())->value == "apples");
  }
  SECTION("Decode object { \"City\" : \"Southampton\", \"Population\" : 500000} and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("{ \"City\" : \"Southampton\", \"Population\" : 500000}");
    checkObject(jNode.get());
  }
  SECTION("Decode an array [ \"Dog\", 1964, true, null ] and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer("[ \"Dog\", 1964, true, null ]");
    checkArray(jNode.get());
  }
}
TEST_CASE("Creation and use of JSON for decode checking various whitespace characters are ignored.", "[JSON][Decode]")
{
  JSON json;
  std::string ws = "";
  SECTION("Decode an array [\"Dog\",1964,true,null] with no whitespace.", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} with no whitespace", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
  ws += " ";
  SECTION("Decode an array [\"Dog\",1964,true,null] with whitespace ' '.", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' '.", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
  ws += "\t";
  SECTION("Decode an array [\"Dog\",1964,true,null] with whitespace ' \\t'.", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' \\t'.", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
  ws += "\n";
  SECTION("Decode an array [\"Dog\",1964,true,null] with whitespace ' \\t\\n'.", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' \\t\\n'.", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
  ws += "\r";
  SECTION("Decode an array [\"Dog\",1964,true,null] with whitespace ' \\t\\n\\r'.", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
    checkArray(jNode.get());
  }
  SECTION("Decode object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' \\t\\n\\r'.", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decodeBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
    checkObject(jNode.get());
  }
}
TEST_CASE("Creation and use of JSON for decode of a table of example JSON files that are read info memory.", "[JSON][Decode]")
{
  auto testFile = GENERATE(values<std::string>({"./testData/testfile001.json",
                                                "./testData/testfile002.json",
                                                "./testData/testfile003.json",
                                                "./testData/testfile004.json"}));
  JSON json;
  std::ifstream jsonFile;
  jsonFile.open(testFile);
  std::ostringstream buffer; 
  buffer << jsonFile.rdbuf();
  REQUIRE_NOTHROW(json.decodeBuffer(buffer.str()));
  std::unique_ptr<JNode> jNode = json.decodeBuffer(buffer.str());
  REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::object);
}
TEST_CASE("Creation and use of JSON decodeFile to decode an table of files from disk.", "[JSON][Decode]")
{
  auto testFile = GENERATE(values<std::string>({"./testData/testfile001.json",
                                                "./testData/testfile002.json",
                                                "./testData/testfile003.json",
                                                "./testData/testfile004.json"}));
  JSON json;
  REQUIRE_NOTHROW(json.decodeFile(testFile));
  std::unique_ptr<JNode> jNode = json.decodeFile(testFile);
  REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::object);
}