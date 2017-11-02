/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_BLOB_HPP__
#define __FWATOMS_BLOB_HPP__

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
    fwCoreClassDefinitionsWithFactoryMacro( (Blob)(::fwAtoms::Base), (()), ::fwAtoms::factory::New< Blob >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Blob(::fwAtoms::Base::Key key)
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

#endif /* __FWATOMS_BLOB_HPP__ */

