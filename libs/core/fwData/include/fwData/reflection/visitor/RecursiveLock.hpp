/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwData/config.hpp"

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include <fwCore/macros.hpp>
#include <fwCore/mt/types.hpp>

#include <vector>

namespace fwData
{
class Object;

namespace mt
{
class ObjectReadLock;
}
}

namespace fwData
{
namespace reflection
{
namespace visitor
{

/**
 * @brief   This class is an helper to lock an object and all its children recursively.
 */
class FWDATA_CLASS_API RecursiveLock : public ::camp::ExtendedClassVisitor
{

public:

    /**
     * @brief Container definition dedicated to object locks storage.
     */
    typedef std::vector< SPTR(::fwCore::mt::ReadLock) > LockVectType;

    /**
     * @brief Constructor.
     * Creates a recursive lock onto the given object and recursively locks it.
     * This recursive lock can be built using a given container of previously acquired locks.
     *
     * @param object data object (containing child data objects) to lock recursively
     * @param locks container of previously acquired locks
     */
    FWDATA_API RecursiveLock( CSPTR(::fwData::Object)object,
                              SPTR(LockVectType)locks = SPTR(LockVectType)(new LockVectType()));

    FWDATA_API virtual ~RecursiveLock();

    /**
     * @name Introspection methods implementation
     * @{ */
    FWDATA_API void visit(const camp::SimpleProperty& property);
    FWDATA_API void visit(const camp::EnumProperty& property);
    FWDATA_API void visit(const camp::UserProperty& property);
    FWDATA_API void visit(const camp::ArrayProperty& property);
    FWDATA_API void visit(const camp::Function& function);
    FWDATA_API virtual void visit(const camp::MapProperty& property);
    /**  @} */

private:

    /// Locks associated object recursively (called by constructor).
    void lock();

    /// Object given in constructor which will introspected
    CSPTR(::fwData::Object) m_object;

    /// Container of acquired locks.
    SPTR(LockVectType) m_locks;

    /// Reflection in camp world of m_object
    ::camp::UserObject m_campObj;

};

} // namespace visitor
} // namespace reflection
} // namespace fwData
