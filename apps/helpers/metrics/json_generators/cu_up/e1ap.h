// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "external/nlohmann/json.hpp"
#include <string>

namespace ocudu {

struct e1ap_cu_up_metrics_container;

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given E1AP CU-UP metrics.
nlohmann::json generate(const e1ap_cu_up_metrics_container& metrics);

/// Generates a string in JSON format that codifies the given E1AP CU-UP metrics.
std::string generate_string(const e1ap_cu_up_metrics_container& metrics, int indent = -1);

} // namespace json_generators
} // namespace app_helpers
} // namespace ocudu
