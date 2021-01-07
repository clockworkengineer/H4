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
// ==========
// Test cases
// ==========
TEST_CASE("Creation and use of JSON for decode of simple types (number, string, boolean, null) ", "[JSON][Decode]")
{
  JSON json;
  SECTION("Decode an string", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("\"example string\"");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::string);
  }
  SECTION("Decode an number", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("6767");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::number);
  }
  SECTION("Decode an boolean (true)", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("true");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::boolean);
  }
  SECTION("Decode an boolean (false)", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("false");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::boolean);
  }
  SECTION("Decode an null", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("null");
    REQUIRE(((JNode *)jNode.get())->nodeType == JSON::JNodeType::null);
  }
}
TEST_CASE("Creation and use of JSON for decode of simple types (number, string, boolean, null) and check values.", "[JSON][Decode]")
{
  JSON json;
  SECTION("Decode an string and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("\"example string\"");
    REQUIRE(((JNodeString *)jNode.get())->value == "example string");
  }
  SECTION("Decode an string and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("\"another example string\"");
    REQUIRE(((JNodeString *)jNode.get())->value == "another example string");
  }
  SECTION("Decode an number and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("6767");
    REQUIRE(((JNodeNumber *)jNode.get())->value == "6767");
  }
  SECTION("Decode an number and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("190000");
    REQUIRE(((JNodeNumber *)jNode.get())->value == "190000");
  }
  SECTION("Decode an boolean (true) and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("true");
    REQUIRE(((JNodeBoolean *)jNode.get())->value == true);
  }
  SECTION("Decode an boolean (false) and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("false");
    REQUIRE(((JNodeBoolean *)jNode.get())->value == false);
  }
  SECTION("Decode an null and check its value", "[JSON][Decode]")
  {
    std::unique_ptr<JNode> jNode = json.decode("null");
    REQUIRE(((JNodeNull *)jNode.get())->value == nullptr);
  }
}