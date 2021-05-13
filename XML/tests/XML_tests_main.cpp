//
// Unit Tests: XML
//
// Description: Helper functions for XML unit tests and also
// unit tests for BufferSource, FileSouce, BufferDestination and
// FileDestination structures that implement the ISource and IDestination
// interfaces used in XML parsing.
//
// ================
// Test definitions
// =================
#include "XML_tests.hpp"
// =======================
// XML class namespace
// =======================
using namespace H4;
// ==========================
// Unit test helper functions
// ==========================
/// <summary>
/// Open a XML file, read its contents (UTF8) into a string buffer
//  and return the buffer.
/// </summary>
/// <param name="xmlFileName">XML file name</param>
/// <returns>String containing UTF8 encoded XML.</returns>
std::string readXMLFromFileUTF8(const std::string &xmlFileName)
{
  std::ifstream xmlFile;
  xmlFile.open(xmlFileName);
  std::ostringstream xmlFileBuffer;
  xmlFileBuffer << xmlFile.rdbuf();
  return (xmlFileBuffer.str());
}
/// <summary>
/// Open a XML file, read its contents (UTF16BE) into a string buffer
//  and return the buffer.
/// </summary>
/// <param name="xmlFileName">XML file name</param>
/// <returnsString containign UTF16 encoded XML.</returns>
std::u16string readXMLFromFileUTF16(const std::string &xmlFileName)
{
  std::ifstream fin(xmlFileName, std::ios::binary);
  fin.seekg(0, std::ios::end);
  size_t size = (size_t)fin.tellg();
  fin.seekg(0, std::ios::beg);
  std::u16string u16;
  char ch[2];
  for (int index = 0; index < (int)size / 2; index++)
  {
    fin.read(ch, 2);
    u16 += ((char16_t)(ch[0]) << 8) | ch[1];
  }
  return (u16);
}
/// <summary>
/// Create an XML file and write XML to it in UTF8 encoding.
/// </summary>
/// <param name="xmlFileName">XML file name</param>
/// <param name="xmlFileName">XML</param>
/// <returns></returns>
void writeXMLToFileUTF8(const std::string &xmlFileName, const std::string &xmlString)
{
  std::remove(xmlFileName.c_str());
  std::ofstream xmlFile;
  xmlFile.open(xmlFileName);
  xmlFile << xmlString;
  xmlFile.close();
}
/// <summary>
/// Create an XML file and write XML to it in UTF16 encoding.
/// </summary>
/// <param name="xmlFileName">XML file name</param>
/// <param name="xmlFileName">XML</param>
/// <returns></returns>
void writeXMLToFileUTF16(const std::string &xmlFileName, const std::u16string &xmlString, bool be = true)
{
  std::remove(xmlFileName.c_str());
  std::ofstream xmlFile;
  char ch[2];
  xmlFile.open(xmlFileName);
  for (unsigned short u16 : xmlString)
  {
    if (be)
    {
      ch[0] = u16 >> 8;
      ch[1] = u16 & 0xFF;
    }
    else
    {
      ch[0] = u16 & 0xFF;
      ch[1] = u16 >> 8;
    }
    xmlFile.write(ch, 2);
  }
  xmlFile.close();
}
/// <summary>
/// Convert CRLF to LF for xmlSource and check number of CR/LF left after with
/// REQUIRE.
/// </summary>
/// <param name="xmlFileName">XML source</param>
/// <param name="crFinal">Final number of CR</param>
/// <param name="lfFinal">FInal number of LF</param>
/// <returns></returns>
void verifyCRLFCount(XML::ISource &xmlSource, long lfFinal, long crFinal)
{
  long crCount = 0;
  long lfCount = 0;
  while (xmlSource.more())
  {
    switch (xmlSource.current())
    {
    case kLineFeed:
      lfCount++;
      break;
    case kCarriageReturn:
      crCount++;
      break;
    }
    xmlSource.next();
  }
  REQUIRE(lfCount == lfFinal);
  REQUIRE(crCount == crFinal);
}
/// <summary>
/// Convert XML to an XMLObject, stringify the XMLObject and
/// compare the result with the passed in XML with REQUIRE.
/// </summary>
/// <param name="xml">XML pasrer object.</param>
/// <param name="xmlString">XML to parse and stringify.</param>
/// <returns></returns>
void checkStringify(const std::string &xmlString)
{
  BufferSource xmlSource(xmlString);
  XML xml(xmlSource);
  xml.parse();
  BufferDestination xmlDestination;
  xml.stringify(xmlDestination);
  REQUIRE(xmlDestination.getBuffer() == xmlString);
}
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
    REQUIRE(length == 8752);                                 // eof
    REQUIRE(xmlSource.current() == static_cast<XChar>(EOF)); // eof
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
    XString xmlResult;
    while (xmlSource.more())
    {
      xmlSource.ignoreWS();
      xmlResult += xmlSource.current();
      xmlSource.next();
    }
    REQUIRE(xmlResult == U"<root>TestTestTest</root>");
    REQUIRE(xmlSource.current() == static_cast<XChar>(EOF));
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
    REQUIRE(xmlSource.current() == static_cast<XChar>(EOF));
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
    REQUIRE(xmlSource.current() == static_cast<XChar>(EOF));
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
    REQUIRE(length == 8752);                                 // eof
    REQUIRE(xmlSource.current() == static_cast<XChar>(EOF)); // eof
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
    XString xmlResult;
    while (xmlSource.more())
    {
      xmlSource.ignoreWS();
      xmlResult += xmlSource.current();
      xmlSource.next();
    }
    REQUIRE(xmlResult == U"<root>TestTestTest</root>");
    REQUIRE(xmlSource.current() == static_cast<XChar>(EOF));
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
    REQUIRE(xmlSource.current() == static_cast<XChar>(EOF));
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
    REQUIRE(xmlSource.current() == static_cast<XChar>(EOF));
    xmlSource.backup(1);
    REQUIRE(xmlSource.current() == '>');
  }
  // SECTION("Check that BufferSource is reporting correct line/column number in an error.", "[XML][Parse][BufferSource]")
  // {
  //   xmlString = "\r\n<!DOCTYPE REPORT [\r\n"
  //               "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\r\n"
  //               "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
  //               "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
  //               "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\r\n"
  //               "<!ELEMENT TITLE %TEXT;>\r\n"
  //               "<!ELEMENT PARA %TEXT;>\r\n"
  //               "<!ELEMENT LIST (ITEM)+>\r\n"
  //               "<!ELEMENT ITEM (%BLOCK;)>\r\n"
  //               "<!ELEMENT CODE (#PCDATA)>\r\n"
  //               "<!ELEMENT KEYWORD (#PCDATA)>\r\n"
  //               "<!ELEMEN EXAMPLE (TITLE?,%BLOCK;)>\r\n"
  //               "<!ELEMENT GRAPHIC EMPTY>\r\n"
  //               "<!ATTLIST REPORT security (high | medium | low ) \"low\">\r\n"
  //               "<!ATTLIST CODE type CDATA #IMPLIED>\r\n"
  //               "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\r\n"
  //               "<!ENTITY xml \"Extensible Markup Language\">\r\n"
  //               "<!ENTITY sgml \"Standard Generalized Markup Language\">\r\n"
  //               "<!ENTITY pxa \"Professional XML Authoring\">\r\n"
  //               "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\r\n"
  //               "<!ENTITY % BLOCK \"(PARA|LIST)+\">\r\n"
  //               "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\r\n"
  //               "<!NOTATION GIF SYSTEM \"\">\r\n"
  //               "<!NOTATION JPG SYSTEM \"\">\r\n"
  //               "<!NOTATION BMP SYSTEM \"\">\r\n"
  //               "]>\r\n"
  //               "<REPORT> </REPORT>\r\n";
  //   BufferSource xmlSource(xmlString);
  //   XML xml;
  //   REQUIRE_THROWS_WITH(xml.parse(xmlSource), "XML Syntax Error [Line: 12 Column: 18] Invalid DTD tag.");
  // }
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
}
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