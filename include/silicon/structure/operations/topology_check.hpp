/**
 * \file topology_check.hpp
 *
 * \copyright 2021 John Harwell, All rights reserved.
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

#ifndef INCLUDE_SILICON_STRUCTURE_OPERATIONS_TOPOLOGY_CHECK_HPP_
#define INCLUDE_SILICON_STRUCTURE_OPERATIONS_TOPOLOGY_CHECK_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <set>
#include <vector>

#include "rcppsw/math/vector3.hpp"

#include "silicon/structure/repr/block_spec.hpp"
#include "silicon/structure/repr/slice2D.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
namespace silicon::structure::ds {
class spec_graph;
} /* namespace silicon::structure */

namespace silicon::structure::repr {
class vshell;
} /* namespace silicon::structure::repr */

NS_START(silicon, structure, operations);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class topology_check
 * \ingroup structure operations
 *
 * \brief Perform topological checks as part of determining if a \ref spec_graph
 * corresponds to a constructible \ref structure3D.
 *
 * These include:
 *
 * - The graph contains no holes in any layer other than the top (required for
 *   structures composed of only cube blocks; will be relaxed later.
 */
class topology_check {
 public:
  using simple_hole_type = const ssrepr::block_anchor_spec*;
  using topological_hole_type = std::set<simple_hole_type>;

  topology_check(void) = default;

  /* Not move/copy constructable/assignable by default */
  topology_check(const topology_check&) = delete;
  topology_check& operator=(const topology_check&) = delete;
  topology_check(topology_check&&) = delete;
  topology_check& operator=(topology_check&&) = delete;

  bool operator()(const ssds::spec_graph* graph,
                  const ssrepr::vshell* vshell) const;

 private:
  /**
   * \brief Perform topological checks on a single layer within the \ref
   * spec_graph.
   */
  bool layer_check(const ssrepr::slice2D& layer,
                   const ssds::spec_graph* graph) const;

  /**
   * \brief Get the set of topological holes in the slice (i.e., within a single
   * layer), where each topological hole is a contiguous group of "simple"
   * holes. Contiguousness is determined by N,S,E,W adjacency.
   */
  topological_hole_type topological_holes(const ssrepr::slice2D& layer) const;

  /**
   * \brief Get the set of "simple" holes in the slice.
   */
  std::vector<simple_hole_type> simple_holes(const ssrepr::slice2D& layer,
                                             const ssds::spec_graph* graph) const;

  /**
   * \brief Determine if the host cell of a block is an anchor hole: a cell
   * (x,y,z) in the bounding box of the spec which meets all of the following
   * criteria.
   *
   * 1. Contains no anchor point.
   * 2. Is not on the exterior of the bounding box/a perimeter cell.
   *
   * \todo This check is only currently valid for structures containing only
   * cube and beam blocks.
   */
  bool is_anchor_hole(const ssrepr::block_anchor_spec* spec,
                      const ssrepr::vshell* vshell) const;

  /**
   * \brief Find all extent holes in a layer. Cells (x,y,z) in the bounding box
   * of the layer are considered extent holes if they meet any of the following
   * criteria.
   *
   * 1. Are not anchor holes.
   * 2. Are not crossed by exactly 1 edge of a block's extent (i.e., one of the
   *    neighboring blocks has an extent that is too short or too long). This is
   *    equivalent to saying L1 norm of the difference in coordinates of the two
   *    corresponding vertices (e = (u,v)) is not equal to the edge length.
   *
   * \todo This check is only currently valid for structures containing only
   * cube and beam blocks.
   */
  std::vector<simple_hole_type> extent_holes(ssrepr::slice2D& layer,
                                             const ssrepr::vshell* vshell) const;

  /**
   * \brief Determine if an anchor point is on the exterior of the structure
   * (i.e., has at least one coordinate = 0 or = the max value for that
   * dimension -1).
   */
  bool on_exterior(const ssrepr::block_anchor_spec* spec,
                   const ssrepr::vshell* vshell) const;

  bool anchors_are_adjacent(const ssrepr::block_anchor_spec* spec1,
                            const ssrepr::block_anchor_spec* spec2) const;


  /* clang-format off */
  /* clang-format on */
};

NS_END(operations, structure, silicon);

#endif /* INCLUDE_SILICON_STRUCTURE_OPERATIONS_TOPOLOGY_CHECK_HPP_ */
