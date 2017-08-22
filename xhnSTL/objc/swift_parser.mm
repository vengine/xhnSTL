//
//  swift_parser.cpp
//  VEngine
//
//  Created by 徐海宁 on 16/4/9.
//  Copyright © 2016年 徐海宁. All rights reserved.
//

#include "swift_parser.hpp"
#include "utility.h"
#include "xhn_lock.hpp"
#import <Foundation/Foundation.h>

#define USING_AST_LOG 1

#if USING_AST_LOG

#define AST_BUFFER_SIZE (1024 * 512)
#define COMMAND_BUFFER_SIZE (1024)
#define ASTLog(fmt,...) { \
int size = \
snprintf(s_ASTBuffer,AST_BUFFER_SIZE - 2,fmt,##__VA_ARGS__); \
fwrite(s_ASTBuffer, 1, size, s_ASTLogFile); }

#define COMMANDLog(fmt,...) { \
int size = \
snprintf(s_COMMANDBuffer,COMMAND_BUFFER_SIZE - 2,fmt,##__VA_ARGS__); \
fwrite(s_COMMANDBuffer, 1, size, s_COMMANDFile); }

static char s_ASTBuffer[AST_BUFFER_SIZE];
static FILE* s_ASTLogFile = nullptr;

static char s_COMMANDBuffer[AST_BUFFER_SIZE];
static FILE* s_COMMANDFile = nullptr;

static FILE* s_ASTFile = nullptr;

#else
#define ASTLog(fmt,...)
#define COMMANDLog(fmt,...)
#endif

static xhn::SpinLock s_SwiftCommandLineUtilsLock;
static NSMutableSet* s_SwiftCommandLineUtils = nil;

@interface SwiftCommandLineUtil : NSObject
@property (assign) xhn::SwiftParser* parser;
- (void) runCommand:(NSString*)commandToRun callback:(void(^)(const xhn::string& bridgeFile,
                                                              const xhn::string& stateActionFile,
                                                              const xhn::vector<xhn::static_string>&,
                                                              const xhn::vector<xhn::static_string>&,
                                                              const xhn::vector<xhn::static_string>&))proc;
@end

namespace xhn {
    
    const static_string SwiftParser::StrSourceFile("source_file");
    const static_string SwiftParser::StrClassDecl("class_decl");
    const static_string SwiftParser::StrImportDecl("import_decl");
    const static_string SwiftParser::StrFuncDecl("func_decl");
    const static_string SwiftParser::StrDeclRefExpr("declref_expr");
    const static_string SwiftParser::StrInherits("inherits:");
    const static_string SwiftParser::StrAccess("access");
    const static_string SwiftParser::StrPrivate("private");
    const static_string SwiftParser::StrInternal("internal");
    const static_string SwiftParser::StrPublic("public");
    const static_string SwiftParser::StrOpen("open");
    const static_string SwiftParser::StrDecl("decl");
    const static_string SwiftParser::StrTypealias("typealias");
    const static_string SwiftParser::StrType("type");
    const static_string SwiftParser::StrInterface("interface");
    
    const static_string SwiftParser::StrSceneNodeAgent("SceneNodeAgent");
    const static_string SwiftParser::StrState("State");
    const static_string SwiftParser::StrStateInterface("StateInterface");
    
    const static_string SwiftParser::StrGUIAgent("GUIAgent");
    const static_string SwiftParser::StrGUIState("GUIState");
    const static_string SwiftParser::StrGUIStateInterface("GUIStateInterface");
    const static_string SwiftParser::StrNormalState("NormalState");
    const static_string SwiftParser::StrHoveringState("HoveringState");
    const static_string SwiftParser::StrSelectedState("SelectedState");
    const static_string SwiftParser::StrPressedState("PressedState");
    const static_string SwiftParser::StrDraggingState("DraggingState");
    const static_string SwiftParser::StrNormalStateBase("NormalStateBase");
    const static_string SwiftParser::StrHoveringStateBase("HoveringStateBase");
    const static_string SwiftParser::StrSelectedStateBase("SelectedStateBase");
    const static_string SwiftParser::StrPressedStateBase("PressedStateBase");
    const static_string SwiftParser::StrDraggingStateBase("DraggingStateBase");
    
    const static_string SwiftParser::StrActorAgent("ActorAgent");
    const static_string SwiftParser::StrAction("Action");
    const static_string SwiftParser::StrActionInterface("ActionInterface");
    
