//
//
//

#ifndef TERRACE_FUNCTIONUTILS_HPP
#define TERRACE_FUNCTIONUTILS_HPP

#include "Config.hpp"

namespace llvm {
class Function;
} // namespace llvm end

namespace commoditas {

void DemoteFunctionArgs(llvm::Function &F);

} // namespace commoditas end

#endif // header
