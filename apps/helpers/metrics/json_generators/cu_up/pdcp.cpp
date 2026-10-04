// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcp.h"
#include "helpers.h"
#include "ocudu/pdcp/pdcp_entity.h"
#include "ocudu/pdcp/pdcp_rx_metrics.h"
#include "ocudu/pdcp/pdcp_tx_metrics.h"

using namespace ocudu;
using namespace app_helpers;
using namespace json_generators;

static nlohmann::json generate_pdcp_tx(const pdcp_tx_metrics_container& metrics, unsigned period, double cpu_usage)
{
  nlohmann::json json;

  // Traffic counters.
  json["num_sdus"]             = metrics.num_sdus;
  json["num_sdu_bytes"]        = metrics.num_sdu_bytes;
  json["num_dropped_sdus"]     = metrics.num_dropped_sdus;
  json["num_pdus"]             = metrics.num_pdus;
  json["num_pdu_bytes"]        = metrics.num_pdu_bytes;
  json["num_discard_timeouts"] = metrics.num_discard_timeouts;

  // Throughput.
  const double sdu_throughput_mbps =
      period > 0 ? static_cast<double>(metrics.num_sdu_bytes) * 8.0 * 1000.0 * 1e-6 /
                       static_cast<double>(period)
                 : 0.0;
  const double pdu_throughput_mbps =
      period > 0 ? static_cast<double>(metrics.num_pdu_bytes) * 8.0 * 1000.0 * 1e-6 /
                       static_cast<double>(period)
                 : 0.0;

  json["sdu_throughput_mbps"] = validate_fp_value(sdu_throughput_mbps);
  json["pdu_throughput_mbps"] = validate_fp_value(pdu_throughput_mbps);

  // Keep the existing field for backwards compatibility. It represents PDU throughput.
  json["average_throughput_mbps"] = validate_fp_value(pdu_throughput_mbps);

  // PDU latency.
  json["average_latency_us"] =
      validate_fp_value(metrics.num_pdus ? static_cast<double>(metrics.sum_pdu_latency_ns) * 1e-3 /
                                              static_cast<double>(metrics.num_pdus)
                                        : 0.0);

  if (metrics.min_pdu_latency_ns) {
    json["min_latency_us"] = validate_fp_value(static_cast<double>(metrics.min_pdu_latency_ns.value()) * 1e-3);
  }

  if (metrics.max_pdu_latency_ns) {
    json["max_latency_us"] = validate_fp_value(static_cast<double>(metrics.max_pdu_latency_ns.value()) * 1e-3);
  }

  json["latency_histogram"]              = metrics.pdu_latency_hist;
  json["histogram_bin_width_us"]         = pdcp_tx_metrics_container::nof_usec_per_bin;
  json["histogram_last_bin_is_overflow"] = true;

  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

static nlohmann::json generate_pdcp_rx(const pdcp_rx_metrics_container& metrics, unsigned period, double cpu_usage)
{
  nlohmann::json json;

  // Traffic counters.
  json["num_sdus"]           = metrics.num_sdus;
  json["num_sdu_bytes"]      = metrics.num_sdu_bytes;
  json["num_pdus"]           = metrics.num_pdus;
  json["num_pdu_bytes"]      = metrics.num_pdu_bytes;
  json["num_data_pdus"]      = metrics.num_data_pdus;
  json["num_data_pdu_bytes"] = metrics.num_data_pdu_bytes;
  json["num_dropped_pdus"]   = metrics.num_dropped_pdus;

  // Integrity and reordering.
  json["num_integrity_verified_pdus"] = metrics.num_integrity_verified_pdus;
  json["num_integrity_failed_pdus"]   = metrics.num_integrity_failed_pdus;
  json["num_t_reordering_timeouts"]   = metrics.num_t_reordering_timeouts;

  json["average_reordering_delay_us"] =
      validate_fp_value(metrics.reordering_counter > 0
                            ? static_cast<double>(metrics.reordering_delay_us) /
                                  static_cast<double>(metrics.reordering_counter)
                            : 0.0);

  // Throughput.
  const double sdu_throughput_mbps =
      period > 0 ? static_cast<double>(metrics.num_sdu_bytes) * 8.0 * 1000.0 * 1e-6 /
                       static_cast<double>(period)
                 : 0.0;
  const double pdu_throughput_mbps =
      period > 0 ? static_cast<double>(metrics.num_pdu_bytes) * 8.0 * 1000.0 * 1e-6 /
                       static_cast<double>(period)
                 : 0.0;

  json["sdu_throughput_mbps"] = validate_fp_value(sdu_throughput_mbps);
  json["pdu_throughput_mbps"] = validate_fp_value(pdu_throughput_mbps);

  // Keep the existing field for backwards compatibility. It represents PDU throughput.
  json["average_throughput_mbps"] = validate_fp_value(pdu_throughput_mbps);

  // SDU latency.
  json["average_latency_us"] =
      validate_fp_value(metrics.num_sdus ? static_cast<double>(metrics.sum_sdu_latency_ns) * 1e-3 /
                                              static_cast<double>(metrics.num_sdus)
                                        : 0.0);

  if (metrics.min_sdu_latency_ns) {
    json["min_latency_us"] = validate_fp_value(static_cast<double>(metrics.min_sdu_latency_ns.value()) * 1e-3);
  }

  if (metrics.max_sdu_latency_ns) {
    json["max_latency_us"] = validate_fp_value(static_cast<double>(metrics.max_sdu_latency_ns.value()) * 1e-3);
  }

  json["latency_histogram"]              = metrics.sdu_latency_hist;
  json["histogram_bin_width_us"]         = pdcp_rx_metrics_container::nof_usec_per_bin;
  json["histogram_last_bin_is_overflow"] = true;

  json["cpu_usage_percent"] = validate_fp_value(cpu_usage);

  return json;
}

nlohmann::json ocudu::app_helpers::json_generators::generate(const pdcp_tx_metrics_container& tx,
                                                             const pdcp_rx_metrics_container& rx,
                                                             double                           tx_cpu_usage,
                                                             double                           rx_cpu_usage,
                                                             timer_duration                   metrics_period)
{
  nlohmann::json json;

  json["timestamp"]          = get_time_stamp();
  nlohmann::json& cu_up_json = json["cu-up"];
  nlohmann::json& pdcp_json  = cu_up_json["pdcp"];

  pdcp_json["dl"] = generate_pdcp_tx(tx, metrics_period.count(), tx_cpu_usage);
  pdcp_json["ul"] = generate_pdcp_rx(rx, metrics_period.count(), rx_cpu_usage);

  return json;
}

std::string ocudu::app_helpers::json_generators::generate_string(const pdcp_tx_metrics_container& tx,
                                                                 const pdcp_rx_metrics_container& rx,
                                                                 double                           tx_cpu_usage,
                                                                 double                           rx_cpu_usage,
                                                                 timer_duration                   metrics_period,
                                                                 int                              indent)
{
  return generate(tx, rx, tx_cpu_usage, rx_cpu_usage, metrics_period).dump(indent);
}
