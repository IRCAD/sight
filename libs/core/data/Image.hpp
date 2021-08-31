/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/Array.hpp"
#include "data/factory/new.hpp"
#include "data/iterator/ImageIterator.hpp"
#include "data/Object.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/memory/IBuffered.hpp>
#include <core/tools/Type.hpp>

#include <boost/shared_array.hpp>

#include <filesystem>
#include <vector>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (Image));

namespace sight::data
{

class Point;
class PointList;

/**
 * @brief This class defines an image
 *
 * An image contains a buffer and is defined by some parameters (size, spacing, pixel type, ...)
 *
 * The buffer type is defined by core::tools::Type that provides the basic types ([u]int8, [u]int16, [u]int32, [u]int64,
 * float and double).
 *
 * The image size is a 3D size_t array but the third dimension can be 0 for a 2D image.
 *
 * The image PixelFormat represents the buffer organization in components (GRAY_SCALE: 1 component, RGB and BGR: 3
 * components, RGBA and BGRA: 4 components).
 *
 * @section Usage Usage
 *
 * @subsection Allocation Allocation
 *
 * The image buffer is allocated using the resize() method.
 * You can get the allocated size using getSizeInBytes() and getAllocatedSizeInBytes().
 *
 * @warning The allocated size can be different from the image size: it can happen if you called setSize() without
 * calling resize(). It may be useful when you don't want to reallocate the image too often, but you need to be sure to
 * allocate enough memory.
 *
 * To resize the image, you must define the Type ([u]int[8|16|32|64], double, float), the size and the pixel
 * format of the buffer. You can use setSize2(const Size& size), setType(core::tools::Type type) and
 * setPixelFormat(PixelFormat format) or directly call
 * resize(const Size& size, const core::tools::Type& type, PixelFormat format).
 *
 * @section Access Buffer access
 *
 * You can access voxel values using at<type>(IndexType id) or
 * at<type>(IndexType x, IndexType y, IndexType z, IndexType c) methods. These methods are slow and should not be used
 * to parse the entire buffer (see iterators).
 *
 * You can also use getPixelAsString() to retrieve the value as a string (useful for displaying information).
 *
 * @warning The image must be locked for dump before accessing the buffer. It prevents the buffer to be dumped on the
 * disk.
 *
 * \b Example:
 *
 * @code{.cpp}

    // 3D image of std::int16_t

    // prevent the buffer to be dumped on the disk
    const auto dumpLock = image->lock();

    // retrieve the value at index (x, y, z)
    value = image->at<std::int16_t>(x, y, z);

    // or you can compute the index like
    const auto size = image->getSize2();
    const size_t index = x + y*size[0] + z*size[0]*size[1];
    value = image->at<std::int16_t>(index);
   @endcode
 *
 * @subsection Iterators Iterators
 *
 * To parse the buffer from beginning to end, the iterator can be used (data::iterator::ImageIteratorBase).
 *
 * The iteration depends on the given format. The format can be the buffer type ([u]int[8|16|32|64], double, float), but
 * can also be a simple struct like:
 *
 * @code{.cpp}
    struct Color {
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;
    };
    @endcode
 *
 * This struct allows to parse the image as an RGBA buffer (RGBARGBARGBA....).
 *
 * To get an iterator on the image, use begin<FORMAT>() and end<FORMAT>() methods.
 *
 * @warning The iterator does not assert that the image type is the same as the given format. It only asserts (in debug)
 * that the iterator does not iterate outside of the buffer bounds).
 *
 * \b Example :
 * @code{.cpp}
    data::Image::sptr img = data::Image::New();
    img->resize(1920, 1080, 1, core::tools::Type::s_UINT8, data::Image::PixelFormat::RGBA);
    auto iter    = img->begin<Color>();
    const auto iterEnd = img->end<Color>();

    for (; iter != iterEnd; ++iter)
    {
        iter->r = val1;
        iter->g = val2;
        iter->b = val2;
        iter->a = val4;
    }
   @endcode
 *
 */
/* *INDENT-OFF* */
/**
 * @note If you need to know (x, y, z) indices, you can parse the array looping from the last dimension to the first,
 * like:
 * @code{.cpp}
    const auto size = image->getSize2();

    auto iter    = image->begin<Color>();

    for (size_t z=0 ; z<size[2] ; ++z)
    {
        for (size_t y=0 ; y<size[1] ; ++y)
        {
            for (size_t x=0 ; x<size[0] ; ++x)
            {
                // do something with x and y ....

                // retrieve the value
                val1 = iter->r;
                val2 = iter->g;
                val3 = iter->b;
                val4 = iter->a;

                // increment iterator
                ++iter;
            }
        }
    }
   @endcode
 */
/* *INDENT-ON* */
class DATA_CLASS_API Image : public data::Object,
                             public core::memory::IBuffered
{
public:

    SIGHT_DECLARE_CLASS(Image, data::Object, data::factory::New<Image>);
    SIGHT_ALLOW_SHARED_FROM_THIS()
    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (Image))

