// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "sdr.h"
#include "apps/helpers/metrics/helpers.h"
#include "helpers.h"
#include "ocudu/ru/sdr/ru_sdr_metrics.h"

using namespace ocudu;
using namespace app_helpers;
using namespace json_generators;

static nlohmann::json generate_tx(const ru_sdr_sector_metrics& metrics)
{
  nlohmann::json json;

  json["average_power_db"]      = validate_fp_value(metrics.tx_avg_power_dB);
  json["peak_power_db"]         = validate_fp_value(metrics.tx_peak_power_dB);
  json["papr_db"]               = validate_fp_value(metrics.tx_papr_dB);
  json["clipping_probability"]  = validate_fp_value(metrics.tx_clipping_prob);

  return json;
}

static nlohmann::json generate_rx(const ru_sdr_sector_metrics& metrics)
{
  nlohmann::json json;

  json["average_power_db"]      = validate_fp_value(metrics.rx_avg_power_dB);
  json["peak_power_db"]         = validate_fp_value(metrics.rx_peak_power_dB);
  json["papr_db"]               = validate_fp_value(metrics.rx_papr_dB);
  json["clipping_probability"]  = validate_fp_value(metrics.rx_clipping_prob);

  return json;
}

static nlohmann::json generate_sdr_cell(const ru_sdr_sector_metrics& metrics, pci_t pci)
{
  nlohmann::json json;

  json["pci"] = pci;
  json["tx"]  = generate_tx(metrics);
  json["rx"]  = generate_rx(metrics);

  return json;
}

static nlohmann::json generate_radio(const radio_metrics& metrics)
{
  nlohmann::json json;

  json["late_count"]      = metrics.late_count;
  json["underflow_count"] = metrics.underflow_count;
  json["overflow_count"]  = metrics.overflow_count;

  return json;
}

nlohmann::json ocudu::app_helpers::json_generators::generate(const ru_sdr_metrics& metrics,
                                                             span<const pci_t>     pci_sector_map)
{
  nlohmann::json json;

  json["timestamp"] = get_time_stamp();

  auto& json_ru    = json["ru"];
  auto& json_sdr   = json_ru["sdr"];
  json_sdr["radio"] = generate_radio(metrics.radio);

  auto& cells_sdr = json_sdr["cells"];

  for (const auto& cell : metrics.cells) {
    ocudu_assert(cell.sector_id < pci_sector_map.size(),
                 "Sector id '{}' out of range of the pci-sector mapper. Size of the mapper is '{}'",
                 cell.sector_id,
                 pci_sector_map.size());

    cells_sdr.emplace_back(generate_sdr_cell(cell, pci_sector_map[cell.sector_id]));
  }

  return json;
}

std::string ocudu::app_helpers::json_generators::generate_string(const ru_sdr_metrics& metrics,
                                                                 span<const pci_t>     pci_sector_map,
                                                                 int                   indent)
{
  return generate(metrics, pci_sector_map).dump(indent);
}
