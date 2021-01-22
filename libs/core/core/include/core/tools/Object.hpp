/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "core/config.hpp"
#include "core/tools/fwID.hpp"
#include <core/base.hpp>

#include <boost/core/noncopyable.hpp>

namespace sight::core::tools
{

class UUID;

/**
 * @brief   Define Base class for Sight objects and services
 */
class CORE_CLASS_API Object  : public core::BaseObject,
                               public ::boost::noncopyable,
                               protected core::tools::fwID
{
public:
    fwCoreClassMacro(Object, core::BaseObject)
    fwCoreAllowSharedFromThis();

    friend class core::tools::UUID;

    // expose API for ID management
    using core::tools::fwID::hasID;
    using core::tools::fwID::getID;
    using core::tools::fwID::setID;
    using core::tools::fwID::resetID;

    CORE_API Object();
    virtual ~Object();

protected:

    /// UUID used to identify the object, notably for serialization
    SPTR(core::tools::UUID) m_uuid;
};

//-----------------------------------------------------------------------------

inline Object::~Object()
{
}

//-----------------------------------------------------------------------------

}
