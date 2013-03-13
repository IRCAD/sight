/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_BASE_BLOB_HPP__
#define __FWATOMS_BASE_BLOB_HPP__

#include <string>

#include <fwTools/BufferObject.hpp>

#include "fwAtoms/config.hpp"
#include "fwAtoms/Base.hpp"
#include "fwAtoms/factory/new.hpp"

#include <fwCamp/macros.hpp>

fwCampAutoDeclareMacro((fwAtoms)(Blob), FWATOMS_API);

namespace fwAtoms
{
/**
 * @brief Represented a buffer in fwAtoms
 *
 * A blob is a buffer. In fact it is a char* in memory.
 */
class FWATOMS_CLASS_API Blob : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Blob)(::fwAtoms::Base), (()), ::fwAtoms::factory::New< Blob >) ;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Blob(::fwAtoms::Base::Key key)
    {}

    /**
     * @brief   Destructor
     */
    virtual ~Blob()
    {}

    /**
     * @brief create a new Blob shared ptr.
     * @param buffer the buffer object
     */
    FWATOMS_API static Blob::sptr New(::fwTools::BufferObject::sptr buffer);

    /**
     * @brief create a new Blob shared ptr.
     * @param buffer the buffer
     * @param size the buffer size
     */
    FWATOMS_API static Blob::sptr New(void* buffer, unsigned int size);


    virtual bool isBlob() const {return true;};
    /**
     * @brief size accessor.
     * @return the buffer size
     */
    FWATOMS_API  size_t getSize() const;

    /**
     * @brief buffer accessor.
     * @return the a pointer to the first buffer element.
     */
    FWATOMS_API  char* getBuffer()const;

    /**
     * @brief buffer accessor.
     * @return the a pointer to the first buffer element.
     */
    FWATOMS_API  ::fwTools::BufferObject::sptr getBufferObject();

    FWATOMS_API virtual Base::sptr clone();

protected:

    ::fwTools::BufferObject::sptr m_bufferObject;
};

}



#endif /* _FWATOMS_BASE_BLOB_HPP_ */