    /// Image size
    typedef std::array<size_t, 3> Size;
    /// Image origin
    typedef std::array<double, 3> Origin;
    /// Image spacing
    typedef std::array<double, 3> Spacing;

    typedef Size::value_type IndexType;
    typedef std::uint8_t BufferType;
    typedef ::boost::shared_array<BufferType> SharedArray;

    /// Image format
    enum PixelFormat
    {
        UNDEFINED = 0, ///< Undefined pixel format
        RGB,           ///< Image with 3 component RGB.
        RGBA,          ///< Image with 4 component RGBA.
        BGR,           ///< Image with 3 component BGR.
        BGRA,          ///< Image with 4 component BGRA.
        GRAY_SCALE     ///< Image with 1 component.
    };

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Image(data::Object::Key key);

    /**
     * @brief Destructor
     */
    DATA_API ~Image() noexcept override;

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// @brief get image information from source. Informations are spacing,origin,size ... expect Fields
    DATA_API void copyInformation(Image::csptr _source);

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
     * @warning This method does not resize the buffer. You must call resize for that.
     * @todo Rename to setSize when the deprecated API is removed
     */
    void setSize2(const Size& size);

    /// Number of dimensions of the image (3 for 3D image)
    DATA_API size_t getNumberOfDimensions() const;

    /** @{
     *  @brief Get/set preferred window center
     */
    double getWindowCenter() const;

    void setWindowCenter(double val);
    /// @}

    /** @{
     *  @brief Get/set preferred window width
     */
    double getWindowWidth() const;
    void setWindowWidth(double val);
    /// @}

    /// Get the number of elements (ie: size[0]*size[1]*size[2]*nbComponents)
    DATA_API size_t getNumElements() const;

    /** @{
     *  @brief Get/set preferred window center
     */
    size_t getNumberOfComponents() const;

    void setNumberOfComponents(size_t val);
    /// @}

    /** @{
     * @brief get/set image type
     * @warning This method does not resize the buffer with the new type. You must call resize for that.
     */
    DATA_API void setType(core::tools::Type type);
    DATA_API void setType(const std::string& type);
    DATA_API core::tools::Type getType() const;
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
    DATA_API size_t resize();

    DATA_API size_t resize(IndexType x, IndexType y, IndexType z, const core::tools::Type& type, PixelFormat format);
    DATA_API size_t resize(const Size& size, const core::tools::Type& type, PixelFormat format);
    /// @}

    /// @brief return image size in bytes
    DATA_API size_t getSizeInBytes() const;
    /// @brief return allocated image size in bytes
    DATA_API size_t getAllocatedSizeInBytes() const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when image's buffer is added
    typedef core::com::Signal<void ()> BufferModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_BUFFER_MODIFIED_SIG;

    /// Type of signal when a landmark is added
    typedef core::com::Signal<void (SPTR(data::Point))> LandmarkAddedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_LANDMARK_ADDED_SIG;

    /// Type of signal when a landmark is removed
    typedef core::com::Signal<void (SPTR(data::Point))> LandmarkRemovedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_LANDMARK_REMOVED_SIG;

    /// Type of signal when a distance is added
    typedef core::com::Signal<void (bool)> LandmarkDisplayedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_LANDMARK_DISPLAYED_SIG;

    /// Type of signal when a distance is added
    typedef core::com::Signal<void (bool)> DistanceDisplayedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_DISTANCE_DISPLAYED_SIG;

    /// Type of signal when a distance is added
    typedef core::com::Signal<void (SPTR(data::PointList))> DistanceAddedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_DISTANCE_ADDED_SIG;

    /// Type of signal when a distance is removed
    typedef core::com::Signal<void (CSPTR(data::PointList))> DistanceRemovedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_DISTANCE_REMOVED_SIG;

    /// Type of signal when slice index is modified (axial index, frontal index, sagittal index)
    typedef core::com::Signal<void (int, int, int)> SliceIndexModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_SLICE_INDEX_MODIFIED_SIG;

    /// Type of signal when slice type is modified (from slice type, to slice type)
    typedef core::com::Signal<void (int, int)> SliceTypeModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_SLICE_TYPE_MODIFIED_SIG;

