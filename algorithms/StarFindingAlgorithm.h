/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#pragma once
#include "Algorithm.h"

namespace blastro {

class StarFindingAlgorithm : public Algorithm {
public:
    StarFindingAlgorithm() = default;
    ~StarFindingAlgorithm() override = default;

    std::string name() const override { return "StarFinding"; }
    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;
};

} // namespace blastro
