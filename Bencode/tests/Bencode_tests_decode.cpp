//
// Unit Tests: Bencode
//
// Description: Decode unit tests for Bencode class using the Catch2 test framework.
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
TEST_CASE("Creation and use of Bencode for decode of simple types (number, string) ", "[Bencode][Decode]")
{
  Bencode bEncode;
  std::unique_ptr<BNode> bNode;
  SECTION("Decode an integer", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("i266e");
    REQUIRE(bNode->nodeType == BNodeType::integer);
  }
  SECTION("Decode an string", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("12:qwertyuiopas");
    REQUIRE(bNode->nodeType == BNodeType::string);
  }
  SECTION("Decode an integer (266) and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("i266e");
    REQUIRE(BNodeRef<BNodeInteger>(*bNode).value == 266);
  }
  SECTION("Decode an integer (1000) and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("i1000e");
    REQUIRE(BNodeRef<BNodeInteger>(*bNode).value == 1000);
  }
  SECTION("Decode an negative integer (-666) and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("i-666e");
    REQUIRE(BNodeRef<BNodeInteger>(*bNode).value == -666);
  }
  SECTION("Decode an string ('qwertyuiopas') and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("12:qwertyuiopas");
    REQUIRE(BNodeRef<BNodeString>(*bNode).value == "qwertyuiopas");
  }
  SECTION("Decode an string ('abcdefghijklmnopqrstuvwxyz') and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("26:abcdefghijklmnopqrstuvwxyz");
    REQUIRE(BNodeRef<BNodeString>(*bNode).value == "abcdefghijklmnopqrstuvwxyz");
  }
}
TEST_CASE("Creation and use of Bencode for decode of a table of integer test data", "[Bencode][Decode]")
{
  auto [testInput, expected] = GENERATE(table<std::string, long>({{"i277e", 277},
                                                                  {"i32767e", 32767}}));
  Bencode bEncode;
  std::unique_ptr<BNode> bNode = bEncode.decodeBuffer(testInput.c_str());
  REQUIRE(BNodeRef<BNodeInteger>(*bNode).value == expected);
}
TEST_CASE("Creation and use of Bencode for decode of a table of string test data", "[Bencode][Decode]")
{
  auto [testInput, expected] = GENERATE(table<std::string, std::string>({{"13:qwertyuiopasd", "qwertyuiopasd"},
                                                                         {"6:mnbvcx", "mnbvcx"}}));
  Bencode bEncode;
  std::unique_ptr<BNode> bNode = bEncode.decodeBuffer(testInput.c_str());
  REQUIRE(BNodeRef<BNodeString>(*bNode).value == expected);
}
TEST_CASE("Creation and use of Bencode for decode of collection types (list, dictionary) ", "[Bencode][Decode]")
{
  Bencode bEncode;
  std::unique_ptr<BNode> bNode;
  SECTION("Decode an List", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("li266ei6780ei88e5:threee");
    REQUIRE(bNode->nodeType == BNodeType::list);
    REQUIRE((*bNode)[0].nodeType == BNodeType::integer);
    REQUIRE((*bNode)[1].nodeType == BNodeType::integer);
    REQUIRE((*bNode)[2].nodeType == BNodeType::integer);
    REQUIRE((*bNode)[3].nodeType == BNodeType::string);
    REQUIRE(BNodeRef<BNodeInteger>((*bNode)[0]).value == 266);
    REQUIRE(BNodeRef<BNodeInteger>((*bNode)[1]).value == 6780);
    REQUIRE(BNodeRef<BNodeInteger>((*bNode)[2]).value == 88);
    REQUIRE(BNodeRef<BNodeString>((*bNode)[3]).value == "three");
  }
  SECTION("Decode an Dictionary", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("d3:onei1e5:threei3e3:twoi2ee");
    REQUIRE(bNode->nodeType == BNodeType::dictionary);
    REQUIRE((*bNode)["one"].nodeType == BNodeType::integer);
    REQUIRE((*bNode)["two"].nodeType == BNodeType::integer);
    REQUIRE((*bNode)["three"].nodeType == BNodeType::integer);
    REQUIRE(BNodeRef<BNodeInteger>((*bNode)["one"]).value == 1);
    REQUIRE(BNodeRef<BNodeInteger>((*bNode)["two"]).value == 2);
    REQUIRE(BNodeRef<BNodeInteger>((*bNode)["three"]).value == 3);
  }
  SECTION("Decode an list of integers and check values", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("li266ei6780ei88ee");
    std::vector<long> numbers;
    for (const auto &bNode : BNodeRef<BNodeList>(*bNode).value)
    {
      numbers.push_back(BNodeRef<BNodeInteger>(*bNode).value);
    }
    REQUIRE(numbers == std::vector<long>{266, 6780, 88});
  }
  SECTION("Decode an list of strings and check values", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze");
    std::vector<std::string> strings;
    for (const auto &bNode : BNodeRef<BNodeList>(*bNode).value)
    {
      strings.push_back(BNodeRef<BNodeString>(*bNode).value);
    }
    REQUIRE(strings == std::vector<std::string>{"sillyy", "poiuytrewqas", "abcdefghijklmnopqrstuvwxyz"});
  }
  SECTION("Decode an Dictionary of ints and check values", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("d3:onei1e5:threei3e3:twoi2ee");
    std::map<std::string, long> entries;
    for (const auto &bNode : BNodeRef<BNodeDict>(*bNode).value)
    {
      entries[bNode.first] = BNodeRef<BNodeInteger>(*bNode.second).value;
    }
    REQUIRE(entries == std::map<std::string, long>{{"one", 1}, {"two", 2}, {"three", 3}});
  }
  SECTION("Decode a Dictionary of strings and check values", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkle");
    std::map<std::string, std::string> entries;
    for (const auto &bNode : BNodeRef<BNodeDict>(*bNode).value)
    {
      entries[bNode.first] = BNodeRef<BNodeString>(*bNode.second).value;
    }
    REQUIRE(entries == std::map<std::string, std::string>{{"one", "0123456789"}, {"two", "asdfghjkl"}, {"three", "qwerty"}});
  }
}
TEST_CASE("Decode generated exceptions", "[Bencode][Decode][Exceptions]")
{
  Bencode bEncode;
  SECTION("Decode passing a empty string", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer(""), "Empty string passed to be decoded.");
  }
  SECTION("Decode an string without terminating ':' on its length", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeBuffer("26abcdefghijklmnopqrstuvwxyz"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("26abcdefghijklmnopqrstuvwxyz"), "Bencoding syntax error detected.");
  }
  SECTION("Decode an integer without a terminating end", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeBuffer("i266"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("i266"), "Bencoding syntax error detected.");
  }
  SECTION("Decode an list without a terminating end", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeBuffer("li266ei6780ei88e"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("li266ei6780ei88e"), "Bencoding syntax error detected.");
  }
  SECTION("Decode an diictionary without a terminating end", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeBuffer("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkl"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkl"), "Bencoding syntax error detected.");
  }
  SECTION("Decode an string that terminates prematurely", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeBuffer("26:abcdefghijklmno"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("26:abcdefghijklmno"), "Decode buffer empty before decode complete.");
  }
}
TEST_CASE("Decode torrent files using decodeFile", "[Bencode][Decode][Torrents]")
{
  Bencode bEncode;
  std::unique_ptr<BNode> bNode;
  SECTION("Decode singlefile.torrent", "[Bencode][Decode][Torrents]")
  {
    bNode = bEncode.decodeFile(kSingleFileTorrent);
    REQUIRE(bNode->nodeType == BNodeType::dictionary);
  }
  SECTION("Decode singlefile.torrent and check value ", "[Bencode][Decode][Torrents]")
  {
    REQUIRE(bEncode.encodeBuffer(bEncode.decodeFile(kSingleFileTorrent)) == Bencode::Bencoding(readBencodedBytesFromFile(kSingleFileTorrent)));
  }
  SECTION("Decode multifile.torrent", "[Bencode][Decode][Torrents]")
  {
    bNode = bEncode.decodeFile(kMultiFileTorrent);
    REQUIRE(bNode->nodeType == BNodeType::dictionary);
  }
  SECTION("Decode multifile.torrent and check value ", "[Bencode][Decode][Torrents]")
  {
    REQUIRE(bEncode.encodeBuffer(bEncode.decodeFile(kMultiFileTorrent)) == Bencode::Bencoding(readBencodedBytesFromFile(kMultiFileTorrent)));
  }
}
TEST_CASE("Decode erronous torrent files using decodeFile", "[Bencode][Decode][Torrents]")
{
  Bencode bEncode;
  SECTION("Decode singlefileerror.torrent", "[Bencode][Decode][Torrents]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeFile(kSingleFileWithErrorTorrent), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeFile(kSingleFileWithErrorTorrent), "Bencoding syntax error detected.");
  }
  SECTION("Decode multifileerror.torrent", "[Bencode][Decode][Torrents]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeFile(kMultiFileWithErrorTorrent), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeFile(kMultiFileWithErrorTorrent), "Bencoding syntax error detected.");
  }
  SECTION("Decode doesntexist.torrent", "[Bencode][Decode][Torrents]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeFile(kNonExistantTorrent), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeFile(kNonExistantTorrent), "Bencode file input stream failed to open or does not exist.");
  }
}