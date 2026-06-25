import os
import re

pcl_api_cpp = "/home/benland100/Desktop/blastro/thirdparty/PCL/src/pcl/APIInterface.cpp"
stubs_h_path = "/home/benland100/Desktop/blastro/core/PCLStubs.h"
stubs_cpp_path = "/home/benland100/Desktop/blastro/core/PCLStubs.cpp"

if not os.path.exists(pcl_api_cpp):
    print(f"Error: {pcl_api_cpp} does not exist.")
    exit(1)

with open(pcl_api_cpp, "r") as f:
    content = f.read()

# Find the array A
match = re.search(r'static\s+const\s+char\*\s+A\[\]\s*=\s*\{(.*?)\};', content, re.DOTALL)
if not match:
    print("Error: Could not find array A in APIInterface.cpp")
    exit(1)

array_content = match.group(1)
# Extract all strings
strings = re.findall(r'"([^"]+)"', array_content)
print(f"Found {len(strings)} API functions.")

# Generate PCLStubs.h
h_code = """#pragma once
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
"""

with open(stubs_h_path, "w") as f:
    f.write(h_code)

# Generate PCLStubs.cpp
cpp_code = []
cpp_code.append('#include "PCLStubs.h"')
cpp_code.append('#include <QDebug>')
cpp_code.append('#include <unordered_map>')
cpp_code.append('#include <string>')
cpp_code.append('')
cpp_code.append('namespace blastro {')
cpp_code.append('')
cpp_code.append('static std::unordered_map<std::string, void*> s_stubMap;')
cpp_code.append('')

# Generate unique stub functions
for s in strings:
    # Replace / with _ for a valid C function name
    func_name = s.replace('/', '_')
    cpp_code.append(f'static void* stub_{func_name}(...) {{')
    cpp_code.append(f'    static int callCount = 0;')
    cpp_code.append(f'    if (callCount++ < 5) {{')
    cpp_code.append(f'        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "{s}";')
    cpp_code.append(f'    }}')
    cpp_code.append(f'    return nullptr;')
    cpp_code.append(f'}}')
    cpp_code.append('')

cpp_code.append('void initPCLStubs() {')
for s in strings:
    func_name = s.replace('/', '_')
    cpp_code.append(f'    s_stubMap["{s}"] = reinterpret_cast<void*>(stub_{func_name});')
cpp_code.append('}')
cpp_code.append('')
cpp_code.append('void* getPCLStub(const std::string& name) {')
cpp_code.append('    auto it = s_stubMap.find(name);')
cpp_code.append('    if (it != s_stubMap.end()) {')
cpp_code.append('        return it->second;')
cpp_code.append('    }')
cpp_code.append('    qWarning() << "PCL STUB NOT FOUND FOR:" << name.c_str();')
cpp_code.append('    return nullptr;')
cpp_code.append('}')
cpp_code.append('')
cpp_code.append('void overridePCLStub(const std::string& name, void* funcPtr) {')
cpp_code.append('    s_stubMap[name] = funcPtr;')
cpp_code.append('}')
cpp_code.append('')
cpp_code.append('} // namespace blastro')

with open(stubs_cpp_path, "w") as f:
    f.write('\n'.join(cpp_code))

print("Successfully generated stubs files.")
