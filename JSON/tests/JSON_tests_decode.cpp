//
// Unit Tests: JSON
//
// Description: JSON decode unit tests for JSON class
// using the Catch2 test framework.
//
// ================
// Test definitions
// =================
#include "JSON_tests.hpp"
// =======================
// JSON class namespace
// =======================
using namespace H4;
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
        REQUIRE(jNode->nodeType == JNodeType::string);
    }
    SECTION("Decode an number (6767)", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("6767");
        REQUIRE(jNode->nodeType == JNodeType::number);
    }
    SECTION("Decode an boolean (true)", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("true");
        REQUIRE(jNode->nodeType == JNodeType::boolean);
    }
    SECTION("Decode an boolean (false)", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("false");
        REQUIRE(jNode->nodeType == JNodeType::boolean);
    }
    SECTION("Decode an null", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("null");
        REQUIRE(jNode->nodeType == JNodeType::null);
    }
}
TEST_CASE("Creation and use of JSON for decode of simple types (number, string, boolean, null) and check values.", "[JSON][Decode]")
{
    JSON json;
    std::unique_ptr<JNode> jNode;
    SECTION("Decode an string (example string) and check its value", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("\"example string\"");
        REQUIRE(JNode::ref<JNodeString>(*jNode).value == "example string");
    }
    SECTION("Decode an string (another example string) and check its value", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("\"another example string\"");
        REQUIRE(JNode::ref<JNodeString>(*jNode).value == "another example string");
    }
    SECTION("Decode an number (6767) and check its value", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("6767");
        REQUIRE(JNode::ref<JNodeNumber>(*jNode).value == "6767");
    }
    SECTION("Decode an number (190000) and check its value", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("190000");
        REQUIRE(JNode::ref<JNodeNumber>(*jNode).value == "190000");
    }
    SECTION("Decode an boolean (true) and check its value", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("true");
        REQUIRE(JNode::ref<JNodeBoolean>(*jNode).value == true);
    }
    SECTION("Decode an boolean (false) and check its value", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("false");
        REQUIRE(JNode::ref<JNodeBoolean>(*jNode).value == false);
    }
    SECTION("Decode an null and check its value", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("null");
        REQUIRE(JNode::ref<JNodeNull>(*jNode).value == nullptr);
    }
}
TEST_CASE("Creation and use of JSON for decode of collection types (array, object) ", "[JSON][Decode]")
{
    JSON json;
    std::unique_ptr<JNode> jNode;
    SECTION("Decode an object ({\"name\":\"Robert\",\"Age\":15})", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("{\"name\":\"Robert\",\"Age\":15}");
        REQUIRE(jNode->nodeType == JNodeType::object);
    }
    SECTION("Decode an array ([ 777,9000,\"apples\"]) ", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("[777,9000,\"apples\"]");
        REQUIRE(jNode->nodeType == JNodeType::array);
    }
}
TEST_CASE("Creation and use of JSON for decode of collection types (array, object) and check values", "[JSON][Decode]")
{
    JSON json;
    std::unique_ptr<JNode> jNode;
    SECTION("Decode an object {\"name\":\"Robert\",\"Age\":15} and check its value", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("{\"Name\":\"Robert\",\"Age\":15}");
        REQUIRE(jNode->nodeType == JNodeType::object);
        REQUIRE(JNode::ref<JNodeObject>(*jNode).value.size() == 2);
        REQUIRE(JNode::ref<JNodeObject>(*jNode).value.count("Name") > 0);
        REQUIRE(JNode::ref<JNodeObject>(*jNode).value.count("Age") > 0);
        REQUIRE(JNode::ref<JNodeString>((*jNode)["Name"]).nodeType == JNodeType::string);
        REQUIRE(JNode::ref<JNodeNumber>((*jNode)["Age"]).nodeType == JNodeType::number);
        REQUIRE(JNode::ref<JNodeString>((*jNode)["Name"]).value == "Robert");
        REQUIRE(JNode::ref<JNodeNumber>((*jNode)["Age"]).value == "15");
    }
    SECTION("Decode an array [777,9000,\"apples\"] and check its value", "[JSON][Decode]")
    {
        jNode = json.decodeBuffer("[777,9000,\"apples\"]");
        REQUIRE(jNode->nodeType == JNodeType::array);
        REQUIRE(JNode::ref<JNodeArray>(*jNode).value.size() == 3);
        REQUIRE(JNode::ref<JNodeNumber>((*jNode)[0]).nodeType == JNodeType::number);
        REQUIRE(JNode::ref<JNodeNumber>((*jNode)[1]).nodeType == JNodeType::number);
        REQUIRE(JNode::ref<JNodeString>((*jNode)[2]).nodeType == JNodeType::string);
        REQUIRE(JNode::ref<JNodeNumber>((*jNode)[0]).value == "777");
        REQUIRE(JNode::ref<JNodeNumber>((*jNode)[1]).value == "9000");
        REQUIRE(JNode::ref<JNodeString>((*jNode)[2]).value == "apples");
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
        REQUIRE(jNode->nodeType == JNodeType::object);
    }
    SECTION("Decode from file directly", "[JSON][Decode]")
    {
        REQUIRE_NOTHROW(json.decodeFile(testFile));
        std::unique_ptr<JNode> jNode = json.decodeFile(testFile);
        REQUIRE(jNode->nodeType == JNodeType::object);
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
        REQUIRE_THROWS_WITH(json.decodeBuffer("{ \"one\" : 18987"), "JSON syntax error detected.");
    }
}