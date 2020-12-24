#include "catch.hpp"
#include "Bencode.hpp"

using namespace H4;

TEST_CASE("Creation and use of Bencode for decode of simple types", "[Bencode]")
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
TEST_CASE("Creation and use of Bencode for decode of a list of integers", "[Bencode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, std::string>({{"i277e", "277"},
                                                                          {"i32767e", "32767"}}));

  Bencode bEncode;
  std::unique_ptr<Bencode::BNode> bNodeNumber = bEncode.decode(test_input.c_str());
  Bencode::BNodeNumber *pBNodeNumber = (Bencode::BNodeNumber *)bNodeNumber.get();
  REQUIRE(pBNodeNumber->number == expected);
}

TEST_CASE("Creation and use of Bencode for decode of a list of strings", "[Bencode]")
{

  auto [test_input, expected] = GENERATE(table<std::string, std::string>({{"13:qwertyuiopasd", "qwertyuiopasd"},
                                                                          {"6:mnbvcx", "mnbvcx"}}));

  Bencode bEncode;
  std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode(test_input.c_str());
  Bencode::BNodeString *pBNodeString = (Bencode::BNodeString *)bNodeString.get();
  REQUIRE(pBNodeString->string == expected);
}