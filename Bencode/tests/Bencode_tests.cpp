#include "catch.hpp"
#include "Bencode.hpp"

using namespace H4;

TEST_CASE("Creation and use of Bencode for decode of simple types (number, string) ", "[Bencode]")
{
  Bencode bEncode;

  SECTION("Decode an integer", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeNumber = bEncode.decode("i266e");
    REQUIRE(dynamic_cast<Bencode::BNodeNumber *>(bNodeNumber.get()) != nullptr);
  }

  SECTION("Decode an string", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode("12:qwertyuiopas");
    REQUIRE(dynamic_cast<Bencode::BNodeString *>(bNodeString.get()) != nullptr);
  }

  SECTION("Decode an integer and check value", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeNumber = bEncode.decode("i266e");
    Bencode::BNodeNumber *pBNodeNumber = (Bencode::BNodeNumber *)bNodeNumber.get();
    REQUIRE(pBNodeNumber->number == "266");
  }

  SECTION("Decode an integer and check value", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeNumber = bEncode.decode("i1000e");
    Bencode::BNodeNumber *pBNodeNumber = (Bencode::BNodeNumber *)bNodeNumber.get();
    REQUIRE(pBNodeNumber->number == "1000");
  }

  SECTION("Decode an string and check value", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode("12:qwertyuiopas");
    Bencode::BNodeString *pBNodeString = (Bencode::BNodeString *)bNodeString.get();
    REQUIRE(pBNodeString->string == "qwertyuiopas");
  }

  SECTION("Decode an string and check value", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode("26:abcdefghijklmnopqrstuvwxyz");
    Bencode::BNodeString *pBNodeString = (Bencode::BNodeString *)bNodeString.get();
    REQUIRE(pBNodeString->string == "abcdefghijklmnopqrstuvwxyz");
  }
}
TEST_CASE("Creation and use of Bencode for decode of a table of integer test data", "[Bencode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, std::string>({{"i277e", "277"},
                                                                          {"i32767e", "32767"}}));

  Bencode bEncode;
  std::unique_ptr<Bencode::BNode> bNodeNumber = bEncode.decode(test_input.c_str());
  Bencode::BNodeNumber *pBNodeNumber = (Bencode::BNodeNumber *)bNodeNumber.get();
  REQUIRE(pBNodeNumber->number == expected);
}

TEST_CASE("Creation and use of Bencode for decode of a table of string test data", "[Bencode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, std::string>({{"13:qwertyuiopasd", "qwertyuiopasd"},
                                                                          {"6:mnbvcx", "mnbvcx"}}));

  Bencode bEncode;
  std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode(test_input.c_str());
  Bencode::BNodeString *pBNodeString = (Bencode::BNodeString *)bNodeString.get();
  REQUIRE(pBNodeString->string == expected);
}

TEST_CASE("Creation and use of Bencode for decode of collection types (list, dictionary) ", "[Bencode]")
{
  Bencode bEncode;

  SECTION("Decode an List", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeList = bEncode.decode("li266ei6780ei88ee");
    REQUIRE(dynamic_cast<Bencode::BNodeList *>(bNodeList.get()) != nullptr);
  }

  SECTION("Decode an Dictionary", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeDictionary = bEncode.decode("d3:onei1e3:twoi2e5:threei3ee");
    REQUIRE(dynamic_cast<Bencode::BNodeDictionary *>(bNodeDictionary.get()) != nullptr);
  }

  SECTION("Decode an list of integers and check values", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeList = bEncode.decode("li266ei6780ei88ee");
    Bencode::BNodeList *pBNodeList = (Bencode::BNodeList *)bNodeList.get();
    std::vector<std::string> numbers;
    for (const auto &bNode : pBNodeList->list)
    {
      numbers.push_back(((Bencode::BNodeNumber *)bNode.get())->number);
    }
    std::vector<std::string> expected{"266", "6780", "88"};
    REQUIRE(numbers == expected);
  }

  SECTION("Decode an list of strings and check values", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeList = bEncode.decode("l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze");
    Bencode::BNodeList *pBNodeList = (Bencode::BNodeList *)bNodeList.get();
    std::vector<std::string> strings;
    for (const auto &bNode : pBNodeList->list)
    {
      strings.push_back(((Bencode::BNodeString *)bNode.get())->string);
    }
    std::vector<std::string> expected{"sillyy", "poiuytrewqas", "abcdefghijklmnopqrstuvwxyz"};
    REQUIRE(strings == expected);
  }


  SECTION("Decode an Dictionary of ints and check values", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeDictionary = bEncode.decode("d3:onei1e3:twoi2e5:threei3ee");
    Bencode::BNodeDictionary *pBNodeDictionary = (Bencode::BNodeDictionary *)bNodeDictionary.get();
    std::map<std::string, std::string> entries;
    for (const auto &bNode : pBNodeDictionary->dict)
    {
       Bencode::BNodeNumber *pBNodeNumber = (Bencode::BNodeNumber *)bNode.second.get();
       entries[bNode.first] = pBNodeNumber->number;
    }
    std::map<std::string, std::string> expected{ {"one","1"}, { "two", "2"}, {"three","3"}};
    REQUIRE(entries == expected);
  }
}
