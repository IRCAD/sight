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

#include "fwData/Array.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwTools/DynamicType.hpp>
#include <fwTools/Type.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/shared_array.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((fwData)(Image), FWDATA_API);

namespace fwData
{

class Point;
class PointList;

/**
 * @brief This class defines an image
 *
 * An image contains a buffer and is defined by some parameters (size, spacing, pixel type, ...)
 */
class FWDATA_CLASS_API Image : public Object
{
public:
    fwCoreClassMacro(Image, ::fwData::Object, ::fwData::factory::New< Image >)
    fwCoreAllowSharedFromThis()
    fwCampMakeFriendDataMacro((fwData)(Image))

    /// Image format
    enum class PixelFormat
    {
        UNDEFINED = 0,
        RGB,           ///< Image with 3 component RGB.
        RGBA,          ///< Image with 4 component RGBA.
        BGR,           ///< Image with 3 component BGR.
        BGRA,          ///< Image with 4 component BGRA.
        GRAY_SCALE     ///< Image with 1 component.
    };

    typedef std::array<size_t, 3> Size;
    typedef std::array<double, 3> Origin;
    typedef std::array<double, 3> Spacing;

    typedef Size::value_type IndexType;
    typedef std::uint8_t BufferType;
    typedef ::boost::shared_array< BufferType > SharedArray;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Image(::fwData::Object::Key key);

    /**
     * @brief Destructor
     */
    FWDATA_API virtual ~Image() noexcept override;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// @brief get image information from source. Informations are spacing,origin,size ... expect Fields
    FWDATA_API void copyInformation( Image::csptr _source );

    /**
     * @brief Get image spacing
     * @todo Rename to getSpacing when the deprecated API is removed
     */
    const Spacing& getSpacing2() const;
    /**
     * @brief Set image spacing
     * @todo Rename to setSpacing when the deprecated API is removed
     */
    void setSpacing2(const Spacing& spacing);

    /**
     * @brief Get image origin
     * @todo Rename to getOrigin when the deprecated API is removed
     */
    const Origin& getOrigin2() const;
    /**
     * @brief Set image origin
     * @todo Rename to setOrigin when the deprecated API is removed
     */
    void setOrigin2(const Origin& origin);

    /**
     * @brief Get image size
     * @todo Rename to getSize when the deprecated API is removed
     */
    const Size& getSize2() const;
    /**
     * @brief Set image size
     * @todo Rename to setSize when the deprecated API is removed
     */
    void setSize2(const Size& size);

    /// Number of dimensions of the image (3 for 3D image)
    FWDATA_API size_t getNumberOfDimensions() const;

    /** @{
     *  @brief Get/set preferred window center
     */
    double  getWindowCenter () const;

    void setWindowCenter (double val);
    /// @}

    /** @{
     *  @brief Get/set preferred window width
     */
    double  getWindowWidth () const;
    void setWindowWidth (double val);
    /// @}

    /** @{
     *  @brief Get/set preferred window center
     */
    size_t  getNumberOfComponents() const;

    void setNumberOfComponents(size_t val);
    /// @}

    /** @{
     * @brief get/set image type
     */
    FWDATA_API void setType(::fwTools::Type type);
    FWDATA_API void setType(const std::string& type);
    FWDATA_API ::fwTools::Type getType() const;
    /// @}

    /**
       @{
     * @brief Allocate image
     *
     * If the data array owns its buffer, these methods will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    FWDATA_API size_t allocate();

    FWDATA_API size_t allocate(IndexType x, IndexType y,  IndexType z, const ::fwTools::Type& type, PixelFormat format,
                               size_t numberOfComponents = 1);
    FWDATA_API size_t allocate(const Size& size, const ::fwTools::Type& type, PixelFormat format,
                               size_t numberOfComponents = 1);
    /// @}

    /// @brief return image size in bytes
    FWDATA_API size_t getSizeInBytes() const;
    /// @brief return allocated image size in bytes
    FWDATA_API size_t getAllocatedSizeInBytes() const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when image's buffer is added
    typedef ::fwCom::Signal< void () > BufferModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_BUFFER_MODIFIED_SIG;

    /// Type of signal when a landmark is added
    typedef ::fwCom::Signal< void (SPTR(::fwData::Point)) > LandmarkAddedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_LANDMARK_ADDED_SIG;

    /// Type of signal when a landmark is removed
    typedef ::fwCom::Signal< void (SPTR(::fwData::Point)) > LandmarkRemovedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_LANDMARK_REMOVED_SIG;

    /// Type of signal when a distance is added
    typedef ::fwCom::Signal< void (bool) > LandmarkDisplayedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_LANDMARK_DISPLAYED_SIG;

    /// Type of signal when a distance is added
    typedef ::fwCom::Signal< void (bool) > DistanceDisplayedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_DISTANCE_DISPLAYED_SIG;

    /// Type of signal when a distance is added
    typedef ::fwCom::Signal< void (SPTR(::fwData::PointList)) > DistanceAddedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_DISTANCE_ADDED_SIG;

    /// Type of signal when a distance is removed
    typedef ::fwCom::Signal< void (CSPTR(::fwData::PointList)) > DistanceRemovedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_DISTANCE_REMOVED_SIG;

    /// Type of signal when slice index is modified (axial index, frontal index, sagittal index)
    typedef ::fwCom::Signal< void (int, int, int) > SliceIndexModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_SLICE_INDEX_MODIFIED_SIG;

    /// Type of signal when slice type is modified (from slice type, to slice type)
    typedef ::fwCom::Signal< void (int, int) > SliceTypeModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_SLICE_TYPE_MODIFIED_SIG;

    /// Type of signal when visibility is modified
    typedef ::fwCom::Signal< void (bool) > VisibilityModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_VISIBILITY_MODIFIED_SIG;

    /// Type of signal when visibility is modified
    typedef ::fwCom::Signal< void () > TransparencyModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_TRANSPARENCY_MODIFIED_SIG;

    /**
     * @}
     */

