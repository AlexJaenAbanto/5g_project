// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1u.h"
#include "helpers.h"
#include "ocudu/f1u/cu_up/f1u_rx_metrics.h"
#include "ocudu/f1u/cu_up/f1u_tx_metrics.h"

using namespace ocudu;
using namespace app_helpers;
using namespace json_generators;

static nlohmann::json generate_f1u_tx(const ocuup::f1u_tx_metrics_container& metrics, unsigned period)
{
  nlohmann::json json;

  json["num_sdus"]         = metrics.num_sdus;
  json["num_sdu_bytes"]    = metrics.num_sdu_bytes;
  json["throughput_mbps"]  = period != 0 ? static_cast<double>(metrics.num_sdu_bytes) * 8.0 / (period * 1000.0) : 0.0;
  json["num_dropped_sdus"] = metrics.num_dropped_sdus;
  json["num_sdu_discards"] = metrics.num_sdu_discards;
  json["num_pdus"]         = metrics.num_pdus;

  return json;
}

static nlohmann::json generate_f1u_rx(const ocuup::f1u_rx_metrics_container& metrics, unsigned period)
{
  nlohmann::json json;

  json["num_pdus"]          = metrics.num_pdus;
  json["num_dropped_pdus"]  = metrics.num_dropped_pdus;
  json["num_sdus"]          = metrics.num_sdus;
  json["num_sdu_bytes"]     = metrics.num_sdu_bytes;
  json["throughput_mbps"]   = period != 0 ? static_cast<double>(metrics.num_sdu_bytes) * 8.0 / (period * 1000.0) : 0.0;
  json["num_dds"]           = metrics.num_dds;
  json["num_dds_failures"]  = metrics.num_dds_failures;

  return json;
}

nlohmann::json ocudu::app_helpers::json_generators::generate(const ocuup::f1u_tx_metrics_container& tx,
                                                             const ocuup::f1u_rx_metrics_container& rx,
                                                             timer_duration                         metrics_period)
{
  nlohmann::json json;

  json["timestamp"]          = get_time_stamp();
  nlohmann::json& cu_up_json = json["cu-up"];
  nlohmann::json& f1u_json   = cu_up_json["nrup"];

  f1u_json["dl"] = generate_f1u_tx(tx, metrics_period.count());
  f1u_json["ul"] = generate_f1u_rx(rx, metrics_period.count());

  return json;
}

std::string ocudu::app_helpers::json_generators::generate_string(const ocuup::f1u_tx_metrics_container& tx,
                                                                 const ocuup::f1u_rx_metrics_container& rx,
                                                                 timer_duration                         metrics_period,
                                                                 int                                    indent)
{
  return generate(tx, rx, metrics_period).dump(indent);
}
