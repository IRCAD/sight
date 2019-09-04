/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwAtoms/Base.hpp"
#include "fwAtoms/config.hpp"
#include "fwAtoms/factory/new.hpp"

#include <fwMemory/BufferObject.hpp>

namespace fwAtoms
{
/**
 * @brief Representation of a buffer
 *
 * A blob wraps a buffer
 */
class FWATOMS_CLASS_API Blob : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Blob)(::fwAtoms::Base), ::fwAtoms::factory::New< Blob >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Blob(::fwAtoms::Base::Key)
    {
    }

    /**
     * @brief   Destructor
     */
    virtual ~Blob()
    {
    }

    /**
     * @brief create a new Blob shared ptr.
     * @param buffer the buffer object
     */
    FWATOMS_API static Blob::sptr New(::fwMemory::BufferObject::sptr buffer);

    /**
     * @brief create a new Blob shared ptr.
     * @param buffer the buffer
     * @param size the buffer size
     */
    FWATOMS_API static Blob::sptr New(void* buffer, unsigned int size);

    /**
     * @brief buffer accessor.
     * @return the a pointer to the first buffer element.
     * @{
     */
    FWATOMS_API ::fwMemory::BufferObject::sptr getBufferObject() const;
    FWATOMS_API void setBufferObject(const ::fwMemory::BufferObject::sptr& bo);
    /// @}

    /**
     * @brief Returns a clone object
     */
    FWATOMS_API virtual Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    ::fwAtoms::Base::AtomType type() const override
    {
        return ::fwAtoms::Base::BLOB;
    }

protected:

    ::fwMemory::BufferObject::sptr m_bufferObject;
};

}