    void SwiftParser::BeginParse()
    {
        ///
    }
    void SwiftParser::EndParse(string& bridgeFile, string& stateActionFile)
    {
        map<static_string, static_string> aliasMap;
        map<static_string, vector<static_string>> inheritMap;
        map<static_string, vector<static_string>> childrenClassMap;
        map<static_string, ASTNode*> classMap;
        
        Lambda<void (ASTNode*)> makeAliasMap;
        Lambda<void (const string&, ASTNode*)> makeInheritMapProc;
        Lambda<void (const string&, ASTNode*)> makeChildrenClassMapProc;
        Lambda<void (const string&, ASTNode*)> makeClassMapProc;
        Lambda<bool (static_string, static_string, vector<static_string>&)> isInheritFromClassProc;
        
        const string dotType(".Type");
        makeAliasMap = [&makeAliasMap, &aliasMap, &dotType](ASTNode* node) -> void {
            if (StrTypealias == node->nodetype) {
                xhn::string strIntType = node->interfacetype.c_str();
                euint pos = strIntType.rfind(dotType);
                if (xhn::string::npos != pos &&
                    pos + dotType.size() == strIntType.size()) {
                    strIntType = strIntType.substr(0, pos);
                    aliasMap[strIntType.c_str()] = node->type;
                    ASTLog("++:%s -> %s\n", strIntType.c_str(), node->type.c_str());
                }
            }
            if (node->children) {
                auto childIter = node->children->begin();
                auto childEnd = node->children->end();
                for (; childIter != childEnd; childIter++) {
                    ASTNode* child = *childIter;
                    makeAliasMap(child);
                }
            }
        };
        
        makeInheritMapProc = [&makeInheritMapProc, &inheritMap](const string& parentPath, ASTNode* node) -> void {
            if (StrClassDecl == node->nodetype) {
                string nodePath = parentPath;
                if (parentPath.size()) {
                    nodePath += ".";
                }
                nodePath += node->name.c_str();
                if (node->inherits) {
                    auto inhIter = node->inherits->begin();
                    auto inhEnd = node->inherits->end();
                    for (; inhIter != inhEnd; inhIter++) {
                        inheritMap[nodePath.c_str()].push_back(*inhIter);
                    }
                }
                if (node->children) {
                    auto childIter = node->children->begin();
                    auto childEnd = node->children->end();
                    for (; childIter != childEnd; childIter++) {
                        ASTNode* child = *childIter;
                        
                        makeInheritMapProc(nodePath, child);
                    }
                }
            }
            else {
                if (node->children) {
                    auto childIter = node->children->begin();
                    auto childEnd = node->children->end();
                    for (; childIter != childEnd; childIter++) {
                        ASTNode* child = *childIter;
                        makeInheritMapProc(parentPath, child);
                    }
                }
            }
        };
        
        makeChildrenClassMapProc = [&makeChildrenClassMapProc, &childrenClassMap](const string& parentPath, ASTNode* node) -> void {
            if (StrClassDecl == node->nodetype) {
                string nodePath = parentPath;
                if (parentPath.size()) {
                    nodePath += ".";
                }
                nodePath += node->name.c_str();
                if (node->children) {
                    auto childIter = node->children->begin();
                    auto childEnd = node->children->end();
                    for (; childIter != childEnd; childIter++) {
                        makeChildrenClassMapProc(nodePath, *childIter);
                    }
                }
                if (parentPath.size()) {
                    childrenClassMap[parentPath.c_str()].push_back(nodePath.c_str());
                }
            }
        };
        
        makeClassMapProc = [&makeClassMapProc, &classMap](const string& parentPath, ASTNode* node) -> void {
            if (StrClassDecl == node->nodetype) {
                string nodePath = parentPath;
                if (parentPath.size()) {
                    nodePath += ".";
                }
                nodePath += node->name.c_str();
                if (node->children) {
                    auto childIter = node->children->begin();
                    auto childEnd = node->children->end();
                    for (; childIter != childEnd; childIter++) {
                        makeClassMapProc(nodePath, *childIter);
                    }
                }
                classMap[nodePath.c_str()] = node;
            }
        };
        
        isInheritFromClassProc = [&isInheritFromClassProc, &inheritMap](static_string _class,
                                                                        static_string parentClass,
                                                                        vector<static_string>& inheritPath) -> bool {
            auto iter = inheritMap.find(_class);
            if (iter != inheritMap.end()) {
                auto inhIter = iter->second.begin();
                auto inhEnd = iter->second.end();
                for (; inhIter != inhEnd; inhIter++) {
                    if (*inhIter == parentClass) {
                        inheritPath.push_back(parentClass);
                        return true;
                    }
                    vector<static_string> path(inheritPath);
                    ///path.push_back(_class);
                    if (isInheritFromClassProc(*inhIter, parentClass, path)) {
                        inheritPath = path;
                        inheritPath.insert(inheritPath.begin(), (*inhIter));
                        return true;
                    }
                }
            }
            return false;
        };
        
        string emptyPath;
        auto rootIter = m_roots.begin();
        auto rootEnd = m_roots.end();
        for (; rootIter != rootEnd; rootIter++) {
            ASTNode* root = *rootIter;
            if (!root->children)
                continue;
            auto rootChildIter = root->children->begin();
            auto rootChildEnd = root->children->end();
            for (; rootChildIter != rootChildEnd; rootChildIter++) {
                ASTNode* node = *rootChildIter;
                makeAliasMap(node);
                makeInheritMapProc(emptyPath, node);
                makeChildrenClassMapProc(emptyPath, node);
                makeClassMapProc(emptyPath, node);
            }
        }
        
        ///
        auto inhMapIter = inheritMap.begin();
        auto inhMapEnd = inheritMap.end();
        for (; inhMapIter != inhMapEnd; inhMapIter++) {
            ASTLog("##%s : ", inhMapIter->first.c_str());
            auto inhIter = inhMapIter->second.begin();
            auto inhEnd = inhMapIter->second.end();
            for (; inhIter != inhEnd; inhIter++) {
                auto aliasiter = aliasMap.find(*inhIter);
                if (aliasiter != aliasMap.end()) {
                    *inhIter = aliasiter->second;
                }
                ASTLog("%s ", (*inhIter).c_str());
            }
            ASTLog("\n");
        }
        vector<static_string> inheritPath;
        if (isInheritFromClassProc("TranslationScript.ZMoveState", "NSObject", inheritPath)) {
            ASTLog("@ TranslationScript.ZMoveState is inherit from NSObject\n");
            ASTLog("PATH: ");
            auto i = inheritPath.begin();
            auto e = inheritPath.end();
            for (; i != e; i++) {
                ASTLog("%s, ", (*i).c_str());
            }
            ASTLog("\n");
        }
        inheritPath.clear();
        if (isInheritFromClassProc("TranslationScript.ZMoveState", "State", inheritPath)) {
            ASTLog("@ TranslationScript.ZMoveState is inherit from State\n");
            ASTLog("PATH: ");
            auto i = inheritPath.begin();
            auto e = inheritPath.end();
            for (; i != e; i++) {
                ASTLog("%s, ", (*i).c_str());
            }
            ASTLog("\n");
        }
        inheritPath.clear();
        if (!isInheritFromClassProc("TranslationScript.ZMoveState", "None", inheritPath)) {
            ASTLog("@ TranslationScript.ZMoveState is not inherit from None\n");
        }
        inheritPath.clear();
        auto childMapIter = childrenClassMap.begin();
        auto childMapEnd = childrenClassMap.end();
        for (; childMapIter != childMapEnd; childMapIter++) {
            ASTLog("%c %s children: ", '%', childMapIter->first.c_str());
            auto childIter = childMapIter->second.begin();
            auto childEnd = childMapIter->second.end();
            for (; childIter != childEnd; childIter++) {
                ASTLog("%s, ", (*childIter).c_str());
            }
            ASTLog("\n");
        }
        bridgeFile = CreateBridgeFile(inheritMap, childrenClassMap, classMap, isInheritFromClassProc);
        stateActionFile = CreateStateActionFile(inheritMap, childrenClassMap, classMap, isInheritFromClassProc);
    }
    string SwiftParser::CreateBridgeFile(const map<static_string, vector<static_string>>& inheritMap,
                                         const map<static_string, vector<static_string>>& childrenClassMap,
                                         const map<static_string, ASTNode*>& classMap,
                                         Lambda<bool (static_string, static_string,
                                                      vector<static_string>&)>& isInheritFromClassProc)
    {
        xhn::string bridgeFile;
        char mbuf[1024];

        bridgeFile = "static xhn::SpinLock s_lock;\n";
        bridgeFile += "static void (*s_printLogProc)(int, const char*) = NULL;\n";
        bridgeFile += "NSMutableSet* s_sceneNodeAgentSet = nil;\n";
        bridgeFile += "NSMutableSet* s_guiAgentSet = nil;\n";
        bridgeFile += "NSMutableSet* s_actorAgentSet = nil;\n";
        bridgeFile += "static NSMutableDictionary* s_createSceneNodeAgentProcDic = nil;\n";
        bridgeFile += "static NSMutableDictionary* s_createGUIAgentProcDic = nil;\n";
        bridgeFile += "static NSMutableDictionary* s_createActorAgentProcDic = nil;\n";
        bridgeFile += "@interface CreateSceneNodeAgentProc : NSObject\n";
        bridgeFile += "@property (strong) SceneNodeAgent* (^createAgentProc)(void*);\n";
        bridgeFile += "-(id)initWithProc:(SceneNodeAgent*(^)(void*))proc;\n";
        bridgeFile += "@end;\n";
        bridgeFile += "@implementation CreateSceneNodeAgentProc \n";
        bridgeFile += "-(id)initWithProc:(SceneNodeAgent*(^)(void*))proc {\n";
        bridgeFile += "   self = [super init];\n";
        bridgeFile += "   if (self) {\n";
        bridgeFile += "       self.createAgentProc = proc;\n";
        bridgeFile += "   }\n";
        bridgeFile += "   return self;\n";
        bridgeFile += "}\n";
        bridgeFile += "@end\n";
        bridgeFile += "@interface CreateGUIAgentProc : NSObject\n";
        bridgeFile += "@property (strong) GUIAgent* (^createAgentProc)(void*, void*);\n";
        bridgeFile += "-(id)initWithProc:(GUIAgent*(^)(void*, void*))proc;\n";
        bridgeFile += "@end;\n";
        bridgeFile += "@implementation CreateGUIAgentProc \n";
        bridgeFile += "-(id)initWithProc:(GUIAgent*(^)(void*, void*))proc {\n";
        bridgeFile += "   self = [super init];\n";
        bridgeFile += "   if (self) {\n";
        bridgeFile += "       self.createAgentProc = proc;\n";
        bridgeFile += "   }\n";
        bridgeFile += "   return self;\n";
        bridgeFile += "}\n";
        bridgeFile += "@end\n";
        bridgeFile += "@interface CreateActorAgentProc : NSObject\n";
        bridgeFile += "@property (strong) ActorAgent* (^createAgentProc)(void*, void*);\n";
        bridgeFile += "-(id)initWithProc:(ActorAgent*(^)(void*, void*))proc;\n";
        bridgeFile += "@end;\n";
        bridgeFile += "@implementation CreateActorAgentProc \n";
        bridgeFile += "-(id)initWithProc:(ActorAgent*(^)(void*, void*))proc {\n";
        bridgeFile += "   self = [super init];\n";
        bridgeFile += "   if (self) {\n";
        bridgeFile += "       self.createAgentProc = proc;\n";
        bridgeFile += "   }\n";
        bridgeFile += "   return self;\n";
        bridgeFile += "}\n";
        bridgeFile += "@end\n";
        
        snprintf(mbuf, 1023,
                 "NSString* swiftClassString(NSString *nameSpace, NSString *className) {\n"
                 "    NSString *appName = @%cVEngineLogic%c;\n"
                 "    NSString *classStringName = [NSString stringWithFormat:@%c_TtCC%cld%c@%cld%c@%cld%c@%c, \n"
                 "    appName.length, appName, nameSpace.length, nameSpace, className.length, className];\n"
                 "    return classStringName;\n"
                 "}\n",
                 '"', '"',
                 '"', '%', '%', '%', '%', '%', '%', '"');
        bridgeFile += mbuf;
        
        snprintf(mbuf, 1023,
        "Class swiftClassFromString(NSString *nameSpace, NSString *className) {\n"
        "    NSString *appName = @%cVEngineLogic%c;\n"
        "    NSString *classStringName = [NSString stringWithFormat:@%c_TtCC%cld%c@%cld%c@%cld%c@%c, \n"
        "    appName.length, appName, nameSpace.length, nameSpace, className.length, className];\n"
        "    return NSClassFromString(classStringName);\n"
        "}\n",
        '"', '"',
        '"', '%', '%', '%', '%', '%', '%', '"');
        bridgeFile += mbuf;
        
        snprintf(mbuf, 1023,
                 "NSString* swiftClassStringFromPath(NSString *path) {\n"
                 "    NSString *appName = @%cVEngineLogic%c;\n"
                 "    NSString *classStringName = [NSString stringWithFormat:@%c_TtCC%cld%c@%c, appName.length, appName];\n"
                 "    NSArray *subpaths = [path componentsSeparatedByString:@%c.%c];\n"
                 "    for (NSString* subpath in subpaths) {\n"
                 "        classStringName = [classStringName stringByAppendingString:[NSString stringWithFormat:@%c%cld%c@%c, subpath.length, subpath]];\n"
                 "    }\n"
                 "    return classStringName;\n"
                 "}\n",
                 '"', '"',
                 '"', '%', '%', '"',
                 '"', '"',
                 '"', '%', '%', '"');
        bridgeFile += mbuf;
        
        snprintf(mbuf, 1023,
        "Class swiftClassFromPath(NSString *path) {\n"
        "    NSString *appName = @%cVEngineLogic%c;\n"
        "    NSString *classStringName = [NSString stringWithFormat:@%c_TtCC%cld%c@%c, appName.length, appName];\n"
        "    NSArray *subpaths = [path componentsSeparatedByString:@%c.%c];\n"
        "    for (NSString* subpath in subpaths) {\n"
        "        classStringName = [classStringName stringByAppendingString:[NSString stringWithFormat:@%c%cld%c@%c, subpath.length, subpath]];\n"
        "    }\n"
        "    return NSClassFromString(classStringName);\n"
        "}\n",
        '"', '"',
        '"', '%', '%', '"',
        '"', '"',
        '"', '%', '%', '"');
        bridgeFile += mbuf;
        
        bridgeFile += "void InitProcDic() {\n";
        bridgeFile += "    s_sceneNodeAgentSet = [NSMutableSet new];\n";
        bridgeFile += "    s_createSceneNodeAgentProcDic = [NSMutableDictionary new];\n";
        bridgeFile += "    s_guiAgentSet = [NSMutableSet new];\n";
        bridgeFile += "    s_createGUIAgentProcDic = [NSMutableDictionary new];\n";
        bridgeFile += "    s_actorAgentSet = [NSMutableSet new];\n";
        bridgeFile += "    s_createActorAgentProcDic = [NSMutableDictionary new];\n";
        /// 继承路径，用来判断一个类是否继承自另一个类或接口的依据
        /// 例：RegularSceneNodeAgent 继承自 SceneNodeAgent，SceneNodeAgent 继承自 NSObject
        /// 则 inheritPath[2] = NSObject， inheritPath[1] = SceneNodeAgent，inheritPath[0] = RegularSceneNodeAgent
        xhn::vector<xhn::static_string> inheritPath;
        
        /// 通过这个i可以知道加入的state有多少个
        auto addStatesProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &bridgeFile](int& i) {
            xhn::vector<xhn::static_string> stateInheritPath;
            char mbuf[512];
            xhn::string firstState;
            xhn::static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            ///
            snprintf(mbuf, 511,
                     "        /// agentClassName = %s\n", agentClassName.c_str());
            bridgeFile += mbuf;
            ///
            /// 找到所有的子类
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                /// 遍历子类
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    ///
                    snprintf(mbuf, 511,
                             "        /// childClassName = %s\n", childClassName.c_str());
                    bridgeFile += mbuf;
                    ///
                    /// 找到子类名对应的ASTNode
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        /// 判断该ASTNode的类型和访问状况，是否是派生类
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            /// 判断是否是派生自State和StateInterface
                            bool isInheritFromState = false;
                            bool isInheritFromStateInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 511, "%lld", node->name.size());
                            xhn::string stateFuncName = mbuf;
                            stateFuncName += node->name.c_str();
                            snprintf(mbuf, 511, "%lld", child->name.size());
                            stateFuncName += mbuf;
                            stateFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            stateInheritPath.clear();
                            isInheritFromState = isInheritFromClassProc(strFullClassName, StrState, stateInheritPath);
                            stateInheritPath.clear();
                            isInheritFromStateInterface = isInheritFromClassProc(strFullClassName, StrStateInterface, stateInheritPath);
                            
