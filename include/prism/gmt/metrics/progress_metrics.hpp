/**
 * \file progress_metrics.hpp
 *
 * \copyright 2020 John Harwell, All rights reserved.
 *
 * This file is part of PRISM.
 *
 * PRISM is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * PRISM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * PRISM.  If not, see <http://www.gnu.org/licenses/
 */

#ifndef INCLUDE_PRISM_GMT_METRICS_PROGRESS_METRICS_HPP_
#define INCLUDE_PRISM_GMT_METRICS_PROGRESS_METRICS_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "rcppsw/metrics/base_metrics.hpp"

#include "prism/prism.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(prism, gmt, metrics);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class progress_metrics
 * \ingroup gmt metrics
 *
 * \brief Interface defining the common metrics to be collected from \ref
 * spc_gmt, \ref subtarget objects as they are built.
 */
class progress_metrics : public virtual rmetrics::base_metrics {
 public:
  /**
   * \brief Return if the \ref spc_gmt, \ref subtarget has been completed.
   */
  virtual bool is_complete(void) const = 0;

  /**
   * \brief Return # of blocks that have been placed on the structure/subtarget
   * since the start of constrution/simulation.
   */
  virtual size_t n_total_placed(void) const = 0;

  /**
   * \brief Return # of blocks that have been placed on the structure/subtarget
   * since the start of the current interval.
   */
  virtual size_t n_interval_placed(void) const = 0;

  /**
   * \brief Return the total # of blocks that comprise the structure/subtarget.
   */
  virtual size_t manifest_size(void) const = 0;
};

NS_END(metrics, gmt, prism);

#endif /* INCLUDE_PRISM_GMT_METRICS_PROGRESS_METRICS_HPP_ */