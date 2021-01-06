#include "catch.hpp"
#include "Bencode.hpp"
#include "BencodeSources.hpp"
#include "BencodeDestinations.hpp"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
using namespace H4;
bool compareFiles(const std::string &p1, const std::string &p2)
{
  std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
  std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);
  if (f1.fail() || f2.fail())
  {
    return false; //file problem
  }
  if (f1.tellg() != f2.tellg())
  {
    return false; //size mismatch
  }
  //seek back to beginning and use std::equal to compare contents
  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}
TEST_CASE("Creation and use of Bencode for decode of simple types (number, string) ", "[Bencode][Decode]")
{
  Bencode bEncode;
  SECTION("Decode an integer", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNode = bEncode.decodeBuffer("i266e");
    ((BNode *)bNode.get())->nodeType = Bencode::BNodeType::integer;
  }
  SECTION("Decode an string", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNode = bEncode.decodeBuffer("12:qwertyuiopas");
    ((BNode *)bNode.get())->nodeType = Bencode::BNodeType::string;
  }
  SECTION("Decode an integer (266) and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeInteger = bEncode.decodeBuffer("i266e");
    REQUIRE(((BNodeInteger *)bNodeInteger.get())->value == 266);
  }
  SECTION("Decode an integer (1000) and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeInteger = bEncode.decodeBuffer("i1000e");
    REQUIRE(((BNodeInteger *)bNodeInteger.get())->value == 1000);
  }
  SECTION("Decode an negative integer (-666) and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeInteger = bEncode.decodeBuffer("i-666e");
    REQUIRE(((BNodeInteger *)bNodeInteger.get())->value == -666);
  }
  SECTION("Decode an string ('qwertyuiopas') and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeString = bEncode.decodeBuffer("12:qwertyuiopas");
    REQUIRE(((BNodeString *)bNodeString.get())->value == "qwertyuiopas");
  }
  SECTION("Decode an string ('abcdefghijklmnopqrstuvwxyz') and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeString = bEncode.decodeBuffer("26:abcdefghijklmnopqrstuvwxyz");
    REQUIRE(((BNodeString *)bNodeString.get())->value == "abcdefghijklmnopqrstuvwxyz");
  }
}
TEST_CASE("Creation and use of Bencode for decode of a table of integer test data", "[Bencode][Decode]")
{
  auto [test_input, expected] = GENERATE(table<std::string, long>({{"i277e", 277},
                                                                   {"i32767e", 32767}}));
  Bencode bEncode;
  std::unique_ptr<BNode> bNodeInteger = bEncode.decodeBuffer(test_input.c_str());
  REQUIRE(((BNodeInteger *)bNodeInteger.get())->value == expected);
}
TEST_CASE("Creation and use of Bencode for decode of a table of string test data", "[Bencode][Decode]")
{
  auto [test_input, expected] = GENERATE(table<std::string, std::string>({{"13:qwertyuiopasd", "qwertyuiopasd"},
                                                                          {"6:mnbvcx", "mnbvcx"}}));
  Bencode bEncode;
  std::unique_ptr<BNode> bNodeString = bEncode.decodeBuffer(test_input.c_str());
  REQUIRE(((BNodeString *)bNodeString.get())->value == expected);
}
TEST_CASE("Creation and use of Bencode for decode of collection types (list, dictionary) ", "[Bencode][Decode]")
{
  Bencode bEncode;
  SECTION("Decode an List", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNode = bEncode.decodeBuffer("li266ei6780ei88ee");
    ((BNode *)bNode.get())->nodeType = Bencode::BNodeType::list;
  }
  SECTION("Decode an Dictionary", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNode = bEncode.decodeBuffer("d3:onei1e5:threei3e3:twoi2ee");
    ((BNode *)bNode.get())->nodeType = Bencode::BNodeType::dictionary;
  }
  SECTION("Decode an list of integers and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeList = bEncode.decodeBuffer("li266ei6780ei88ee");
    std::vector<long> numbers;
    for (const auto &bNode : ((BNodeList *)bNodeList.get())->value)
    {
      numbers.push_back(((BNodeInteger *)bNode.get())->value);
    }
    REQUIRE(numbers == std::vector<long>{266, 6780, 88});
  }
  SECTION("Decode an list of strings and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeList = bEncode.decodeBuffer("l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze");
    std::vector<std::string> strings;
    for (const auto &bNode : ((BNodeList *)bNodeList.get())->value)
    {
      strings.push_back(((BNodeString *)bNode.get())->value);
    }
    REQUIRE(strings == std::vector<std::string>{"sillyy", "poiuytrewqas", "abcdefghijklmnopqrstuvwxyz"});
  }
  SECTION("Decode an Dictionary of ints and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeDict = bEncode.decodeBuffer("d3:onei1e5:threei3e3:twoi2ee");
    std::map<std::string, long> entries;
    for (const auto &bNode : ((BNodeDict *)bNodeDict.get())->value)
    {
      entries[bNode.first] = ((BNodeInteger *)bNode.second.get())->value;
    }
    REQUIRE(entries == std::map<std::string, long>{{"one", 1}, {"two", 2}, {"three", 3}});
  }
  SECTION("Decode a Dictionary of strings and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeDict = bEncode.decodeBuffer("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkle");
    std::map<std::string, std::string> entries;
    for (const auto &bNode : ((BNodeDict *)bNodeDict.get())->value)
    {
      entries[bNode.first] = ((BNodeString *)bNode.second.get())->value;
    }
    REQUIRE(entries == std::map<std::string, std::string>{{"one", "0123456789"}, {"two", "asdfghjkl"}, {"three", "qwerty"}});
  }
}
TEST_CASE("Creation and use of Bencode for encode of simple types (number, string) ", "[Bencode][Encode]")
{
  Bencode bEncode;
  SECTION("Encode an integer (266) and check value", "[Bencode][Encode]")
  {
    Bencode::Bencoding actual = bEncode.encodeToBuffer(std::make_unique<BNodeInteger>(BNodeInteger(266)));
    REQUIRE(actual == Bencode::Bencoding("i266e"));
  }
  SECTION("Encode an integer (10000) and check value", "[Bencode][Encode]")
  {
    Bencode::Bencoding actual = bEncode.encodeToBuffer(std::make_unique<BNodeInteger>(BNodeInteger(10000)));
    REQUIRE(actual == Bencode::Bencoding("i10000e"));
  }
  SECTION("Encode an string ('qwertyuiopas') and check its value", "[Bencode][Encode]")
  {
    Bencode::Bencoding actual = bEncode.encodeToBuffer(std::make_unique<BNodeString>(BNodeString("qwertyuiopas")));
    REQUIRE(actual == Bencode::Bencoding("12:qwertyuiopas"));
  }
  SECTION("Encode an string ('abcdefghijklmnopqrstuvwxyz') and check its value", "[Bencode][Encode]")
  {
    Bencode::Bencoding actual = bEncode.encodeToBuffer(std::make_unique<BNodeString>(BNodeString("abcdefghijklmnopqrstuvwxyz")));
    REQUIRE(actual == Bencode::Bencoding("26:abcdefghijklmnopqrstuvwxyz"));
  }
}
TEST_CASE("Creation and use of Bencode for encode of a table of integer test data", "[Bencode][Encode]")
{
  auto [test_input, expected] = GENERATE(table<long, Bencode::Bencoding>({{277, "i277e"},
                                                                          {32767, "i32767e"}}));
  Bencode bEncode;
  Bencode::Bencoding actual = bEncode.encodeToBuffer(std::make_unique<BNodeInteger>(BNodeInteger(test_input)));
  REQUIRE(actual == expected);
}
TEST_CASE("Creation and use of Bencode for encode of a table of string test data", "[Bencode][Encode]")
{
  auto [test_input, expected] = GENERATE(table<std::string, Bencode::Bencoding>({{"qwertyuiopasd", "13:qwertyuiopasd"},
                                                                                 {"mnbvcx", "6:mnbvcx"}}));
  Bencode bEncode;
  Bencode::Bencoding actual = bEncode.encodeToBuffer(std::make_unique<BNodeString>(BNodeString(test_input)));
  REQUIRE(actual == expected);
}
TEST_CASE("Creation and use of Bencode for encode of collection types (list, dictionary) ", "[Bencode][Encode]")
{
  Bencode bEncode;
  SECTION("Encode an List of integers('li266ei6780ei88ee') and check value", "[Bencode][Encode]")
  {
    Bencode::Bencoding expected("li266ei6780ei88ee");
    REQUIRE(bEncode.encodeToBuffer(bEncode.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode an List of strings ('l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze') and check value", "[Bencode][Encode]")
  {
    Bencode::Bencoding expected = "l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze";
    REQUIRE(bEncode.encodeToBuffer(bEncode.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode an Dictionary of integers and check balue", "[Bencode][Encode]")
  {
    Bencode::Bencoding expected = "d3:onei1e5:threei3e3:twoi2ee";
    REQUIRE(bEncode.encodeToBuffer(bEncode.decodeBuffer(expected)) == expected);
  }
  SECTION("Encode an Dictionary of strings and check balue", "[Bencode][Encode]")
  {
    Bencode::Bencoding expected = "d3:one10:01234567895:three6:qwerty3:two9:asdfghjkle";
    REQUIRE(bEncode.encodeToBuffer(bEncode.decodeBuffer(expected)) == expected);
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
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("26abcdefghijklmnopqrstuvwxyz"), "Missing terminating ':' on string length.");
  }
  SECTION("Decode an integer without a terminating end", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeBuffer("i266"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("i266"), "Missing terminating 'e' on integer.");
  }
  SECTION("Decode an list without a terminating end", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeBuffer("li266ei6780ei88e"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("li266ei6780ei88e"), "Missing terminating 'e' on list.");
  }
  SECTION("Decode an diictionary without a terminating end", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeBuffer("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkl"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkl"), "Missing terminating 'e' on dictionary.");
  }
  SECTION("Decode an string that terminates prematurely", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeBuffer("26:abcdefghijklmno"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeBuffer("26:abcdefghijklmno"), "Decode buffer empty before decode complete.");
  }
}
TEST_CASE("Encode generated exceptions", "[Bencode][Encode][Exceptions]")
{
  Bencode bEncode;
  SECTION("Encode passed nullptr", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.encodeToBuffer(nullptr), std::invalid_argument);
    REQUIRE_THROWS_WITH(bEncode.encodeToBuffer(nullptr), "Nullptr passed as bNode to be encoded.");
    REQUIRE_THROWS_AS(bEncode.encodeToBuffer(std::unique_ptr<BNode>(nullptr)), std::invalid_argument);
    REQUIRE_THROWS_WITH(bEncode.encodeToBuffer(std::unique_ptr<BNode>(nullptr)), "Nullptr passed as bNode to be encoded.");
  }
  SECTION("Encode passed invalid BNode type", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.encodeToBuffer(std::unique_ptr<BNode>(new BNode())), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.encodeToBuffer(std::unique_ptr<BNode>(new BNode())), "Unknown BNode type encountered during encode.");
  }
}
TEST_CASE("Decode torrent files using decodeFile", "[Bencode][Decode][Torrents]")
{
  Bencode bEncode;
  SECTION("Decode singlefile.torrent", "[Bencode][Decode][Torrents]")
  {
    std::unique_ptr<BNode> bNode = bEncode.decodeFile("./testData/singlefile.torrent");
    ((BNode *)bNode.get())->nodeType = Bencode::BNodeType::dictionary;
  }
  SECTION("Decode singlefile.torrent and check value ", "[Bencode][Decode][Torrents]")
  {
    std::ifstream torrentFile{"./testData/singlefile.torrent"};
    std::ostringstream expected;
    expected << torrentFile.rdbuf();
    REQUIRE(bEncode.encodeToBuffer(bEncode.decodeFile("./testData/singlefile.torrent")) == Bencode::Bencoding(expected.str()));
  }
  SECTION("Decode multifile.torrent", "[Bencode][Decode][Torrents]")
  {
    std::unique_ptr<BNode> bNode = bEncode.decodeFile("./testData/multifile.torrent");
    ((BNode *)bNode.get())->nodeType = Bencode::BNodeType::dictionary;
  }
  SECTION("Decode multifile.torrent and check value ", "[Bencode][Decode][Torrents]")
  {
    std::ifstream torrentFile{"./testData/multifile.torrent"};
    std::ostringstream expected;
    expected << torrentFile.rdbuf();
    REQUIRE(bEncode.encodeToBuffer(bEncode.decodeFile("./testData/multifile.torrent")) == Bencode::Bencoding(expected.str()));
  }
}
TEST_CASE("Decode erronous torrent files using decodeFile", "[Bencode][Decode][Torrents]")
{
  Bencode bEncode;
  SECTION("Decode singlefileerror.torrent", "[Bencode][Decode][Torrents]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeFile("./testData/singlefileerror.torrent"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeFile("./testData/singlefileerror.torrent"), "Missing terminating ':' on string length.");
  }
  SECTION("Decode multifileerror.torrent", "[Bencode][Decode][Torrents]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeFile("./testData/multifileerror.torrent"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeFile("./testData/multifileerror.torrent"), "Missing terminating ':' on string length.");
  }
  SECTION("Decode doesntexist.torrent", "[Bencode][Decode][Torrents]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeFile("./testData/doesntexist.torrent"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeFile("./testData/doesntexist.torrent"), "Bencode file input stream failed to open or does not exist.");
  }
}
TEST_CASE("Encode torrent files using encodeToFile", "[Bencode][Encode][Torrents]")
{
  Bencode bEncode;
  SECTION("Encode singlefile.torrent and check value", "[Bencode][Encode][Torrents]")
  {
    if (std::filesystem::exists("./testData/generated.torrent"))
    {
      std::filesystem::remove("./testData/generated.torrent");
    }
    bEncode.encodeToFile(bEncode.decodeFile("./testData/singlefile.torrent"), "./testData/generated.torrent");
    REQUIRE_FALSE(!compareFiles("./testData/singlefile.torrent", "./testData/generated.torrent"));
  }
  SECTION("Encode multifile.torrent and check value", "[Bencode][Encode][Torrents]")
  {
    if (std::filesystem::exists("./testData/generated.torrent"))
    {
      std::filesystem::remove("./testData/generated.torrent");
    }
    bEncode.encodeToFile(bEncode.decodeFile("./testData/multifile.torrent"), "./testData/generated.torrent");
    REQUIRE_FALSE(!compareFiles("./testData/multifile.torrent", "./testData/generated.torrent"));
  }
}
TEST_CASE("Creation and use of ISource (File) interface.", "[Bencode][Decode][ISource]")
{

  SECTION("Create FileSource with singlefile.torrent.", "[Bencode][Decode][ISource]")
  {
    REQUIRE_NOTHROW(FileSource("./testData/singlefile.torrent"));
  }

  SECTION("Create FileSource with non existants file.", "[Bencode][Decode][ISource]")
  {
    REQUIRE_THROWS_AS(FileSource("./testData/doesntexist.torrent"), std::runtime_error);
    REQUIRE_THROWS_WITH(FileSource("./testData/doesntexist.torrent"), "Bencode file input stream failed to open or does not exist.");
  }

  SECTION("Create FileSource with singlefile.torrent. and positioned on the correct first character", "[Bencode][Decode][ISource]")
  {
    FileSource source = FileSource("./testData/singlefile.torrent");
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == 'd');
  }

  SECTION("Create FileSource with singlefile.torrent moveToNextByte positions to correct next character", "[Bencode][Decode][ISource]")
  {
    FileSource source = FileSource("./testData/singlefile.torrent");
    source.moveToNextByte();
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == '8');
  }

  SECTION("Create FileSource with singlefile.torrent move to last character, check it and the bytes moved.", "[Bencode][Decode][ISource]")
  {
    FileSource source = FileSource("./testData/singlefile.torrent");
    long length = 0;
    while (source.bytesToDecode())
    {
      source.moveToNextByte();
      length++;
    }
    REQUIRE(length == 764);                     // eof
    REQUIRE((int)source.currentByte() == 0xff); // eof
  }
}

TEST_CASE("Creation and use of ISource (Buffer) interface (buffer contains file singlefile.torrent).", "[Bencode][Decode][ISource]")
{

  std::ifstream torrentFile{"./testData/singlefile.torrent"};
  std::ostringstream buffer;
  buffer << torrentFile.rdbuf();

  SECTION("Create BufferSource.", "[Bencode][Decode][ISource]")
  {
    REQUIRE_NOTHROW(BufferSource(buffer.str()));
  }

  SECTION("Create BufferSource with empty buffer.", "[Bencode][Decode][ISource]")
  {
    REQUIRE_THROWS_AS(BufferSource(""), std::invalid_argument);
    REQUIRE_THROWS_WITH(BufferSource(""), "Empty source buffer passed to be encoded.");
  }

  SECTION("Create BufferSource with singlefile.torrent and that it is positioned on the correct first character", "[Bencode][Decode][ISource]")
  {
    BufferSource source = BufferSource(buffer.str());
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == 'd');
  }

  SECTION("Create BufferSource with singlefile.torrent and that moveToNextByte positions to correct next character", "[Bencode][Decode][ISource]")
  {
    BufferSource source = BufferSource(buffer.str());
    source.moveToNextByte();
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == '8');
  }

  SECTION("Create BufferSource with singlefile.torrent move to last character, check it and the bytes moved.", "[Bencode][Decode][ISource]")
  {
    BufferSource source = BufferSource(buffer.str());
    long length = 0;
    while (source.bytesToDecode())
    {
      source.moveToNextByte();
      length++;
    }
    REQUIRE(length == 764); // eof
    REQUIRE((int) source.currentByte() == 255); //eof
  }
}