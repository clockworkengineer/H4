//
// Unit Tests: XML
//
// Description: Unit tests for BufferSource, FileSouce structures that implement
// the ISource and IDestination interfaces used in XML parsing.
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
TEST_CASE("Creation and use of ISource (File) interface.", "[XML][Parse][FileSource]")
{
    SECTION("Create FileSource with testfile001.xml.", "[XML][Parse][FileSource]")
    {
        REQUIRE_NOTHROW(FileSource(kGeneratedXMLFile));
    }
    SECTION("Create FileSource with non existants file.", "[XML][Parse][FileSource][Exception]")
    {
        REQUIRE_THROWS_AS(FileSource(kNonExistantXMLFile), std::runtime_error);
        REQUIRE_THROWS_WITH(FileSource(kNonExistantXMLFile), "XML file input stream failed to open or does not exist.");
    }
    SECTION("Create FileSource with testfile001.xml. and positioned on the correct first character", "[XML][Parse][FileSource]")
    {
        FileSource xmlSource(kSingleXMLFile);
        REQUIRE_FALSE(!xmlSource.more());
        REQUIRE(xmlSource.current() == '<');
    }
    SECTION("Create FileSource with testfile001.xml and then check move positions to correct next character", "[XML][Parse][FileSource]")
    {
        FileSource xmlSource(kSingleXMLFile);
        xmlSource.next();
        REQUIRE_FALSE(!xmlSource.more());
        REQUIRE(xmlSource.current() == '?');
    }
    SECTION("Create FileSource with testfile001.xml  move past last character, check it and the bytes moved.", "[XML][Parse][FileSource]")
    {
        FileSource xmlSource(kSingleXMLFile);
        long length = 0;
        while (xmlSource.more())
        {
            xmlSource.next();
            length++;
        }
        REQUIRE(length == 8752);                                   // eof
        REQUIRE(xmlSource.current() == static_cast<ISource::Char>(EOF)); // eof
    }
    std::string xmlString;
    SECTION("Check that FileSource is  performing CRLF to LF conversion on windows format data correctly.", "[XML][Parse][FileSource]")
    {
        xmlString = "\r\r\n<!DOCTYPE REPORT [\r\n"
                    "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\r\n"
                    "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
                    "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
                    "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\r\n"
                    "<!ELEMENT TITLE %TEXT;>\r\n"
                    "<!ELEMENT PARA %TEXT;>\r\n"
                    "<!ELEMENT LIST (ITEM)+>\r\n"
                    "<!ELEMENT ITEM (%BLOCK;)>\r\n"
                    "<!ELEMENT CODE (#PCDATA)>\r\n"
                    "<!ELEMENT KEYWORD (#PCDATA)>\r\n"
                    "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\r\n"
                    "<!ELEMENT GRAPHIC EMPTY>\r\n"
                    "<!ATTLIST REPORT security (high | medium | low ) \"low\">\r\n"
                    "<!ATTLIST CODE type CDATA #IMPLIED>\r\n"
                    "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\r\n"
                    "<!ENTITY xml \"Extensible Markup Language\">\r\n"
                    "<!ENTITY sgml \"Standard Generalized Markup Language\">\r\n"
                    "<!ENTITY pxa \"Professional XML Authoring\">\r\n"
                    "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\r\n"
                    "<!ENTITY % BLOCK \"(PARA|LIST)+\">\r\n"
                    "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\r\n"
                    "<!NOTATION GIF SYSTEM \"\">\r\n"
                    "<!NOTATION JPG SYSTEM \"\">\r\n"
                    "<!NOTATION BMP SYSTEM \"\">\r\n"
                    "]>\r\n"
                    "<REPORT>\r\r </REPORT>\r\n";
        writeXMLToFileUTF8(kGeneratedXMLFile, xmlString);
        FileSource xmlSource(kGeneratedXMLFile);
        verifyCRLFCount(xmlSource, 28, 3);
    }
    SECTION("Check that FileSource is  performing CRLF to LF conversion on linux format data correctly.", "[XML][Parse][FileSource]")
    {
        xmlString = "\r \n<!DOCTYPE REPORT [\n"
                    "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\n"
                    "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\n"
                    "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\n"
                    "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\n"
                    "<!ELEMENT TITLE %TEXT;>\n"
                    "<!ELEMENT PARA %TEXT;>\n"
                    "<!ELEMENT LIST (ITEM)+>\n"
                    "<!ELEMENT ITEM (%BLOCK;)>\n"
                    "<!ELEMENT CODE (#PCDATA)>\n"
                    "<!ELEMENT KEYWORD (#PCDATA)>\n"
                    "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\n"
                    "<!ELEMENT GRAPHIC EMPTY>\n"
                    "<!ATTLIST REPORT security (high | medium | low ) \"low\">\n"
                    "<!ATTLIST CODE type CDATA #IMPLIED>\n"
                    "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\n"
                    "<!ENTITY xml \"Extensible Markup Language\">\n"
                    "<!ENTITY sgml \"Standard Generalized Markup Language\">\n"
                    "<!ENTITY pxa \"Professional XML Authoring\">\n"
                    "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\n"
                    "<!ENTITY % BLOCK \"(PARA|LIST)+\">\n"
                    "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\n"
                    "<!NOTATION GIF SYSTEM \"\">\n"
                    "<!NOTATION JPG SYSTEM \"\">\n"
                    "<!NOTATION BMP SYSTEM \"\">\n"
                    "]>\n"
                    "<REPORT>\r\r </REPORT>\n";
        writeXMLToFileUTF8(kGeneratedXMLFile, xmlString);
        FileSource xmlSource(kGeneratedXMLFile);
        verifyCRLFCount(xmlSource, 28, 3);
    }
    SECTION("Check that FileSource is ignoring whitespace corectly.", "[XML][Parse][FileSource]")
    {
        xmlString = "<root>   Test\t\t\t\r\r\r\r\r\r\r\f\n       Test       Test   \r\r\r\r</root>";
        writeXMLToFileUTF8(kGeneratedXMLFile, xmlString);
        FileSource xmlSource(kGeneratedXMLFile);
        ISource::String xmlResult;
        while (xmlSource.more())
        {
            xmlSource.ignoreWS();
            xmlResult += xmlSource.current();
            xmlSource.next();
        }
        REQUIRE(xmlResult == U"<root>TestTestTest</root>");
        REQUIRE(xmlSource.current() == static_cast<ISource::Char>(EOF));
    }
    SECTION("Check that FileSource ignoreWS() at end of file does not throw but next() does.", "[XML][Parse][FileSource]")
    {
        xmlString = "<root>Test Test Test Test</root>";
        writeXMLToFileUTF8(kGeneratedXMLFile, xmlString);
        FileSource xmlSource(kGeneratedXMLFile);
        while (xmlSource.more())
        {
            xmlSource.next();
        }
        REQUIRE_NOTHROW(xmlSource.ignoreWS());
        REQUIRE_THROWS_AS(xmlSource.next(), std::runtime_error);
        REQUIRE_THROWS_WITH(xmlSource.next(), "Parse buffer empty before parse complete.");
    }
    SECTION("Check that FileSource match works correctly when match found or not.", "[XML][Parse][FileSource]")
    {
        xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHH4 &</root>";
        writeXMLToFileUTF8(kGeneratedXMLFile, xmlString);
        FileSource xmlSource(kGeneratedXMLFile);
        REQUIRE_FALSE(xmlSource.match(U"<root> "));
        REQUIRE_FALSE(!xmlSource.match(U"<root>"));
        REQUIRE(xmlSource.current() == 'M');
        REQUIRE_FALSE(!xmlSource.match(U"Match1"));
        REQUIRE(xmlSource.current() == ' ');
        xmlSource.ignoreWS();
        REQUIRE(xmlSource.current() == 'M');
        REQUIRE_FALSE(xmlSource.match(U"Match3"));
        REQUIRE_FALSE(!xmlSource.match(U"Match2"));
        xmlSource.ignoreWS();
        REQUIRE(xmlSource.current() == '2');
        REQUIRE_FALSE(!xmlSource.match(U"2hctam"));
        REQUIRE(xmlSource.current() == ' ');
        xmlSource.ignoreWS();
        REQUIRE_FALSE(!xmlSource.match(U"MMAATTCCHHHH4"));
        REQUIRE(xmlSource.current() == ' ');
        xmlSource.next();
        REQUIRE(xmlSource.current() == '&');
        xmlSource.next();
        REQUIRE_FALSE(!xmlSource.match(U"</root>"));
        REQUIRE(xmlSource.current() == static_cast<ISource::Char>(EOF));
        REQUIRE_THROWS_WITH(xmlSource.next(), "Parse buffer empty before parse complete.");
    }
    SECTION("Check that FileSource backup works and doesnt go negative.", "[XML][Parse][FileSource]")
    {
        xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHH4 &</root>";
        writeXMLToFileUTF8(kGeneratedXMLFile, xmlString);
        FileSource xmlSource(kGeneratedXMLFile);
        xmlSource.match(U"<root>Match1");
        REQUIRE(xmlSource.current() == ' ');
        xmlSource.backup(12);
        REQUIRE(xmlSource.current() == '<');
        xmlSource.next();
        REQUIRE(xmlSource.current() == 'r');
        xmlSource.backup(12);
        REQUIRE(xmlSource.current() == '<');
        while (xmlSource.more())
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.current() == static_cast<ISource::Char>(EOF));
        xmlSource.backup(1);
        REQUIRE(xmlSource.current() == '>');
    }
}
TEST_CASE("Creation and use of ISource (Buffer) interface (buffer contains file testfile001.xml).", "[XML][Parse][BufferSource]")
{
    std::string buffer = readXMLFromFileUTF8(kSingleXMLFile);
    SECTION("Create BufferSource.", "[XML][Parse][BufferSource]")
    {
        REQUIRE_NOTHROW(BufferSource(buffer));
    }
    SECTION("Create BufferSource with empty buffer.", "[XML][Parse][BufferSource][Exception]")
    {
        REQUIRE_THROWS_AS(BufferSource(""), std::invalid_argument);
        REQUIRE_THROWS_WITH(BufferSource(""), "Empty source buffer passed to be parsed.");
    }
    SECTION("Create BufferSource with testfile001.xml and that it is positioned on the correct first character", "[XML][Parse][BufferSource]")
    {
        BufferSource xmlSource(buffer);
        REQUIRE_FALSE(!xmlSource.more());
        REQUIRE((char)xmlSource.current() == '<');
    }
    SECTION("Create BufferSource with testfile001.xml and then check more positions to correct next character", "[XML][Parse][BufferSource]")
    {
        BufferSource xmlSource(buffer);
        xmlSource.next();
        REQUIRE_FALSE(!xmlSource.more());
        REQUIRE((char)xmlSource.current() == '?');
    }
    SECTION("Create BufferSource with testfile001.xml move past last character, check it and the bytes moved.", "[XML][Parse][BufferSource]")
    {
        BufferSource xmlSource(buffer);
        long length = 0;
        while (xmlSource.more())
        {
            xmlSource.next();
            length++;
        }
        REQUIRE(length == 8752);                                   // eof
        REQUIRE(xmlSource.current() == static_cast<ISource::Char>(EOF)); // eof
    }
    std::string xmlString;
    SECTION("Check that BufferSource is  performing CRLF to LF conversion on windows format data correctly.", "[XML][Parse][BufferSource]")
    {
        xmlString = "\r\r\n<!DOCTYPE REPORT [\r\n"
                    "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\r\n"
                    "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
                    "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
                    "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\r\n"
                    "<!ELEMENT TITLE %TEXT;>\r\n"
                    "<!ELEMENT PARA %TEXT;>\r\n"
                    "<!ELEMENT LIST (ITEM)+>\r\n"
                    "<!ELEMENT ITEM (%BLOCK;)>\r\n"
                    "<!ELEMENT CODE (#PCDATA)>\r\n"
                    "<!ELEMENT KEYWORD (#PCDATA)>\r\n"
                    "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\r\n"
                    "<!ELEMENT GRAPHIC EMPTY>\r\n"
                    "<!ATTLIST REPORT security (high | medium | low ) \"low\">\r\n"
                    "<!ATTLIST CODE type CDATA #IMPLIED>\r\n"
                    "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\r\n"
                    "<!ENTITY xml \"Extensible Markup Language\">\r\n"
                    "<!ENTITY sgml \"Standard Generalized Markup Language\">\r\n"
                    "<!ENTITY pxa \"Professional XML Authoring\">\r\n"
                    "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\r\n"
                    "<!ENTITY % BLOCK \"(PARA|LIST)+\">\r\n"
                    "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\r\n"
                    "<!NOTATION GIF SYSTEM \"\">\r\n"
                    "<!NOTATION JPG SYSTEM \"\">\r\n"
                    "<!NOTATION BMP SYSTEM \"\">\r\n"
                    "]>\r\n"
                    "<REPORT>\r\r </REPORT>\r\n";
        BufferSource xmlSource(xmlString);
        verifyCRLFCount(xmlSource, 28, 3);
    }
    SECTION("Check that BufferSource is  performing CRLF to LF conversion on linux format data correctly.", "[XML][Parse][BufferSource]")
    {
        xmlString = "\r \n<!DOCTYPE REPORT [\n"
                    "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\n"
                    "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\n"
                    "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\n"
                    "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\n"
                    "<!ELEMENT TITLE %TEXT;>\n"
                    "<!ELEMENT PARA %TEXT;>\n"
                    "<!ELEMENT LIST (ITEM)+>\n"
                    "<!ELEMENT ITEM (%BLOCK;)>\n"
                    "<!ELEMENT CODE (#PCDATA)>\n"
                    "<!ELEMENT KEYWORD (#PCDATA)>\n"
                    "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\n"
                    "<!ELEMENT GRAPHIC EMPTY>\n"
                    "<!ATTLIST REPORT security (high | medium | low ) \"low\">\n"
                    "<!ATTLIST CODE type CDATA #IMPLIED>\n"
                    "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\n"
                    "<!ENTITY xml \"Extensible Markup Language\">\n"
                    "<!ENTITY sgml \"Standard Generalized Markup Language\">\n"
                    "<!ENTITY pxa \"Professional XML Authoring\">\n"
                    "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\n"
                    "<!ENTITY % BLOCK \"(PARA|LIST)+\">\n"
                    "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\n"
                    "<!NOTATION GIF SYSTEM \"\">\n"
                    "<!NOTATION JPG SYSTEM \"\">\n"
                    "<!NOTATION BMP SYSTEM \"\">\n"
                    "]>\n"
                    "<REPORT>\r\r </REPORT>\n";
        BufferSource xmlSource(xmlString);
        verifyCRLFCount(xmlSource, 28, 3);
    }
    SECTION("Check that BufferSource is ignoring whitespace corectly.", "[XML][Parse][BufferSource]")
    {
        xmlString = "<root>   Test\t\t\t\r\r\r\r\r\r\r\f\n       Test       Test   \r\r\r\r</root>";
        BufferSource xmlSource(xmlString);
        ISource::String xmlResult;
        while (xmlSource.more())
        {
            xmlSource.ignoreWS();
            xmlResult += xmlSource.current();
            xmlSource.next();
        }
        REQUIRE(xmlResult == U"<root>TestTestTest</root>");
        REQUIRE(xmlSource.current() == static_cast<ISource::Char>(EOF));
    }
    SECTION("Check that BufefrSource ignoreWS() at end of file does not throw but next() does.", "[XML][Parse][BufferSource]")
    {
        xmlString = "<root>Test Test Test Test</root>";
        BufferSource xmlSource(xmlString);
        while (xmlSource.more())
        {
            xmlSource.next();
        }
        REQUIRE_NOTHROW(xmlSource.ignoreWS());
        REQUIRE_THROWS_AS(xmlSource.next(), std::runtime_error);
        REQUIRE_THROWS_WITH(xmlSource.next(), "Parse buffer empty before parse complete.");
    }
    SECTION("Check that BufferSource match works correctly when match found or not.", "[XML][Parse][BufferSource]")
    {
        xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHH4 &</root>";
        BufferSource xmlSource(xmlString);
        REQUIRE_FALSE(xmlSource.match(U"<root> "));
        REQUIRE_FALSE(!xmlSource.match(U"<root>"));
        REQUIRE(xmlSource.current() == 'M');
        REQUIRE_FALSE(!xmlSource.match(U"Match1"));
        REQUIRE(xmlSource.current() == ' ');
        xmlSource.ignoreWS();
        REQUIRE(xmlSource.current() == 'M');
        REQUIRE_FALSE(xmlSource.match(U"Match3"));
        REQUIRE_FALSE(!xmlSource.match(U"Match2"));
        xmlSource.ignoreWS();
        REQUIRE(xmlSource.current() == '2');
        REQUIRE_FALSE(!xmlSource.match(U"2hctam"));
        REQUIRE(xmlSource.current() == ' ');
        xmlSource.ignoreWS();
        REQUIRE_FALSE(!xmlSource.match(U"MMAATTCCHHHH4"));
        REQUIRE(xmlSource.current() == ' ');
        xmlSource.next();
        REQUIRE(xmlSource.current() == '&');
        xmlSource.next();
        REQUIRE_FALSE(!xmlSource.match(U"</root>"));
        REQUIRE(xmlSource.current() == static_cast<ISource::Char>(EOF));
        REQUIRE_THROWS_WITH(xmlSource.next(), "Parse buffer empty before parse complete.");
    }
    SECTION("Check that BufferSource backup works and doesnt go negative.", "[XML][Parse][BufferSource]")
    {
        xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHH4 &</root>";
        BufferSource xmlSource(xmlString);
        xmlSource.match(U"<root>Match1");
        REQUIRE(xmlSource.current() == ' ');
        xmlSource.backup(12);
        REQUIRE(xmlSource.current() == '<');
        xmlSource.backup(12);
        REQUIRE(xmlSource.current() == '<');
        while (xmlSource.more())
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.current() == static_cast<ISource::Char>(EOF));
        xmlSource.backup(1);
        REQUIRE(xmlSource.current() == '>');
    }
    SECTION("Check that BufferSource is reporting correct line/column number in an error.", "[XML][Parse][BufferSource]")
    {
        xmlString = "<?xml version=\"1.0\"?>\n"
                    "<!DOCTYPE queue ["
                    "<!ELEMENT queue (person)+>\n"
                    "<!ELEMENT person (firstName, lastName, nationality)>\n"
                    "<!ELEMENT firstName (#PCDATA)>\n"
                    "<!ELEMENT lastName (#PCDATA)>\n"
                    "<!ELEMENT nationality (#PCDATA)\n"
                    "<!ATTLIST person gender ( M | F | F) \"M\">\n"
                    "]>\n"
                    "<queue>\n"
                    "<person gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></person>\n"
                    "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
                    "</queue>\n";
        BufferSource xmlSource(xmlString);
        XML xml(xmlSource);
        REQUIRE_THROWS_WITH(xml.parse(), "DTD Syntax Error [Line: 7 Column: 2] Missing '>' terminator.");
    }
    SECTION("Check that BufferSource position() and getRange works correctly.", "[XML][Parse][BufferSource]")
    {
        xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHH4 &</root>";
        BufferSource xmlSource(xmlString);
        while (xmlSource.more() && !xmlSource.match(U"Match"))
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.position() == 11);
        long start = xmlSource.position();
        while (xmlSource.more() && !xmlSource.match(U"Match2"))
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.position() == 22);
        REQUIRE(xmlSource.getRange(start, xmlSource.position()) == "1    Match2");
        REQUIRE(xmlSource.position() == 22);
    }
    SECTION("Check that FileSource position() and getRange works correctly.", "[XML][Parse][FileSource]")
    {
        xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHH4 &</root>";
        writeXMLToFileUTF8(kGeneratedXMLFile, xmlString);
        FileSource xmlSource(kGeneratedXMLFile);
        while (xmlSource.more() && !xmlSource.match(U"Match"))
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.position() == 11);
        long start = xmlSource.position();
        while (xmlSource.more() && !xmlSource.match(U"Match2"))
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.position() == 22);
        REQUIRE(xmlSource.getRange(start, xmlSource.position()) == "1    Match2");
        REQUIRE(xmlSource.position() == 22);
    }

    SECTION("Check that BufferSource reset() works correctly.", "[XML][Parse][BufferSource]")
    {
        xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHH4 &</root>";
        BufferSource xmlSource(xmlString);
        while (xmlSource.more() && !xmlSource.match(U"Match"))
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.position() == 11);
        xmlSource.reset();
        REQUIRE(xmlSource.position() == 0);
        while (xmlSource.more() && !xmlSource.match(U"Match"))
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.position() == 11);
    }
    SECTION("Check that FileSource reset() works correctly.", "[XML][Parse][FileSource]")
    {
        xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHH4 &</root>";
        writeXMLToFileUTF8(kGeneratedXMLFile, xmlString);
        FileSource xmlSource(kGeneratedXMLFile);
        while (xmlSource.more() && !xmlSource.match(U"Match"))
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.position() == 11);
        xmlSource.reset();
        REQUIRE(xmlSource.position() == 0);
        while (xmlSource.more() && !xmlSource.match(U"Match"))
        {
            xmlSource.next();
        }
        REQUIRE(xmlSource.position() == 11);
    }
}
