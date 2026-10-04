// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_up_pdcp_metrics.h"
#include "ocudu/ocudulog/log_channel.h"
#include <map>
#include <tuple>

namespace ocudu {

namespace app_services {
class remote_server_metrics_gateway;
} // namespace app_services

class pdcp_metrics_notifier;

/// Consumer for the E2 CU-UP PDCP metrics.
class cu_up_pdcp_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit cu_up_pdcp_metrics_consumer_e2(pdcp_metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  pdcp_metrics_notifier& notifier;
};

/// Consumer for the json CU-UP PDCP metrics.
class cu_up_pdcp_metrics_consumer_json : public app_services::metrics_consumer
{
  struct bearer_key {
    uint32_t  ue_index;
    rb_type_t rb_type;
    uint8_t   rb_id;

    bool operator<(const bearer_key& other) const
    {
      return std::tie(ue_index, rb_type, rb_id) < std::tie(other.ue_index, other.rb_type, other.rb_id);
    }
  };

  struct aggregated_metrics {
    void reset()
    {
      tx             = {};
      rx             = {};
      tx_cpu_usage   = 0.0;
      rx_cpu_usage   = 0.0;
      metrics_period = {};
      is_empty       = true;
    }

    bool is_empty = true;

    pdcp_tx_metrics_container tx;
    pdcp_rx_metrics_container rx;
    double                    tx_cpu_usage = 0.0;
    double                    rx_cpu_usage = 0.0;
    timer_duration            metrics_period;
  };

public:
  cu_up_pdcp_metrics_consumer_json(ocudulog::basic_logger&                      logger_,
                                   app_services::remote_server_metrics_gateway& gateway_,
                                   task_executor&                               executor_,
                                   unique_timer                                 timer_,
                                   unsigned                                     report_period_ms_);

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  // Aggregate one PDCP metrics report.
  void aggregate_metrics(aggregated_metrics& aggr, const pdcp_metrics_container& metric);

  // Print metrics.
  void print_metrics();

  // Clear metrics.
  void clear_metrics()
  {
    aggr_metrics.reset();
    bearer_metrics.clear();
  }

  // Initialize timer.
  void initialize_timer();

  const unsigned                               report_period_ms;
  ocudulog::basic_logger&                      logger;
  app_services::remote_server_metrics_gateway& gateway;
  task_executor&                               executor;
  unique_timer                                 timer;
  aggregated_metrics                           aggr_metrics;
  std::map<bearer_key, aggregated_metrics>     bearer_metrics;
};

/// Consumer for the log PDCP metrics.
class cu_up_pdcp_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit cu_up_pdcp_metrics_consumer_log(ocudulog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    ocudu_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  ocudulog::log_channel& log_chan;
};

} // namespace ocudu
