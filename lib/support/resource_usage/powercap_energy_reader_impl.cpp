// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "powercap_energy_reader_impl.h"
#include <charconv>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using namespace ocudu;
using namespace resource_usage_utils;

// Rutas directas en sysfs según la interfaz del kernel Linux RAPL
static constexpr auto rapl_powercap_pkg_path  = "/sys/class/powercap/intel-rapl:0/energy_uj";
static constexpr auto rapl_powercap_core_path = "/sys/class/powercap/intel-rapl:0:0/energy_uj";

energy_consumption resource_usage_utils::sysfs_powercap_reader::read_consumed_energy() const
{
  energy_consumption probe{};
  // Leer consumo total del socket (Package)
  read_value_from_sysfs(rapl_powercap_pkg_path, probe.package_consumed_uj);
  // Leer consumo exclusivo de cores (si está presente el subdominio)
  read_value_from_sysfs(rapl_powercap_core_path, probe.cpu_core_consumed_uj);

  return probe;
}

uint64_t resource_usage_utils::sysfs_powercap_reader::parse_uint64(const std::string& value) const
{
  uint64_t result{};
  auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);

  if (ec != std::errc()) {
    return 0;
  }
  return result;
}

void resource_usage_utils::sysfs_powercap_reader::read_value_from_sysfs(const std::string& sysfs_path,
                                                                        uint64_t&          value_uj) const
{
  std::ifstream file(sysfs_path);
  if (file.is_open()) {
    std::string line;
    if (std::getline(file, line)) {
      value_uj = parse_uint64(line);
    }
  }
}

std::unique_ptr<energy_consumption_reader>
resource_usage_utils::build_sysfs_powercap_reader(ocudulog::basic_logger& logger)
{
  // Comprobación real de existencia del fichero energy_uj del Package 0
  if (!fs::exists(rapl_powercap_pkg_path)) {
    logger.warning("Energy consumption utils: Powercap sysfs is not available at {}.", rapl_powercap_pkg_path);
    return nullptr;
  }

  logger.info("Energy consumption utils: Successfully initialized RAPL sysfs reader.");
  return std::make_unique<sysfs_powercap_reader>();
}
