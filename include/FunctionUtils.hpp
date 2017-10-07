//
//
//

#ifndef TERRACE_FUNCTIONUTILS_HPP
#define TERRACE_FUNCTIONUTILS_HPP

#include "Config.hpp"

namespace llvm {
class Function;
} // namespace llvm end

namespace icsa {

void DemoteFunctionArgs(llvm::Function &F);

} // namespace icsa end

#endif // header
