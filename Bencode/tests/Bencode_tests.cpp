#include "catch.hpp"
#include "Bencode.hpp"

using namespace H4;

using Catch::Matchers::Equals;

TEST_CASE("Creation and use of Bencode for decode of simple types (number, string) ", "[Bencode][Decode]")
{
  Bencode bEncode;

  SECTION("Decode an integer", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeInteger = bEncode.decode("i266e");
    REQUIRE(dynamic_cast<BNodeInteger *>(bNodeInteger.get()) != nullptr);
  }

  SECTION("Decode an string", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeString = bEncode.decode("12:qwertyuiopas");
    REQUIRE(dynamic_cast<BNodeString *>(bNodeString.get()) != nullptr);
  }

  SECTION("Decode an integer (266) and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeInteger = bEncode.decode("i266e");
    REQUIRE(((BNodeInteger *)bNodeInteger.get())->number == 266);
  }

  SECTION("Decode an integer (1000) and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeInteger = bEncode.decode("i1000e");
    REQUIRE(((BNodeInteger *)bNodeInteger.get())->number == 1000);
  }

  SECTION("Decode an negative integer (-666) and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeInteger = bEncode.decode("i-666e");
    REQUIRE(((BNodeInteger *)bNodeInteger.get())->number == -666);
  }

  SECTION("Decode an string ('qwertyuiopas') and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeString = bEncode.decode("12:qwertyuiopas");
    REQUIRE(((BNodeString *)bNodeString.get())->string == "qwertyuiopas");
  }

  SECTION("Decode an string ('abcdefghijklmnopqrstuvwxyz') and check value", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeString = bEncode.decode("26:abcdefghijklmnopqrstuvwxyz");
    REQUIRE(((BNodeString *)bNodeString.get())->string == "abcdefghijklmnopqrstuvwxyz");
  }
}
TEST_CASE("Creation and use of Bencode for decode of a table of integer test data", "[Bencode][Decode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, long>({{"i277e", 277},
                                                                   {"i32767e", 32767}}));

  Bencode bEncode;
  std::unique_ptr<BNode> bNodeInteger = bEncode.decode(test_input.c_str());
  REQUIRE(((BNodeInteger *)bNodeInteger.get())->number == expected);
}

TEST_CASE("Creation and use of Bencode for decode of a table of string test data", "[Bencode][Decode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, std::string>({{"13:qwertyuiopasd", "qwertyuiopasd"},
                                                                          {"6:mnbvcx", "mnbvcx"}}));

  Bencode bEncode;
  std::unique_ptr<BNode> bNodeString = bEncode.decode(test_input.c_str());
  REQUIRE(((BNodeString *)bNodeString.get())->string == expected);
}

TEST_CASE("Creation and use of Bencode for decode of collection types (list, dictionary) ", "[Bencode][Decode]")
{
  Bencode bEncode;

  SECTION("Decode an List", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeList = bEncode.decode("li266ei6780ei88ee");
    REQUIRE(dynamic_cast<BNodeList *>(bNodeList.get()) != nullptr);
  }

  SECTION("Decode an Dictionary", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeDict = bEncode.decode("d3:onei1e5:threei3e3:twoi2ee");
    REQUIRE(dynamic_cast<BNodeDict *>(bNodeDict.get()) != nullptr);
  }

  SECTION("Decode an list of integers and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeList = bEncode.decode("li266ei6780ei88ee");
    std::vector<long> numbers;
    for (const auto &bNode : ((BNodeList *)bNodeList.get())->list)
    {
      numbers.push_back(((BNodeInteger *)bNode.get())->number);
    }
    REQUIRE(numbers == std::vector<long>{266, 6780, 88});
  }

  SECTION("Decode an list of strings and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeList = bEncode.decode("l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze");
    std::vector<std::string> strings;
    for (const auto &bNode : ((BNodeList *)bNodeList.get())->list)
    {
      strings.push_back(((BNodeString *)bNode.get())->string);
    }
    REQUIRE(strings == std::vector<std::string>{"sillyy", "poiuytrewqas", "abcdefghijklmnopqrstuvwxyz"});
  }

  SECTION("Decode an Dictionary of ints and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeDict = bEncode.decode("d3:onei1e5:threei3e3:twoi2ee");
    std::map<std::string, long> entries;
    for (const auto &bNode : ((BNodeDict *)bNodeDict.get())->dict)
    {
      entries[bNode.first] = ((BNodeInteger *)bNode.second.get())->number;
    }
    REQUIRE(entries == std::map<std::string, long>{{"one", 1}, {"two", 2}, {"three", 3}});
  }

  SECTION("Decode a Dictionary of strings and check values", "[Bencode][Decode]")
  {
    std::unique_ptr<BNode> bNodeDict = bEncode.decode("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkle");
    std::map<std::string, std::string> entries;
    for (const auto &bNode : ((BNodeDict *)bNodeDict.get())->dict)
    {
      entries[bNode.first] = ((BNodeString *)bNode.second.get())->string;
    }
    REQUIRE(entries == std::map<std::string, std::string>{{"one", "0123456789"}, {"two", "asdfghjkl"}, {"three", "qwerty"}});
  }
}

