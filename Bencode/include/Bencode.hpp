#ifndef BENCODE_HPP
#define BENCODE_HPP

//
// C++ STL
//

#include <string>
#include <map>
#include <vector>
#include <list>
#include <memory>

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

        /// <summary>
        /// Base for BNode structure.
        /// </summary>
        struct BNode
        {
            virtual ~BNode() = default;
        };
        /// <summary>
        /// Dictionary BNode.
        /// </summary>
        struct BNodeDict : BNode
        {
            std::map<std::string, std::unique_ptr<BNode>> dict;
        };
        /// <summary>
        /// List BNode.
        /// </summary>
        struct BNodeList : BNode
        {
            std::list<std::unique_ptr<BNode>> list;
        };
        /// <summary>
        /// Number BNode.
        /// </summary>
        struct BNodeNumber : BNode
        {
            long number;
            BNodeNumber(long number)
            {
                this->number = number;
            }
        };
        /// <summary>
        /// String BNode.
        /// </summary>
        struct BNodeString : BNode
        {
        public:
            std::string string;
            BNodeString(std::string string)
            {
                this->string = string;
            }
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

        std::unique_ptr<Bencode::BNode> decode(const char *toDecode);
        std::string encode(std::unique_ptr<Bencode::BNode> bNodeRoot);

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

        long decodeNumber();
        std::string decodeString();
        std::unique_ptr<Bencode::BNode> decodeToBNodes();

        // =================
        // PRIVATE VARIABLES
        // =================

        std::string m_workBuffer;
        const char *m_decodeBuffer;
    };

    using BNode = Bencode::BNode;
    using BNodeNumber = Bencode::BNodeNumber;
    using BNodeString = Bencode::BNodeString;
    using BNodeList = Bencode::BNodeList;
    using BNodeDict = Bencode::BNodeDict;

} // namespace H4
#endif /* BENCODE_HPP */
