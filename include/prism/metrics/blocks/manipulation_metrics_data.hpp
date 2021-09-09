/**
 * \file manipulation_metrics_data.hpp
 *
 * \copyright 2021 John Harwell, All rights reserved.
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

#ifndef INCLUDE_PRISM_METRICS_BLOCKS_MANIPULATION_METRICS_DATA_HPP_
#define INCLUDE_PRISM_METRICS_BLOCKS_MANIPULATION_METRICS_DATA_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <atomic>

#include "rcppsw/metrics/base_metrics_data.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(prism, metrics, blocks, detail);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \struct manipulation_metrics_data
 * \ingroup metrics blocks detail
 *
 * \brief Container for holding collected statistics of \ref
 * manipulation_metrics. Must be atomic so counts are valid in parallel metric
 * collection contexts.
 */
struct manipulation_metrics_data {
  std::atomic_size_t arena_pickup_events{0};
  std::atomic_size_t arena_pickup_penalty{0};

  std::atomic_size_t structure_place_events{0};
  std::atomic_size_t structure_place_penalty{0};
};

NS_END(detail);

struct manipulation_metrics_data : public rmetrics::base_metrics_data {
  detail::manipulation_metrics_data interval{};
  detail::manipulation_metrics_data cum{};
};

NS_END(metrics, manipulation, prism);

#endif /* INCLUDE_PRISM_METRICS_BLOCKS_MANIPULATION_METRICS_DATA_HPP_ */