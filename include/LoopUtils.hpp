//
//
//

#ifndef TERRACE_LOOPUTILS_H
#define TERRACE_LOOPUTILS_H

#include "Config.hpp"

namespace llvm {
class Instruction;
class BasicBlock;
class Function;
class LoopInfo;
class Loop;
class DominatorTree;
class PostDominatorTree;
} // namespace llvm

namespace commoditas {

llvm::Loop *GetTopLevelLoopFor(const llvm::LoopInfo &LI,
                               const llvm::Instruction &I);

llvm::Loop *GetTopLevelLoopFor(const llvm::LoopInfo &LI,
                               const llvm::BasicBlock &BB);

llvm::BasicBlock *GetPredecessorBlockFor(const llvm::Loop &CurLoop,
                                         llvm::PostDominatorTree &PDT);

llvm::BasicBlock *GetSuccessorBlockFor(const llvm::Loop &CurLoop,
                                       llvm::DominatorTree &DT);

} // namespace commoditas end

#endif // header
