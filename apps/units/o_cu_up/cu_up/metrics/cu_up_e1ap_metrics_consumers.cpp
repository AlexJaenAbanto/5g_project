// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_e1ap_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/cu_up/e1ap.h"
#include "apps/helpers/metrics/json_generators/generator_helpers.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"

using namespace ocudu;

void cu_up_e1ap_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const e1ap_cu_up_metrics_container& e1ap_metric =
      static_cast<const cu_up_e1ap_metrics_impl&>(metric).get_metrics();

  gateway.send(
      app_helpers::json_generators::generate_string(e1ap_metric, DEFAULT_JSON_INDENT));
}

void cu_up_e1ap_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const e1ap_cu_up_metrics_container& e1ap_metric =
      static_cast<const cu_up_e1ap_metrics_impl&>(metric).get_metrics();

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "CU-UP E1AP metrics: {}", format_e1ap_cu_up_metrics({}, e1ap_metric));
  log_chan("{}", to_c_str(buffer));
}
