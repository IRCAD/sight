/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "viz/qt3d/registry/detail.hpp"

#include <core/LazyInstantiator.hpp>

namespace sight::viz::qt3d
{

namespace registry
{

struct FwRenderQt3DRegistryInstantiatorTag {};

SPTR(Type) get()
{
    typedef core::LazyInstantiator<Type, FwRenderQt3DRegistryInstantiatorTag> InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace registry.

} // namespace sight::viz::qt3d.
