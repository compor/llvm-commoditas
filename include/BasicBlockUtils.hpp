//
//
//

#ifndef TERRACE_BASICBLOCKUTILS_HPP
#define TERRACE_BASICBLOCKUTILS_HPP

#include "Config.hpp"

namespace llvm {
class BasicBlock;
class DominatorTree;
class PostDominatorTree;
template <typename T> class SmallVectorImpl;
} // namespace llvm end

namespace icsa {

void GetIntermediateLoopBlocks(
    llvm::SmallVectorImpl<llvm::BasicBlock *> &IntermediateBlocks,
    const llvm::BasicBlock &StartBB, const llvm::BasicBlock &EndBB,
    llvm::DominatorTree &DT, llvm::PostDominatorTree &PDT);

} // namespace icsa end

#endif // header
