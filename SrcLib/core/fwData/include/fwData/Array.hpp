/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_ARRAY_HPP_
#define _FWDATA_ARRAY_HPP_

#include <fwTools/Type.hpp>

#include "fwData/Exception.hpp"

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{

/**
 * @class   Array
 * @brief   Provides a way to manage a view on a multidimentionnal array.
 * If the array own his buffer, it will perform the allocation, reallocation,
 * destruction of the buffer. Else, this class will provide a array "view" of the
 * buffer
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class FWDATA_CLASS_API Array : public ::fwData::Object
{
public :

    fwCoreClassDefinitionsWithFactoryMacro( (Array)(::fwData::Object), (()), ::fwData::Factory::New< Array >) ;

    fwDataObjectMacro();

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

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Array::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Array::csptr _source );



    /**
     * @brief Resizes and allocate (if needed) the array.
     *
     * If no buffer is allocated and reallocate is true, this method will
     * allocate a buffer and take it ownership.
     *
     * If the combination of type, size and components parametters do not match
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
    FWDATA_API virtual size_t resize(const ::fwTools::Type &type, const SizeType &size, size_t nbOfComponents, bool reallocate = false) throw(::fwData::Exception);

    /// Aliases to the resize method
    FWDATA_API virtual size_t resize(const std::string &type, const SizeType &size, size_t nbOfComponents, bool reallocate = false) throw(::fwData::Exception);
    FWDATA_API virtual size_t resize(const SizeType &size, size_t nbOfComponents, bool reallocate = false) throw(::fwData::Exception);
    FWDATA_API virtual size_t resize(const SizeType &size, bool reallocate = false) throw(::fwData::Exception);

    /**
     * @brief Clear this array.
     * Size, type, nbOfComponents are reset, buffer is released.
     */
    FWDATA_API virtual void clear();


    /**
     * @brief Setter for one item components of the array
     *
     * @param id Item id
     * @param value Valid buffer of elements of type <m_type> with a length equal to <m_nbOfComponents> to be copied to array 'id'
     */
    FWDATA_API virtual void setItem(const IndexType &id, const void *value);

    /**
     * @brief Setter for one item component of the array
     *
     * @param id Item id
     * @param component Component id to write data in.
     * @param value Valid buffer of elements of type <m_type> with a length equal to <m_nbOfComponents> to be copied to array 'id', component n° 'component'
     *
     */
    FWDATA_API virtual void setItem(const IndexType &id, const size_t component, const void *value);

    ///
    /**
     * @brief Getter for a buffer item. pointer to the requested item in the buffer
     *
     * @param id Item id
     * @param component Component id
     *
     * @return Pointer to the requested item in the buffer
     */
    FWDATA_API virtual void* getItem(const IndexType &id, const size_t component = 0) const;

    /**
     * @brief Typed version of getItem
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item id
     * @param component Component id
     *
     * @return Array buffer pointer casted to T
     */
    template< typename T > T* getItem(const IndexType &id, const size_t component = 0) const;

    /**
     * @brief Copies the data into the buffer pointed by <value>
     *
     * @param id Item id
     * @param[out] value Buffer to write into
     */
    FWDATA_API virtual void getItem(const IndexType &id, void *value) const;
    FWDATA_API virtual void getItem(const IndexType &id, const size_t component, void *value) const;


    /**
     * @brief Getter for the array buffer
     *
     * @return Array's buffer, if exists, else NULL
     */
    FWDATA_API virtual void *getBuffer() const;

    /**
     * @brief Setter for the array buffer.
     * An existing buffer will be released if the array own it.
     *
     * @param buf Buffer to set as Array's buffer
     * @param takeOwnership if true, the Array will manage allocation and destroy the buffer when needed.
     */
    FWDATA_API virtual void setBuffer(void *buf, bool takeOwnership = false);

    /**
     * @brief Setter for the array buffer.
     *
     * An existing buffer will be released if the array own it.
     *
     * @param buf            Buffer to set as Array's buffer
     * @param takeOwnership  if true, the Array will manage allocation and destroy the buffer when needed.
     * @param type           Type of the array view
     * @param size           Size of the array view
     * @param nbOfComponents Number of components of the array view, Min value : 1
     * @param reallocate     If true, allow buffer reallocation
     */
    FWDATA_API void setBuffer(  void *buf, bool takeOwnership,
                                const ::fwTools::Type &type,const SizeType &size, size_t nbOfComponents );

    /// Returns the begining/end of the buffer interpreted as a char buffer
    virtual char* begin() const;
    virtual char* end() const;

    /// Returns the begining/end of the buffer, casted to T
    template< typename T > T* begin() const;
    template< typename T > T* end() const;
    template< typename T > const T* cbegin() const;
    template< typename T > const T* cend() const;

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
    FWDATA_API size_t getBufferOffset( const ::fwData::Array::IndexType &id, size_t component, size_t sizeOfType ) const;

    /**
     * @brief Get a pointer to the value described by given parameters
     *
     * @param id Item id
     * @param component Item component id
     * @param sizeOfType size of a component
     *
     * @return buffer item pointer
     */
    FWDATA_API char *getBufferPtr( const ::fwData::Array::IndexType &id, size_t component, size_t sizeOfType ) const;

    /**
     * @brief Compute strides for given parameters
     *
     * @param size Array size
     * @param nbOfComponents number of components
     * @param sizeOfType size of a component
     *
     * @return
     */
    static OffsetType computeStrides( SizeType size, size_t nbOfComponents, size_t sizeOfType );

protected:
    FWDATA_API Array();
    FWDATA_API virtual ~Array();

    /// Not implemented
    Array( const Array& );
    const Array & operator= ( const Array& );

    //-----------------------------------------------------------------------------

    OffsetType m_strides;


    //=============================================================================
    ::fwTools::Type m_type;
    void * m_buffer;
    SizeType  m_size;
    size_t m_nbOfComponents;
    bool   m_isBufferOwner;

};

template< typename T > T*       Array::begin() const
{
    return static_cast<T*>(m_buffer);
}

template< typename T > T*       Array::end() const
{
    //return reinterpret_cast<T*> (static_cast<char*>(m_buffer) + this->getSizeInBytes());
    return reinterpret_cast<T*> (static_cast<char*>(m_buffer) + this->getSizeInBytes());
}

template< typename T > const T* Array::cbegin() const
{
    return static_cast<const T*>(begin<T>());
}

template< typename T > const T* Array::cend() const
{
    return reinterpret_cast<const T*> (end<T>());
}

template< typename T > T* Array::getItem(const IndexType &id, const size_t component) const
{
    return static_cast<T*> (getItem(id, component));
}



} // namespace fwData

#endif // _FWDATA_ARRAY_HPP_
