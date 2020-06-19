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

#ifndef __FWATOMSPATCH_TYPES_HPP__
#define __FWATOMSPATCH_TYPES_HPP__

#include <string>

#include <fwAtomConversion/DataVisitor.hpp>

#include "fwAtomsPatch/config.hpp"

namespace fwAtomsPatch
{
/// Key used in object meta infos to define an object classname.
static const std::string s_OBJ_CLASSNAME = ::fwAtomConversion::DataVisitor::CLASSNAME_METAINFO;

/// Key used in object meta infos to define an object version.
static const std::string s_OBJ_VERSION = "object_version";

/// Key used in object meta infos to define an object ID.
static const std::string s_OBJ_ID = ::fwAtomConversion::DataVisitor::ID_METAINFO;

} // namespace fwAtomsPatch

#endif // __FWATOMSPATCH_TYPES_HPP__
