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

#include "io/atoms/config.hpp"

#include <atoms/Object.hpp>

#include <core/base.hpp>

#include <io/atoms/patch/helper/Object.hpp>

#include <map>
#include <string>

namespace sight::io::atoms::patch
{

/**
 * @brief Base class of all patches
 */
class IO_ATOMS_CLASS_API IPatch : public core::BaseObject
{
public:

    SIGHT_DECLARE_CLASS(IPatch);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @name Typedefs
     * @{ */
    typedef std::map<sight::atoms::Object::sptr, sight::atoms::Object::sptr> NewVersionsType;
    /**  @} */

    /// Constructor. Does nothing.
    IO_ATOMS_API IPatch();

    /// Copy constructor
    IO_ATOMS_API IPatch(const IPatch& cpy);

    /// Destructor. Does nothing.
    IO_ATOMS_API virtual ~IPatch();

    /// Applies the patch to the specified object
    IO_ATOMS_API virtual void apply(
        const sight::atoms::Object::sptr& previous,
        const sight::atoms::Object::sptr& current,
        io::atoms::patch::IPatch::NewVersionsType& newVersions
    ) = 0;

    /// Returns the origin classname of the object
    IO_ATOMS_API virtual const std::string& getOriginClassname() const;

    /// Returns the origin version of the object
    IO_ATOMS_API virtual const std::string& getOriginVersion() const;

protected:

    /// Origin classname of the object
    std::string m_originClassname;

    /// Origin version of the object
    std::string m_originVersion;
};

} //fwAtomsPatch