    template <typename TYPE>
    using Iterator = Array::IteratorBase<TYPE, false>;
    template <typename TYPE>
    using ConstIterator = Array::IteratorBase<TYPE, true>;

    /**
     * @brief Returns the begin/end iterators to the image buffer, cast to T
     *
     * Iterate through all the element of the buffer.
     *
     * @warning Print a warning if T is different from the array type
     * @note These functions lock the buffer
     * @{
     */
    template< typename T > Iterator<T> begin();
    template< typename T > Iterator<T> end();
    template< typename T > ConstIterator<T> begin() const;
    template< typename T > ConstIterator<T> end() const;
    /// @}
    /**
     * @brief Returns the begin/end iterators to the array buffer, cast to char
     *
     * Iterate through all the element of the buffer.
     *
     * @note These functions lock the buffer
     * @{
     */
    Iterator<char> begin();
    Iterator<char> end();
    ConstIterator<char> begin() const;
    ConstIterator<char> end() const;
    /// @}

    ///
    /// @{
    /// Returns image buffer
    FWDATA_API void* getBuffer();
    FWDATA_API void* getBuffer() const;
    /// @}

    /** @{
     *  @brief Get/set pixel format
     */
    PixelFormat  getPixelFormat() const;
    void setPixelFormat(PixelFormat format);
    /// @}

    /**
     * @{
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item image index
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     * @throw ::fwData::Exception Index out of bounds
     */
    template< typename T > T& at(IndexType id);
    template< typename T > T at(IndexType id) const;
    /// @}
    /**
     * @{
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     * @throw ::fwData::Exception Index out of bounds
     */
    template< typename T > T& at(IndexType x, IndexType y, IndexType z);
    template< typename T > T at(IndexType x, IndexType y, IndexType z) const;
    /// @}
    ///

    /// Return the pixel value in a std::string
    FWDATA_API const std::string getPixelAsString(IndexType x,
                                                  IndexType y,
                                                  IndexType z );

    /**
     * @brief Return a lock on the image to prevent from dumping the buffer on the disk
     *
     * The buffer cannot be accessed if the image is not locked
     */
    FWDATA_API ::fwMemory::BufferObject::Lock lock() const;

    // ---------------------------------------
    // Deprecated API
    // ---------------------------------------

    /**
     * @brief Image size type
     */
    typedef ::fwData::Array::SizeType SizeType;

    typedef size_t BufferIndexType;

    /**
     * @brief Image spacing type
     */
    typedef std::vector< double > SpacingType;

    /**
     * @brief Image origin type
     */
    typedef std::vector< double > OriginType;
    /** @{
     * @brief get/set image spacing
     */

    FWDATA_API const SpacingType getSpacing() const;
    FWDATA_API void setSpacing(const SpacingType& spacing);
    /// @}

    /** @{
     *  @brief get/set image origin
     */
    FWDATA_API const OriginType getOrigin() const;
    FWDATA_API void setOrigin(const OriginType& origin);
    /// @}

    /** @{
     * @brief get/set image size
     */
    FWDATA_API const SizeType getSize() const;
    FWDATA_API void setSize(const SizeType& size);
    /// @}

    /** @{
     * @brief Allocate image
     *
     * If the data array owns its buffer, these methods will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    FWDATA_API size_t allocate(SizeType::value_type x, SizeType::value_type y,  SizeType::value_type z,
                               const ::fwTools::Type& type, size_t numberOfComponents = 1);
    FWDATA_API size_t allocate(const SizeType& size, const ::fwTools::Type& type, size_t numberOfComponents = 1);
    /// @}

    /**
     * @brief get a DynamicType for retrocompatibility
     * @deprecated Use getType()
     */
    FWDATA_API ::fwTools::DynamicType getPixelType() const;