    /// Type of signal when visibility is modified
    typedef core::com::Signal<void (bool)> VisibilityModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_VISIBILITY_MODIFIED_SIG;

    /// Type of signal when visibility is modified
    typedef core::com::Signal<void ()> TransparencyModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_TRANSPARENCY_MODIFIED_SIG;

    /**
     * @}
     */

    /**
     * @name Iteration typedefs
     * @{
     */
    /// Image iterator
    template<typename FORMAT>
    using Iterator = iterator::ImageIteratorBase<FORMAT, false>;
    template<typename FORMAT>
    /// Image const iterator
    using ConstIterator = iterator::ImageIteratorBase<FORMAT, true>;
    /// @}

    /**
     * @brief Returns the begin/end iterators to the image buffer, cast to T
     *
     * Iterate through all the elements of the buffer.
     * The format can be the buffer type ([u]int[8|16|32|64], double, float), and can also be a simple struct like:
     *
     * @code{.cpp}
        struct Color {
            std::uint8_t r;
            std::uint8_t g;
            std::uint8_t b;
            std::uint8_t a;
        };
        @endcode
     * @see data::iterator::RGBA
     *
     * Example:
     * @code{.cpp}
        data::Image::sptr img = data::Image::New();
        img->resize(1920, 1080, 0, core::tools::Type::s_UINT8, data::Image::PixelFormat::RGBA);
        data::Image::Iterator< Color > iter    = img->begin< Color >();
        const data::Image::Iterator< Color > iterEnd = img->end< Color >();

        for (; iter != iterEnd; ++iter)
        {
            iter->r = val1;
            iter->g = val2;
            iter->b = val2;
            iter->a = val4;
        }
       @endcode
     *
     * @warning The iterator does not assert that the buffer type is the same as the given format. It only asserts
     * (in debug) that the iterator does not iterate outside of the buffer bounds).
     * @note These functions lock the buffer for dump (see lock()).
     * @{
     */
    template<typename FORMAT>
    Iterator<FORMAT> begin();
    template<typename FORMAT>
    Iterator<FORMAT> end();
    template<typename FORMAT>
    ConstIterator<FORMAT> begin() const;
    template<typename FORMAT>
    ConstIterator<FORMAT> end() const;
    /// @}
    /**
     * @brief Returns the begin/end iterators to the array buffer, cast to char
     *
     * Iterate through all the element of the buffer.
     *
     * @note These functions lock the buffer
     * @{
     */
    DATA_API Iterator<char> begin();
    DATA_API Iterator<char> end();
    DATA_API ConstIterator<char> begin() const;
    DATA_API ConstIterator<char> end() const;
    /// @}

    ///
    /// @{
    /// Returns image buffer
    DATA_API void* getBuffer();
    DATA_API void* getBuffer() const;
    /// @}

