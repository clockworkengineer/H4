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
// ============
// STL includes
// ============
// ===================
// Unit test constants
// ===================
#define kXMLDeclaration1 "<?xml version = \"1.0\" encoding = \"UTF-8\" standalone = \"no\" ?>"
#define kXMLDeclaration2 "<?xml version = \"1.0\" encoding = \"UTF-16\" standalone = \"yes\" ?>"
#define kXMLDeclaration3 "<?xml version = \"1.1\" standalone = \"yes\" ?>"
#define kXMLDeclaration4 "<?xml ?>"
#define kXMLDeclaration5 "<?xml >"
#define kXMLDeclaration6 "<?xml version = \"1.0\" standalone = \"no\" encoding = \"UTF-8\"?>"
// ==========================
// Unit test helper functions
// ==========================
#endif /* XML_TESTS_HPP */