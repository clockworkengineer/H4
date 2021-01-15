//
// Unit Tests: Bencode
//
// Description: ISource, IDestination and BNode unit 
// tests for Bencode class using the Catch2 test framework.
//
// ================
// Test definitions
// ================
//
#include "Bencode_tests.hpp"
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
TEST_CASE("Use of BNode indexing operators", "[Bencode][BNode][Index]")
{
  Bencode bEncode;
  std::unique_ptr<BNode> bNode;
  SECTION("Decode dictionary and check its components using indexing", "[Bencode][BNode][Index]")
  {
    bNode = bEncode.decodeBuffer("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkle");
    REQUIRE(BNode::ref<BNodeString>((*bNode)["one"]).value == "0123456789");
    REQUIRE(BNode::ref<BNodeString>((*bNode)["two"]).value == "asdfghjkl");
    REQUIRE(BNode::ref<BNodeString>((*bNode)["three"]).value == "qwerty");
  }
  SECTION("Decode list and check its components using indexing", "[Bencode][BNode][Index]")
  {
    bNode = bEncode.decodeBuffer("l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze");
    REQUIRE(BNode::ref<BNodeString>((*bNode)[0]).value == "sillyy");
    REQUIRE(BNode::ref<BNodeString>((*bNode)[1]).value == "poiuytrewqas");
    REQUIRE(BNode::ref<BNodeString>((*bNode)[2]).value == "abcdefghijklmnopqrstuvwxyz");
  }
  SECTION("Decode list with embedded dictioanry and check its components using indexing", "[Bencode][BNode][Index]")
  {
    bNode = bEncode.decodeBuffer("l6:sillyyd3:one10:01234567895:three6:qwerty3:two9:asdfghjkle26:abcdefghijklmnopqrstuvwxyze");
    REQUIRE(BNode::ref<BNodeString>((*bNode)[0]).value == "sillyy");
    REQUIRE(BNode::ref<BNodeString>((*bNode)[1]["one"]).value == "0123456789");
    REQUIRE(BNode::ref<BNodeString>((*bNode)[1]["two"]).value == "asdfghjkl");
    REQUIRE(BNode::ref<BNodeString>((*bNode)[1]["three"]).value == "qwerty");
    REQUIRE(BNode::ref<BNodeString>((*bNode)[2]).value == "abcdefghijklmnopqrstuvwxyz");
  }
  SECTION("Decode dictionary and check an invalid key generates exception", "[Bencode][BNode][Index]")
  {
    bNode = bEncode.decodeBuffer("d3:one10:01234567895:three6:qwerty3:two9:asdfghjkle");
    REQUIRE_THROWS_AS((*bNode)["onee"].nodeType == Bencode::BNodeType::dictionary, std::runtime_error);
    REQUIRE_THROWS_WITH((*bNode)["onee"].nodeType == Bencode::BNodeType::dictionary, "Invalid key used in dictionary.");
  }
  SECTION("Decode list and check an invalid index generates exception", "[Bencode][BNode][Index]")
  {
    bNode = bEncode.decodeBuffer("l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze");
    REQUIRE_THROWS_AS((*bNode)[3].nodeType == Bencode::BNodeType::list, std::runtime_error);
    REQUIRE_THROWS_WITH((*bNode)[3].nodeType == Bencode::BNodeType::list, "Invalid index used in list.");
  }
}
TEST_CASE("Check BNode reference functions work.", "[Bencode][BNode][Reference]")
{
  Bencode bEncode;
  std::unique_ptr<BNode> bNode;
  SECTION("Integer reference.", "[Bencode][BNode][Reference]")
  {
    bNode = bEncode.decodeBuffer("i45500e");
    REQUIRE(BNode::ref<BNodeInteger>((*bNode)).value == 45500);
  }
  SECTION("String reference.", "[Bencode][BNode][Reference]")
  {
    bNode = bEncode.decodeBuffer("10:0123456789");
    REQUIRE(BNode::ref<BNodeString>((*bNode)).value == "0123456789");
  }
  SECTION("List reference.", "[Bencode][BNode][Reference]")
  {
    bNode = bEncode.decodeBuffer("l6:sillyy12:poiuytrewqas26:abcdefghijklmnopqrstuvwxyze");
    REQUIRE(BNode::ref<BNodeList>((*bNode)).value.size() == 3);
  }
  SECTION("Dictionary reference.", "[Bencode][BNode][Reference]")
  {
    bNode = bEncode.decodeBuffer("d3:one10:01234567894:four8:123456785:three6:qwerty3:two9:asdfghjkle");
    REQUIRE(BNode::ref<BNodeDict>((*bNode)).value.size() == 4);
  }
}