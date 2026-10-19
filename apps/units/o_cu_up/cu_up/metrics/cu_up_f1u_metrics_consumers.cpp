// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_f1u_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/cu_up/f1u.h"
#include "apps/helpers/metrics/json_generators/generator_helpers.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "cu_up_f1u_metrics.h"
#include "ocudu/f1u/cu_up/f1u_metrics.h"

using namespace ocudu;

void cu_up_f1u_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const cu_up_f1u_metrics_impl&>(metric).get_metrics());
}

cu_up_f1u_metrics_consumer_json::cu_up_f1u_metrics_consumer_json(ocudulog::basic_logger& logger_,
                                                                 app_services::remote_server_metrics_gateway& gateway_,
                                                                 task_executor&          executor_,
                                                                 unique_timer            timer_,
                                                                 unsigned                report_period_ms_) :
  report_period_ms(report_period_ms_),
  logger(logger_),
  gateway(gateway_),
  executor(executor_),
  timer(std::move(timer_))
{
  ocudu_assert(report_period_ms > 10, "CU-UP report period is too fast to work with current JSON consumer");
  ocudu_assert(timer.is_valid(), "Invalid timer passed to metrics controller");

  // Shift the timer a little.
  timer.set(std::chrono::milliseconds(report_period_ms / 10), [this](timer_id_t tid) { initialize_timer(); });
  timer.run();
}

void cu_up_f1u_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  // Implement aggregation.
  const ocuup::f1u_metrics_container& f1u_metric = static_cast<const cu_up_f1u_metrics_impl&>(metric).get_metrics();

  // Tx aggregation.
  const ocuup::f1u_tx_metrics_container& tx_metric = f1u_metric.tx;
  ocuup::f1u_tx_metrics_container&       aggr_tx   = aggr_metrics.tx;
  aggr_tx.num_sdus += tx_metric.num_sdus;
  aggr_tx.num_sdu_bytes += tx_metric.num_sdu_bytes;
  aggr_tx.num_dropped_sdus += tx_metric.num_dropped_sdus;
  aggr_tx.num_sdu_discards += tx_metric.num_sdu_discards;
  aggr_tx.num_pdus += tx_metric.num_pdus;

  // Rx aggregation.
  const ocuup::f1u_rx_metrics_container& rx_metric = f1u_metric.rx;
  ocuup::f1u_rx_metrics_container&       aggr_rx   = aggr_metrics.rx;
  aggr_rx.num_pdus += rx_metric.num_pdus;
  aggr_rx.num_dropped_pdus += rx_metric.num_dropped_pdus;
  aggr_rx.num_sdus += rx_metric.num_sdus;
  aggr_rx.num_sdu_bytes += rx_metric.num_sdu_bytes;
  aggr_rx.num_dds += rx_metric.num_dds;
  aggr_rx.num_dds_failures += rx_metric.num_dds_failures;

  aggr_metrics.metrics_period = f1u_metric.metrics_period;

  aggr_metrics.is_empty = false;
}

void cu_up_f1u_metrics_consumer_json::print_metrics()
{
  if (aggr_metrics.is_empty) {
    return;
  }

  gateway.send(app_helpers::json_generators::generate_string(
      aggr_metrics.tx, aggr_metrics.rx, aggr_metrics.metrics_period, DEFAULT_JSON_INDENT));

  // Clear metrics after printing.
  clear_metrics();
}

void cu_up_f1u_metrics_consumer_json::initialize_timer()
{
  timer.set(std::chrono::milliseconds(report_period_ms), [this](timer_id_t tid) {
    if (!executor.execute([this]() {
          print_metrics();
          timer.run();
        })) {
      logger.warning("Failed to enqueue task to print CU-UP metrics");
    }
  });
  timer.run();
}

void cu_up_f1u_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  // Implement aggregation.
  const ocuup::f1u_metrics_container& f1u_metric = static_cast<const cu_up_f1u_metrics_impl&>(metric).get_metrics();

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "NRUP Metrics: {}", f1u_metric);
  log_chan("{}", to_c_str(buffer));
}
