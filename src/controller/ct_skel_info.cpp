/**
 * \file ct_skel_info.cpp
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
#include "silicon/controller/perception/ct_skel_info.hpp"

#include "silicon/structure/structure3D.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(silicon, controller, perception);

/*******************************************************************************
 * Constructors/Destructors
 ******************************************************************************/

/*******************************************************************************
 * Member Functions
 ******************************************************************************/
rmath::vector3d ct_skel_info::originr(void) const {
  return m_target->originr();
} /* originr() */

rmath::vector3z ct_skel_info::origind(void) const {
  return m_target->origind();
} /* origind() */

rmath::vector3d ct_skel_info::bbr(void) const {
  return {m_target->xrsize(), m_target->yrsize(), m_target->zrsize()};
} /* bbr() */

rmath::vector3z ct_skel_info::bbd(void) const {
  return {m_target->xdsize(), m_target->ydsize(), m_target->zdsize()};
} /* bbd() */

const rmath::radians& ct_skel_info::orientation(void) const {
  return m_target->orientation();
} /* orientation() */

rmath::vector3d ct_skel_info::cell_loc_abs(const rmath::vector3z& coord) const {
  return m_target->cell_loc_abs(m_target->access(coord));
} /* cell_loc_abs() */

rtypes::type_uuid ct_skel_info::id(void) const {
  return m_target->id();
} /* id() */

rmath::ranged ct_skel_info::xrange(void) const {
  return m_target->xrange();
} /* xrange() */

rmath::ranged ct_skel_info::yrange(void) const {
  return m_target->yrange();
} /* yrange() */

NS_END(perception, controller, silicon);