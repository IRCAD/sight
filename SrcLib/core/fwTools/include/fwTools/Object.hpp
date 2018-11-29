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

#ifndef __FWTOOLS_OBJECT_HPP__
#define __FWTOOLS_OBJECT_HPP__

#include "fwTools/config.hpp"
#include "fwTools/fwID.hpp"

#include <fwCore/base.hpp>

namespace fwTools
{

class UUID;

/**
 * @brief   Define Base class for FW4SPL objects and services
 */
class FWTOOLS_CLASS_API Object  : public ::fwCore::BaseObject,
                                  public ::boost::noncopyable,
                                  protected ::fwTools::fwID
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (Object)(::fwCore::BaseObject) );
    fwCoreAllowSharedFromThis();

    friend class ::fwTools::UUID;

    // expose API for ID management
    using ::fwTools::fwID::hasID;
    using ::fwTools::fwID::getID;
    using ::fwTools::fwID::setID;
    using ::fwTools::fwID::resetID;

    FWTOOLS_API Object();
    virtual ~Object();

protected:

    /// UUID used to identify the object, notably for serialization
    SPTR(::fwTools::UUID) m_uuid;
};

//-----------------------------------------------------------------------------

inline Object::~Object()
{
}

//-----------------------------------------------------------------------------

}

#endif /* __FWTOOLS_OBJECT_HPP__ */
