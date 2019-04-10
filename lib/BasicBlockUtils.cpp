//
//
//

#include "BasicBlockUtils.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/IR/Dominators.h"
// using llvm::DominatorTree

#include "llvm/Analysis/PostDominators.h"
// using llvm::PostDominatorTree

#include "llvm/ADT/SmallVector.h"
// using SmallVectorImpl
// using SmallVector

#include <algorithm>
// using std::find
// using std::remove
// using std::for_each

#include <iterator>
// using std::begin
// using std::end

#include <type_traits>
// using std::remove_reference_t

namespace commoditas {

void GetIntermediateLoopBlocks(
    llvm::SmallVectorImpl<llvm::BasicBlock *> &IntermediateBlocks,
    const llvm::BasicBlock &StartBB, const llvm::BasicBlock &EndBB,
    llvm::DominatorTree &DT, llvm::PostDominatorTree &PDT) {
  using BlockTy =
      std::remove_reference_t<decltype(IntermediateBlocks)>::value_type;
  using BlocksVectorTy = llvm::SmallVector<BlockTy, 16>;
  BlocksVectorTy DomDescendants, PostDomDescendants;

  DT.getDescendants(const_cast<BlockTy>(&StartBB), DomDescendants);
  PDT.getDescendants(const_cast<BlockTy>(&EndBB), PostDomDescendants);

  auto IntersectFn = [&](BlocksVectorTy::value_type bb) {
    if (PostDomDescendants.end() !=
        std::find(PostDomDescendants.begin(), PostDomDescendants.end(), bb))
      IntermediateBlocks.push_back(bb);
  };

  std::for_each(DomDescendants.begin(), DomDescendants.end(), IntersectFn);

  auto EraseRemoveFn = [](decltype(IntermediateBlocks) c, BlockTy e) {
    auto p = std::remove(std::begin(c), std::end(c), e);
    if (std::end(c) != p)
      c.erase(p);
  };

  EraseRemoveFn(IntermediateBlocks, const_cast<BlockTy>(&StartBB));
  EraseRemoveFn(IntermediateBlocks, const_cast<BlockTy>(&EndBB));

  return;
}

} // namespace commoditas end
