/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_ARRAY_HPP__
#define __FWDATA_ARRAY_HPP__

#include "fwData/Exception.hpp"
#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

#include <fwTools/Type.hpp>

#include <fwMemory/BufferObject.hpp>

fwCampAutoDeclareDataMacro((fwData)(Array), FWDATA_API);

namespace fwData
{

/**
 * @brief   Provides a way to manage a view on a multidimentionnal array.
 *
 * If the array own his buffer, it will perform the allocation, reallocation,
 * destruction of the buffer. Else, this class will provide an array "view" of the
 * buffer
 *
 */
class FWDATA_CLASS_API Array : public ::fwData::Object
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (Array)(::fwData::Object), (()), ::fwData::factory::New< Array >);

    fwCampMakeFriendDataMacro((fwData)(Array));

    /**
     * @brief Array size type
     */
    typedef std::vector<size_t> SizeType;
    /**
     * @brief Offset type
     */
    typedef std::vector<size_t> OffsetType;
    /**
     * @brief Index type
     */
    typedef OffsetType IndexType;


    /*
     * public API
     */

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Array( ::fwData::Object::Key key );

    FWDATA_API virtual ~Array();


    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /**
     * @brief Resizes and allocate (if needed) the array.
     *
     * If no buffer is allocated and reallocate is true, this method will
     * allocate a buffer and take it ownership.
     *
     * If the combination of type, size and components parameters do not match
     * anymore the size of the previously allocated buffer, a reallocation is needed.
     * In this case :
     *  * if reallocate is true and if the Array do not own the buffer, an
     *  exception is thrown
     *  * else if reallocate is false, the array will update the view
     *  informations
     *  * else, the reallocation is performed.
     *
     * @param type           Type of the array view
     * @param size           Size of the array view
     * @param nbOfComponents Number of components of the array view, Min value : 1
     * @param reallocate     If true, allow buffer reallocation
     *
     * @return return the size of the array view
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API virtual size_t resize(const ::fwTools::Type &type, const SizeType &size, size_t nbOfComponents,
                                     bool reallocate = false);

    /// Aliases to the resize method
    FWDATA_API virtual size_t resize(const std::string &type, const SizeType &size, size_t nbOfComponents,
                                     bool reallocate = false);
    FWDATA_API virtual size_t resize(const SizeType &size, size_t nbOfComponents, bool reallocate =
                                         false);
    FWDATA_API virtual size_t resize(const SizeType &size, bool reallocate = false);

    /**
     * @brief Clear this array.
     * Size, type, nbOfComponents are reset, buffer is released.
     */
    FWDATA_API virtual void clear();

    /**
     * @brief Test whether array is empty
     *
     * @return Returns whether array is empty, ie. this->getSize() is an empty
     * vector.
     */
    FWDATA_API virtual bool empty() const;


    /**
     * @brief Get the size of one element of the array,
     * ie. sizeof type * nbOfComponents
     *
     * @return One array elemet size in bytes.
     */
    FWDATA_API virtual size_t getElementSizeInBytes() const;

    /**
     * @brief Get the number of elements of type <getType()> in the array
     *
     * @return number of array elements
     */
    FWDATA_API virtual size_t getNumberOfElements() const;

    /**
     * @brief Getter for the array view size
     *
     * @return  Array view size in bytes.
     */
    FWDATA_API virtual size_t getSizeInBytes() const;

    /**
     * @brief Getter for the array size
     *
     * @return vector of size lengths in each dimension
     */
    FWDATA_API virtual const SizeType &getSize() const;

    /**
     * @brief Getter for the array strides
     *
     * @return vector of steps in each dimension for array walking
     */
    FWDATA_API virtual const OffsetType &getStrides() const;

    /**
     * @brief Setter for array's number of components
     * If the array has a buffer and owns it, the buffer will be reallocated
     *
     * @param nb number of components
     */
    FWDATA_API virtual void setNumberOfComponents(size_t nb);

    /**
     * @brief Getter for number of components
     *
     * @return Array's number of components
     */
    FWDATA_API virtual size_t getNumberOfComponents() const;

    /**
     * @brief Getter for number of dimensions, ie. getSize().size()
     *
     * @return Array's number of dimensions
     */
    FWDATA_API virtual size_t getNumberOfDimensions() const;

    /**
     * @brief Set array's buffer ownership
     *
     * @param own New ownership value
     */
    FWDATA_API virtual void setIsBufferOwner(const bool own);

    /**
     * @brief Getter for array's buffer ownership
     *
     * @return Current array buffer ownership
     */
    FWDATA_API virtual bool getIsBufferOwner() const;

    /**
     * @brief Setter for array's type
     *
     * @param type new array type
     */
    FWDATA_API virtual void setType(const std::string &type);
    FWDATA_API virtual void setType(const ::fwTools::Type &type);

    /**
     * @brief Getter for array's type
     *
     * @return Type of array
     */
    FWDATA_API virtual ::fwTools::Type getType() const;

    /**
     * @brief Compute offset in buffer for given parameters
     * of type
     *
     * @param id Item id
     * @param component Item component id
     * @param sizeOfType size of a component
     *
     * @return buffer offset
     */
    FWDATA_API size_t getBufferOffset( const ::fwData::Array::IndexType &id, size_t component,
                                       size_t sizeOfType ) const;

    /**
     * @brief Compute strides for given parameters
     *
     * @param size Array size
     * @param nbOfComponents number of components
     * @param sizeOfType size of a component
     *
     * @return
     */
    FWDATA_API static OffsetType computeStrides( SizeType size, size_t nbOfComponents, size_t sizeOfType );

    ::fwMemory::BufferObject::sptr getBufferObject () const;

    void setBufferObject (const ::fwMemory::BufferObject::sptr& val);


    /// Exchanges the content of the Array with the content of _source.
    FWDATA_API void swap( Array::sptr _source );

protected:

    /// Not implemented
    Array( const Array& );

    const Array & operator= ( const Array& );

    OffsetType m_strides;
    ::fwTools::Type m_type;
    ::fwMemory::BufferObject::sptr m_bufferObject;
    SizeType m_size;
    size_t m_nbOfComponents;
    bool m_isBufferOwner;

};

//-----------------------------------------------------------------------------

inline ::fwMemory::BufferObject::sptr Array::getBufferObject () const
{
    return m_bufferObject;
}

//-----------------------------------------------------------------------------

inline void Array::setBufferObject (const ::fwMemory::BufferObject::sptr& val)
{
    m_bufferObject = val;
}

//-----------------------------------------------------------------------------


} // namespace fwData

#endif // __FWDATA_ARRAY_HPP__

