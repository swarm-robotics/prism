/**
 * \file construction_loop_functions.hpp
 *
 * \copyright 2020 John Harwell, All rights reserved.
 *
 * This file is part of SILICON.
 *
 * SILICON is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * SILICON is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * SILICON.  If not, see <http://www.gnu.org/licenses/
 */

#ifndef INCLUDE_SILICON_SUPPORT_CONSTRUCTION_LOOP_FUNCTIONS_HPP_
#define INCLUDE_SILICON_SUPPORT_CONSTRUCTION_LOOP_FUNCTIONS_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <memory>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

#include "rcppsw/ds/type_map.hpp"
#include "rcppsw/mpl/typelist.hpp"

#include "cosm/foraging/operations/robot_los_update.hpp"
#include "cosm/controller/operations/metrics_extract.hpp"

#include "silicon/support/base_loop_functions.hpp"
#include "silicon/controller/controller_fwd.hpp"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace silicon::metrics {
class silicon_metrics_aggregator;
} /* namespace silicon::metrics */

NS_START(silicon, support);

template<typename ControllerType>
class robot_arena_interactor;

namespace detail {
struct functor_maps_initializer;
} /* namespace detail */

/*******************************************************************************
 * Classes
 ******************************************************************************/
/**
 * \class construction_loop_functions
 * \ingroup support construction
 *
 * \brief Contains the simulation support functions for construction,
 * such as:
 *
 * - Metric collection from robots
 * - Robot arena interactions
 */
class construction_loop_functions : public base_loop_functions,
                                    public rer::client<construction_loop_functions> {
 public:
  construction_loop_functions(void) RCSW_COLD;
  ~construction_loop_functions(void) override RCSW_COLD;

  /* swarm manager overrides */
  void init(ticpp::Element& node) override RCSW_COLD;
  void pre_step(void) override;
  void post_step(void) override;
  void reset(void) override RCSW_COLD;
  void destroy(void) override RCSW_COLD;

 protected:
  /**
   * \brief Initialize construction support to be shared with derived classes:
   *
   * - Construction metric collection
   */
  void shared_init(ticpp::Element& node) RCSW_COLD;

 private:
  using interactor_map_type = rds::type_map<
   rmpl::typelist_wrap_apply<controller::typelist,
                             robot_arena_interactor>::type>;
  using los2D_updater_map_type = rds::type_map<
    rmpl::typelist_wrap_apply<controller::typelist,
                              cfops::robot_los_update,
                              carena::base_arena_map<crepr::base_block3D>>::type>;

  using metric_extraction_typelist = rmpl::typelist<
    ccops::metrics_extract<controller::constructing_controller,
                           metrics::silicon_metrics_aggregator>
    >;

  using metric_extraction_map_type = rds::type_map<metric_extraction_typelist>;

  /**
   * \brief These are friend classes because they are basically just pieces of
   * the loop functions pulled out for increased clarity/modularity, and are not
   * meant to be used in other contexts.
   *
   * Doing things this way rather than passing 8 parameters to the functors
   * seemed much cleaner.
   */
  friend detail::functor_maps_initializer;

  /**
   * \brief Initialize construction support not shared with derived classes:
   *
   * - Robot interactions with arena
   * - Various maps mapping controller types to metric collection, controller
   *   initialization, and arena interaction maps (reflection basically).
   */
  void private_init(void) RCSW_COLD;

  /**
   * \brief Process a single robot on a timestep, before running its controller:
   *
   * - Set its new position, time from ARGoS and send it its LOS.
   *
   * \note These operations are done in parallel for all robots (lock free).
   */
  void robot_pre_step(argos::CFootBotEntity& robot);

  /**
   * \brief Process a single robot on a timestep, after running its controller.
   *
   * - Have it interact with the environment.
   * - Collect metrics from it.
   *
   * \note These operations are done in parallel for all robots (with mutual
   *       exclusion as needed).
   */
  void robot_post_step(argos::CFootBotEntity& robot);

  argos::CColor GetFloorColor(const argos::CVector2& plane_pos) override RCSW_PURE;

  /**
   * \brief Update the LOS for a robot currently in the arena somewhere, and not
   * on the structure.
   */

  void robot_los2D_update(controller::constructing_controller* c);

  /* clang-format off */
  std::unique_ptr<metrics::silicon_metrics_aggregator> m_metrics_agg;
  std::unique_ptr<interactor_map_type>                 m_interactor_map;
  std::unique_ptr<metric_extraction_map_type>          m_metrics_map;
  std::unique_ptr<los2D_updater_map_type>              m_los2D_update_map;
  /* clang-format on */
};

NS_END(support, silicon);

#endif /* INCLUDE_SILICON_SUPPORT_CONSTRUCTION_LOOP_FUNCTIONS_HPP_ */