    /**
     * @brief set data array
     *
     * @param[in] array data array
     * @param[in] copyArrayInfo if true, the image will copy the size and type information from the array
     *
     */
    FWDATA_API void setDataArray(::fwData::Array::sptr array, bool copyArrayInfo = true);

    ///get data array
    FWDATA_API ::fwData::Array::sptr getDataArray() const;

private:

    /// Get Pixel buffer
    FWDATA_API void* getPixelBuffer( IndexType index );
    FWDATA_API void* getPixelBuffer( IndexType index ) const;

    //! Size of the image (in terms of points)
    Size m_size{0, 0, 0};

    //! An array on the voxel size of the image
    Spacing m_spacing{0., 0., 0.};

    //! Origin of the image in 3D repair
    Origin m_origin{0., 0., 0.};

    //! Preferred window center/with
    ///@{
    double m_windowCenter{0.};
    double m_windowWidth{0.};
    ///@}

    //! Number of components
    size_t m_numberOfComponents{1};

    //! type of image pixel
    ::fwTools::Type m_type{::fwTools::Type::s_UNSPECIFIED_TYPE};

    //! image format
    PixelFormat m_pixelFormat {PixelFormat::UNDEFINED};

    //! image buffer
    ::fwData::Array::sptr m_dataArray;
};

//-----------------------------------------------------------------------------

inline double Image::getWindowCenter () const
{
    return m_windowCenter;
}

//-----------------------------------------------------------------------------

inline void Image::setWindowCenter (double val)
{
    m_windowCenter = val;
}

//-----------------------------------------------------------------------------

inline double Image::getWindowWidth () const
{
    return m_windowWidth;
}

//-----------------------------------------------------------------------------

inline void Image::setWindowWidth (double val)
{
    m_windowWidth = val;
}

//-----------------------------------------------------------------------------

inline size_t Image::getNumberOfComponents() const
{
    return m_numberOfComponents;
}

//-----------------------------------------------------------------------------

inline void Image::setNumberOfComponents(size_t val)
{
    m_numberOfComponents = val;
}

//-----------------------------------------------------------------------------

inline void Image::setPixelFormat(PixelFormat format)
{
    m_pixelFormat = format;
}

//-----------------------------------------------------------------------------

inline Image::PixelFormat Image::getPixelFormat() const
{
    return m_pixelFormat;
}

//------------------------------------------------------------------------------

inline const Image::Spacing& Image::getSpacing2() const
{
    return m_spacing;
}
//------------------------------------------------------------------------------

inline void Image::setSpacing2(const Spacing& spacing)
{
    m_spacing = spacing;
}

//------------------------------------------------------------------------------

inline const Image::Origin& Image::getOrigin2() const
{
    return m_origin;
}

//------------------------------------------------------------------------------

inline void Image::setOrigin2(const Origin& origin)
{
    m_origin = origin;
}

//------------------------------------------------------------------------------

inline const Image::Size& Image::getSize2() const
{
    return m_size;
}

//------------------------------------------------------------------------------

inline void Image::setSize2(const Size& size)
{
    m_size = size;
}

//------------------------------------------------------------------------------

template< typename T >
inline Image::Iterator<T> Image::begin()
{
    return m_dataArray->begin<T>();
}

//------------------------------------------------------------------------------

template< typename T >
inline Image::Iterator<T> Image::end()
{
    return m_dataArray->end<T>();
}

//------------------------------------------------------------------------------

template< typename T >
inline Image::ConstIterator<T> Image::begin() const
{
    return m_dataArray->begin<T>();
}

//------------------------------------------------------------------------------

template< typename T >
inline Image::ConstIterator<T> Image::end() const
{
    return m_dataArray->end<T>();
}

//------------------------------------------------------------------------------

template< typename T >
inline T& Image::at(IndexType id)
{
    return *reinterpret_cast<T*>(this->getPixelBuffer(id));
}

//------------------------------------------------------------------------------

template< typename T >
inline T Image::at(IndexType id) const
{
    return *reinterpret_cast<T*>(this->getPixelBuffer(id));
}

//------------------------------------------------------------------------------

template< typename T >
inline T& Image::at(IndexType x, IndexType y, IndexType z)
{
    const IndexType offset = x + m_size[0]*y + z*m_size[0]*m_size[1];
    return *reinterpret_cast<T*>(this->getPixelBuffer(offset));
}

//------------------------------------------------------------------------------

template< typename T >
inline T Image::at(IndexType x, IndexType y, IndexType z) const
{
    const IndexType offset = x + m_size[0]*y + z*m_size[0]*m_size[1];
    return *reinterpret_cast<T*>(this->getPixelBuffer(offset));
}

//-----------------------------------------------------------------------------

} // namespace fwData
