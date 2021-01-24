#ifndef BENCODE_HPP
#define BENCODE_HPP
//
// C++ STL
//
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
//
// Bencode types
//
#include "BNode.hpp"
#include "Bencoding.hpp"
// =========
// NAMESPACE
// =========
namespace H4
{
    // ================
    // CLASS DEFINITION
    // ================
    class Bencode
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        //
        // Bencode syntax error.
        //
        struct SyntaxError : public std::exception
        {
        public:
            SyntaxError(const std::string &errorMessage = "") : errorMessage(errorMessage) {}
            virtual const char *what() const throw()
            {
                return ("Bencoding syntax error detected.");
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
            virtual std::byte currentByte() = 0;
            virtual void moveToNextByte() = 0;
            virtual bool bytesToDecode() = 0;
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
        std::unique_ptr<BNode> decodeBuffer(const Bencoding &source);
        std::unique_ptr<BNode> decodeFile(const std::string &fileName);
        Bencoding encodeBuffer(std::unique_ptr<BNode> bNodeRoot);
        void encodeFile(std::unique_ptr<BNode> bNodeRoot, const std::string &destinationFileName);
        std::unique_ptr<BNode> decode(ISource &source);
        void encode(std::unique_ptr<BNode> bNodeRoot, IDestination &destination);
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
        long extractPositiveInteger(ISource &source);
        std::string extractString(ISource &source);
        std::unique_ptr<BNode> decodeString(ISource &source);
        std::unique_ptr<BNode> decodeInteger(ISource &source);
        std::unique_ptr<BNode> decodeDictionary(ISource &source);
        std::unique_ptr<BNode> decodeList(ISource &source);
        std::unique_ptr<BNode> decodeBNodes(ISource &source);
        void encodeBNodes(BNode *bNode, IDestination &destination);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::string m_workBuffer;
    };
} // namespace H4
#endif /* BENCODE_HPP */
