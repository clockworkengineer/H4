#ifndef XML_HPP
#define XML_HPP
//
// C++ STL
//
#include <string>
#include <stdexcept>
//
// XML XNodes
//
// =========
// NAMESPACE
// =========
namespace H4
{
    // ================
    // CLASS DEFINITION
    // ================
    class XML
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        //
        // XML syntax error.
        //
        struct SyntaxError : public std::exception
        {
        public:
            SyntaxError(const std::string &errorMessage="") : errorMessage(errorMessage) {}
            virtual const char *what() const throw()
            {
                return ("XML syntax error detected.");
            }
        private:
            std::string errorMessage;
        };
        //
        // Source interface
        //
        class ISource
        {
        public:
            virtual char currentByte() = 0;
            virtual void moveToNextByte() = 0;
            virtual bool bytesToParse() = 0;
        };
        //
        // Destination interface
        //
        class IDestination
        {
        public:
            virtual void addBytes(std::string bytes) = 0;
        };
        // ============
        // CONSTRUCTORS
        // ============
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
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
    };
} // namespace H4
#endif /* XML_HPP */
