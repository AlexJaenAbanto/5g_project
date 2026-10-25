// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/resource_usage/power_consumption.h"
#include "powercap_energy_reader_impl.h"
#include "rapl_msr_energy_reader_impl.h"
#include <fstream>

using namespace ocudu;
using namespace resource_usage_utils;

namespace {

/// Dummy implementation of energy consumption reader.
class dummy_energy_consumption_reader : public energy_consumption_reader
{
  energy_consumption read_consumed_energy() const override { return {}; }
};

} // namespace

std::unique_ptr<energy_consumption_reader>
resource_usage_utils::build_energy_consumption_reader(ocudulog::basic_logger& logger)
{
  // First try to build Powercap energy consumption reader.
  if (auto reader = build_sysfs_powercap_reader(logger)) {
    return reader;
  }

  // Try to build RAPL MSR energy consumption reader.
  if (auto reader = build_rapl_msr_reader(logger)) {
    return reader;
  }

  logger.info("Energy consumption utils are not available.");
  return std::make_unique<dummy_energy_consumption_reader>();
}

uint64_t resource_usage_utils::calculate_energy_diff(uint64_t current_uj, uint64_t previous_uj)
{
  // Cachear el límite de desbordamiento de hardware (max_energy_range_uj)
  static uint64_t max_limit_uj = []() -> uint64_t {
    std::ifstream limit_file("/sys/class/powercap/intel-rapl:0/max_energy_range_uj");
    uint64_t val = 0;
    if (limit_file >> val && val > 0) {
      return val;
    }
    return 0;
  }();

  if (max_limit_uj > 0) {
    return (current_uj + max_limit_uj - previous_uj) % max_limit_uj;
  }

  // Fallback estándar en caso de no poder leer max_energy_range_uj
  return (current_uj >= previous_uj) ? (current_uj - previous_uj) : 0;
}

