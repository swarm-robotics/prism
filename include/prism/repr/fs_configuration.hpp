/**
 * \file fs_configuration.hpp
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

#ifndef INCLUDE_PRISM_REPR_FS_CONFIGURATION_HPP_
#define INCLUDE_PRISM_REPR_FS_CONFIGURATION_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "prism/prism.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(prism, repr);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \brief Represents the various types of stimulating fs configurations
 * that robots can encounter in the environment as they proceed down their
 * construction lane.
 *
 * \ingroup repr
 */
enum class fs_configuration {
  /**
   * No configuration has been encountred.
   */
  ekNONE,

  /**
   * A configuration in which the robot has made it close enough to the back of
   * a construction lane to determine that it is in fact empty.
   */
  ekLANE_EMPTY,

  /**
   * A configuration in which BOTH the ingress and egress lanes in the
   * construction lanes have blocks placed up to the same location in X or Y
   * (depending on structure orientation).
   */
  ekLANE_FILLED,

  /**
   * A configuration in which the ingress lane is missing a block, and there is
   * one in the egress lane.
   */
  ekLANE_GAP_INGRESS,

  /**
   * A configuration in which the egress lane is missing a block, and there is
   * one in the ingress lane.
   *
   */
  ekLANE_GAP_EGRESS
};

NS_END(repr, prism);

#endif /* INCLUDE_PRISM_REPR_FS_CONFIGURATION_HPP_ */