//
// Unit Tests: JSON
//
// Description: JSON encode unit tests for JSON class
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
TEST_CASE("Creation and use of JSON object for encode of simple types (number, string, boolean, null) and check its value", "[JSON][Stringify]")
{
    JSON json;
    SECTION("Stringify a string (Test string) and check its value", "[JSON][Stringify]")
    {
        std::string expected = "\"Test string.\"";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
    SECTION("Stringify a string (Test another string) and check its value", "[JSON][Stringify]")
    {
        std::string expected = "\"Test another string.\"";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
    SECTION("Stringify a boolean (true) and check its value", "[JSON][Stringify]")
    {
        std::string expected = "true";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
    SECTION("Stringify a boolean (false) and check its value", "[JSON][Stringify]")
    {
        std::string expected = "false";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
    SECTION("Stringify a number (98345) and check its value", "[JSON][Stringify]")
    {
        std::string expected = "98345";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
    SECTION("Stringify a number (250000) and check its value", "[JSON][Stringify]")
    {
        std::string expected = "25000";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
    SECTION("Stringify a null and check its value", "[JSON][Stringify]")
    {
        std::string expected = "null";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
}
TEST_CASE("Creation and use of JSON object for encode of collection types (object, array) and check its value", "[JSON][Stringify]")
{
    JSON json;
    SECTION("Stringify an object ({\"Age\":77,\"Name\":\"Rob\"}) and check its value", "[JSON][Stringify]")
    {
        std::string expected = "{\"Age\":77,\"Name\":\"Rob\"}";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
    SECTION("Stringify an array ([999,\"Time\",null,true] and check its value", "[JSON][Stringify]")
    {
        std::string expected = "[999,\"Time\",null,true]";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
    SECTION("Stringify an object ({\"City\":\"London\",\"Population\":8000000}) and check its value", "[JSON][Stringify]")
    {
        std::string expected = "{\"City\":\"London\",\"Population\":8000000}";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
    SECTION("Stringify an array ([true,\"Out of time\",789043e13,true]) and check its value", "[JSON][Stringify]")
    {
        std::string expected = "[true,\"Out of time\",789043e13,true]";
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(expected)) == expected);
    }
}
TEST_CASE("Stringify to a file and check result", "[JSON][Stringify][Exceptions]")
{
    JSON json;
    SECTION("Stringify object to file and check value", "[JSON][Stringify]")
    {
        std::string expected = "{\"City\":\"London\",\"Population\":8000000}";
        std::filesystem::remove(kGeneratedTorrentFile);
        json.stringifyToFile(json.parseBuffer(expected), kGeneratedTorrentFile);
        REQUIRE(readJSONFromFile(kGeneratedTorrentFile) == expected);
    }
    SECTION("Stringify array to file and check value", "[JSON][Stringify]")
    {
        std::string expected = "[999,\"Time\",null,true]";
        std::filesystem::remove(kGeneratedTorrentFile);
        json.stringifyToFile(json.parseBuffer(expected), kGeneratedTorrentFile);
        REQUIRE(readJSONFromFile(kGeneratedTorrentFile) == expected);
    }
}
TEST_CASE("Stringify generated exceptions", "[JSON][Stringify][Exceptions]")
{
    JSON json;
    SECTION("Stringify passed nullptr", "[JSON][Stringify][Exceptions]")
    {
        REQUIRE_THROWS_AS(json.stringifyToBuffer(std::unique_ptr<JNode>(nullptr)), std::invalid_argument);
        REQUIRE_THROWS_WITH(json.stringifyToBuffer(std::unique_ptr<JNode>(nullptr)), "Nullptr passed as JNode root to be stringified.");
    }
    SECTION("Stringify passed invalid JNode type", "[JSON][Stringify][Exceptions]")
    {
        REQUIRE_THROWS_AS(json.stringifyToBuffer(std::unique_ptr<JNode>(new JNode())), std::runtime_error);
        REQUIRE_THROWS_WITH(json.stringifyToBuffer(std::unique_ptr<JNode>(new JNode())), "Unknown JNode type encountered during encode.");
    }
    SECTION("Stringify file passed invalid JNode type", "[JSON][Stringify][Exceptions]")
    {
        REQUIRE_THROWS_AS(json.stringifyToFile(std::unique_ptr<JNode>(nullptr), kGeneratedTorrentFile), std::invalid_argument);
        REQUIRE_THROWS_WITH(json.stringifyToFile(std::unique_ptr<JNode>(nullptr), kGeneratedTorrentFile), "Nullptr passed as JNode root to be stringified.");
    }
    SECTION("Stringify file passed empty string for file name", "[JSON][Stringify][Exceptions]")
    {
        REQUIRE_THROWS_AS(json.stringifyToFile(std::unique_ptr<JNode>(new JNode()), ""), std::invalid_argument);
        REQUIRE_THROWS_WITH(json.stringifyToFile(std::unique_ptr<JNode>(new JNode()), ""), "Empty file name passed to be stringified.");
    }
}
TEST_CASE("Creation and use of JSON object for encode of a list of example JSON files.", "[JSON][Stringify]")
{
    auto testFile = GENERATE(values<std::string>({"./testData/testfile001.json",
                                                  "./testData/testfile002.json",
                                                  "./testData/testfile003.json",
                                                  "./testData/testfile004.json",
                                                  "./testData/testfile005.json"}));
    JSON json;
    SECTION("Stringify to  buffer and check value", "[JSON][Stringify]") 
    {
        std::string jsonFileBuffer = readJSONFromFile(testFile);  
        REQUIRE(json.stringifyToBuffer(json.parseBuffer(jsonFileBuffer)) == json.stripWhiteSpaceBuffer(jsonFileBuffer));
    }
    SECTION("Stringify to file and check value", "[JSON][Stringify]")
    {
        std::filesystem::remove(kGeneratedTorrentFile);
        std::string jsonFileBuffer = readJSONFromFile(testFile);
        json.stringifyToFile(json.parseBuffer(jsonFileBuffer), kGeneratedTorrentFile);
        REQUIRE(readJSONFromFile(kGeneratedTorrentFile) == json.stripWhiteSpaceBuffer(jsonFileBuffer));
    }
}
TEST_CASE("Creation and use of JSON object for stringification of strings with escape characters.", "[JSON][Stringify]")
{
    JSON json;
    SECTION("Stringify string with escapes '\\t' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\t \"")) == "\"Test String \\t \"");
    }
    SECTION("Stringify string with escapes '\\\"' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\\" \"")) == "\"Test String \\\" \"");
    }
    SECTION("Stringify string with escapes '\\\\' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\\\ \"")) == "\"Test String \\\\ \"");
    }
    SECTION("Stringify string with escapes '\\b' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\b \"")) == "\"Test String \\b \"");
    }
    SECTION("Stringify string with escapes '\\/' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\/ \"")) == "\"Test String \\/ \"");
    }
    SECTION("Stringify string with escapes '\\f' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\f \"")) == "\"Test String \\f \"");
    }
    SECTION("Stringify string with escapes '\\n' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\n \"")) == "\"Test String \\n \"");
    }
    SECTION("Stringify string with escapes '\\r' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\r \"")) == "\"Test String \\r \"");
    }
    SECTION("Stringify string with escapes '\\t' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\t \"")) == "\"Test String \\t \"");
    }
    SECTION("Stringify string with escapes '\\u0123' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\u0123 \"")) == "\"Test String \\u0123 \"");
    }
    SECTION("Stringify string with escapes '\\u0123 \\u0456' to buffer and check value", "[JSON][Stringify]")
    {
        REQUIRE(json.stringifyToBuffer(json.parseBuffer("\"Test String \\u0123 \\u0456 \"")) == "\"Test String \\u0123 \\u0456 \"");
    }
}