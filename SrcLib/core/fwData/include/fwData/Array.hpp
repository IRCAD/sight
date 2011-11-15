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


namespace fwData
{

/**
 * @class   Array
 * @brief   Provides a way to manage a multidimentionnal array.  If the
 * array own his buffer, it will support the allocation, reallocation,
 * destruction of the buffer. Else, this class will behave like a "view" on the
 * buffer
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class FWDATA_CLASS_API Array : public ::fwData::Object
{
public :

    fwCoreClassDefinitionsWithFactoryMacro( (Array)(::fwData::Object), (()), ::fwTools::Factory::New< Array >) ;

    fwDataObjectMacro();

    typedef std::vector<size_t> SizeType;
    typedef std::vector<size_t> OffsetType;
    typedef OffsetType IndexType;


    /*
     * public API
     */
    /// Defines shallow copy
    FWDATA_API void shallowCopy( Array::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Array::csptr _source );


    //Allocates or resizes allocated buffer. If the new combination of type, size
    //and components do not match anymore the previously allocated size, a
    //reallocation is required. If the reallocation is needed and the
    //reallocate parameter is true, the reallocation is performed. Otherwise,
    //an exception is thrown.
    FWDATA_API virtual size_t resize(const ::fwTools::Type &type, const SizeType &size, const size_t nbOfComponents, const bool reallocate = false) throw(::fwData::Exception);
    FWDATA_API virtual size_t resize(const std::string &type, const SizeType &size, const size_t nbOfComponents, const bool reallocate = false) throw(::fwData::Exception);
    FWDATA_API virtual size_t resize(const SizeType &size, const size_t nbOfComponents, const bool reallocate = false) throw(::fwData::Exception);
    FWDATA_API virtual size_t resize(const SizeType &size, const bool reallocate = false) throw(::fwData::Exception);

    FWDATA_API virtual void clear();


    ///<value> must be a valid buffer of elements of type <m_type> with a length equal to <m_nbOfComponents>
    FWDATA_API virtual void setItem(const IndexType &id, const void *value);
    ///<value> must be a valid pointer to an element of type <m_type>
    FWDATA_API virtual void setItem(const IndexType &id, const size_t component, const void *value);

    ///Returns the pointer to the data in the buffer
    FWDATA_API virtual void* getItem(const IndexType &id, const size_t component = 0) const;

    ///Copies the data to the buffer pointed by <value>
    FWDATA_API virtual void  getItem(const IndexType &id, void *value) const;
    FWDATA_API virtual void  getItem(const IndexType &id, const size_t component, void *value) const;

    template< typename T > T* getItem(const IndexType &id, const size_t component = 0) const;

    FWDATA_API virtual void *getBuffer() const;

    /// Returns the begining/end of the buffer interpreted as a char buffer
    char* begin() const;
    char* end() const;

    /// Returns the begining/end of the buffer, casted to T
    template< typename T > T* begin() const;
    template< typename T > T* end() const;
    template< typename T > const T* cbegin() const;
    template< typename T > const T* cend() const;



    FWDATA_API size_t getElementSizeInBytes() const;
    FWDATA_API virtual size_t getSizeInBytes() const;

    FWDATA_API virtual SizeType   getSize() const;
    FWDATA_API virtual OffsetType getStrides() const;

    FWDATA_API virtual void setNumberOfComponents(size_t nb);
    FWDATA_API virtual size_t getNumberOfComponents() const;
    FWDATA_API virtual size_t getNumberOfDimensions() const;

    FWDATA_API virtual void setIsBufferOwner(const bool own);
    FWDATA_API virtual bool getIsBufferOwner() const;

    FWDATA_API virtual void setType(const std::string &type);
    FWDATA_API virtual void setType(const ::fwTools::Type &type);
    FWDATA_API virtual ::fwTools::Type getType() const;

    FWDATA_API size_t getBufferOffset( const ::fwData::Array::IndexType &id, size_t component, size_t sizeOfType ) const;
    FWDATA_API char *getBufferPtr( const ::fwData::Array::IndexType &id, size_t component, size_t sizeOfType ) const;

    static OffsetType computeStrides( SizeType size, size_t nbOfComponents, size_t sizeOfType );

protected:
    FWDATA_API Array();
    FWDATA_API virtual ~Array();

    /// Not implemented
    Array( const Array& );
    const Array & operator= ( const Array& );

    //-----------------------------------------------------------------------------

    OffsetType m_strides; // Liste de valeurs représentant le pas dans chaque dimension pour parcourir le tableau


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
    return reinterpret_cast<T*> (static_cast<char*>(m_buffer) + this->getSizeInBytes());
}

template< typename T > const T* Array::cbegin() const
{
    return static_cast<const T*>(m_buffer);
}

template< typename T > const T* Array::cend() const
{
    return reinterpret_cast<const T*> (static_cast<char*>(m_buffer) + this->getSizeInBytes());
}

template< typename T > T* Array::getItem(const IndexType &id, const size_t component) const
{
    return static_cast<T*> (getItem(id, component));
}



} // namespace fwData

#endif // _FWDATA_ARRAY_HPP_
