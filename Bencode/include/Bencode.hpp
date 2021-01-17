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
        std::unique_ptr<BNode> decodeFile(std::string fileName);
        Bencoding encodeBuffer(std::unique_ptr<BNode> bNodeRoot);
        void encodeFile(std::unique_ptr<BNode> bNodeRoot, std::string destinationFileName);
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
        long decodePositiveInteger(ISource *source);
        std::string decodeString(ISource *source);
        std::unique_ptr<BNode> decodeBNodes(ISource *source);
        void encodeBNodes(BNode *bNode, IDestination *destination);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::string m_workBuffer;
    };
    //
    // Shortcuts for BNode structure and Bencoding type
    //
//using Bencoding = Bencode::Bencoding;
} // namespace H4
#endif /* BENCODE_HPP */
