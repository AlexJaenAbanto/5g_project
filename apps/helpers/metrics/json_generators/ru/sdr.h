// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "external/nlohmann/json.hpp"
#include "ocudu/adt/span.h"
#include "ocudu/ran/pci.h"
#include <string>

namespace ocudu {

struct ru_sdr_metrics;

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given SDR Radio Unit metrics.
nlohmann::json generate(const ru_sdr_metrics& metrics, span<const pci_t> pci_sector_map);

/// Generates a string in JSON format that codifies the given SDR Radio Unit metrics.
std::string generate_string(const ru_sdr_metrics& metrics,
                            span<const pci_t>      pci_sector_map,
                            int                    indent = -1);

} // namespace json_generators
} // namespace app_helpers
} // namespace ocudu
