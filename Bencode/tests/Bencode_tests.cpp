#include "catch.hpp"
#include "Bencode.hpp"

using namespace H4;

TEST_CASE("Creation and use of Bencode for decode of simple types (number, string) ", "[Bencode][Decode]")
{
  Bencode bEncode;

  SECTION("Decode an integer", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeNumber = bEncode.decode("i266e");
    REQUIRE(dynamic_cast<Bencode::BNodeNumber *>(bNodeNumber.get()) != nullptr);
  }

  SECTION("Decode an string", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode("12:qwertyuiopas");
    REQUIRE(dynamic_cast<Bencode::BNodeString *>(bNodeString.get()) != nullptr);
  }

  SECTION("Decode an integer (266) and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeNumber = bEncode.decode("i266e");
    REQUIRE(((Bencode::BNodeNumber *)bNodeNumber.get())->number == 266);
  }

  SECTION("Decode an integer (1000) and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeNumber = bEncode.decode("i1000e");
    REQUIRE(((Bencode::BNodeNumber *)bNodeNumber.get())->number == 1000);
  }

  SECTION("Decode an string ('qwertyuiopas') and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode("12:qwertyuiopas");
    REQUIRE(((Bencode::BNodeString *)bNodeString.get())->string == "qwertyuiopas");
  }

  SECTION("Decode an string ('abcdefghijklmnopqrstuvwxyz') and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode("26:abcdefghijklmnopqrstuvwxyz");
    REQUIRE(((Bencode::BNodeString *)bNodeString.get())->string == "abcdefghijklmnopqrstuvwxyz");
  }
}
TEST_CASE("Creation and use of Bencode for decode of a table of integer test data", "[Bencode][Decode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, long>({{"i277e", 277},
                                                                   {"i32767e", 32767}}));

  Bencode bEncode;
  std::unique_ptr<Bencode::BNode> bNodeNumber = bEncode.decode(test_input.c_str());
  REQUIRE(((Bencode::BNodeNumber *)bNodeNumber.get())->number == expected);
}

TEST_CASE("Creation and use of Bencode for decode of a table of string test data", "[Bencode][Decode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, std::string>({{"13:qwertyuiopasd", "qwertyuiopasd"},
                                                                          {"6:mnbvcx", "mnbvcx"}}));

  Bencode bEncode;
  std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode(test_input.c_str());
  REQUIRE(((Bencode::BNodeString *)bNodeString.get())->string == expected);
}

TEST_CASE("Creation and use of Bencode for decode of collection types (list, dictionary) ", "[Bencode][Decode]")
{
  Bencode bEncode;

  SECTION("Decode an List", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeList = bEncode.decode("li266ei6780ei88ee");
    REQUIRE(dynamic_cast<Bencode::BNodeList *>(bNodeList.get()) != nullptr);
  }

  SECTION("Decode an Dictionary", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeDict = bEncode.decode("d3:onei1e3:twoi2e5:threei3ee");
    REQUIRE(dynamic_cast<Bencode::BNodeDict *>(bNodeDict.get()) != nullptr);
  }

  SECTION("Decode an list of integers and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeList = bEncode.decode("li266ei6780ei88ee");
    std::vector<long> numbers;
    for (const auto &bNode : ((Bencode::BNodeList *)bNodeList.get())->list)
    {
      numbers.push_back(((Bencode::BNodeNumber *)bNode.get())->number);
    }
    REQUIRE(numbers == std::vector<long>{266, 6780, 88});
  }

  SECTION("Decode an list of strings and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeList = bEncode.decode("l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze");
    std::vector<std::string> strings;
    for (const auto &bNode : ((Bencode::BNodeList *)bNodeList.get())->list)
    {
      strings.push_back(((Bencode::BNodeString *)bNode.get())->string);
    }
    REQUIRE(strings == std::vector<std::string>{"sillyy", "poiuytrewqas", "abcdefghijklmnopqrstuvwxyz"});
  }

  SECTION("Decode an Dictionary of ints and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeDict = bEncode.decode("d3:onei1e3:twoi2e5:threei3ee");
    std::map<std::string, long> entries;
    for (const auto &bNode : ((Bencode::BNodeDict *)bNodeDict.get())->dict)
    {
      entries[bNode.first] = ((Bencode::BNodeNumber *)bNode.second.get())->number;
    }
    REQUIRE(entries == std::map<std::string, long>{{"one", 1}, {"two", 2}, {"three", 3}});
  }

  SECTION("Decode a Dictionary of strings and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeDict = bEncode.decode("d3:one10:01234567893:two6:qwerty5:three9:asdfghjkle");
    std::map<std::string, std::string> entries;
    for (const auto &bNode : ((Bencode::BNodeDict *)bNodeDict.get())->dict)
    {
      entries[bNode.first] = ((Bencode::BNodeString *)bNode.second.get())->string;
    }
    REQUIRE(entries == std::map<std::string, std::string>{{"one", "0123456789"}, {"two", "qwerty"}, {"three", "asdfghjkl"}});
  }
}

TEST_CASE("Creation and use of Bencode for encode of simple types (number, string) ", "[Bencode][Encode]")
{
  Bencode bEncode;

  SECTION("Encode an integer (266) and check value", "[Bencode][Encode]")
  {
    std::string actual = bEncode.encode(std::make_unique<Bencode::BNodeNumber>(Bencode::BNodeNumber(266)));
    REQUIRE(actual == "i266e");
  }

  SECTION("Encode an integer (10000) and check value", "[Bencode][Encode]")
  {
    std::string actual = bEncode.encode(std::make_unique<Bencode::BNodeNumber>(Bencode::BNodeNumber(10000)));
    REQUIRE(actual == "i10000e");
  }

  SECTION("Encode an string ('qwertyuiopas') and check its value", "[Bencode][Encode]")
  {
    std::string actual = bEncode.encode(std::make_unique<Bencode::BNodeString>(Bencode::BNodeString("qwertyuiopas")));
    REQUIRE(actual == "12:qwertyuiopas");
  }

  SECTION("Encode an string ('abcdefghijklmnopqrstuvwxyz') and check its value", "[Bencode][Encode]")
  {
    std::string actual = bEncode.encode(std::make_unique<Bencode::BNodeString>(Bencode::BNodeString("abcdefghijklmnopqrstuvwxyz")));
    REQUIRE(actual == "26:abcdefghijklmnopqrstuvwxyz");
  }
}
TEST_CASE("Creation and use of Bencode for encode of a table of integer test data", "[Bencode][Encode]")
{

  auto [test_input, expected] = GENERATE(table<long, std::string>({{277, "i277e"},
                                                                   {32767, "i32767e"}}));

  Bencode bEncode;
  std::string actual = bEncode.encode(std::make_unique<Bencode::BNodeNumber>(Bencode::BNodeNumber(test_input)));
  REQUIRE(actual == expected);
}

TEST_CASE("Creation and use of Bencode for encode of a table of string test data", "[Bencode][Encode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, std::string>({{"qwertyuiopasd", "13:qwertyuiopasd"},
                                                                          {"mnbvcx", "6:mnbvcx"}}));

  Bencode bEncode;
  std::string actual = bEncode.encode(std::make_unique<Bencode::BNodeString>(Bencode::BNodeString(test_input)));
  REQUIRE(actual== expected);
}