                            if (isInheritFromState && isInheritFromStateInterface) {
                                /// 真正开始添加state了
                                snprintf(mbuf, 511,
                                         "        NSString* state%dName = swiftClassStringFromPath(@%c%s%c);\n"
                                         ///"        id state%d = [[swiftClassFromPath(@%c%s%c) alloc] init];\n"
                                         "        id state%d = [SwiftStates _TtCC12VEngineLogic%s];\n"
                                         "        [ret setState:state%dName state:state%d];\n",
                                         i, '"', fullClassName.c_str(), '"',
                                         i, stateFuncName.c_str(),
                                         ///i, '"', fullClassName.c_str(), '"',
                                         i, i);
                                bridgeFile += mbuf;
                                
                                if (!firstState.size()) {
                                    snprintf(mbuf, 511, "state%d", i);
                                    firstState = mbuf;
                                }
                                
                                i++;
                            }
                        }
                    }
                }
                /// 将第一个state设为默认的state
                if (firstState.size()) {
                    snprintf(mbuf, 511,
                             "        [ret setCurrentState:%s];\n",
                             firstState.c_str());
                    bridgeFile += mbuf;
                }
            }
        };
        
        /// 通过这个i可以知道加入的action有多少个
        auto addActionsProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &bridgeFile](int& i) {
            xhn::vector<xhn::static_string> actionInheritPath;
            char mbuf[512];
            xhn::string firstAction;
            xhn::static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            ///
            snprintf(mbuf, 511,
                     "        /// agentClassName = %s\n", agentClassName.c_str());
            bridgeFile += mbuf;
            ///
            /// 找到所有的子类
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                /// 遍历子类
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    ///
                    snprintf(mbuf, 511,
                             "        /// childClassName = %s\n", childClassName.c_str());
                    bridgeFile += mbuf;
                    ///
                    /// 找到子类名对应的ASTNode
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            bool isInheritFromAction = false;
                            bool isInheritFromActionInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 511, "%lld", node->name.size());
                            xhn::string actionFuncName = mbuf;
                            actionFuncName += node->name.c_str();
                            snprintf(mbuf, 511, "%lld", child->name.size());
                            actionFuncName += mbuf;
                            actionFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            actionInheritPath.clear();
                            isInheritFromAction = isInheritFromClassProc(strFullClassName, StrAction, actionInheritPath);
                            actionInheritPath.clear();
                            isInheritFromActionInterface = isInheritFromClassProc(strFullClassName, StrActionInterface, actionInheritPath);
                            
                            if (isInheritFromAction && isInheritFromActionInterface) {
                                snprintf(mbuf, 511,
                                         ///"        NSString* action%dName = swiftClassStringFromPath(@%c%s%c);\n"
                                         "        id action%d = [SwiftActions _TtCC12VEngineLogic%s];\n"
                                         ///"        id action%d = [[swiftClassFromPath(@%c%s%c) alloc] init];\n"
                                         "#pragma clang diagnostic push\n"
                                         "#pragma clang diagnostic ignored \"-Wundeclared-selector\"\n"
                                         "        if ([action%d respondsToSelector:@selector(resourceName)]) {\n"
                                         "            NSString* action%dResName = [action%d performSelector:@selector(resourceName)];\n"
                                         "            [ret setActionWithResName:action%dResName action:action%d];\n"
                                         "        }\n"
                                         "#pragma clang diagnostic pop\n",
                                         ///i, '"', fullClassName.c_str(), '"',
                                         i, actionFuncName.c_str(),
                                         i,
                                         i, i,
                                         ///i, '"', fullClassName.c_str(), '"',
                                         i, i);
                                bridgeFile += mbuf;
                                
                                if (!firstAction.size()) {
                                    snprintf(mbuf, 511, "action%d", i);
                                    firstAction = mbuf;
                                }
                                
                                i++;
                            }
                        }
                    }
                }
                if (firstAction.size()) {
                    snprintf(mbuf, 511,
                             "        [ret setCurrentAction:%s];\n",
                             firstAction.c_str());
                    bridgeFile += mbuf;
                }
            }
        };
        
        auto addGUIStatesProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &bridgeFile](xhn::static_string& normalState,
                                                                                                                   xhn::static_string& hoveringState,
                                                                                                                   xhn::static_string& selectedState,
                                                                                                                   xhn::static_string& pressedState,
                                                                                                                   xhn::static_string& draggingState) {
            xhn::vector<xhn::static_string> guiStateInheritPath;
            char mbuf[512];
            xhn::string firstAction;
            xhn::static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            ///
            snprintf(mbuf, 511,
                     "        /// agentClassName = %s\n", agentClassName.c_str());
            bridgeFile += mbuf;
            ///
            /// 找到所有的子类
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                /// 遍历子类
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    ///
                    snprintf(mbuf, 511,
                             "        /// childClassName = %s\n", childClassName.c_str());
                    bridgeFile += mbuf;
                    ///
                    /// 找到子类名对应的ASTNode
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        /// 判断该ASTNode的类型和访问状况，是否是派生类
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            
                            /// 判断是否是派生自GUIState和GUIStateInterface
                            bool isInheritFromGUIState = false;
                            bool isInheritFromGUIStateInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 511, "%lld", node->name.size());
                            xhn::string stateFuncName = mbuf;
                            stateFuncName += node->name.c_str();
                            snprintf(mbuf, 511, "%lld", child->name.size());
                            stateFuncName += mbuf;
                            stateFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            guiStateInheritPath.clear();
                            isInheritFromGUIState = isInheritFromClassProc(strFullClassName, StrGUIState, guiStateInheritPath);
                            guiStateInheritPath.clear();
                            isInheritFromGUIStateInterface = isInheritFromClassProc(strFullClassName, StrGUIStateInterface, guiStateInheritPath);
                            
                            if (isInheritFromGUIState && isInheritFromGUIStateInterface) {
                                /// 这里要判断状态的类型
                                /// NormalState、HoveringState、SelectedState、PressedState、DraggingState
                                xhn::string tmpFullClassName = strFullClassName.c_str();
                                euint pos = tmpFullClassName.rfind(".");
                                if (xhn::string::npos != pos) {
                                    xhn::string stateName = tmpFullClassName.substr(pos + 1, tmpFullClassName.size() - pos - 1);
                                    ///
                                    snprintf(mbuf, 511,
                                             "        /// stateName = %s\n", stateName.c_str());
                                    bridgeFile += mbuf;
                                    ///
                                    if (stateName == StrNormalState.c_str()) {
                                        normalState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrHoveringState.c_str()) {
                                        hoveringState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrSelectedState.c_str()) {
                                        selectedState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrPressedState.c_str()) {
                                        pressedState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrDraggingState.c_str()) {
                                        draggingState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrNormalStateBase.c_str()) {
                                        normalState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrHoveringStateBase.c_str()) {
                                        hoveringState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrSelectedStateBase.c_str()) {
                                        selectedState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrPressedStateBase.c_str()) {
                                        pressedState = stateFuncName.c_str();
                                    }
                                    else if (stateName == StrDraggingStateBase.c_str()) {
                                        draggingState = stateFuncName.c_str();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        };
        
        m_sceneNodeAgentNameVector.clear();
        m_guiAgentNameVector.clear();
        m_actorAgentNameVector.clear();
        
        auto rootIter = m_roots.begin();
        auto rootEnd = m_roots.end();
        for (; rootIter != rootEnd; rootIter++) {
            ASTNode* root = *rootIter;
            if (!root->children)
                continue;
            auto rootChildIter = root->children->begin();
            auto rootChildEnd = root->children->end();
            for (; rootChildIter != rootChildEnd; rootChildIter++) {
                ASTNode* node = *rootChildIter;
                if (StrClassDecl == node->nodetype &&
                    (StrPublic == node->access || StrOpen == node->access)) {
                    if (isInheritFromClassProc(node->name, StrSceneNodeAgent, inheritPath)) {
                        /// 这里将创建节点代理的回调放进s_createSceneNodeAgentProcDic里
                        inheritPath.insert(inheritPath.begin(), node->name);
                        char mbuf[512];
                        snprintf(mbuf, 511,
                                 "    s_createSceneNodeAgentProcDic[@%c%s%c] = [[CreateSceneNodeAgentProc alloc] initWithProc:^(void* sceneNode)\n"
                                 "    {\n"
                                 "        %s* ret = [[%s alloc] initWithSceneNode:[[VSceneNode alloc] initWithVSceneNode:sceneNode]];\n",
                                 '"', node->name.c_str(), '"', node->name.c_str(), node->name.c_str());
                        m_sceneNodeAgentNameVector.push_back(node->name);
                        
                        bridgeFile += mbuf;
                        int i = 0;
                        while (inheritPath.size()) {
                            addStatesProc(i);
                            inheritPath.pop_back();
                        }
                        bridgeFile +=
                        "        [ret start];\n"
                        "        return ret;\n"
                        "    }];\n";
                    }
                    else {
                        inheritPath.clear();
                        if (isInheritFromClassProc(node->name, StrActorAgent, inheritPath)) {
                            /// 这里将创建actor代理的回调放在s_createActorAgentProcDic里
                            inheritPath.insert(inheritPath.begin(), node->name);
                            char mbuf[512];
                            snprintf(mbuf, 511,
                                     "    s_createActorAgentProcDic[@%c%s%c] = [[CreateActorAgentProc alloc] initWithProc:^(void* renderSys, void* actor)\n"
                                     "    {\n"
                                     "        %s* ret = [[%s alloc] initWithActor:[[VActor alloc] initWithVRenderSystemActor:renderSys actor:actor]];\n",
                                     '"', node->name.c_str(), '"', node->name.c_str(), node->name.c_str());
                            m_actorAgentNameVector.push_back(node->name);
                            
                            bridgeFile += mbuf;
                            int i = 0;
                            while (inheritPath.size()) {
                                addActionsProc(i);
                                inheritPath.pop_back();
                            }
                            bridgeFile +=
                            "        [ret start];\n"
                            "        return ret;\n"
                            "    }];\n";
                        }
                        else {
                            inheritPath.clear();
                            if (isInheritFromClassProc(node->name, StrGUIAgent, inheritPath)) {
                                inheritPath.insert(inheritPath.begin(), node->name);
                                
                                char mbuf[512];
                                snprintf(mbuf, 511,
                                         "    s_createGUIAgentProcDic[@%c%s%c] = [[CreateGUIAgentProc alloc] initWithProc:^(void* renderSys, void* widget)\n"
                                         "    {\n"
                                         "        %s* ret = [[%s alloc] initWithWidget:[[VWidget alloc] initWithVRenderSystemWidget:renderSys widget:widget]];\n",
                                         '"', node->name.c_str(), '"', node->name.c_str(), node->name.c_str());
                                m_guiAgentNameVector.push_back(node->name);
                                
                                bridgeFile += mbuf;
                                ///
                                /// 这里配置
                                xhn::static_string normalState = xhn::static_string::empty_string;
                                xhn::static_string hoveringState = xhn::static_string::empty_string;
                                xhn::static_string selectedState = xhn::static_string::empty_string;
                                xhn::static_string pressedState = xhn::static_string::empty_string;
                                xhn::static_string draggingState = xhn::static_string::empty_string;
                                
                                while (inheritPath.size()) {
                                    addGUIStatesProc(normalState, hoveringState, selectedState, pressedState, draggingState);
                                    inheritPath.pop_back();
                                }
                                
                                EAssert(xhn::static_string::empty_string != normalState &&
                                        xhn::static_string::empty_string != hoveringState &&
                                        xhn::static_string::empty_string != selectedState &&
                                        xhn::static_string::empty_string != pressedState &&
                                        xhn::static_string::empty_string != draggingState, "gui state member are incomplete!, %s, %s, %s, %s, %s",
                                        normalState.c_str(), hoveringState.c_str(), selectedState.c_str(), pressedState.c_str(), draggingState.c_str());
                                ///
                                snprintf(mbuf, 511, "        id normalState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", normalState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 511, "        id hoveringState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", hoveringState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 511, "        id selectedState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", selectedState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 511, "        id pressedState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", pressedState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 511, "        id draggingState = [SwiftGUIStates _TtCC12VEngineLogic%s];\n", draggingState.c_str());
                                bridgeFile += mbuf;
                                snprintf(mbuf, 511, "        [ret setState:@\"NormalState\" state:normalState];\n"
                                                    "        [ret setState:@\"HoveringState\" state:hoveringState];\n"
                                                    "        [ret setState:@\"SelectedState\" state:selectedState];\n"
                                                    "        [ret setState:@\"PressedState\" state:pressedState];\n"
                                                    "        [ret setState:@\"DraggingState\" state:draggingState];\n"
                                                    "        [ret setCurrentState:normalState];\n");
                                bridgeFile += mbuf;
                                ///
                                bridgeFile +=
                                "        [ret start];\n"
                                "        return ret;\n"
                                "    }];\n";
                            }
                        }
                    }
                }
            }
        }
        
        bridgeFile += "}\n";
        bridgeFile += "void* CreateSceneNodeAgent(const char* type, void* sceneNode) {\n";
        bridgeFile += "    NSString* strType = [[NSString alloc] initWithUTF8String:type];\n";
        bridgeFile += "    CreateSceneNodeAgentProc* proc = s_createSceneNodeAgentProcDic[strType];\n";
        bridgeFile += "    SceneNodeAgent* agent = proc.createAgentProc(sceneNode);\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        [s_sceneNodeAgentSet addObject:agent];\n";
        bridgeFile += "        return  (__bridge void *)agent;\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void RemoveSceneNodeAgent(void* agent) {\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        SceneNodeAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "        [s_sceneNodeAgentSet removeObject:agentID];\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void UpdateSceneNodeAgent(void* agent, double elapsedTime) {\n";
        bridgeFile += "    SceneNodeAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID update:elapsedTime];\n";
        bridgeFile += "}\n";
        bridgeFile += "void SceneNodeAgentGotoState(void* agent, const char* nextStateName) {\n";
        bridgeFile += "    SceneNodeAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    NSString* _Nonnull strNextStateName = [NSString stringWithUTF8String:nextStateName];\n";
        bridgeFile += "    [agentID gotoState:strNextStateName];\n";
        bridgeFile += "}\n";
        bridgeFile += "void* CreateGUIAgent(const char* type, void* renderSys, void* widget) {\n";
        bridgeFile += "    NSString* strType = [[NSString alloc] initWithUTF8String:type];\n";
        bridgeFile += "    CreateGUIAgentProc* proc = s_createGUIAgentProcDic[strType];\n";
        bridgeFile += "    GUIAgent* agent = proc.createAgentProc(renderSys, widget);\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        [s_guiAgentSet addObject:agent];\n";
        bridgeFile += "        return  (__bridge void *)agent;\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void RemoveGUIAgent(void* agent) {\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "        [s_guiAgentSet removeObject:agentID];\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void UpdateGUIAgent(void* agent, double elapsedTime) {\n";
        bridgeFile += "    GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID update:elapsedTime];\n";
        bridgeFile += "}\n";
        bridgeFile += "void GotoGUIAgentState(void* agent, int stateKind) {\n";
        bridgeFile += "    GUIAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    NSString* strStateKind = nil;\n";
        bridgeFile += "    switch (stateKind)\n";
        bridgeFile += "    {\n";
        bridgeFile += "        case 0: strStateKind = @\"NormalState\"; break;\n";
        bridgeFile += "        case 1: strStateKind = @\"HoveringState\"; break;\n";
        bridgeFile += "        case 2: strStateKind = @\"SelectedState\"; break;\n";
        bridgeFile += "        case 3: strStateKind = @\"PressedState\"; break;\n";
        bridgeFile += "        case 4: strStateKind = @\"DraggingState\"; break;\n";
        bridgeFile += "        default: break;\n";
        bridgeFile += "    }\n";
        bridgeFile += "    if (strStateKind) {\n";
        bridgeFile += "        [agentID gotoState:strStateKind];\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void* CreateActorAgent(const char* type, void* renderSys, void* actor) {\n";
        bridgeFile += "    NSString* strType = [[NSString alloc] initWithUTF8String:type];\n";
        bridgeFile += "    CreateActorAgentProc* proc = s_createActorAgentProcDic[strType];\n";
        bridgeFile += "    ActorAgent* agent = proc.createAgentProc(renderSys, actor);\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        [s_actorAgentSet addObject:agent];\n";
        bridgeFile += "        return  (__bridge void *)agent;\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void RemoveActorAgent(void* agent) {\n";
        bridgeFile += "    {\n";
        bridgeFile += "        auto inst = s_lock.Lock();\n";
        bridgeFile += "        ActorAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "        [s_actorAgentSet removeObject:agentID];\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        bridgeFile += "void UpdateActorAgent(void* agent, double elapsedTime) {\n";
        bridgeFile += "    ActorAgent* agentID = (__bridge id)agent;\n";
        bridgeFile += "    [agentID update:elapsedTime];\n";
        bridgeFile += "}\n";
        bridgeFile += "int GetNumSceneNodeAgentProcs() {\n";
        bridgeFile += "    return (int)[s_createSceneNodeAgentProcDic count];\n";
        bridgeFile += "}\n";
        bridgeFile += "int GetNumActorAgentProcs() {\n";
        bridgeFile += "    return (int)[s_createActorAgentProcDic count];\n";
        bridgeFile += "}\n";
        bridgeFile += "char** GetSceneNodeAgentProcTypes() {\n";
        bridgeFile += "    char** ret = (char**)malloc(sizeof(char*) * [s_createSceneNodeAgentProcDic count]);\n";
        bridgeFile += "    NSEnumerator *enumeratorKey = [s_createSceneNodeAgentProcDic keyEnumerator];\n";
        bridgeFile += "    int i = 0;\n";
        bridgeFile += "    for (NSString* key in enumeratorKey) {\n";
        bridgeFile += "        int len = (int)strlen([key UTF8String]);\n";
        bridgeFile += "        ret[i] = (char*)malloc(len + 1);\n";
        bridgeFile += "        memcpy(ret[i], [key UTF8String], len + 1);\n";
        bridgeFile += "        i++;\n";
        bridgeFile += "    }\n";
        bridgeFile += "    return ret;\n";
        bridgeFile += "}\n";
        bridgeFile += "char** GetActorAgentProcTypes() {\n";
        bridgeFile += "    char** ret = (char**)malloc(sizeof(char*) * [s_createActorAgentProcDic count]);\n";
        bridgeFile += "    NSEnumerator *enumeratorKey = [s_createActorAgentProcDic keyEnumerator];\n";
        bridgeFile += "    int i = 0;\n";
        bridgeFile += "    for (NSString* key in enumeratorKey) {\n";
        bridgeFile += "        int len = (int)strlen([key UTF8String]);\n";
        bridgeFile += "        ret[i] = (char*)malloc(len + 1);\n";
        bridgeFile += "        memcpy(ret[i], [key UTF8String], len + 1);\n";
        bridgeFile += "        i++;\n";
        bridgeFile += "    }\n";
        bridgeFile += "    return ret;\n";
        bridgeFile += "}\n";
        bridgeFile += "void SetPrintLogProc(void (*printLogProc)(int, const char*)) {\n";
        bridgeFile += "    s_printLogProc = printLogProc;\n";
        bridgeFile += "}\n";
        bridgeFile += "void PrintLog(int type, const char* log) {\n";
        bridgeFile += "    if (s_printLogProc) {\n";
        bridgeFile += "        s_printLogProc(type, log);\n";
        bridgeFile += "    }\n";
        bridgeFile += "}\n";
        ///printf("%s\n", bridgeFile.c_str());
        return bridgeFile;
    }
    string SwiftParser::CreateStateActionFile(const map<static_string, vector<static_string>>& inheritMap,
                                              const map<static_string, vector<static_string>>& childrenClassMap,
                                              const map<static_string, ASTNode*>& classMap,
                                              Lambda<bool (static_string, static_string,
                                                           vector<static_string>&)>& isInheritFromClassProc)
    {
        string actionFile;
        
        /// func class
        map<string, string> stateFuncClassMap;
        /// func class
        map<string, string> guiStateFuncClassMap;
        /// func class
        map<string, string> actionFuncClassMap;
        
        /// 继承路径，用来判断一个类是否继承自另一个类或接口的依据
        vector<static_string> inheritPath;
        
        auto addStatesProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &stateFuncClassMap]() {
            xhn::vector<xhn::static_string> stateInheritPath;
            char mbuf[512];
            xhn::string firstState;
            xhn::static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            bool isInheritFromState = false;
                            bool isInheritFromStateInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 511, "%lld", node->name.size());
                            xhn::string stateFuncName = mbuf;
                            stateFuncName += node->name.c_str();
                            snprintf(mbuf, 511, "%lld", child->name.size());
                            stateFuncName += mbuf;
                            stateFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            stateInheritPath.clear();
                            isInheritFromState = isInheritFromClassProc(strFullClassName, StrState, stateInheritPath);
                            stateInheritPath.clear();
                            isInheritFromStateInterface = isInheritFromClassProc(strFullClassName, StrStateInterface, stateInheritPath);
                            
                            if (isInheritFromState && isInheritFromStateInterface) {
                                stateFuncClassMap.insert(make_pair(stateFuncName, fullClassName));
                            }
                        }
                    }
                }
            }
        };
        
        auto addGUIStatesProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &guiStateFuncClassMap]() {
            xhn::vector<xhn::static_string> guiStateInheritPath;
            char mbuf[512];
            xhn::string firstAction;
            xhn::static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            
            /// 找到所有的子类
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                /// 遍历子类
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    
                    /// 找到子类名对应的ASTNode
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        /// 判断该ASTNode的类型和访问状况，是否是派生类
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            
                            /// 判断是否是派生自GUIState和GUIStateInterface
                            bool isInheritFromGUIState = false;
                            bool isInheritFromGUIStateInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 511, "%lld", node->name.size());
                            xhn::string stateFuncName = mbuf;
                            stateFuncName += node->name.c_str();
                            snprintf(mbuf, 511, "%lld", child->name.size());
                            stateFuncName += mbuf;
                            stateFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            guiStateInheritPath.clear();
                            isInheritFromGUIState = isInheritFromClassProc(strFullClassName, StrGUIState, guiStateInheritPath);
                            guiStateInheritPath.clear();
                            isInheritFromGUIStateInterface = isInheritFromClassProc(strFullClassName, StrGUIStateInterface, guiStateInheritPath);
                            
                            if (isInheritFromGUIState && isInheritFromGUIStateInterface) {
                                guiStateFuncClassMap.insert(make_pair(stateFuncName, fullClassName));
                            }
                        }
                    }
                }
            }
        };
        
        auto addActionsProc = [&inheritPath, &childrenClassMap, &classMap, &isInheritFromClassProc, &actionFuncClassMap]() {
            vector<static_string> actionInheritPath;
            char mbuf[512];
            string firstAction;
            static_string agentClassName = inheritPath.back();
            
            ASTNode* node = nullptr;
            auto nodeIter = classMap.find(agentClassName);
            if (nodeIter != classMap.end()) {
                node = nodeIter->second;
            }
            
            auto childClassIter = childrenClassMap.find(agentClassName);
            if (childClassIter != childrenClassMap.end()) {
                auto childIter = childClassIter->second.begin();
                auto childEnd = childClassIter->second.end();
                for (; childIter != childEnd; childIter++) {
                    xhn::static_string childClassName = *childIter;
                    
                    auto childNodeIter = classMap.find(childClassName);
                    if (childNodeIter != classMap.end()) {
                        ASTNode* child = childNodeIter->second;
                        if (StrClassDecl == child->nodetype &&
                            (StrPublic == child->access || StrOpen == child->access) &&
                            child->inherits) {
                            bool isInheritFromAction = false;
                            bool isInheritFromActionInterface = false;
                            
                            xhn::string fullClassName = node->name.c_str();
                            fullClassName += ".";
                            fullClassName += child->name.c_str();
                            
                            snprintf(mbuf, 511, "%lld", node->name.size());
                            xhn::string actionFuncName = mbuf;
                            actionFuncName += node->name.c_str();
                            snprintf(mbuf, 511, "%lld", child->name.size());
                            actionFuncName += mbuf;
                            actionFuncName += child->name.c_str();
                            
                            xhn::static_string strFullClassName = fullClassName.c_str();
                            
                            actionInheritPath.clear();
                            isInheritFromAction = isInheritFromClassProc(strFullClassName, StrAction, actionInheritPath);
                            actionInheritPath.clear();
                            isInheritFromActionInterface = isInheritFromClassProc(strFullClassName, StrActionInterface, actionInheritPath);
                            
                            if (isInheritFromAction && isInheritFromActionInterface) {
                                actionFuncClassMap.insert(make_pair(actionFuncName, fullClassName));
                            }
                        }
                    }
                }
            }
        };
        
        m_sceneNodeAgentNameVector.clear();
        m_guiAgentNameVector.clear();
        m_actorAgentNameVector.clear();
        
        auto rootIter = m_roots.begin();
        auto rootEnd = m_roots.end();
        for (; rootIter != rootEnd; rootIter++) {
            ASTNode* root = *rootIter;
            if (!root->children)
                continue;
            auto rootChildIter = root->children->begin();
            auto rootChildEnd = root->children->end();
            for (; rootChildIter != rootChildEnd; rootChildIter++) {
                ASTNode* node = *rootChildIter;
                if (StrClassDecl == node->nodetype &&
                    (StrPublic == node->access || StrOpen == node->access)) {
                    if (isInheritFromClassProc(node->name, StrSceneNodeAgent, inheritPath)) {
                        /// 这里将创建节点代理的回调放进s_createSceneNodeAgentProcDic里
                        inheritPath.insert(inheritPath.begin(), node->name);
                        
                        m_sceneNodeAgentNameVector.push_back(node->name);
                        
                        while (inheritPath.size()) {
                            addStatesProc();
                            inheritPath.pop_back();
                        }
                        
                    }
                    else {
                        inheritPath.clear();
                        if (isInheritFromClassProc(node->name, StrActorAgent, inheritPath)) {
                            /// 这里将创建actor代理的回调放在s_createActorAgentProcDic里
                            inheritPath.insert(inheritPath.begin(), node->name);
                            
                            m_actorAgentNameVector.push_back(node->name);
                            
                            while (inheritPath.size()) {
                                addActionsProc();
                                inheritPath.pop_back();
                            }
                            
                        }
                        else {
                            inheritPath.clear();
                            if (isInheritFromClassProc(node->name, StrGUIAgent, inheritPath)) {
                                /// 这里将创建actor代理的回调放在s_createGUIAgentProcDic里
                                inheritPath.insert(inheritPath.begin(), node->name);
                                
                                m_guiAgentNameVector.push_back(node->name);
                                
                                while (inheritPath.size()) {
                                    addGUIStatesProc();
                                    inheritPath.pop_back();
                                }
                            }
                        }
                    }
                }
            }
        }
        
        actionFile += "open class SwiftStates : NSObject {\n";
        
        for (auto& p : stateFuncClassMap) {
            char mbuf[512];
            snprintf(mbuf, 511,
                     "    static open func _TtCC12VEngineLogic%s() -> AnyObject {\n"
                     "        return %s();\n"
                     "    }\n",
                     p.first.c_str(), p.second.c_str());
            actionFile += mbuf;
        }
        
        actionFile += "}\n";
        
        actionFile += "open class SwiftGUIStates : NSObject {\n";
        for (auto& p : guiStateFuncClassMap) {
            char mbuf[512];
            snprintf(mbuf, 511,
                     "    static open func _TtCC12VEngineLogic%s() -> AnyObject {\n"
                     "        return %s();\n"
                     "    }\n",
                     p.first.c_str(), p.second.c_str());
            actionFile += mbuf;
        }
        actionFile += "}\n";
        
        actionFile += "open class SwiftActions : NSObject {\n";
        
        for (auto& p : actionFuncClassMap) {
            char mbuf[512];
            snprintf(mbuf, 511,
                     "    static open func _TtCC12VEngineLogic%s() -> AnyObject {\n"
                     "        return %s();\n"
                     "    }\n",
                     p.first.c_str(), p.second.c_str());
            actionFile += mbuf;
        }
        
        actionFile += "}\n";
        return actionFile;
    }
    void SwiftParser::Parse(const char* strBuffer, euint length)
    {
        euint count = 0;
        
        auto reduce = [this]()
        {
            if (m_isNodeType) {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                m_symbolBuffer.bufferTop = 0;
                if (StrSourceFile == symbol) {
                    m_roots.push_back(m_nodeStack.back());
                }
                if (m_nodeStack.size()) {
                    m_nodeStack.back()->nodetype = symbol;
                    if (StrTypealias == symbol) {
                        ASTLog("TYPEALIAS\n");
                    }
                }
                m_isNodeType = false;
                m_isName = true;
            }
            else if (m_isAccess) {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                m_symbolBuffer.bufferTop = 0;
                if (m_nodeStack.size()) {
                    m_nodeStack.back()->access = symbol;
                }
                m_isAccess = false;
            }
            else if (m_isDecl) {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                m_symbolBuffer.bufferTop = 0;
                if (m_nodeStack.size()) {
                    m_nodeStack.back()->decl = symbol;
                }
                m_isDecl = false;
            }
            else if (m_isType) {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                m_symbolBuffer.bufferTop = 0;
                if (m_nodeStack.size()) {
                    if (!m_isInterface) {
                        m_nodeStack.back()->type = symbol;
                    }
                    else {
                        m_nodeStack.back()->interfacetype = symbol;
                        m_isInterface = false;
                    }
                }
                m_isType = false;
            }
            else {
                xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                if (StrInherits == symbol) {
                    m_isInherits = true;
                }
                else if (StrAccess == symbol) {
                    m_isAccess = true;
                }
                else if (StrDecl == symbol) {
                    m_isDecl = true;
                }
                else if (StrType == symbol) {
                    m_isType = true;
                }
                else if (StrInterface == symbol) {
                    m_isInterface = true;
                }
                else {
                    ASTLog("SYMBOL=%s\n", symbol.c_str());
                }
            }
        };
        auto reduceInherit = [this]() {
            xhn::static_string symbol = m_symbolBuffer.GetSymbol();
            if (xhn::static_string::empty_string != symbol) {
                if (m_nodeStack.size()) {
                    if (!m_nodeStack.back()->inherits) {
                        m_nodeStack.back()->inherits = VNEW xhn::vector<xhn::static_string>();
                    }
                    m_nodeStack.back()->inherits->push_back(symbol);
                }
            }
        };
        while (count < length) {
            char c = strBuffer[count];
            ASTLog("%c, m_isNodeType %d, m_isName %d, m_isInterface %d, m_isApostropheBlock %d, m_isQuotationBlock %d\n",
                   c,   m_isNodeType,    m_isName,    m_isInterface,    m_isApostropheBlock,    m_isQuotationBlock);
            switch (c)
            {
                case '\0':
                    break;
                case '(':
                    m_symbolBuffer.bufferTop = 0;
                    m_isName = false;
                    if (!m_isApostropheBlock &&
                        !m_isQuotationBlock) {
                        m_nodeStack.push_back(VNEW ASTNode());
                        m_isNodeType = true;
                        ASTLog("NEW NODE\n");
                    }
                    else {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    break;
                case ')':
                    m_isName = false;
                    if (!m_isApostropheBlock &&
                        !m_isQuotationBlock) {
                        reduce();
                        m_symbolBuffer.bufferTop = 0;
                        ASTNode* currentNode = m_nodeStack.back();
                        m_nodeStack.pop_back();
                        if (m_nodeStack.size()) {
                            ASTNode* parentNode = m_nodeStack.back();
                            ASTLog("NODETYPE=%s, NAME=%s, ACCESS=%s, DECL=%s, TYPE=%s, INTERFACETYPE=%s\n",
                                   currentNode->nodetype.c_str(),
                                   currentNode->name.c_str(),
                                   currentNode->access.c_str(),
                                   currentNode->decl.c_str(),
                                   currentNode->type.c_str(),
                                   currentNode->interfacetype.c_str());
                            ASTLog("%s <- %s\n", parentNode->nodetype.c_str(), currentNode->nodetype.c_str());
                            if (!parentNode->children) {
                                parentNode->children = VNEW xhn::vector<ASTNode*>();
                            }
                            parentNode->children->push_back(currentNode);
                        }
                        ASTLog("PUSH TO PARENT NODE\n");
                    }
                    else {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    break;
                case 0x22: /// 双引号
                    if (!m_isQuotationBlock) {
                        m_isQuotationBlock = true;
                    }
                    else {
                        if (m_isName) {
                            xhn::static_string symbol = m_symbolBuffer.GetSymbol();
                            if (m_nodeStack.size()) {
                                m_nodeStack.back()->name = symbol;
                                ASTLog("NODETYPE=%s NAME=%s\n",
                                       m_nodeStack.back()->nodetype.c_str(), m_nodeStack.back()->name.c_str());
                            }
                            m_isName = false;
                        }
                        m_symbolBuffer.bufferTop = 0;
                        m_isQuotationBlock = false;
                    }
                    break;
                case 0x27: /// 单引号
                    m_isName = false;
                    if (!m_isApostropheBlock) {
                        m_isApostropheBlock = true;
                    }
                    else {
                        m_symbolBuffer.bufferTop = 0;
                        m_isApostropheBlock = false;
                    }
                    break;
                case '=':
                case ' ':
                    if (m_isName && !m_isQuotationBlock) {
                        m_isName = false;
                    }
                    if (m_isApostropheBlock ||
                        m_isQuotationBlock) {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    else {
                        reduce();
                        m_symbolBuffer.bufferTop = 0;
                    }
                    break;
                case '\n':
                    if (m_isName && !m_isQuotationBlock) {
                        m_isName = false;
                    }
                    if (m_isApostropheBlock ||
                        m_isQuotationBlock) {
                        m_symbolBuffer.AddCharacter(c);
                    }
                    else {
                        reduce();
                        m_symbolBuffer.bufferTop = 0;
                    }
                    if (m_isInherits) {
                        reduceInherit();
                        m_isInherits = false;
                    }
                    break;
                case ',':
                    if (m_isInherits) {
                        reduceInherit();
                    }
                    else {
                        if (m_isName && !m_isQuotationBlock) {
                            m_isName = false;
                        }
                        m_symbolBuffer.AddCharacter(c);
                    }
                    break;
                default:
                    if (m_isName && !m_isQuotationBlock) {
                        m_isName = false;
                    }
                    m_symbolBuffer.AddCharacter(c);
                    break;
            }
            count++;
        }
    }
    void SwiftParser::ParseSwifts(const string& paths, xhn::Lambda<void (const xhn::string& bridgeFile,
                                                                         const xhn::string& stateActionFile,
                                                                         const xhn::vector<xhn::static_string>&,
                                                                         const xhn::vector<xhn::static_string>&,
                                                                         const xhn::vector<xhn::static_string>&)>& callback)
    {
        SwiftCommandLineUtil* sclu = [SwiftCommandLineUtil new];
        NSString* swiftc = @"/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/swiftc";
        NSString* sdk = @"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk";
        NSString* command = [[NSString alloc] initWithFormat:@"%@ -sdk %@ -dump-ast %@",
                             swiftc, sdk,
                             [[NSString alloc] initWithUTF8String:paths.c_str()]];
        __block xhn::Lambda<void (const xhn::string& bridgeFile,
                                  const xhn::string& stateActionFile,
                                  const xhn::vector<xhn::static_string>&,
                                  const xhn::vector<xhn::static_string>&,
                                  const xhn::vector<xhn::static_string>&)> tmpCallback = callback;
        void (^objcCallback)(const xhn::string& bridgeFile,
                             const xhn::string& stateActionFile,
                             const xhn::vector<xhn::static_string>&,
                             const xhn::vector<xhn::static_string>&,
                             const xhn::vector<xhn::static_string>&)  = ^(const xhn::string& bridgeFile,
                                                                          const xhn::string& stateActionFile,
                                                                          const xhn::vector<xhn::static_string>& sceneNodeAgentNames,
                                                                          const xhn::vector<xhn::static_string>& guiAgentNames,
                                                                          const xhn::vector<xhn::static_string>& actorAgentNames) {
            tmpCallback(bridgeFile, stateActionFile, sceneNodeAgentNames, guiAgentNames, actorAgentNames);
        };
        [sclu runCommand:command callback:objcCallback];
    }
    SwiftParser::SwiftParser()
    : m_isApostropheBlock(false)
    , m_isQuotationBlock(false)
    , m_isNodeType(false)
    , m_isName(false)
    , m_isInterface(false)
    , m_isInherits(false)
    , m_isAccess(false)
    , m_isDecl(false)
    , m_isType(false)
    {
#if USING_AST_LOG
        s_ASTLogFile = fopen("/Users/xhnsworks/测试工程/swiftTest/swiftTest/main2.txt", "wb");
        s_ASTFile = fopen("/Users/xhnsworks/测试工程/swiftTest/swiftTest/main3.txt", "wb");
#endif
    }
    SwiftParser::~SwiftParser()
    {
#if USING_AST_LOG
        fclose(s_ASTLogFile);
        fclose(s_ASTFile);
#endif
    }
}

@implementation SwiftCommandLineUtil
{
    void(^mCallback)(const xhn::string& bridgeFile,
                     const xhn::string& stateActionFile,
                     const xhn::vector<xhn::static_string>&,
                     const xhn::vector<xhn::static_string>&,
                     const xhn::vector<xhn::static_string>&);
    NSTask *mTask;
    NSPipe *mPipe;
    NSFileHandle *mFile;
}
- (void) dealloc
{
    ASTLog("here\n");
}

- (void)receivedData:(NSNotification *)notif {
    NSFileHandle *fh = [notif object];
    NSData *data = [fh availableData];
    if (data.length > 0) { // if data is found, re-register for more data (and print)
        [fh waitForDataInBackgroundAndNotify];

        self.parser->Parse((const char*)[data bytes], [data length]);
#if USING_AST_LOG
        fwrite([data bytes], 1, [data length], s_ASTFile);
#endif
    }
    else {
        xhn::string bridgeFile;
        xhn::string stateActionFile;
        self.parser->EndParse(bridgeFile, stateActionFile);
        xhn::vector<xhn::static_string> sceneNodeAgentNameVector = self.parser->GetSceneNodeAgentNameVector();
        xhn::vector<xhn::static_string> guiAgentNameVector = self.parser->GetGUIAgentNameVector();
        xhn::vector<xhn::static_string> actorAgentNameVector = self.parser->GetActorAgentNameVector();
        ASTLog("%s\n", bridgeFile.c_str());
        ASTLog("%s\n", stateActionFile.c_str());
        delete self.parser;
        {
            auto inst = s_SwiftCommandLineUtilsLock.Lock();
            if (s_SwiftCommandLineUtils) {
                [s_SwiftCommandLineUtils removeObject:self];
            }
        }
        mCallback(bridgeFile, stateActionFile, sceneNodeAgentNameVector, guiAgentNameVector, actorAgentNameVector);
    }
}

- (void) runCommand:(NSString*)commandToRun callback:(void(^)(const xhn::string& bridgeFile,
                                                              const xhn::string& stateActionFile,
                                                              const xhn::vector<xhn::static_string>&,
                                                              const xhn::vector<xhn::static_string>&,
                                                              const xhn::vector<xhn::static_string>&))proc
{
    mCallback = proc;
    mTask = [[NSTask alloc] init];
    [mTask setLaunchPath: @"/bin/sh"];
    
    NSArray *arguments = [NSArray arrayWithObjects:
                          @"-c" ,
                          [NSString stringWithFormat:@"%@", commandToRun],
                          nil];
#if USING_AST_LOG
    s_COMMANDFile = fopen("/Users/xhnsworks/测试工程/swiftTest/swiftTest/command.txt", "wb");
#endif
    COMMANDLog("run command: %s", [commandToRun UTF8String]);
#if USING_AST_LOG
    fclose(s_COMMANDFile);
#endif
    [mTask setArguments: arguments];
    
    mPipe = [NSPipe pipe];
    [mTask setStandardOutput: mPipe];
    [mTask setStandardError: [mTask standardOutput]];
    
    mFile = [mPipe fileHandleForReading];
    
    [mFile waitForDataInBackgroundAndNotify];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(receivedData:) name:NSFileHandleDataAvailableNotification object:mFile];
    
    {
        auto inst = s_SwiftCommandLineUtilsLock.Lock();
        if (!s_SwiftCommandLineUtils) {
            s_SwiftCommandLineUtils = [NSMutableSet new];
        }
        [s_SwiftCommandLineUtils addObject:self];
    }
    self.parser = VNEW xhn::SwiftParser();
    self.parser->BeginParse();
    [mTask launch];
}
@end
