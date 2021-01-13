//
// Unit Tests: Bencode
//
// Description: Unit tests for Bencode class using the Catch2 test framework.
//
// ======================
// Test framework include
// ======================
#include "catch.hpp"
// =========================
// Class under test includes
// =========================
#include "Bencode.hpp"
#include "BencodeSources.hpp"
#include "BencodeDestinations.hpp"
// ============
// STL includes
// ============
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
// ===================
// Unit test constants
// ===================
const char *kSingleFileTorrent = "./testData/singlefile.torrent";
const char *kMultiFileTorrent = "./testData/multifile.torrent";
const char *kSingleFileWithErrorTorrent = "./testData/singlefileerror.torrent";
const char *kMultiFileWithErrorTorrent = "./testData/multifileerror.torrent";
const char *kNonExistantTorrent = "./testData/doesntexist.torrent";
const char *kGeneratedTorrentFile = "./testData/generated.torrent";
// =======================
// Bencode class namespace
// =======================
using namespace H4;
// =======================
// Local support functions
// =======================
/// <summary>
/// Compare two Bencoded files.
/// </summary>
/// <param name="fileName1">Bencoded file 1</param>
/// <param name="fileName2">Bencoded file 2</param>
/// <returns>true if files the same, false otherwise</returns>
bool compareFiles(const std::string &fileName1, const std::string &fileName2)
{
  std::ifstream file1(fileName1, std::ifstream::binary | std::ifstream::ate);
  std::ifstream file2(fileName2, std::ifstream::binary | std::ifstream::ate);
  if (file1.fail() || file2.fail())
  {
    return false; //file problem
  }
  if (file1.tellg() != file2.tellg())
  {
    return false; //size mismatch
  }
  //seek back to beginning and use std::equal to compare contents
  file1.seekg(0, std::ifstream::beg);
  file2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(file1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(file2.rdbuf()));
}
/// <summary>
/// Open a Bencoded file, read its contents into a string buffer and return
/// the buffer.
/// </summary>
/// <param name="bencodedFileName">Bencoded file name</param>
/// <returns></returns>
std::string readBencodedBytesFromFile(const std::string &bencodedFileName)
{
  std::ifstream bencodedFile;
  bencodedFile.open(bencodedFileName);
  std::ostringstream bencodedFileBuffer;
  bencodedFileBuffer << bencodedFile.rdbuf();
  return (bencodedFileBuffer.str());
}
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
    static_cast<BNode *>(bNode.get())->nodeType = Bencode::BNodeType::integer;
  }
  SECTION("Decode an string", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("12:qwertyuiopas");
    static_cast<BNode *>(bNode.get())->nodeType = Bencode::BNodeType::string;
  }
  SECTION("Decode an integer (266) and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("i266e");
    REQUIRE(static_cast<BNodeInteger *>(bNode.get())->value == 266);
  }
  SECTION("Decode an integer (1000) and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("i1000e");
    REQUIRE(static_cast<BNodeInteger *>(bNode.get())->value == 1000);
  }
  SECTION("Decode an negative integer (-666) and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("i-666e");
    REQUIRE(static_cast<BNodeInteger *>(bNode.get())->value == -666);
  }
  SECTION("Decode an string ('qwertyuiopas') and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("12:qwertyuiopas");
    REQUIRE(static_cast<BNodeString *>(bNode.get())->value == "qwertyuiopas");
  }
  SECTION("Decode an string ('abcdefghijklmnopqrstuvwxyz') and check value", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("26:abcdefghijklmnopqrstuvwxyz");
    REQUIRE(static_cast<BNodeString *>(bNode.get())->value == "abcdefghijklmnopqrstuvwxyz");
  }
}
TEST_CASE("Creation and use of Bencode for decode of a table of integer test data", "[Bencode][Decode]")
{
  auto [testInput, expected] = GENERATE(table<std::string, long>({{"i277e", 277},
                                                                  {"i32767e", 32767}}));
  Bencode bEncode;
  std::unique_ptr<BNode> bNode = bEncode.decodeBuffer(testInput.c_str());
  REQUIRE(static_cast<BNodeInteger *>(bNode.get())->value == expected);
}
TEST_CASE("Creation and use of Bencode for decode of a table of string test data", "[Bencode][Decode]")
{
  auto [testInput, expected] = GENERATE(table<std::string, std::string>({{"13:qwertyuiopasd", "qwertyuiopasd"},
                                                                         {"6:mnbvcx", "mnbvcx"}}));
  Bencode bEncode;
  std::unique_ptr<BNode> bNode = bEncode.decodeBuffer(testInput.c_str());
  REQUIRE(static_cast<BNodeString *>(bNode.get())->value == expected);
}
TEST_CASE("Creation and use of Bencode for decode of collection types (list, dictionary) ", "[Bencode][Decode]")
{
  Bencode bEncode;
  std::unique_ptr<BNode> bNode;
  SECTION("Decode an List", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("li266ei6780ei88ee");
    static_cast<BNode *>(bNode.get())->nodeType = Bencode::BNodeType::list;
  }
  SECTION("Decode an Dictionary", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("d3:onei1e5:threei3e3:twoi2ee");
    static_cast<BNode *>(bNode.get())->nodeType = Bencode::BNodeType::dictionary;
  }
  SECTION("Decode an list of integers and check values", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("li266ei6780ei88ee");
    std::vector<long> numbers;
    for (const auto &bNode : static_cast<BNodeList *>(bNode.get())->value)
    {
      numbers.push_back(((BNodeInteger *)bNode.get())->value);
    }
    REQUIRE(numbers == std::vector<long>{266, 6780, 88});
  }
  SECTION("Decode an list of strings and check values", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze");
    std::vector<std::string> strings;
    for (const auto &bNode : static_cast<BNodeList *>(bNode.get())->value)
    {
      strings.push_back(((BNodeString *)bNode.get())->value);
    }
    REQUIRE(strings == std::vector<std::string>{"sillyy", "poiuytrewqas", "abcdefghijklmnopqrstuvwxyz"});
  }
  SECTION("Decode an Dictionary of ints and check values", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("d3:onei1e5:threei3e3:twoi2ee");
    std::map<std::string, long> entries;
    for (const auto &bNode : static_cast<BNodeDict *>(bNode.get())->value)
    {
      entries[bNode.first] = ((BNodeInteger *)bNode.second.get())->value;
    }
    REQUIRE(entries == std::map<std::string, long>{{"one", 1}, {"two", 2}, {"three", 3}});
  }
  SECTION("Decode a Dictionary of strings and check values", "[Bencode][Decode]")
  {
    bNode = bEncode.decodeBuffer("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkle");
    std::map<std::string, std::string> entries;
    for (const auto &bNode : static_cast<BNodeDict *>(bNode.get())->value)
    {
      entries[bNode.first] = ((BNodeString *)bNode.second.get())->value;
    }
    REQUIRE(entries == std::map<std::string, std::string>{{"one", "0123456789"}, {"two", "asdfghjkl"}, {"three", "qwerty"}});
  }
}
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
TEST_CASE("Decode torrent files using decodeFile", "[Bencode][Decode][Torrents]")
{
  Bencode bEncode;
  std::unique_ptr<BNode> bNode;
  SECTION("Decode singlefile.torrent", "[Bencode][Decode][Torrents]")
  {
    bNode = bEncode.decodeFile(kSingleFileTorrent);
    static_cast<BNode *>(bNode.get())->nodeType = Bencode::BNodeType::dictionary;
  }
  SECTION("Decode singlefile.torrent and check value ", "[Bencode][Decode][Torrents]")
  {
    REQUIRE(bEncode.encodeBuffer(bEncode.decodeFile(kSingleFileTorrent)) == Bencode::Bencoding(readBencodedBytesFromFile(kSingleFileTorrent)));
  }
  SECTION("Decode multifile.torrent", "[Bencode][Decode][Torrents]")
  {
    bNode = bEncode.decodeFile(kMultiFileTorrent);
    static_cast<BNode *>(bNode.get())->nodeType = Bencode::BNodeType::dictionary;
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
    REQUIRE_THROWS_WITH(bEncode.decodeFile(kSingleFileWithErrorTorrent), "Missing terminating ':' on string length.");
  }
  SECTION("Decode multifileerror.torrent", "[Bencode][Decode][Torrents]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeFile(kMultiFileWithErrorTorrent), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeFile(kMultiFileWithErrorTorrent), "Missing terminating ':' on string length.");
  }
  SECTION("Decode doesntexist.torrent", "[Bencode][Decode][Torrents]")
  {
    REQUIRE_THROWS_AS(bEncode.decodeFile(kNonExistantTorrent), std::runtime_error);
    REQUIRE_THROWS_WITH(bEncode.decodeFile(kNonExistantTorrent), "Bencode file input stream failed to open or does not exist.");
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
TEST_CASE("Creation and use of ISource (File) interface.", "[Bencode][Decode][ISource]")
{
  SECTION("Create FileSource with singlefile.torrent.", "[Bencode][Decode][ISource]")
  {
    REQUIRE_NOTHROW(FileSource(kSingleFileTorrent));
  }
  SECTION("Create FileSource with non existants file.", "[Bencode][Decode][ISource]")
  {
    REQUIRE_THROWS_AS(FileSource(kNonExistantTorrent), std::runtime_error);
    REQUIRE_THROWS_WITH(FileSource(kNonExistantTorrent), "Bencode file input stream failed to open or does not exist.");
  }
  SECTION("Create FileSource with singlefile.torrent. and positioned on the correct first character", "[Bencode][Decode][ISource]")
  {
    FileSource source = FileSource(kSingleFileTorrent);
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == 'd');
  }
  SECTION("Create FileSource with singlefile.torrent and then check moveToNextByte positions to correct next character", "[Bencode][Decode][ISource]")
  {
    FileSource source = FileSource(kSingleFileTorrent);
    source.moveToNextByte();
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == '8');
  }
  SECTION("Create FileSource with singlefile.torrent move past last character, check it and the bytes moved.", "[Bencode][Decode][ISource]")
  {
    FileSource source = FileSource(kSingleFileTorrent);
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
  std::string bencodedBuffer = readBencodedBytesFromFile(kSingleFileTorrent);
  SECTION("Create BufferSource.", "[Bencode][Decode][ISource]")
  {
    REQUIRE_NOTHROW(BufferSource(bencodedBuffer));
  }
  SECTION("Create BufferSource with empty buffer.", "[Bencode][Decode][ISource]")
  {
    REQUIRE_THROWS_AS(BufferSource(""), std::invalid_argument);
    REQUIRE_THROWS_WITH(BufferSource(""), "Empty source buffer passed to be encoded.");
  }
  SECTION("Create BufferSource with singlefile.torrent and that it is positioned on the correct first character", "[Bencode][Decode][ISource]")
  {
    BufferSource source = BufferSource(bencodedBuffer);
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == 'd');
  }
  SECTION("Create BufferSource with singlefile.torrent and then check moveToNextByte positions to correct next character", "[Bencode][Decode][ISource]")
  {
    BufferSource source = BufferSource(bencodedBuffer);
    source.moveToNextByte();
    REQUIRE_FALSE(!source.bytesToDecode());
    REQUIRE((char)source.currentByte() == '8');
  }
  SECTION("Create BufferSource with singlefile.torrent move past last character, check it and the bytes moved.", "[Bencode][Decode][ISource]")
  {
    BufferSource source = BufferSource(bencodedBuffer);
    long length = 0;
    while (source.bytesToDecode())
    {
      source.moveToNextByte();
      length++;
    }
    REQUIRE(length == 764);                    // eof
    REQUIRE((int)source.currentByte() == 255); // eof
  }
}
TEST_CASE("Creation and use of IDestination (Buffer) interface.", "[Bencode][Decode][ISource]")
{
  SECTION("Create BufferDesination.", "[Bencode][Encode][IDesination]")
  {
    REQUIRE_NOTHROW(BufferDestination());
  }
  SECTION("Create BufferDestination and get buffer which should be empty.", "[Bencode][Encode][IDesination]")
  {
    BufferDestination buffer;
    REQUIRE_FALSE(!buffer.getBuffer().isEmpty());
  }
  SECTION("Create BufferDestination and add one character.", "[Bencode][Encode][IDesination]")
  {
    BufferDestination buffer;
    buffer.addBytes("i");
    REQUIRE(buffer.getBuffer().size() == 1);
  }
  SECTION("Create BufferDestination and add an encoded integer and chekc result.", "[Bencode][Encode][IDesination]")
  {
    BufferDestination buffer;
    buffer.addBytes("i65767e");
    REQUIRE(buffer.getBuffer().size() == 7);
    REQUIRE(buffer.getBuffer() == Bencoding("i65767e"));
  }
}
TEST_CASE("Creation and use of IDestination (File) interface.", "[Bencode][Decode][ISource]")
{
  SECTION("Create FileDestination.", "[Bencode][Encode][IDesination]")
  {
    std::filesystem::remove(kGeneratedTorrentFile);
    REQUIRE_NOTHROW(FileDestination(kGeneratedTorrentFile));
  }
  SECTION("Create FileDestination when file already exists.", "[Bencode][Encode][IDesination]")
  {
    FileDestination file(kGeneratedTorrentFile);
    file = FileDestination(kGeneratedTorrentFile);
    REQUIRE_NOTHROW(FileDestination(kGeneratedTorrentFile));
  }
  SECTION("Create FileDestination and test file exists and should be empty.", "[Bencode][Encode][IDesination]")
  {
    std::filesystem::remove(kGeneratedTorrentFile);
    FileDestination file(kGeneratedTorrentFile);
    REQUIRE_FALSE(!std::filesystem::exists(kGeneratedTorrentFile));
    std::filesystem::path filePath(kGeneratedTorrentFile);
    REQUIRE(std::filesystem::file_size(filePath) == 0);
  }
  SECTION("Create FileDestination and add one character.", "[Bencode][Encode][IDesination]")
  {
    std::filesystem::remove(kGeneratedTorrentFile);
    FileDestination file(kGeneratedTorrentFile);
    file.addBytes("i");
    std::filesystem::path filePath(kGeneratedTorrentFile);
    REQUIRE(std::filesystem::file_size(filePath) == 1);
  }
  SECTION("Create FileDestination, add an encoded integer and check result.", "[Bencode][Encode][IDesination]")
  {
    std::filesystem::remove(kGeneratedTorrentFile);
    FileDestination file(kGeneratedTorrentFile);
    file.addBytes("i65767e");
    std::filesystem::path filePath(kGeneratedTorrentFile);
    REQUIRE(std::filesystem::file_size(filePath) == 7);
    std::string expected = readBencodedBytesFromFile(kGeneratedTorrentFile);
    REQUIRE(expected == "i65767e");
  }
}
