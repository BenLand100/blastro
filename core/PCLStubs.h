/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
