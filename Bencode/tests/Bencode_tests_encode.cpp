//
// Unit Tests: Bencode
//
// Description: Encode unit tests for Bencode class using 
// the Catch2 test framework.
//
// ================
// Test definitions
// ================
#include "Bencode_tests.hpp"
// =======================
// Bencode class namespace
// =======================
using namespace H4;
// ==========
// Test cases
// ==========
TEST_CASE("Creation and use of Bencode for encode of simple types (number, string) ", "[Bencode][Encode]")
{
    Bencode bEncode;
    Bencode::Bencoding actual;
    SECTION("Encode an integer (266) and check value", "[Bencode][Encode]")
    {
        actual = bEncode.encodeBuffer(std::make_unique<BNodeInteger>(BNodeInteger(266)));
        REQUIRE(actual == Bencode::Bencoding("i266e"));
    }
    SECTION("Encode an integer (10000) and check value", "[Bencode][Encode]")
    {
        actual = bEncode.encodeBuffer(std::make_unique<BNodeInteger>(BNodeInteger(10000)));
        REQUIRE(actual == Bencode::Bencoding("i10000e"));
    }
    SECTION("Encode an string ('qwertyuiopas') and check its value", "[Bencode][Encode]")
    {
        actual = bEncode.encodeBuffer(std::make_unique<BNodeString>(BNodeString("qwertyuiopas")));
        REQUIRE(actual == Bencode::Bencoding("12:qwertyuiopas"));
    }
    SECTION("Encode an string ('abcdefghijklmnopqrstuvwxyz') and check its value", "[Bencode][Encode]")
    {
        actual = bEncode.encodeBuffer(std::make_unique<BNodeString>(BNodeString("abcdefghijklmnopqrstuvwxyz")));
        REQUIRE(actual == Bencode::Bencoding("26:abcdefghijklmnopqrstuvwxyz"));
    }
}
TEST_CASE("Creation and use of Bencode for encode of a table of integer test data", "[Bencode][Encode]")
{
    auto [testInput, expected] = GENERATE(table<long, Bencode::Bencoding>({{277, "i277e"},
                                                                           {32767, "i32767e"}}));
    Bencode bEncode;
    Bencode::Bencoding actual = bEncode.encodeBuffer(std::make_unique<BNodeInteger>(BNodeInteger(testInput)));
    REQUIRE(actual == expected);
}
TEST_CASE("Creation and use of Bencode for encode of a table of string test data", "[Bencode][Encode]")
{
    auto [testInput, expected] = GENERATE(table<std::string, Bencode::Bencoding>({{"qwertyuiopasd", "13:qwertyuiopasd"},
                                                                                  {"mnbvcx", "6:mnbvcx"}}));
    Bencode bEncode;
    Bencode::Bencoding actual = bEncode.encodeBuffer(std::make_unique<BNodeString>(BNodeString(testInput)));
    REQUIRE(actual == expected);
}
TEST_CASE("Creation and use of Bencode for encode of collection types (list, dictionary) ", "[Bencode][Encode]")
{
    Bencode bEncode;
    SECTION("Encode an List of integers('li266ei6780ei88ee') and check value", "[Bencode][Encode]")
    {
        Bencode::Bencoding expected("li266ei6780ei88ee");
        REQUIRE(bEncode.encodeBuffer(bEncode.decodeBuffer(expected)) == expected);
    }
    SECTION("Encode an List of strings ('l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze') and check value", "[Bencode][Encode]")
    {
        Bencode::Bencoding expected = "l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze";
        REQUIRE(bEncode.encodeBuffer(bEncode.decodeBuffer(expected)) == expected);
    }
    SECTION("Encode an Dictionary of integers and check balue", "[Bencode][Encode]")
    {
        Bencode::Bencoding expected = "d3:onei1e5:threei3e3:twoi2ee";
        REQUIRE(bEncode.encodeBuffer(bEncode.decodeBuffer(expected)) == expected);
    }
    SECTION("Encode an Dictionary of strings and check balue", "[Bencode][Encode]")
    {
        Bencode::Bencoding expected = "d3:one10:01234567895:three6:qwerty3:two9:asdfghjkle";
        REQUIRE(bEncode.encodeBuffer(bEncode.decodeBuffer(expected)) == expected);
    }
}
TEST_CASE("Encode generated exceptions", "[Bencode][Encode][Exceptions]")
{
    Bencode bEncode;
    SECTION("Encode passed nullptr", "[Bencode][Decode]")
    {
        REQUIRE_THROWS_AS(bEncode.encodeBuffer(nullptr), std::invalid_argument);
        REQUIRE_THROWS_WITH(bEncode.encodeBuffer(nullptr), "Nullptr passed as bNode to be encoded.");
        REQUIRE_THROWS_AS(bEncode.encodeBuffer(std::unique_ptr<BNode>(nullptr)), std::invalid_argument);
        REQUIRE_THROWS_WITH(bEncode.encodeBuffer(std::unique_ptr<BNode>(nullptr)), "Nullptr passed as bNode to be encoded.");
    }
    SECTION("Encode passed invalid BNode type", "[Bencode][Decode]")
    {
        REQUIRE_THROWS_AS(bEncode.encodeBuffer(std::unique_ptr<BNode>(new BNode())), std::runtime_error);
        REQUIRE_THROWS_WITH(bEncode.encodeBuffer(std::unique_ptr<BNode>(new BNode())), "Unknown BNode type encountered during encode.");
    }
}
TEST_CASE("Encode torrent files using encodeToFile", "[Bencode][Encode][Torrents]")
{
    Bencode bEncode;
    SECTION("Encode singlefile.torrent and check value", "[Bencode][Encode][Torrents]")
    {
        std::filesystem::remove(kGeneratedTorrentFile);
        bEncode.encodeFile(bEncode.decodeFile(kSingleFileTorrent), kGeneratedTorrentFile);
        REQUIRE_FALSE(!compareFiles(kSingleFileTorrent, kGeneratedTorrentFile));
    }
    SECTION("Encode multifile.torrent and check value", "[Bencode][Encode][Torrents]")
    {
        std::filesystem::remove(kGeneratedTorrentFile);
        bEncode.encodeFile(bEncode.decodeFile(kMultiFileTorrent), kGeneratedTorrentFile);
        REQUIRE_FALSE(!compareFiles(kMultiFileTorrent, kGeneratedTorrentFile));
    }
}