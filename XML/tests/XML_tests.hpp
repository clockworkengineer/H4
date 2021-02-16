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
// ===================
// Unit test constants
// ===================
// ==========================
// Unit test helper functions
// ==========================
std::string readXMLFromFileUTF8(const std::string &xmlFileName);
std::u16string readXMLFromFileUTF16(const std::string &xmlFileName);
#endif /* XML_TESTS_HPP */