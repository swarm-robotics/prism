/**
 * \file construction_loop_functions.cpp
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

/*******************************************************************************
 * Includes
 ******************************************************************************/
/*
 * This is needed because without it boost instantiates static assertions that
 * verify that every possible handler<controller> instantiation is valid. These
 * will not happen in reality (or shouldn't), and if they do it's 100% OK to
 * crash with an exception.
 */
#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
#include "silicon/support/construction_loop_functions.hpp"

#include <boost/mpl/for_each.hpp>

#include "cosm/convergence/convergence_calculator.hpp"
#include "cosm/arena/config/arena_map_config.hpp"
#include "cosm/metrics/blocks/transport_metrics_collector.hpp"
#include "cosm/pal/argos_swarm_iterator.hpp"
#include "cosm/operations/robot_arena_interaction_applicator.hpp"

#include "silicon/metrics/silicon_metrics_aggregator.hpp"
#include "silicon/support/robot_arena_interactor.hpp"
#include "silicon/support/robot_configurer.hpp"
#include "cosm/controller/operations/metrics_extract.hpp"
#include "cosm/controller/operations/applicator.hpp"
#include "silicon/support/interactor_status.hpp"
#include "silicon/metrics/silicon_metrics_aggregator.hpp"
#include "silicon/support/interactor_status.hpp"

#include "silicon/support/tv/tv_manager.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(silicon, support);

/*******************************************************************************
 * Struct Definitions
 ******************************************************************************/
NS_START(detail);

using configurer_map_type = rds::type_map<
  rmpl::typelist_wrap_apply<controller::typelist,
                            robot_configurer>::type>;

/**
 * \struct functor_maps_initializer
 * \ingroup support construction detail
 *
 * Convenience class containing initialization for all of the typeid ->
 * boost::variant maps for all controller types that are used throughout
 * initialization and simulation.
 */
struct functor_maps_initializer {
  RCSW_COLD functor_maps_initializer(configurer_map_type* const cmap,
                                     construction_loop_functions* const lf_in)

      : lf(lf_in), config_map(cmap) {}
  template <typename T>
  RCSW_COLD void operator()(const T& controller) const {
    lf->m_interactor_map->emplace(
        typeid(controller),
        robot_arena_interactor<T>(lf->arena_map(),
                                  lf->m_metrics_agg.get(),
                                  lf->floor(),
                                  lf->tv_manager()->dynamics<ctv::dynamics_type::ekENVIRONMENT>()));
    lf->m_metrics_map->emplace(
        typeid(controller),
        ccops::metrics_extract<T, metrics::silicon_metrics_aggregator>(
            lf->m_metrics_agg.get()));
    config_map->emplace(
        typeid(controller),
        robot_configurer<T>(
            lf->config()->config_get<cvconfig::visualization_config>()));
    lf->m_los2D_update_map->emplace(typeid(controller),
                                    cfops::robot_los_update<T,
                                    carena::base_arena_map<crepr::base_block3D>>(lf->arena_map()));
  }

  /* clang-format off */
  construction_loop_functions * const      lf;
  detail::configurer_map_type* const config_map;
  /* clang-format on */
};

NS_END(detail);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
construction_loop_functions::construction_loop_functions(void)
    : ER_CLIENT_INIT("silicon.loop.construction"),
      m_metrics_agg(nullptr),
      m_interactor_map(nullptr),
      m_metrics_map(nullptr),
      m_los2D_update_map(nullptr) {}

construction_loop_functions::~construction_loop_functions(void) = default;

/*******************************************************************************
 * Initialization Functions
 ******************************************************************************/
void construction_loop_functions::init(ticpp::Element& node) {
  ndc_push();
  ER_INFO("Initializing...");

  shared_init(node);
  private_init();

  ER_INFO("Initialization finished");
  ndc_pop();
} /* init() */

void construction_loop_functions::shared_init(ticpp::Element& node) {
  base_loop_functions::init(node);
} /* shared_init() */

