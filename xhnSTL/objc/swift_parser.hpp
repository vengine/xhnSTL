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
#include "xhn_map.hpp"
#include "symbol_buffer.hpp"

namespace xhn {

class SwiftParser : public MemObject
{
public:
    static const static_string StrSourceFile;
    static const static_string StrClassDecl;
    static const static_string StrImportDecl;
    static const static_string StrFuncDecl;
    static const static_string StrDeclRefExpr;
    static const static_string StrInherits;
    static const static_string StrAccess;
    static const static_string StrPrivate;
    static const static_string StrInternal;
    static const static_string StrPublic;
    static const static_string StrOpen;
    static const static_string StrDecl;
    
    static const static_string StrSceneNodeAgent;
    static const static_string StrState;
    static const static_string StrStateInterface;
    
    static const static_string StrGUIAgent;
    static const static_string StrGUIState;
    static const static_string StrGUIStateInterface;
    static const static_string StrNormalState;
    static const static_string StrHoveringState;
    static const static_string StrSelectedState;
    static const static_string StrPressedState;
    static const static_string StrDraggingState;
    
    static const static_string StrActorAgent;
    static const static_string StrAction;
    static const static_string StrActionInterface;
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
        static_string type;
        static_string name;
        static_string access;
        static_string decl;
        SmartPtr<vector<static_string>> inherits;
        SmartPtr<vector<ASTNode*>> children;
        ASTNode()
        {}
        ASTNode(const static_string nodeName)
        {}
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
    bool m_isAccess;
    bool m_isDecl;
private:
    vector<static_string> m_sceneNodeAgentNameVector;
    vector<static_string> m_guiAgentNameVector;
    vector<static_string> m_actorAgentNameVector;
public:
    void BeginParse();
    void EndParse(string& bridgeFile, string& stateActionFile);
    string CreateBridgeFile(const map<static_string, vector<static_string>>& inheritMap,
                            const map<static_string, vector<static_string>>& childrenClassMap,
                            const map<static_string, ASTNode*>& classMap,
                            xhn::Lambda<bool (static_string,
                                              static_string, vector<static_string>&)>& isInheritFromClassProc);
    string CreateStateActionFile(const map<static_string, vector<static_string>>& inheritMap,
                                 const map<static_string, vector<static_string>>& childrenClassMap,
                                 const map<static_string, ASTNode*>& classMap,
                                 Lambda<bool (static_string, static_string,
                                              vector<static_string>&)>& isInheritFromClassProc);
    void Parse(const char* strBuffer, euint length);
    static void ParseSwifts(const string& paths, Lambda<void (const xhn::string& bridgeFile,
                                                              const xhn::string& stateActionFile,
                                                              const vector<static_string>&,
                                                              const vector<static_string>&,
                                                              const vector<static_string>&)>& callback);
    SwiftParser();
    ~SwiftParser();
    inline const vector<static_string>& GetSceneNodeAgentNameVector() const { return m_sceneNodeAgentNameVector; }
    inline const vector<static_string>& GetGUIAgentNameVector() const { return m_guiAgentNameVector; }
    inline const vector<static_string>& GetActorAgentNameVector() const { return m_actorAgentNameVector; }
};
    
}
#endif
#endif /* swift_parser_hpp */
