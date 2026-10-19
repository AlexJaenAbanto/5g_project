// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "external/nlohmann/json.hpp"
#include "ocudu/pdcp/pdcp_rx_metrics.h"
#include "ocudu/pdcp/pdcp_tx_metrics.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/support/timers.h"
#include <string>
#include <vector>

namespace ocudu {

struct pdcp_bearer_metrics {
  uint32_t                  ue_index;
  rb_type_t                 rb_type;
  uint8_t                   rb_id;
  pdcp_tx_metrics_container tx;
  pdcp_rx_metrics_container rx;
  double                    tx_cpu_usage;
  double                    rx_cpu_usage;
  timer_duration            metrics_period;
};

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given PDCP metrics.
nlohmann::json generate(const pdcp_tx_metrics_container&          tx,
                        const pdcp_rx_metrics_container&          rx,
                        double                                    tx_cpu_usage,
                        double                                    rx_cpu_usage,
                        timer_duration                            metrics_period,
                        const std::vector<pdcp_bearer_metrics>& bearers);

/// Generates a string in JSON format that codifies the given PDCP metrics.
std::string generate_string(const pdcp_tx_metrics_container&          tx,
                            const pdcp_rx_metrics_container&          rx,
                            double                                    tx_cpu_usage,
                            double                                    rx_cpu_usage,
                            timer_duration                            metrics_period,
                            const std::vector<pdcp_bearer_metrics>& bearers,
                            int                                       indent = -1);

} // namespace json_generators
} // namespace app_helpers
} // namespace ocudu
