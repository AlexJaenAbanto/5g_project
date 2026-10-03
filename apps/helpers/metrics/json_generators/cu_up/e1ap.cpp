// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e1ap.h"
#include "helpers.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up_metrics.h"

using namespace ocudu;
using namespace app_helpers;
using namespace json_generators;

nlohmann::json ocudu::app_helpers::json_generators::generate(const e1ap_cu_up_metrics_container& metrics)
{
  nlohmann::json json;

  json["timestamp"]          = get_time_stamp();
  nlohmann::json& cu_up_json = json["cu-up"];
  nlohmann::json& e1ap_json  = cu_up_json["e1ap"];

  e1ap_json["successful_bearer_context_setups"] =
      metrics.nof_successful_bearer_context_setup;

  e1ap_json["successful_bearer_context_modifications"] =
      metrics.nof_successful_bearer_context_modification;

  e1ap_json["bearer_context_releases"] =
      metrics.nof_bearer_context_release;

  nlohmann::json& latency_json = e1ap_json["release_latency"];

  latency_json["average_us"] =
      metrics.nof_bearer_context_release != 0
          ? static_cast<double>(metrics.sum_release_latency.count()) /
                static_cast<double>(metrics.nof_bearer_context_release)
          : 0.0;

  latency_json["max_us"] = metrics.max_release_latency.count();

  latency_json["histogram"] = metrics.release_latency_hist;

  latency_json["histogram_bin_width_ms"] =
      e1ap_cu_up_metrics_container::nof_msec_per_bin;

  return json;
}

std::string
ocudu::app_helpers::json_generators::generate_string(const e1ap_cu_up_metrics_container& metrics, int indent)
{
  return generate(metrics).dump(indent);
}
