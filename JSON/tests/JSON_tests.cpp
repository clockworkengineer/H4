//
// Unit Tests: JSON
//
// Description: Unit tests for JSON class using the Catch2 test framework.
//
// ======================
// Test framework include
// ======================
#include "catch.hpp"
// =========================
// Class under test includes
// =========================
#include "JSON.hpp"
#include "JSONSources.hpp"
#include "JSONDestinations.hpp"
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
// =======================
// JSON class namespace
// =======================
using namespace H4;
// =======================
// Local support functions
// =======================
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
// ==========
// Test cases
// ==========
