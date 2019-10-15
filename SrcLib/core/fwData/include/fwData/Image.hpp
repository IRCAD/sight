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
#include "fwData/iterator/ImageIterator.hpp"
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
    enum PixelFormat
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

    /// Get the number of elements (ie: size[0]*size[1]*size[2]*nbComponents)
    FWDATA_API size_t getNumElements() const;

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
     * @brief Resize the image and allocate the memory if needed.
     *
     * If the data array owns its buffer, these methods will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    FWDATA_API size_t resize();

    FWDATA_API size_t resize(IndexType x, IndexType y,  IndexType z, const ::fwTools::Type& type, PixelFormat format);
    FWDATA_API size_t resize(const Size& size, const ::fwTools::Type& type, PixelFormat format);
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

    /**
     * @name Iteration typedefs
     * @{
     */
    /// Image iterator
    template <typename FORMAT>
    using Iterator = iterator::ImageIteratorBase<FORMAT, false>;
    template <typename FORMAT>
    /// Image const iterator
    using ConstIterator = iterator::ImageIteratorBase<FORMAT, true>;
    /// Format used to iterate though all the buffer values
    template <typename TYPE>
    using Iteration = typename iterator::IterationBase<TYPE>::Raw;
    /// Format used to iterate though a RGB image in uint8
    typedef iterator::IterationBase<std::uint8_t>::RGB RGBIteration;
    /// Format used to iterate though a RGBA image in uint8
    typedef iterator::IterationBase<std::uint8_t>::RGBA RGBAIteration;
    /// Format used to iterate though a BGR image in uint8
    typedef iterator::IterationBase<std::uint8_t>::BGR BGRIteration;
    /// Format used to iterate though a BGRA image in uint8
    typedef iterator::IterationBase<std::uint8_t>::BGRA BGRAIteration;
    /// @}

    /**
     * @brief Returns the begin/end iterators to the image buffer, cast to T
     *
     * Iterate through all the elements of the buffer.
     * The format should be one of the formats defined by IterationBase (.
     *
     * Example:
     * @code{.cpp}
        ::fwData::Image::sptr img = ::fwData::Image::New();
        img->resize({1920, 1080}, ::fwTools::Type::s_UINT8, ::fwData::Image::PixelFormat::RGBA);
        ::fwData::Image:Iterator<RGBAIteration> iter    = img->begin<RGBAIteration>();
        const ImageIteratorBase<RGBAIteration> iterEnd = img->end<RGBAIteration>();

        for (; iter != iterEnd; ++iter)
        {
            iter->r = val1;
            iter->g = val2;
            iter->b = val2;
            iter->a = val4;
        }
       @endcode
     *
     * @warning Print a warning if T is different from the array type
     * @note These functions lock the buffer
     * @{
     */
    template< typename FORMAT > Iterator<FORMAT> begin();
    template< typename FORMAT > Iterator<FORMAT> end();
    template< typename FORMAT > ConstIterator<FORMAT> begin() const;
    template< typename FORMAT > ConstIterator<FORMAT> end() const;
    /// @}
    /**
     * @brief Returns the begin/end iterators to the array buffer, cast to char
     *
     * Iterate through all the element of the buffer.
     *
     * @note These functions lock the buffer
     * @{
     */
    FWDATA_API Iterator<iterator::IterationBase<char>::Raw> begin();
    FWDATA_API Iterator<iterator::IterationBase<char>::Raw> end();
    FWDATA_API ConstIterator<iterator::IterationBase<char>::Raw> begin() const;
    FWDATA_API ConstIterator<iterator::IterationBase<char>::Raw> end() const;
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
     * @deprecated Use getSizeSpacing2()/setSpacing2(), it will be removed in sight 22.0
     */

    [[deprecated("it will be removed in sight 22.0, use getSpacing2()")]]
    FWDATA_API const SpacingType& getSpacing() const;
    [[deprecated("it will be removed in sight 22.0, use setSpacing2()")]]
    FWDATA_API void setSpacing(const SpacingType& spacing);
    /// @}

    /** @{
     * @brief get/set image origin
     * @deprecated Use getOrigin2()/setOrigin2(), it will be removed in sight 22.0
     */
    [[deprecated("it will be removed in sight 22.0, use getOrigin2()")]]
    FWDATA_API const OriginType& getOrigin() const;
    [[deprecated("it will be removed in sight 22.0, use setOrigin2()")]]
    FWDATA_API void setOrigin(const OriginType& origin);
    /// @}

    /** @{
     * @brief get/set image size
     * @deprecated Use getSize2()/setSize2(), it will be removed in sight 22.0
     */
    [[deprecated("it will be removed in sight 22.0, use getSize2()")]]
    FWDATA_API const SizeType& getSize() const;
    [[deprecated("it will be removed in sight 22.0, use setSize2()")]]
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
     * @deprecated Allocate methods will be removed in sight 22.0. Use resize() methods instead
     */
    [[deprecated("it will be removed in sight 22.0, use resize() instead")]]
    FWDATA_API size_t allocate();
    [[deprecated("it will be removed in sight 22.0, use resize() instead")]]
    FWDATA_API size_t allocate(SizeType::value_type x, SizeType::value_type y,  SizeType::value_type z,
                               const ::fwTools::Type& type, size_t numberOfComponents = 1);
    [[deprecated("it will be removed in sight 22.0, use resize() instead")]]
    FWDATA_API size_t allocate(const SizeType& size, const ::fwTools::Type& type, size_t numberOfComponents = 1);
    /// @}

    /**
     * @brief get a DynamicType for retrocompatibility
     * @deprecated Use getType()
     */
    [[deprecated("it will be removed in sight 22.0, use getType()")]]
    FWDATA_API ::fwTools::DynamicType getPixelType() const;

    /**
     * @brief set data array
     *
     * @param[in] array data array
     * @param[in] copyArrayInfo if true, the image will copy the size and type information from the array
     *
     */
    [[deprecated("it will be removed in sight 22.0")]]
    FWDATA_API void setDataArray(::fwData::Array::sptr array, bool copyArrayInfo = true);

    ///get data array
    [[deprecated("it will be removed in sight 22.0")]]
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

    /// Deprecated
    SizeType m_oldSize;
    SpacingType m_oldSpacing;
    OriginType m_oldOrigin;

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

    // deprecated: to support old API
    const size_t dims = this->getNumberOfDimensions();
    m_oldSpacing.resize(dims);
    for (size_t i = 0; i < dims; ++i)
    {
        m_oldSpacing[i] = m_spacing[i];
    }
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

    // deprecated: to support old API
    const size_t dims = this->getNumberOfDimensions();
    m_oldOrigin.resize(dims);
    for (size_t i = 0; i < dims; ++i)
    {
        m_oldOrigin[i] = m_origin[i];
    }
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

    // deprecated: to support old API
    const size_t dims = this->getNumberOfDimensions();
    m_oldSize.resize(dims);
    for (size_t i = 0; i < dims; ++i)
    {
        m_oldSize[i] = m_size[i];
    }
}

//------------------------------------------------------------------------------

template< typename F >
inline Image::Iterator<F> Image::begin()
{
    return Iterator<F>(this);
}

//------------------------------------------------------------------------------

template< typename F >
inline Image::Iterator<F> Image::end()
{
    auto itr = Iterator<F>(this);
    itr += static_cast< typename Iterator<F>::difference_type>(this->getNumElements()/F::elementSize);
    return itr;
}

//------------------------------------------------------------------------------

template< typename F >
inline Image::ConstIterator<F> Image::begin() const
{
    return ConstIterator<F>(this);
}

//------------------------------------------------------------------------------

template< typename F >
inline Image::ConstIterator<F> Image::end() const
{
    auto itr = ConstIterator<F>(this);
    itr += static_cast< typename Iterator<F>::difference_type>(this->getNumElements()/F::elementSize);
    return itr;
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
