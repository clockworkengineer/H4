#ifndef XNODE_HPP
#define XNODE_HPP
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
    //
    // XNode structure.
    //
    enum XNodeType
    {
        base = 0,
        root = 1
    };
    //
    // Base XNode/
    //
    struct XNode
    {
    public:
        XNode(XNodeType nodeType = XNodeType::base) : nodeType(nodeType)
        {
        }
        const XNodeType nodeType;
    };

    //
    // Base XNode/
    //
    struct XNodeRoot : XNode
    {
    public:
        XNodeRoot(XNodeType nodeType = XNodeType::root) : XNode(nodeType){}
        std::string version;
        std::string encoding;
        std::string standalone;
    };

} // namespace H4
#endif /* XNODE_HPP */
