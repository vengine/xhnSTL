//
//  swift_parser.hpp
//  VEngine
//
//  Created by 徐海宁 on 16/4/9.
//  Copyright © 2016年 徐海宁. All rights reserved.
//

#ifndef swift_parser_hpp
#define swift_parser_hpp

#ifdef __cplusplus
#include "common.h"
#include "etypes.h"
#include "xhn_string.hpp"
#include "xhn_static_string.hpp"
#include "xhn_vector.hpp"
#include "xhn_smart_ptr.hpp"

namespace xhn {

class SwiftParser : public MemObject
{
public:
    static const xhn::static_string StrSourceFile;
    static const xhn::static_string StrClassDecl;
    static const xhn::static_string StrImportDecl;
    static const xhn::static_string StrFuncDecl;
    static const xhn::static_string StrInherits;
    
    static const xhn::static_string StrSceneNodeAgent;
    enum ASTNodeType
    {
        SourceFileNode,
        ClassDeclNode,
        ImportDeclNode,
        FuncDeclNode,
        UnknownNode,
    };
    struct ASTNode : public MemObject
    {
        xhn::static_string type;
        xhn::static_string name;
        xhn::SmartPtr<xhn::vector<xhn::static_string>> inherits;
        xhn::vector<ASTNode*> children;
        ASTNode()
        {}
        ASTNode(const xhn::static_string nodeName)
        {}
    };
    class SymbolBuffer
    {
    public:
        char buffer[1024];
        int bufferTop;
    public:
        SymbolBuffer();
        void AddCharacter(char c);
        bool IsInteger();
        bool IsIdentifier();
        bool IsFunctionResult();
        static_string GetSymbol();
    };
private:
    vector<ASTNode*> m_roots;
    vector<ASTNode*> m_nodes;
    vector<ASTNode*> m_classDeclNodes;
private:
    vector<ASTNode*> m_nodeStack;
    SymbolBuffer m_symbolBuffer;
    bool m_isApostropheBlock;
    bool m_isQuotationBlock;
    bool m_isNodeType;
    bool m_isName;
    bool m_isInherits;
public:
    void BeginParse();
    string EndParse();
    string CreateBridgeFile();
    void Parse(const char* strBuffer, euint length);
    static string Parse(const xhn::string& paths);
    SwiftParser();
    ~SwiftParser();
};
    
}
#endif
#endif /* swift_parser_hpp */
