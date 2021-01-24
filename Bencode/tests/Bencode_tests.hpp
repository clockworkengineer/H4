#ifndef BENCODE_TESTS_HPP
#define BENCODE_TESTS_HPP
// ======================
// Test framework include
// ======================
#include "catch2/catch_all.hpp"
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
// ===================
// Unit test constants
// ===================
#define kSingleFileTorrent "./testData/singlefile.torrent"
#define kMultiFileTorrent "./testData/multifile.torrent"
#define kSingleFileWithErrorTorrent  "./testData/singlefileerror.torrent"
#define kMultiFileWithErrorTorrent  "./testData/multifileerror.torrent"
#define kNonExistantTorrent  "./testData/doesntexist.torrent"
#define kGeneratedTorrentFile  "./testData/generated.torrent"
extern bool compareFiles(const std::string &fileName1, const std::string &fileName2);
extern std::string readBencodedBytesFromFile(const std::string &bencodedFileName);
#endif /* BENCODE_TESTS_HPP */