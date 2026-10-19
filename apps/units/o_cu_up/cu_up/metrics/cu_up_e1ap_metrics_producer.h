// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up_metrics.h"

namespace ocudu {

/// CU-UP E1AP metrics producer implementation.
class cu_up_e1ap_metrics_producer_impl : public e1ap_cu_up_metrics_notifier,
                                         public app_services::metrics_producer
{
public:
  explicit cu_up_e1ap_metrics_producer_impl(app_services::metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void report_metrics(const e1ap_cu_up_metrics_container& metrics) override;

  // See interface for documentation.
  void on_new_report_period() override {}

private:
  app_services::metrics_notifier& notifier;
};

} // namespace ocudu
