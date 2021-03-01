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

#include "atoms/Base.hpp"
#include "atoms/config.hpp"
#include "atoms/factory/new.hpp"

#include <core/memory/BufferObject.hpp>

namespace sight::atoms
{
/**
 * @brief Representation of a buffer
 *
 * A blob wraps a buffer
 */
class ATOMS_CLASS_API Blob : public Base
{
public:
    SIGHT_DECLARE_CLASS(Blob, atoms::Base, atoms::factory::New< Blob >)

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Blob(atoms::Base::Key)
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
    ATOMS_API static Blob::sptr New(core::memory::BufferObject::sptr buffer);

    /**
     * @brief create a new Blob shared ptr.
     * @param buffer the buffer
     * @param size the buffer size
     */
    ATOMS_API static Blob::sptr New(void* buffer, unsigned int size);

    /**
     * @brief buffer accessor.
     * @return the a pointer to the first buffer element.
     * @{
     */
    ATOMS_API core::memory::BufferObject::sptr getBufferObject() const;
    ATOMS_API void setBufferObject(const core::memory::BufferObject::sptr& bo);
    /// @}

    /**
     * @brief Returns a clone object
     */
    ATOMS_API virtual Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    atoms::Base::AtomType type() const override
    {
        return atoms::Base::BLOB;
    }

protected:

    core::memory::BufferObject::sptr m_bufferObject;
};

}
