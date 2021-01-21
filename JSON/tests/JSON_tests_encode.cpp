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
TEST_CASE("Creation and use of JSON object for encode of simple types (number, string, boolean, null) and check its value", "[JSON][Encode]")
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
TEST_CASE("Creation and use of JSON object for encode of collection types (object, array) and check its value", "[JSON][Encode]")
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
        std::filesystem::remove(kGeneratedTorrentFile);
        json.encodeFile(json.decodeBuffer(expected), kGeneratedTorrentFile);
        REQUIRE(readJSONFromFile(kGeneratedTorrentFile) == expected);
    }
    SECTION("Encode array to file and check value", "[JSON][ENcode]")
    {
        std::string expected = "[999,\"Time\",null,true]";
        std::filesystem::remove(kGeneratedTorrentFile);
        json.encodeFile(json.decodeBuffer(expected), kGeneratedTorrentFile);
        REQUIRE(readJSONFromFile(kGeneratedTorrentFile) == expected);
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
        REQUIRE_THROWS_AS(json.encodeFile(std::unique_ptr<JNode>(nullptr), kGeneratedTorrentFile), std::invalid_argument);
        REQUIRE_THROWS_WITH(json.encodeFile(std::unique_ptr<JNode>(nullptr), kGeneratedTorrentFile), "Nullptr passed as JNode root to be encoded.");
    }
    SECTION("Encode file passed empty string for file name", "[JSON][Encode][Exceptions]")
    {
        REQUIRE_THROWS_AS(json.encodeFile(std::unique_ptr<JNode>(new JNode()), ""), std::invalid_argument);
        REQUIRE_THROWS_WITH(json.encodeFile(std::unique_ptr<JNode>(new JNode()), ""), "Empty file name passed to be encoded.");
    }
}
TEST_CASE("Creation and use of JSON object for encode of a list of example JSON files.", "[JSON][Encode]")
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
        std::filesystem::remove(kGeneratedTorrentFile);
        std::string jsonFileBuffer = readJSONFromFile(testFile);
        json.encodeFile(json.decodeBuffer(jsonFileBuffer), kGeneratedTorrentFile);
        REQUIRE(readJSONFromFile(kGeneratedTorrentFile) == json.stripWhiteSpaceBuffer(jsonFileBuffer));
    }
}
TEST_CASE("Creation and use of JSON object for encoding of strings with escape characters.", "[JSON][Encode]")
{
    JSON json;
    SECTION("Encode string with escapes '\\t' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\t \"")) == "\"Test String \\t \"");
    }
    SECTION("Encode string with escapes '\\\"' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\\" \"")) == "\"Test String \\\" \"");
    }
    SECTION("Encode string with escapes '\\\\' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\\\ \"")) == "\"Test String \\\\ \"");
    }
    SECTION("Encode string with escapes '\\b' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\b \"")) == "\"Test String \\b \"");
    }
    SECTION("Encode string with escapes '\\/' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\/ \"")) == "\"Test String \\/ \"");
    }
    SECTION("Encode string with escapes '\\f' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\f \"")) == "\"Test String \\f \"");
    }
    SECTION("Encode string with escapes '\\n' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\n \"")) == "\"Test String \\n \"");
    }
    SECTION("Encode string with escapes '\\r' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\r \"")) == "\"Test String \\r \"");
    }
    SECTION("Encode string with escapes '\\t' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\t \"")) == "\"Test String \\t \"");
    }
    SECTION("Encode string with escapes '\\u0123' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\u0123 \"")) == "\"Test String \\u0123 \"");
    }
    SECTION("Encode string with escapes '\\u0123 \\u0456' to buffer and check value", "[JSON][Decode]")
    {
        REQUIRE(json.encodeBuffer(json.decodeBuffer("\"Test String \\u0123 \\u0456 \"")) == "\"Test String \\u0123 \\u0456 \"");
    }
}