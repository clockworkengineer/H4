#ifndef XML_TESTS_HPP
#define XML_TESTS_HPP
// ======================
// Test framework include
// ======================
#include "catch2/catch_all.hpp"
// =========================
// Class under test includes
// =========================
#include "XML.hpp"
#include "XMLSources.hpp"
#include "XMLDestinations.hpp"
// ============
// STL includes
// ============
#include <fstream>
#include <filesystem>
// ===================
// Unit test constants
// ===================
#define kGeneratedXMLFile "./testData/generated.xml"
#define kSingleXMLFile "./testData/testfile001.xml"
#define kNonExistantXMLFile "./testData/doesntexist.xml"
// ==========================
// Unit test helper functions
// ==========================
std::string readXMLFromFileUTF8(const std::string &xmlFileName);
std::u16string readXMLFromFileUTF16(const std::string &xmlFileName);
void writeXMLToFileUTF8(const std::string &xmlFileName, const std::string &xmlString);
void writeXMLToFileUTF16(const std::string &xmlFileName, const std::u16string &xmlString, bool be);
void checkStringify(const std::string &xmlString);
#endif /* XML_TESTS_HPP */