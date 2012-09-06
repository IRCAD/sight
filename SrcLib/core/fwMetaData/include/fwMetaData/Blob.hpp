/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMETADATA_BASE_BLOB_HPP__
#define __FWMETADATA_BASE_BLOB_HPP__

#include <string>
#include <fwTools/BufferObject.hpp>
#include "fwMetaData/config.hpp"
#include "fwMetaData/Base.hpp"

#include <fwCamp/macros.hpp>

fwCampAutoDeclareMacro((fwMetaData)(Blob), FWMETADATA_API);

namespace fwMetaData
{
/**
 * @brief Represented a buffer in fwMetaData
 *
 * A blob is a buffer. In fact it is a char* in memory.
 */
class FWMETADATA_CLASS_API Blob : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Blob)(::fwMetaData::Blob), (()), new Blob) ;


    /**
     * @brief create a new Blob shared ptr.
     * @param value the buffer
     * @param size the buffer size
     * @param name the name of the current buffer.
     */
    FWMETADATA_API static Blob::sptr New(::fwTools::BufferObject::sptr buffer);
    FWMETADATA_API static Blob::sptr New(void* buffer, unsigned int size);


    FWMETADATA_API  virtual bool isBlob() const {return true;};
    /**
     * @brief size accessor.
     * @return the buffer size
     */
    FWMETADATA_API  unsigned int getSize() const;

    /**
     * @brief buffer accessor.
     * @return the a pointer to the first buffer element.
     */
    FWMETADATA_API  char* getBuffer()const;

    /**
     * @brief buffer accessor.
     * @return the a pointer to the first buffer element.
     */
    FWMETADATA_API  ::fwTools::BufferObject::sptr getBufferObject();

    FWMETADATA_API virtual Base::sptr clone();

protected:
    Blob(){};
    Blob(::fwTools::BufferObject::sptr buffer);

    ::fwTools::BufferObject::sptr m_bufferObject;
};

}



#endif /* _FWMETADATA_BASE_BLOB_HPP_ */
