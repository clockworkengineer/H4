#ifndef JSON_TESTS_HPP
#define JSON_TESTS_HPP
// ======================
// Test framework include
// ======================
#include "catch2/catch_all.hpp"
// =========================
// Class under test includes
// =========================
#include "JSON.hpp"
#include "JSONSources.hpp"
#include "JSONDestinations.hpp"
#include "JSONTranslator.hpp"
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
#define kGeneratedJSONFile "./testData/generated.json"
#define kSIngleJSONFile "./testData/testfile001.json"
#define kNonExistantJSONFile "./testData/doesntexist.json"
// ==========================
// Unit test helper functions
// ==========================
extern std::string readJSONFromFile(const std::string &jsonFileName);
extern void checkObject(H4::JNode *jNode);
extern void checkArray(H4::JNode *jNode);
#endif /* JSON_TESTS_HPP */