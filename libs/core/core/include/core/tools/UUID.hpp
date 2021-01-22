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
#include "core/tools/Object.hpp"
#include <core/base.hpp>
#include <core/mt/types.hpp>

#include <map>
#include <string>

namespace sight::core::tools
{
/**
 * @brief   Management of UUID on objects.
 */
class CORE_CLASS_API UUID
{
public:

    fwCoreClassMacro(UUID)

    //------------------------------------------------------------------------------

    static sptr New()
    {
        return sptr(new UUID());
    }

    typedef std::string UUIDType;

    /**
     * @brief   Return true iff the given uuid is used
     * @note This method is thread-safe.
     */
    CORE_API static bool exist( const UUIDType& uuid);

    /**
     * @brief   Return an uuid to the given object : if no one previously set then generate a new one
     * @note This method is thread-safe.
     */
    CORE_API static const UUIDType& get(core::tools::Object::sptr object);

    /**
     * @brief   Return a smart ptr on the object related to a given UUID : return null shared if not supervised
     * @note This method thread-safe.
     */
    CORE_API static core::tools::Object::sptr get( const UUIDType& uuid );

    /**
     * @brief   Return a new extended UUID;
     * @note This method is thread-safe.
     */
    CORE_API static UUIDType generateUUID();

    /**
     * @brief Attempt to set an UUID. If uuid already exists, do nothing.
     * @return true if the uuid is set, else false.
     * @note This method is thread-safe.
     */
    CORE_API static bool set(core::tools::Object::sptr object,  const UUID::UUIDType& uuid);

    /**
     * @brief   Destructor : does nothing.
     */
    CORE_API virtual ~UUID();

protected:

    /// Store association std::weak_ptr <--> uuid as a string
    typedef std::map< UUIDType, core::tools::Object::wptr > UUIDContainer;

    CORE_API static UUIDContainer s_uuidMap;

    /**
     * @brief   Default constructor : does nothing.
     */
    CORE_API UUID();

    /// Mutex used to lock uuid object access.
    core::mt::ReadWriteMutex m_uuidMutex;

private:

    /// Local UUID, empty by default if not generated.
    UUIDType m_uuid;

    /// Read/Write mutex used to thread-safe UUID get/set/exist methods.
    static core::mt::ReadWriteMutex s_uuidMapMutex;

    /// Mutex used by generateUUID().
    static core::mt::Mutex s_generateUUIDMutex;

};

}
