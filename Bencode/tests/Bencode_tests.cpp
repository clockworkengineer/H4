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

  SECTION("Decode an string", "[Bencode]")
  {
    std::unique_ptr<Bencode::BNode> bNodeString = bEncode.decode("12:qwertyuiopas");
    Bencode::BNodeString *pBNodeString = (Bencode::BNodeString *)bNodeString.get();
    REQUIRE(pBNodeString->string == "qwertyuiopas");
  }
}