TEST_CASE("Creation and use of Bencode for encode of simple types (number, string) ", "[Bencode][Encode]")
{
  Bencode bEncode;

  SECTION("Encode an integer (266) and check value", "[Bencode][Encode]")
  {
    std::string actual = bEncode.encode(std::make_unique<BNodeInteger>(BNodeInteger(266)));
    REQUIRE(actual == "i266e");
  }

  SECTION("Encode an integer (10000) and check value", "[Bencode][Encode]")
  {
    std::string actual = bEncode.encode(std::make_unique<BNodeInteger>(BNodeInteger(10000)));
    REQUIRE(actual == "i10000e");
  }

  SECTION("Encode an string ('qwertyuiopas') and check its value", "[Bencode][Encode]")
  {
    std::string actual = bEncode.encode(std::make_unique<BNodeString>(BNodeString("qwertyuiopas")));
    REQUIRE(actual == "12:qwertyuiopas");
  }

  SECTION("Encode an string ('abcdefghijklmnopqrstuvwxyz') and check its value", "[Bencode][Encode]")
  {
    std::string actual = bEncode.encode(std::make_unique<BNodeString>(BNodeString("abcdefghijklmnopqrstuvwxyz")));
    REQUIRE(actual == "26:abcdefghijklmnopqrstuvwxyz");
  }
}
TEST_CASE("Creation and use of Bencode for encode of a table of integer test data", "[Bencode][Encode]")
{

  auto [test_input, expected] = GENERATE(table<long, std::string>({{277, "i277e"},
                                                                   {32767, "i32767e"}}));

  Bencode bEncode;
  std::string actual = bEncode.encode(std::make_unique<BNodeInteger>(BNodeInteger(test_input)));
  REQUIRE(actual == expected);
}

TEST_CASE("Creation and use of Bencode for encode of a table of string test data", "[Bencode][Encode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, std::string>({{"qwertyuiopasd", "13:qwertyuiopasd"},
                                                                          {"mnbvcx", "6:mnbvcx"}}));

  Bencode bEncode;
  std::string actual = bEncode.encode(std::make_unique<BNodeString>(BNodeString(test_input)));
  REQUIRE(actual == expected);
}

TEST_CASE("Creation and use of Bencode for encode of collection types (list, dictionary) ", "[Bencode][Encode]")
{
  Bencode bEncode;

  SECTION("Encode an List of integers('li266ei6780ei88ee') and check value", "[Bencode][Encode]")
  {
    std::string expected = "li266ei6780ei88ee";
    REQUIRE(bEncode.encode(bEncode.decode(expected.c_str())) == expected);
  }

  SECTION("Encode an List of strings ('l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze') and check value", "[Bencode][Encode]")
  {
    std::string expected = "l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze";
    REQUIRE(bEncode.encode(bEncode.decode(expected.c_str())) == expected);
  }

  SECTION("Encode an Dictionary of integers and check balue", "[Bencode][Encode]")
  {
    std::string expected = "d3:onei1e5:threei3e3:twoi2ee";
    REQUIRE(bEncode.encode(bEncode.decode(expected.c_str())) == expected);
  }

  SECTION("Encode an Dictionary of strings and check balue", "[Bencode][Encode]")
  {
    std::string expected = "d3:one10:01234567895:three6:qwerty3:two9:asdfghjkle";
    REQUIRE(bEncode.encode(bEncode.decode(expected.c_str())) == expected);
  }
}

TEST_CASE("Decode generated exceptions", "[Bencode][Decode][Exceptions]")
{
  Bencode bEncode;

  SECTION("Decode passing a null/empty string", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decode(nullptr), std::invalid_argument);
    REQUIRE_THROWS_WITH(bEncode.decode(nullptr), "nullptr/empty string passed to be decoded.");
    REQUIRE_THROWS_AS(bEncode.decode(""), std::invalid_argument);
    REQUIRE_THROWS_WITH(bEncode.decode(""), "nullptr/empty string passed to be decoded.");
  }

    SECTION("Decode an string without terminating ':' on its length", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decode("26abcdefghijklmnopqrstuvwxyz"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decode("26abcdefghijklmnopqrstuvwxyz"), "Missing terminating ':' on string length.");
  }

  SECTION("Decode an integer without an end", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decode("i266"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decode("i266"), "Missing terminating 'e' on integer.");
  }

  SECTION("Decode an list without an end", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decode("li266ei6780ei88e"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decode("li266ei6780ei88e"), "Missing terminating 'e' on list.");
  }

  SECTION("Decode an diictionary without an end", "[Bencode][Decode]")
  {
    REQUIRE_THROWS_AS(bEncode.decode("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkl"), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decode("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkl"), "Missing terminating 'e' on dictionary.");
  }
}
