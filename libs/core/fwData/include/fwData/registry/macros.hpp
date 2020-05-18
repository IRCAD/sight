/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWDATA_REGISTRY_MACROS_HPP__
#define __FWDATA_REGISTRY_MACROS_HPP__

#include "fwData/registry/detail.hpp"

namespace fwData
{
namespace registry
{
#define fwDataRegisterMacro( classname )                                  \
    static ::fwData::Object::Registrar< classname > BOOST_PP_CAT(s__factory__record__, __LINE__);

} // end namespace registry
} // end namespace fwData

#endif /*__FWDATA_REGISTRY_MACROS_HPP__*/
