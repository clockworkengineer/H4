//
// Unit Tests: JSON
//
// Description: JSON parse unit tests for JSON class
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
TEST_CASE("Creation and use of JSON object for parse of simple types (number, string, boolean, null) ", "[JSON][Parse]")
{
    JSON json;
    std::unique_ptr<JNode> jNode;
    SECTION("Parse an string (example string)", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("\"example string\"");
        REQUIRE(jNode->nodeType == JNodeType::string);
    }
    SECTION("Parse an number (6767)", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("6767");
        REQUIRE(jNode->nodeType == JNodeType::number);
    }
    SECTION("Parse an boolean (true)", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("true");
        REQUIRE(jNode->nodeType == JNodeType::boolean);
    }
    SECTION("Parse an boolean (false)", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("false");
        REQUIRE(jNode->nodeType == JNodeType::boolean);
    }
    SECTION("Parse an null", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("null");
        REQUIRE(jNode->nodeType == JNodeType::null);
    }
}
TEST_CASE("Creation and use of JSON object for parse of simple types (number, string, boolean, null) and check values.", "[JSON][Parse]")
{
    JSON json;
    std::unique_ptr<JNode> jNode;
    SECTION("Parse an string (example string) and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("\"example string\"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "example string");
    }
    SECTION("Parse an string (another example string) and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("\"another example string\"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "another example string");
    }
    SECTION("Parse an number (6767) and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("6767");
        REQUIRE(JNodeRef<JNodeNumber>(*jNode).getNumber() == "6767");
    }
    SECTION("Parse an number (190000) and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("190000");
        REQUIRE(JNodeRef<JNodeNumber>(*jNode).getNumber() == "190000");
    }
    SECTION("Parse an boolean (true) and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("true");
        REQUIRE(JNodeRef<JNodeBoolean>(*jNode).getBoolean() == true);
    }
    SECTION("Parse an boolean (false) and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("false");
        REQUIRE(JNodeRef<JNodeBoolean>(*jNode).getBoolean() == false);
    }
    SECTION("Parse an null and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("null");
        REQUIRE(JNodeRef<JNodeNull>(*jNode).getNull() == nullptr);
    }
}
TEST_CASE("Creation and use of JSON object for parse of collection types (array, object) ", "[JSON][Parse]")
{
    JSON json;
    std::unique_ptr<JNode> jNode;
    SECTION("Parse an object ({\"name\":\"Robert\",\"Age\":15})", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("{\"name\":\"Robert\",\"Age\":15}");
        REQUIRE(jNode->nodeType == JNodeType::object);
    }
    SECTION("Parse an array ([ 777,9000,\"apples\"]) ", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("[777,9000,\"apples\"]");
        REQUIRE(jNode->nodeType == JNodeType::array);
    }
}
TEST_CASE("Creation and use of JSON object for parse of collection types (array, object) and check values", "[JSON][Parse]")
{
    JSON json;
    std::unique_ptr<JNode> jNode;
    SECTION("Parse an object {\"name\":\"Robert\",\"Age\":15} and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("{\"Name\":\"Robert\",\"Age\":15}");
        REQUIRE(jNode->nodeType == JNodeType::object);
        REQUIRE(JNodeRef<JNodeObject>(*jNode).size() == 2);
        REQUIRE(JNodeRef<JNodeObject>(*jNode).containsKey("Name"));
        REQUIRE(JNodeRef<JNodeObject>(*jNode).containsKey("Age"));
        REQUIRE(JNodeRef<JNodeString>((*jNode)["Name"]).nodeType == JNodeType::string);
        REQUIRE(JNodeRef<JNodeNumber>((*jNode)["Age"]).nodeType == JNodeType::number);
        REQUIRE(JNodeRef<JNodeString>((*jNode)["Name"]).getString() == "Robert");
        REQUIRE(JNodeRef<JNodeNumber>((*jNode)["Age"]).getNumber() == "15");
    }
    SECTION("Parse an array [777,9000,\"apples\"] and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("[777,9000,\"apples\"]");
        REQUIRE(jNode->nodeType == JNodeType::array);
        REQUIRE(JNodeRef<JNodeArray>(*jNode).size() == 3);
        REQUIRE(JNodeRef<JNodeNumber>((*jNode)[0]).nodeType == JNodeType::number);
        REQUIRE(JNodeRef<JNodeNumber>((*jNode)[1]).nodeType == JNodeType::number);
        REQUIRE(JNodeRef<JNodeString>((*jNode)[2]).nodeType == JNodeType::string);
        REQUIRE(JNodeRef<JNodeNumber>((*jNode)[0]).getNumber() == "777");
        REQUIRE(JNodeRef<JNodeNumber>((*jNode)[1]).getNumber() == "9000");
        REQUIRE(JNodeRef<JNodeString>((*jNode)[2]).getString() == "apples");
    }
    SECTION("Parse object {\"City\":\"Southampton\",\"Population\":500000} and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("{\"City\":\"Southampton\",\"Population\":500000}");
        checkObject(jNode.get());
    }
    SECTION("Parse an array [\"Dog\",1964,true,null ] and check its value", "[JSON][Parse]")
    {
        jNode = json.parseBuffer("[\"Dog\",1964,true,null]");
        checkArray(jNode.get());
    }
}
TEST_CASE("Creation and use of JSON object for parse checking various whitespace characters are ignored.", "[JSON][Parse]")
{
    JSON json;
    std::unique_ptr<JNode> jNode;
    std::string ws = "";
    SECTION("Parse an array [\"Dog\",1964,true,null] with no whitespace.", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
        checkArray(jNode.get());
    }
    SECTION("Parse object {\"City\":\"Southampton\",\"Population\":500000} with no whitespace", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
        checkObject(jNode.get());
    }
    ws += " ";
    SECTION("Parse an array [\"Dog\",1964,true,null] with whitespace ' '.", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
        checkArray(jNode.get());
    }
    SECTION("Parse object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' '.", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
        checkObject(jNode.get());
    }
    ws += "\t";
    SECTION("Parse an array [\"Dog\",1964,true,null] with whitespace ' \\t'.", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
        checkArray(jNode.get());
    }
    SECTION("Parse object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' \\t'.", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
        checkObject(jNode.get());
    }
    ws += "\n";
    SECTION("Parse an array [\"Dog\",1964,true,null] with whitespace ' \\t\\n'.", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
        checkArray(jNode.get());
    }
    SECTION("Parse object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' \\t\\n'.", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
        checkObject(jNode.get());
    }
    ws += "\r";
    SECTION("Parse an array [\"Dog\",1964,true,null] with whitespace ' \\t\\n\\r'.", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "[" + ws + "\"Dog\"" + ws + "," + ws + "1964" + ws + "," + ws + "true" + ws + "," + ws + "null" + ws + "]");
        checkArray(jNode.get());
    }
    SECTION("Parse object {\"City\":\"Southampton\",\"Population\":500000} whitespace ' \\t\\n\\r'.", "[JSON][Parse]")
    {
        jNode = json.parseBuffer(ws + "{" + ws + "\"City\"" + ws + ":" + ws + "\"Southampton\"" + ws + "," + ws + "\"Population\"" + ws + ":" + ws + "500000" + ws + "}");
        checkObject(jNode.get());
    }
}
TEST_CASE("Creation and use of JSON object for parse of a list of example JSON files.", "[JSON][Parse]")
{
    auto testFile = GENERATE(values<std::string>({"./testData/testfile001.json",
                                                  "./testData/testfile002.json",
                                                  "./testData/testfile003.json",
                                                  "./testData/testfile004.json",
                                                  "./testData/testfile005.json"}));
    JSON json;
    SECTION("Parse from buffer", "[JSON][Parse]")
    {
        std::string jsonFileBuffer = readJSONFromFile(testFile);
        REQUIRE_NOTHROW(json.parseBuffer(jsonFileBuffer));
        std::unique_ptr<JNode> jNode = json.parseBuffer(jsonFileBuffer);
        REQUIRE(jNode->nodeType == JNodeType::object);
    }
    SECTION("Parse from file directly", "[JSON][Parse]")
    {
        REQUIRE_NOTHROW(json.parseFile(testFile));
        std::unique_ptr<JNode> jNode = json.parseFile(testFile);
        REQUIRE(jNode->nodeType == JNodeType::object);
    }
}
TEST_CASE("Parse generated exceptions.", "[JSON][Parse][Exceptions]")
{
    // Note: The tests for syntax errors is not exhaustive and more may be added over time.
    JSON json;
    SECTION("Parse passing a empty string", "[JSON][Parse]")
    {
        REQUIRE_THROWS_AS(json.parseBuffer(""), std::invalid_argument);
        REQUIRE_THROWS_WITH(json.parseBuffer(""), "Empty string passed to be parsed.");
    }
    SECTION("Parse file passing a empty file name", "[JSON][Parse]")
    {
        REQUIRE_THROWS_AS(json.parseBuffer(""), std::invalid_argument);
        REQUIRE_THROWS_WITH(json.parseFile(""), "Empty file name passed to be parsed.");
    }
    SECTION("Parse missing terminating '\"' in string", "[JSON][Parse]")
    {
        REQUIRE_THROWS_AS(json.parseBuffer("{ \"one\" : \"Apple }"), std::runtime_error);
        REQUIRE_THROWS_WITH(json.parseBuffer("{ \"one\" : \"Apple }"), "JSON syntax error detected.");
    }
    SECTION("Parse number with starting with invalid character", "[JSON][Parse]")
    {
        REQUIRE_THROWS_AS(json.parseBuffer("{ \"one\" : z19034}"), std::runtime_error);
        REQUIRE_THROWS_WITH(json.parseBuffer("{ \"one\" : z19034 }"), "JSON syntax error detected.");
    }
    SECTION("Parse object with invalid value field (number).", "[JSON][Parse]")
    {
        REQUIRE_THROWS_AS(json.parseBuffer("{ \"one\" : 18987u3 }"), std::runtime_error);
        REQUIRE_THROWS_WITH(json.parseBuffer("{ \"one\" : 18987u3 }"), "JSON syntax error detected.");
    }
    SECTION("Parse object with missing value field.", "[JSON][Parse]")
    {
        REQUIRE_THROWS_AS(json.parseBuffer("{ \"one\" : }"), std::runtime_error);
        REQUIRE_THROWS_WITH(json.parseBuffer("{ \"one\" : }"), "JSON syntax error detected.");
    }
    SECTION("Parse object with missing key field.", "[JSON][Parse]")
    {
        REQUIRE_THROWS_AS(json.parseBuffer("{  : 89012 }"), std::runtime_error);
        REQUIRE_THROWS_WITH(json.parseBuffer("{ : }"), "JSON syntax error detected.");
    }
    SECTION("Parse object with missing closing '}'.", "[JSON][Parse]")
    {
        REQUIRE_THROWS_AS(json.parseBuffer("{  \"one\" : 18987"), std::runtime_error);
        REQUIRE_THROWS_WITH(json.parseBuffer("{ \"one\" : 18987"), "JSON syntax error detected.");
    }
}
TEST_CASE("Creation and use of JSON object for decoding of strings with escape characters.", "[JSON][Parse]")
{
    JSON json;
    SECTION("Stringify JSON string with escapes '\\t' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\t \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \t ");
    }
    SECTION("Stringify JSON string with escapes '\\\"' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\\" \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \" ");
    }
    SECTION("Stringify JSON string with escapes '\\\\' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\\\ \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \\ ");
    }
    SECTION("Stringify JSON string with escapes '\\b' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\b \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \b ");
    }
    SECTION("Stringify JSON string with escapes '\\/' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\/ \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String / ");
    }
    SECTION("Stringify JSON string with escapes '\\f' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\f \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \f ");
    }
    SECTION("Stringify JSON string with escapes '\\n' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\n \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \n ");
    }
    SECTION("Stringify JSON string with escapes '\\r' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\r \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \r ");
    }
    SECTION("Stringify JSON string with escapes '\\t' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\t \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \t ");
    }
    SECTION("Stringify JSON string with escapes '\\u0123' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\u0123 \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \u0123 ");
    }
    SECTION("Stringify JSON string with escapes '\\u0123 \\u0456' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String \\u0123 \\u0456 \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String \u0123 \u0456 ");
    }
    SECTION("Stringify JSON string with escapes  '\\uD834\\uDD1E' to buffer and check value", "[JSON][Parse]")
    {
        std::unique_ptr<JNode> jNode = json.parseBuffer("\"Test String  \\uD834\\uDD1E \"");
        REQUIRE(JNodeRef<JNodeString>(*jNode).getString() == "Test String  \U0001D11E ");
    }
}