    /**
     * @brief Setter for the array buffer.
     *
     * An existing buffer will be released if the array own it.
     *
     * @param buf            Buffer to set as Array's buffer
     * @param takeOwnership  if true, the Array will manage allocation and destroy the buffer when needed.
     * @param type           Type of the array view
     * @param size           Size of the array view
     * @param policy If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    DATA_API void setBuffer(
        void* buf,
        bool takeOwnership,
        const core::tools::Type& type,
        const data::Image::Size& size,
        core::memory::BufferAllocationPolicy::sptr policy = core::memory::BufferMallocPolicy::New()
    );

    /** @{
     *  @brief Get/set pixel format
     */
    PixelFormat getPixelFormat() const;
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
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock())
     * @throw data::Exception Index out of bounds
     */
    template<typename T>
    T& at(IndexType id);
    template<typename T>
    T at(IndexType id) const;
    /// @}
    /**
     * @{
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param x x index
     * @param y y index
     * @param z z index
     * @param c component index
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock())
     * @throw data::Exception Index out of bounds
     */
    template<typename T>
    T& at(IndexType x, IndexType y, IndexType z, IndexType c = 0);
    template<typename T>
    T at(IndexType x, IndexType y, IndexType z, IndexType c = 0) const;
    /// @}
    ///

    /**
     * @brief Return a pointer on a image pixel
     * @param index offset of the pixel
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock())
     */
    DATA_API void* getPixelBuffer(IndexType index);

    /**
     * @brief Return a pointer on a image pixel
     * @param index offset of the pixel
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock())
     */
    DATA_API void* getPixelBuffer(IndexType index) const;

    /**
     * @brief Set pixel value represented as a void* buffer
     * @param index offset of the pixel
     * @param pixBuf pixel value represented as a void* buffer
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock())
     */
    DATA_API void setPixelBuffer(IndexType index, BufferType* pixBuf);

    /// Return the pixel value in a std::string
    DATA_API const std::string getPixelAsString(
        IndexType x,
        IndexType y,
        IndexType z
    ) const;

    /**
     * @brief Return a lock on the image to prevent from dumping the buffer on the disk
     *
     * When the buffer is dumped, the memory is released and the buffer will not be accessible. When lock() is called,
     * the buffer is restored from the disk if it was dumped and as long as the core::memory::BufferObject::Lock is
     * maintained, the buffer will not be dumped.
     *
     * An exception will be raised if you try to access while the array is not locked.
     */
    [[nodiscard]] DATA_API core::memory::BufferObject::Lock lock() const;

    /// Return the buffer object
    DATA_API core::memory::BufferObject::sptr getBufferObject();

    /// Return the buffer object
    DATA_API core::memory::BufferObject::csptr getBufferObject() const;
    /**
     * @brief Set a stream factory for the image's buffer manager
     *
     * The factory will be used to load the image on demand.
     *
     * @param factory core::memory::stream::in::IFactory stream factory
     * @param size size of data provided by the stream
     * @param sourceFile Filesystem path of the source file, if applicable
     * @param format file format (RAW,RAWZ,OTHER), if sourceFile is provided
     * @param policy Buffer allocation policy
     */
    DATA_API void setIStreamFactory(
        const SPTR(core::memory::stream::in::IFactory)& factory,
        const size_t size,
        const std::filesystem::path& sourceFile                  = "",
        const core::memory::FileFormatType format                = core::memory::OTHER,
        const core::memory::BufferAllocationPolicy::sptr& policy = core::memory::BufferMallocPolicy::New()
    );

    // ---------------------------------------
    // Deprecated API
    // ---------------------------------------

    /**
     * @brief Image size type
     */
    [[deprecated("it will be removed in sight 22.0, use Size")]] typedef data::Array::SizeType SizeType;

    [[deprecated("it will be removed in sight 22.0")]] typedef size_t BufferIndexType;

    /**
     * @brief Image spacing type
     */
    [[deprecated("it will be removed in sight 22.0, use Spacing")]] typedef std::vector<double> SpacingType;

    /**
     * @brief Image origin type
     */
    [[deprecated("it will be removed in sight 22.0, use Origin")]] typedef std::vector<double> OriginType;
    /** @{
     * @brief get/set image spacing
     * @deprecated Use getSizeSpacing2()/setSpacing2(), it will be removed in sight 22.0
     */

    [[deprecated("it will be removed in sight 22.0, use getSpacing2()")]]
    DATA_API const SpacingType& getSpacing() const;
    [[deprecated("it will be removed in sight 22.0, use setSpacing2()")]]
    DATA_API void setSpacing(const SpacingType& spacing);
    /// @}

    /** @{
     * @brief get/set image origin
     * @deprecated Use getOrigin2()/setOrigin2(), it will be removed in sight 22.0
     */
    [[deprecated("it will be removed in sight 22.0, use getOrigin2()")]]
    DATA_API const OriginType& getOrigin() const;
    [[deprecated("it will be removed in sight 22.0, use setOrigin2()")]]
    DATA_API void setOrigin(const OriginType& origin);
    /// @}

    /** @{
     * @brief get/set image size
     * @deprecated Use getSize2()/setSize2(), it will be removed in sight 22.0
     */
    [[deprecated("it will be removed in sight 22.0, use getSize2()")]]
    DATA_API const SizeType& getSize() const;
    [[deprecated("it will be removed in sight 22.0, use setSize2()")]]
    DATA_API void setSize(const SizeType& size);
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
    DATA_API size_t allocate();
    [[deprecated("it will be removed in sight 22.0, use resize() instead")]]
    DATA_API size_t allocate(
        SizeType::value_type x,
        SizeType::value_type y,
        SizeType::value_type z,
        const core::tools::Type& type,
        size_t numberOfComponents = 1
    );
    [[deprecated("it will be removed in sight 22.0, use resize() instead")]]
    DATA_API size_t allocate(const SizeType& size, const core::tools::Type& type, size_t numberOfComponents = 1);
    /// @}

    /**
     * @brief set data array
     *
     * @param[in] array data array
     * @param[in] copyArrayInfo if true, the image will copy the size and type information from the array
     *
     */
    [[deprecated("it will be removed in sight 22.0")]]
    DATA_API void setDataArray(data::Array::sptr array, bool copyArrayInfo = true);

    ///get data array
    [[deprecated("it will be removed in sight 22.0")]]
    DATA_API data::Array::sptr getDataArray() const;

