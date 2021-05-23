//
// Unit Tests: XML
//
// Description: BufferDestination and FileDestination structures that implement
// the IDestination interfaces used in XML parsing.
//
// ================
// Test definitions
// =================
#include "XML_tests.hpp"
// =======================
// XML class namespace
// =======================
using namespace H4;
// ==========
// Test cases
// ==========
TEST_CASE("Creation and use of IDestination (Buffer) interface.", "[XML][Parse][BufferDestination]")
{
    SECTION("Create BufferDesination.", "[XML][Stringify][IDesination]")
    {
        REQUIRE_NOTHROW(BufferDestination());
    }
    SECTION("Create BufferDestination and get buffer which should be empty.", "[XML][Stringify][BufferDestination]")
    {
        BufferDestination buffer;
        REQUIRE_FALSE(!buffer.getBuffer().empty());
    }
    SECTION("Create BufferDestination and add one character.", "[XML][Stringify][BufferDestination]")
    {
        BufferDestination buffer;
        buffer.add("i");
        REQUIRE(buffer.getBuffer().size() == 1);
    }
    SECTION("Create BufferDestination and add an stringified integer and check result.", "[XML][Stringify][BufferDestination]")
    {
        BufferDestination buffer;
        buffer.add("65767");
        REQUIRE(buffer.getBuffer().size() == 5);
        REQUIRE(buffer.getBuffer() == ("65767"));
    }
}
TEST_CASE("Creation and use of IDestination (File) interface.", "[XML][Parse][FileDestination]")
{
    SECTION("Create FileDestination.", "[XML][Stringify][FileDestination]")
    {
        std::filesystem::remove(kGeneratedXMLFile);
        REQUIRE_NOTHROW(FileDestination(kGeneratedXMLFile));
    }
    SECTION("Create FileDestination when file already exists.", "[XML][Stringify][FileDestination]")
    {
        FileDestination file(kGeneratedXMLFile);
        REQUIRE_NOTHROW(FileDestination(kGeneratedXMLFile));
    }
    SECTION("Create FileDestination and test file exists and should be empty.", "[XML][Stringify][FileDestination]")
    {
        std::filesystem::remove(kGeneratedXMLFile);
        FileDestination file(kGeneratedXMLFile);
        REQUIRE_FALSE(!std::filesystem::exists(kGeneratedXMLFile));
        std::filesystem::path filePath(kGeneratedXMLFile);
        REQUIRE(std::filesystem::file_size(filePath) == 0);
    }
    SECTION("Create FileDestination and add one character.", "[XML][Stringify][FileDestination]")
    {
        std::filesystem::remove(kGeneratedXMLFile);
        FileDestination file(kGeneratedXMLFile);
        file.add("t");
        std::filesystem::path filePath(kGeneratedXMLFile);
        REQUIRE(std::filesystem::file_size(filePath) == 1);
    }
    SECTION("Create FileDestination, add an stringified integer and check result.", "[XML][Stringify][FileDestination]")
    {
        std::filesystem::remove(kGeneratedXMLFile);
        FileDestination file(kGeneratedXMLFile);
        file.add("65767");
        std::filesystem::path filePath(kGeneratedXMLFile);
        REQUIRE(std::filesystem::file_size(filePath) == 5);
        std::string expected = readXMLFromFileUTF8(kGeneratedXMLFile);
        REQUIRE(expected == "65767");
    }
}