void construction_loop_functions::private_init(void) {
  /*
   * Need to give spatial metrics collectors the padded arena size in order to
   * avoid boost::assert failures when robots are near the upper edge of the
   * arena map. The arena map pads the size obtained from the XML file after
   * initialization, so we just need to grab it.
   */
  auto padded_size = rmath::vector2d(arena_map()->xrsize(),
                                     arena_map()->yrsize());
  auto arena = *config()->config_get<caconfig::arena_map_config>();
  auto* output = config()->config_get<cmconfig::output_config>();
  arena.grid.upper = padded_size;
  m_metrics_agg = std::make_unique<metrics::silicon_metrics_aggregator>(&output->metrics,
                                                                        &arena.grid,
                                                                        output_root());
  /* this starts at 0, and ARGoS starts at 1, so sync up */
  m_metrics_agg->timestep_inc_all();

  m_interactor_map = std::make_unique<interactor_map_type>();
  m_metrics_map = std::make_unique<metric_extraction_map_type>();
  m_los2D_update_map = std::make_unique<los2D_updater_map_type>();

  /* only needed for initialization, so not a member */
  auto config_map = detail::configurer_map_type();

  /*
   * Intitialize controller interactions with environment via various
   * functors/type maps for all construction controller types.
   */
  detail::functor_maps_initializer f_initializer(&config_map, this);
  /* boost::mpl::for_each<controller::typelist>(f_initializer); */

  /* configure robots */
  auto cb = [&](auto* controller) {
    /* ER_ASSERT(config_map.end() != config_map.find(controller->type_index()), */
    /*           "Controller '%s' type '%s' not in construction configuration map", */
    /*           controller->GetId().c_str(), */
    /*           controller->type_index().name()); */
    /* auto applicator = ccops::applicator<controller::constructing_controller, */
    /* robot_configurer>(controller); */
    /* /\* boost::apply_visitor(applicator, *\/ */
    /*                      config_map.at(controller->type_index())); */
  };

  /*
   * Even though this CAN be done in dynamic order, during initialization ARGoS
   * threads are not set up yet so doing dynamicaly causes a deadlock. Also, it
   * only happens once, so it doesn't really matter if it is slow.
   */
  cpal::argos_swarm_iterator::controllers<argos::CFootBotEntity,
                                          controller::constructing_controller,
                                          cpal::iteration_order::ekSTATIC>(
                                              this, cb, kARGoSRobotType);
} /* private_init() */

/*******************************************************************************
 * ARGoS Hooks
 ******************************************************************************/
void construction_loop_functions::pre_step(void) {
  ndc_push();
  base_loop_functions::pre_step();
  ndc_pop();

  /* Process all robots */
  auto cb = [&](argos::CControllableEntity* robot) {
    ndc_push();
    robot_pre_step(dynamic_cast<argos::CFootBotEntity&>(robot->GetParent()));
    ndc_pop();
  };
  cpal::argos_swarm_iterator::robots<cpal::iteration_order::ekDYNAMIC>(this, cb);
} /* pre_step() */

void construction_loop_functions::post_step(void) {
  ndc_push();
  base_loop_functions::post_step();
  ndc_pop();

  /* Process all robots: interact with environment then collect metrics */
  auto cb = [&](argos::CControllableEntity* robot) {
    ndc_push();
    robot_post_step(dynamic_cast<argos::CFootBotEntity&>(robot->GetParent()));
    ndc_pop();
  };
  cpal::argos_swarm_iterator::robots<cpal::iteration_order::ekDYNAMIC>(this, cb);

  ndc_push();
  /* Update block distribution status */
  auto* collector =
      m_metrics_agg->get<cmetrics::blocks::transport_metrics_collector>(
          "blocks::transport");
  arena_map()->redist_governor()->update(
      rtypes::timestep(GetSpace().GetSimulationClock()),
      collector->cum_transported(),
      false); /* @todo never converged until that stuff is incorporated... */

  /* Collect metrics from loop functions */
  m_metrics_agg->collect_from_loop(this);

  m_metrics_agg->metrics_write(rmetrics::output_mode::ekTRUNCATE);
  m_metrics_agg->metrics_write(rmetrics::output_mode::ekCREATE);

  /* Not a clean way to do this in the metrics collectors... */
  if (m_metrics_agg->metrics_write(rmetrics::output_mode::ekAPPEND)) {
    tv_manager()->dynamics<ctv::dynamics_type::ekPOPULATION>()->reset_metrics();
  }
  m_metrics_agg->interval_reset_all();
  m_metrics_agg->timestep_inc_all();

  ndc_pop();
} /* post_step() */

void construction_loop_functions::destroy(void) {
  if (nullptr != m_metrics_agg) {
    m_metrics_agg->finalize_all();
  }
} /* destroy() */

void construction_loop_functions::reset(void) {
  ndc_push();
  base_loop_functions::reset();
  m_metrics_agg->reset_all();
  ndc_pop();
} /* reset() */

