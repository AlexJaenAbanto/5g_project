// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_up_e1ap_metrics.h"
#include "ocudu/ocudulog/log_channel.h"

namespace ocudu {

namespace app_services {
class remote_server_metrics_gateway;
} // namespace app_services

/// Consumer for the JSON CU-UP E1AP metrics.
class cu_up_e1ap_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit cu_up_e1ap_metrics_consumer_json(app_services::remote_server_metrics_gateway& gateway_) : gateway(gateway_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  app_services::remote_server_metrics_gateway& gateway;
};

/// Consumer for the log CU-UP E1AP metrics.
class cu_up_e1ap_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit cu_up_e1ap_metrics_consumer_log(ocudulog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    ocudu_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  ocudulog::log_channel& log_chan;
};

} // namespace ocudu
