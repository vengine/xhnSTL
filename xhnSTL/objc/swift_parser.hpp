/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef swift_parser_hpp
#define swift_parser_hpp

#ifdef __cplusplus
#include "common.h"
#include "etypes.h"
#include "rtti.hpp"
#include "xhn_string.hpp"
#include "xhn_static_string.hpp"
#include "xhn_vector.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_map.hpp"
#include "symbol_buffer.hpp"

namespace xhn {
    
#define OBJC_MASK  0x00000001
#define SWIFT_MASK 0x00000002
#define PARSE_MASK 0x00000002
    
class Parser
{
    DeclareRootRTTI;
public:
    virtual void BeginParse() = 0;
    virtual void Parse(const char* strBuffer, euint length) = 0;
};
    
class SwiftVerisonInfoParser : public MemObject, public Parser
{
    DeclareRTTI;
public:
    string m_versionInfo;
public:
    virtual void BeginParse() override;
    void EndParser(string& result);
    virtual void Parse(const char* strBuffer, euint length) override;
    static void GetSwiftVersion(const string& logDir,
                                Lambda<void (const xhn::string& versionInfo)>& callback);
};
    
class ASTReformatter : public RefObjectBase, public MemObject
{
public:
    virtual xhn::string Reformat(const char* strBuffer, euint length) = 0;
};
typedef xhn::SmartPtr<ASTReformatter> ASTReformatterPtr;

class XHN_EXPORT SwiftParser : public MemObject, public Parser
{
    DeclareRTTI;
public:
    enum BridgeFileLanguage
    {
        ObjC,
        Swift,
    };
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
    static const static_string StrTypealias;
    static const static_string StrType;
    static const static_string StrInterface;
    
    static const static_string StrSceneNodeAgent;
    static const static_string StrSceneNodeState;
    static const static_string StrSceneNodeStateInterface;
    
    static const static_string StrGUIAgent;
    static const static_string StrGUIState;
    static const static_string StrGUIStateInterface;
    static const static_string StrNormalState;
    static const static_string StrHoveringState;
    static const static_string StrSelectedState;
    static const static_string StrPressedState;
    static const static_string StrDraggingState;
    static const static_string StrNormalStateBase;
    static const static_string StrHoveringStateBase;
    static const static_string StrSelectedStateBase;
    static const static_string StrPressedStateBase;
    static const static_string StrDraggingStateBase;
    
    static const static_string StrActorAgent;
    static const static_string StrAction;
    static const static_string StrActionInterface;
    
    static const static_string StrGUIListAgent;
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
        static_string nodetype;
        static_string name;
        static_string access;
        static_string decl;
        static_string type;
        static_string interfacetype;
        SmartPtr<vector<static_string>> inherits;
        SmartPtr<vector<ASTNode*>> children;
        ASTNode* parent;
        ASTNode()
        : parent(nullptr)
        {}
        ASTNode(const static_string nodeName)
        : parent(nullptr)
        {}
    };
private:
    vector<ASTNode*> m_roots;
    vector<ASTNode*> m_nodes;
    vector<ASTNode*> m_classDeclNodes;
private:
    vector<ASTNode*> m_nodeStack;
    SymbolBuffer m_symbolBuffer;
	///< 单引号
    bool m_isApostropheBlock;
	///< 双引号
    bool m_isQuotationBlock;
	///< parentheses 小括号 
	///< brackets 中括号 
	///< braces 大括号
	bool m_isBracketBlock;
	bool m_isBraceBlock;
    bool m_isNodeType;
    bool m_isName;
    bool m_isInterface;
    bool m_isInherits;
    bool m_isAccess;
    bool m_isDecl;
    bool m_isType;
private:
    vector<static_string> m_sceneNodeAgentNameVector;
    vector<static_string> m_guiAgentNameVector;
    vector<static_string> m_guiListAgentNameVector;
    vector<static_string> m_actorAgentNameVector;
private:
    ASTReformatterPtr m_reformatter;
public:
    virtual void BeginParse() override;
    void EndParse(string& bridgeFile, string& stateActionFile, BridgeFileLanguage language = ObjC);
    string CreateBridgeFile(const map<static_string, vector<static_string>>& inheritMap,
                            const map<static_string, vector<static_string>>& childrenClassMap,
                            const map<static_string, ASTNode*>& classMap,
                            xhn::Lambda<bool (static_string,
                                              static_string, vector<static_string>&)>& isInheritFromClassProc,
                            BridgeFileLanguage language = ObjC);
    string CreateStateActionFile(const map<static_string, vector<static_string>>& inheritMap,
                                 const map<static_string, vector<static_string>>& childrenClassMap,
                                 const map<static_string, ASTNode*>& classMap,
                                 Lambda<bool (static_string, static_string,
                                              vector<static_string>&)>& isInheritFromClassProc);
    virtual void Parse(const char* strBuffer, euint length) override;
    static void ParseSwifts(const string& logDir,
                            ASTReformatterPtr reformatter,
                            const string& importPaths,
                            const string& paths, Lambda<void (const string& objcBridgeFile,
                                                              const string& swiftBridgeFile,
                                                              const string& stateActionFile,
                                                              const vector<static_string>&,
                                                              const vector<static_string>&,
                                                              const vector<static_string>&,
                                                              const vector<static_string>&)>& callback);
    SwiftParser(const string& logDir,
                ASTReformatterPtr reformatter);
    ~SwiftParser();
    inline const vector<static_string>& GetSceneNodeAgentNameVector() const { return m_sceneNodeAgentNameVector; }
    inline const vector<static_string>& GetGUIAgentNameVector() const { return m_guiAgentNameVector; }
    inline const vector<static_string>& GetGUIListAgentNameVector() const { return m_guiListAgentNameVector; }
    inline const vector<static_string>& GetActorAgentNameVector() const { return m_actorAgentNameVector; }
};
    
}
#endif
#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
