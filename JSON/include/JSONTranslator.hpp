#ifndef JSONTRANSLATOR_HPP
#define JSONTRANSLATOR_HPP
//
// C++ STL
//
#include <string>
#include <codecvt>
#include <locale>
#include <sstream>
// =========
// NAMESPACE
// =========
namespace H4
{
    // ================
    // CLASS DEFINITION
    // ================
    class JSONTranslator
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        // ============
        // CONSTRUCTORS
        // ============
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        std::string translateEscapesFromString(const std::string &jsonString);
        std::string translateEscapeToString(const std::string &utf8String);
        // ================
        // PUBLIC VARIABLES
        // ================
    private:
        // ===========================
        // PRIVATE TYPES AND CONSTANTS
        // ===========================
        // ===========================================
        // DISABLED CONSTRUCTORS/DESTRUCTORS/OPERATORS
        // ===========================================
        // ===============
        // PRIVATE METHODS
        // ===============
        // =================
        // PRIVATE VARIABLES
        // =================
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> m_utf8ToUnicode;
        std::ostringstream m_escapedString;
    };
} // namespace H4
#endif /* JSONTRANSLATOR_HPP */
