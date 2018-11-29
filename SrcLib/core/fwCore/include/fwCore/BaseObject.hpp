/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWCORE_BASEOBJECT_HPP__
#define __FWCORE_BASEOBJECT_HPP__

#include "fwCore/macros.hpp"
#include "fwCore/config.hpp"

//keep it first to prevent nasty warns in MSVC
#include <boost/config.hpp>

namespace fwCore
{

/**
 * @brief Base class for all Sight's classes
 */
class FWCORE_CLASS_API BaseObject : public std::enable_shared_from_this<BaseObject>
{
public:
    fwCoreBaseClassDefinitionsMacro( (BaseObject) )

protected:
    FWCORE_API virtual ~BaseObject();
    using std::enable_shared_from_this<BaseObject>::shared_from_this;
};

} // namespace fwCore

#endif // __FWCORE_BASEOBJECT_HPP__

