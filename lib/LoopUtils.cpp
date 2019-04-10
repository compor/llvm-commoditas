//
//
//

#include "LoopUtils.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/Dominators.h"
// using llvm::DominatorTree

#include "llvm/Analysis/PostDominators.h"
// using llvm::PostDominatorTree

#include "llvm/Analysis/LoopInfo.h"
// using llvm::LoopInfo
// using llvm::Loop

#include "llvm/IR/CFG.h"
// using llvm::pred_begin
// using llvm::pred_end

namespace commoditas {

llvm::Loop *GetTopLevelLoopFor(const llvm::LoopInfo &LI,
                               const llvm::BasicBlock &BB) {
  auto *CurLoop = LI.getLoopFor(&BB);

  if (CurLoop)
    while (CurLoop->getParentLoop())
      CurLoop = CurLoop->getParentLoop();

  return CurLoop;
}

llvm::Loop *GetTopLevelLoopFor(const llvm::LoopInfo &LI,
                               const llvm::Instruction &I) {
  return GetTopLevelLoopFor(LI, *(I.getParent()));
}

llvm::BasicBlock *GetPredecessorBlockFor(const llvm::Loop &CurLoop,
                                         llvm::PostDominatorTree &PDT) {
  auto *Header = CurLoop.getHeader();
  auto *LoopPredBlock = CurLoop.getLoopPreheader();
  using BlockTy = decltype(Header);

  if (!LoopPredBlock) {
    llvm::SmallVector<BlockTy, 16> OutsideBlocks;

    std::for_each(llvm::pred_begin(Header), llvm::pred_end(Header),
                  [&](BlockTy e) {
                    if (!CurLoop.contains(e))
                      OutsideBlocks.push_back(e);
                  });

    LoopPredBlock = OutsideBlocks[0];
    for (auto *BB : OutsideBlocks)
      LoopPredBlock = PDT.findNearestCommonDominator(LoopPredBlock, BB);
  }

  return LoopPredBlock;
}

llvm::BasicBlock *GetSuccessorBlockFor(const llvm::Loop &CurLoop,
                                       llvm::DominatorTree &DT) {
  auto *Header = CurLoop.getHeader();
  auto *LoopSuccBlock = CurLoop.getExitBlock();
  using BlockTy = decltype(Header);

  if (!LoopSuccBlock) {
    llvm::SmallVector<BlockTy, 16> OutsideBlocks;
    CurLoop.getExitBlocks(OutsideBlocks);

    LoopSuccBlock = OutsideBlocks[0];
    for (auto *BB : OutsideBlocks)
      LoopSuccBlock = DT.findNearestCommonDominator(LoopSuccBlock, BB);
  }

  return LoopSuccBlock;
}

} // namespace commoditas end