argos::CColor construction_loop_functions::GetFloorColor(
    const argos::CVector2& plane_pos) {
  rmath::vector2d tmp(plane_pos.GetX(), plane_pos.GetY());
  if (arena_map()->nest().contains_point(tmp)) {
    return argos::CColor(arena_map()->nest().color().red(),
                         arena_map()->nest().color().green(),
                         arena_map()->nest().color().blue());
  }

  for (auto* block : arena_map()->blocks()) {
    /*
     * If Z > 0, the block is on the structure, so no need to even do the
     * comparisons for determining floor structure.
     */
    if (block->rloc3D().z() > 0) {
      continue;
    }
    /*
     * Even though each block type has a unique color, the only distinction
     * that robots can make to determine if they are on a block or not is
     * between shades of black/white. So, all blocks must appear as black, even
     * when they are not actually (when blocks are picked up their correct color
     * is shown through visualization).
     */
    if (block->contains_point2D(tmp)) {
      return argos::CColor::BLACK;
    }
  } /* for(block..) */

  return argos::CColor::WHITE;
} /* GetFloorColor() */

/*******************************************************************************
 * General Member Functions
 ******************************************************************************/
void construction_loop_functions::robot_pre_step(argos::CFootBotEntity& robot) {
  /* auto controller = static_cast<controller::constructing_controller*>( */
  /*     &robot.GetControllableEntity().GetController()); */

  /*
   * Update robot position, time. This can't be done as part of the robot's
   * control step because we need access to information only available in the
   * loop functions.
   */
  /* controller->sensing_update(rtypes::timestep(GetSpace().GetSimulationClock()), */
  /*                            arena_map()->grid_resolution()); */

  /*
   * @todo Unconditionally use the foraging LOS for now. Once I'm ready to have
   * robots traverse the structure, this will need to change to the 2D foraging
   * LOS if they are in the arena, and the 3D construction LOS if they are on
   * the structure.
   */
  /* robot_los2D_update(controller); */
} /* robot_pre_step() */

void construction_loop_functions::robot_post_step(argos::CFootBotEntity& robot) {
  /* auto controller = static_cast<controller::constructing_controller*>( */
  /*     &robot.GetControllableEntity().GetController()); */
  /*
   * Watch the robot interact with its environment after physics have been
   * updated and its controller has run.
   */
  /* auto it = m_interactor_map->find(controller->type_index()); */
  /* ER_ASSERT(m_interactor_map->end() != it, */
  /*           "Controller '%s' type '%s' not in construction interactor map", */
  /*           controller->GetId().c_str(), */
  /*           controller->type_index().name()); */

  /* auto iapplicator = */
  /*     cops::robot_arena_interaction_applicator<controller::constructing_controller, */
  /*                                              robot_arena_interactor>( */
  /*                                                  controller, */
  /*                                                  rtypes::timestep(GetSpace().GetSimulationClock())); */
  /* /\* auto status = *\/ */
  /*     boost::apply_visitor(iapplicator, */
  /*                          m_interactor_map->at(controller->type_index())); */

  /*
   * Collect metrics from robot, now that it has finished interacting with the
   * environment and no more changes to its state will occur this timestep.
   */
  /* auto it2 = m_metrics_map->find(controller->type_index()); */
  /* ER_ASSERT(m_metrics_map->end() != it2, */
  /*           "Controller '%s' type '%s' not in construction metrics map", */
  /*           controller->GetId().c_str(), */
  /*           controller->type_index().name()); */
  /* auto mapplicator = ccops::applicator<controller::constructing_controller, */
  /*                                      ccops::metrics_extract, */
  /*                                      metrics::silicon_metrics_aggregator>(controller); */
  /* boost::apply_visitor(mapplicator, m_metrics_map->at(controller->type_index())); */

  /* controller->block_manip_recorder()->reset(); */
} /* robot_post_step() */

void construction_loop_functions::robot_los2D_update(controller::constructing_controller* c) {
    /* Send robot its new LOS */
  /* auto it = m_los2D_update_map->find(c->type_index()); */
  /* ER_ASSERT(m_los2D_update_map->end() != it, */
  /*           "Controller '%s' type '%s' not in construction LOS update map", */
  /*           c->GetId().c_str(), */
  /*           c->type_index().name()); */
  /* auto applicator = ccops::applicator<controller::constructing_controller, */
  /*                                     cfops::robot_los_update>(c); */
/*   boost::apply_visitor(applicator, */
/*                        m_los2D_update_map->at(c->type_index())); */
} /* robot_los2D_update() */

using namespace argos; // NOLINT

RCPPSW_WARNING_DISABLE_PUSH()
RCPPSW_WARNING_DISABLE_MISSING_VAR_DECL()
RCPPSW_WARNING_DISABLE_MISSING_PROTOTYPE()
RCPPSW_WARNING_DISABLE_GLOBAL_CTOR()

REGISTER_LOOP_FUNCTIONS(construction_loop_functions, "construction_loop_functions");

RCPPSW_WARNING_DISABLE_POP()

NS_END(support, silicon);