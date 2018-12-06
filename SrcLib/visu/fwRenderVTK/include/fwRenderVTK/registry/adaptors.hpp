/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __FWRENDERVTK_REGISTRY_ADAPTORS_HPP__
#define __FWRENDERVTK_REGISTRY_ADAPTORS_HPP__

#include "fwRenderVTK/config.hpp"

#include <map>
#include <string>

namespace fwRenderVTK
{

class IAdaptor;

namespace registry
{

/// map <adaptor Id, SRender Id>
typedef std::map<std::string, std::string> AdaptorRegistryType;

/// Get the association <adaptor, scene>
FWRENDERVTK_API AdaptorRegistryType& getAdaptorRegistry();

} // namespace registry

} // namespace fwRenderVTK

#endif /* __FWRENDERVTK_REGISTRY_ADAPTORS_HPP__ */
