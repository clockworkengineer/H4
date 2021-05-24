//
// Unit Tests: XML
//
// Description: Helper functions for XML unit tests.
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
void verifyCRLFCount(ISource &xmlSource, long lfFinal, long crFinal)
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