protected:

    // To allow locked_ptr to access protected lockBuffer()
    template<class DATATYPE>
    friend class data::mt::locked_ptr;

    /**
     * @brief Add a lock on the image in the given vector to prevent from dumping the buffer on the disk
     *
     * This is needed for IBuffered interface implementation
     * The buffer cannot be accessed if the image is not locked
     */
    DATA_API void lockBuffer(std::vector<core::memory::BufferObject::Lock>& locks) const override;

private:

    /**
     * @brief Protected setter for the array buffer.
     * An existing buffer will be released if the array own it.
     *
     * @param buf Buffer to set as Array's buffer
     * @param takeOwnership if true, the Array will manage allocation and destroy the buffer when needed.
     * @param policy If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    void setBuffer(
        void* buf,
        bool takeOwnership                                = false,
        core::memory::BufferAllocationPolicy::sptr policy = core::memory::BufferMallocPolicy::New()
    );

    //! Size of the image (in terms of points)
    Size m_size {0, 0, 0};

    //! An array on the voxel size of the image
    Spacing m_spacing {0., 0., 0.};

    //! Origin of the image in 3D repair
    Origin m_origin {0., 0., 0.};

    /// Deprecated: set as mutable to be able to change it according to m_size, m_spacing and m_origin when we call
    /// the getters.
    [[deprecated("will be removed in sight 22.0")]] mutable SizeType m_oldSize;
    [[deprecated("will be removed in sight 22.0")]] mutable SpacingType m_oldSpacing;
    [[deprecated("will be removed in sight 22.0")]] mutable OriginType m_oldOrigin;

    //! Preferred window center/with
    ///@{
    double m_windowCenter {0.};
    double m_windowWidth {0.};
    ///@}

    //! Number of components
    size_t m_numberOfComponents {1};

    //! type of image pixel
    core::tools::Type m_type {core::tools::Type::s_UNSPECIFIED_TYPE};

    //! image format
    PixelFormat m_pixelFormat {PixelFormat::UNDEFINED};

    //! image buffer
    data::Array::sptr m_dataArray;
};

//-----------------------------------------------------------------------------

inline double Image::getWindowCenter() const
{
    return m_windowCenter;
}

//-----------------------------------------------------------------------------

inline void Image::setWindowCenter(double val)
{
    m_windowCenter = val;
}

//-----------------------------------------------------------------------------

inline double Image::getWindowWidth() const
{
    return m_windowWidth;
}

//-----------------------------------------------------------------------------

inline void Image::setWindowWidth(double val)
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

template<typename FORMAT>
inline Image::Iterator<FORMAT> Image::begin()
{
    return Iterator<FORMAT>(this);
}

//------------------------------------------------------------------------------

template<typename FORMAT>
inline Image::Iterator<FORMAT> Image::end()
{
    auto itr = Iterator<FORMAT>(this);
    itr += static_cast<typename Iterator<FORMAT>::difference_type>(this->getSizeInBytes() / sizeof(FORMAT));
    return itr;
}

//------------------------------------------------------------------------------

template<typename FORMAT>
inline Image::ConstIterator<FORMAT> Image::begin() const
{
    return ConstIterator<FORMAT>(this);
}

//------------------------------------------------------------------------------

template<typename FORMAT>
inline Image::ConstIterator<FORMAT> Image::end() const
{
    auto itr = ConstIterator<FORMAT>(this);
    itr += static_cast<typename Iterator<FORMAT>::difference_type>(this->getSizeInBytes() / sizeof(FORMAT));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline T& Image::at(IndexType id)
{
    return *reinterpret_cast<T*>(this->getPixelBuffer(id));
}

//------------------------------------------------------------------------------

template<typename T>
inline T Image::at(IndexType id) const
{
    return *reinterpret_cast<T*>(this->getPixelBuffer(id));
}

//------------------------------------------------------------------------------

template<typename T>
inline T& Image::at(IndexType x, IndexType y, IndexType z, IndexType c)
{
    const IndexType offset = x + m_size[0] * y + z * m_size[0] * m_size[1];
    return *(reinterpret_cast<T*>(this->getPixelBuffer(offset)) + c);
}

//------------------------------------------------------------------------------

template<typename T>
inline T Image::at(IndexType x, IndexType y, IndexType z, IndexType c) const
{
    const IndexType offset = x + m_size[0] * y + z * m_size[0] * m_size[1];
    return *(reinterpret_cast<T*>(this->getPixelBuffer(offset)) + c);
}

//-----------------------------------------------------------------------------

} // namespace sight::data
