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

#ifndef __FWRENDERQT_REGISTRY_ADAPTOR_HPP__
#define __FWRENDERQT_REGISTRY_ADAPTOR_HPP__

#include "fwRenderQt/config.hpp"

#include <map>
#include <string>

namespace fwRenderQt
{

class IAdaptor;

namespace registry
{

typedef std::map<std::string, std::string> AdaptorRegistryType;

/// Returns map containing Adaptor-SRender association
FWRENDERQT_API AdaptorRegistryType& getAdaptorRegistry();

} // namespace registry

} // namespace fwRenderQt

#endif /* __FWRENDERQT_REGISTRY_ADAPTOR_HPP__ */
