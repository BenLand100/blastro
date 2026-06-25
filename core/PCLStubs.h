#pragma once
#include <unordered_map>
#include <string>

namespace blastro {

// Initializes the PCL API stub map.
void initPCLStubs();

// Resolves a function name to its stub pointer.
void* getPCLStub(const std::string& name);

// Allows overriding a stub with a custom implementation.
void overridePCLStub(const std::string& name, void* funcPtr);

} // namespace blastro
