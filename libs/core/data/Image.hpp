/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "data/iterator.hpp"
#include "data/Object.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/memory/IBuffered.hpp>
#include <core/Type.hpp>

#include <data/iterator.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <array>
#include <filesystem>
#include <vector>

namespace sight::data
{

class Point;
class PointList;

/**
 * @brief This class defines an image
 *
 * An image contains a buffer and is defined by some parameters (size, spacing, pixel type, ...)
 *
 * The buffer type is defined by core::Type that provides the basic types ([u]int8, [u]int16, [u]int32,
 *[u]int64,
 * float and double).
 *
 * The image size is a 3D std::size_t array but the third dimension can be 0 for a 2D image.
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
 * To resize the image, you must pass the Type ([u]int[8|16|32|64], double, float), the size and the pixel
 * format of the buffer when calling resize(const Size& size, const core::Type& type, PixelFormat format).
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
    const auto dumpLock = image->dump_lock();

    // retrieve the value at index (x, y, z)
    value = image->at<std::int16_t>(x, y, z);

    // or you can compute the index like
    const auto size = image->getSize();
    const std::size_t index = x + y*size[0] + z*size[0]*size[1];
    value = image->at<std::int16_t>(index);
   @endcode
 *
 * @subsection iterators iterators
 *
 * To parse the buffer from beginning to end, the iterator can be used (iterator::ImageiteratorBase).
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
 * To get an iterator on the image, use begin<T>() and end<T>() methods.
 *
 * @warning The iterator does not assert that the image type is the same as the given format. It only asserts (in debug)
 * that the iterator does not iterate outside of the buffer bounds).
 *
 * \b Example :
 * @code{.cpp}
    Image::sptr img = Image::New();
    img->resize(1920, 1080, 1, core::Type::UINT8, Image::PixelFormat::RGBA);
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
    const auto size = image->getSize();

    auto iter    = image->begin<Color>();

    for (std::size_t z=0 ; z<size[2] ; ++z)
    {
        for (std::size_t y=0 ; y<size[1] ; ++y)
        {
            for (std::size_t x=0 ; x<size[0] ; ++x)
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
class DATA_CLASS_API Image : public virtual Object,
                             public core::memory::IBuffered
{
public:

    SIGHT_DECLARE_CLASS(Image, Object, factory::New<Image>);
    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// Image size
    typedef std::array<std::size_t, 3> Size;
    /// Image origin
    typedef std::array<double, 3> Origin;
    /// Image spacing
    typedef std::array<double, 3> Spacing;

    typedef Size::value_type IndexType;
    typedef std::uint8_t BufferType;

    /// Image format
    enum PixelFormat
    {
        UNDEFINED = 0, ///< Undefined pixel format
        RGB,           ///< Image with 3 component RGB.
        RGBA,          ///< Image with 4 component RGBA.
        BGR,           ///< Image with 3 component BGR.
        BGRA,          ///< Image with 4 component BGRA.
        GRAY_SCALE,    ///< Image with 1 component.
        RG,            ///< Image with 2 components RG.
        _SIZE
    };

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Image(Object::Key key);

    /**
     * @brief Destructor
     */
    DATA_API ~Image() noexcept override = default;

    /// @brief get image information from source. Informations are spacing,origin,size ... expect Fields
    DATA_API void copyInformation(Image::csptr _source);

    /// Get image spacing
    const Spacing& getSpacing() const;
    /// Set image spacing
    void setSpacing(const Spacing& spacing);

    /// Get image origin
    const Origin& getOrigin() const;
    /// Set image origin
    void setOrigin(const Origin& origin);

    /// Get image size
    const Size& getSize() const;

    /// Number of dimensions of the image (3 for 3D image)
    DATA_API std::size_t numDimensions() const;

    /** @{
     *  @brief Get/set preferred window center
     */
    virtual std::vector<double> getWindowCenter() const noexcept;
    virtual void setWindowCenter(const std::vector<double>& windowCenters);
    /// @}

    /** @{
     *  @brief Get/set preferred window width
     */
    virtual std::vector<double> getWindowWidth() const noexcept;
    virtual void setWindowWidth(const std::vector<double>& windowWidths);
    /// @}

    /// Get the number of elements (ie: size[0]*size[1]*size[2]*nbComponents)
    DATA_API std::size_t numElements() const;

    /// Get the number of components of an image pixel
    std::size_t numComponents() const;

    /// Get image type
    DATA_API core::Type getType() const;

    /// Get pixel format
    PixelFormat getPixelFormat() const;

    /**
       @{
     * @brief Resize the image and allocate the memory if needed.
     *
     * @param size array of size in each direction (x,y,z)
     * @param type type of a single pixel component value
     * @param format specify the ordering and the meaning of a pixel components
     *
     * If the data array owns its buffer, this method will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    DATA_API std::size_t resize(const Size& size, const core::Type& type, PixelFormat format);
    /// @}

    /// @brief return image size in bytes
    DATA_API std::size_t getSizeInBytes() const;
    /// @brief return allocated image size in bytes
    DATA_API std::size_t getAllocatedSizeInBytes() const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when image's buffer is added
    typedef core::com::Signal<void ()> BufferModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_BUFFER_MODIFIED_SIG;

    /// Type of signal when a landmark is added
    typedef core::com::Signal<void (SPTR(Point))> LandmarkAddedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_LANDMARK_ADDED_SIG;

    /// Type of signal when a landmark is removed
    typedef core::com::Signal<void (SPTR(Point))> LandmarkRemovedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_LANDMARK_REMOVED_SIG;

    /// Type of signal when a distance is added
    typedef core::com::Signal<void (bool)> LandmarkDisplayedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_LANDMARK_DISPLAYED_SIG;

    /// Type of signal when a distance is added
    typedef core::com::Signal<void (bool)> DistanceDisplayedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_DISTANCE_DISPLAYED_SIG;

    /// Type of signal when a distance is added
    typedef core::com::Signal<void (SPTR(PointList))> DistanceAddedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_DISTANCE_ADDED_SIG;

    /// Type of signal when a distance is removed
    typedef core::com::Signal<void (CSPTR(PointList))> DistanceRemovedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_DISTANCE_REMOVED_SIG;

    /// Type of signal when slice index is modified (axial index, frontal index, sagittal index)
    typedef core::com::Signal<void (int, int, int)> SliceIndexModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_SLICE_INDEX_MODIFIED_SIG;

    /// Type of signal when slice type is modified (from slice type, to slice type)
    typedef core::com::Signal<void (int, int)> SliceTypeModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_SLICE_TYPE_MODIFIED_SIG;
    /**
     * @}
     */

    /**
     * @name Iteration typedefs
     * @{
     */
    /// Image iterator
    template<typename T>
    using iterator = array_iterator<T>;
    template<typename T>
    using const_iterator = array_iterator<const T>;
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
     * @see iterator::rgba
     *
     * Example:
     * @code{.cpp}
        Image::sptr img = Image::New();
        img->resize(1920, 1080, 0, core::Type::UINT8, Image::PixelFormat::RGBA);
        Image::iterator< Color > iter    = img->begin< Color >();
        const Image::iterator< Color > iterEnd = img->end< Color >();

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
     * @{
     */
    template<typename T>
    iterator<T> begin();
    template<typename T>
    iterator<T> end();
    template<typename T>
    const_iterator<T> begin() const;
    template<typename T>
    const_iterator<T> end() const;
    template<typename T>
    const_iterator<T> cbegin() const;
    template<typename T>
    const_iterator<T> cend() const;
    /// @}

    /**
     * @brief Returns a range of begin/end iterators, especially useful to be used in "for range loops".
     * @{
     */
    template<typename T>
    auto range();
    template<typename T>
    auto crange() const;
    /// @}

    /**
     * @brief Returns the begin/end iterators to the array buffer, cast to char
     *
     * Iterate through all the element of the buffer.
     * @{
     */
    DATA_API iterator<char> begin();
    DATA_API iterator<char> end();
    DATA_API const_iterator<char> begin() const;
    DATA_API const_iterator<char> end() const;
    /// @}

    ///
    /// @{
    /// Returns image buffer
    DATA_API void* getBuffer();
    DATA_API const void* getBuffer() const;
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
        const core::Type& type,
        const Image::Size& size,
        PixelFormat format,
        core::memory::BufferAllocationPolicy::sptr policy = core::memory::BufferMallocPolicy::New()
    );

    /**
     * @{
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item image index
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     * @throw Exception Index out of bounds
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
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     * @throw Exception Index out of bounds
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
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     */
    DATA_API void* getPixel(IndexType index);

    /**
     * @brief Return a pointer on a image pixel
     * @param index offset of the pixel
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     */
    DATA_API const void* getPixel(IndexType index) const;

    /**
     * @brief Set pixel value represented as a void* buffer
     * @param index offset of the pixel
     * @param pixBuf pixel value represented as a void* buffer
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     */
    DATA_API void setPixel(IndexType index, const BufferType* pixBuf);

    /// Return the pixel value in a std::string
    DATA_API std::string getPixelAsString(
        IndexType x,
        IndexType y,
        IndexType z
    ) const;

    /// Return the buffer object
    DATA_API core::memory::BufferObject::sptr getBufferObject();

    /// Return the buffer object
    DATA_API core::memory::BufferObject::csptr getBufferObject() const;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Image& other) const noexcept;
    DATA_API bool operator!=(const Image& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

protected:

    /// Add a lock on the image in the given vector to prevent from dumping the buffer on the disk
    /// This is needed for IBuffered interface implementation
    DATA_API void dump_lock_impl(std::vector<core::memory::BufferObject::Lock>& locks) const override;

private:

    /**
       @{
     * @brief Resize the image and allocate the memory if needed.
     *
     * @param size array of size in each direction (x,y,z)
     * @param type type of a single pixel component value
     * @param format specify the ordering and the meaning of a pixel components
     * @param realloc allows to not reallocate, for instance when importing directly the buffer with setBuffer()
     *
     * If the data array owns its buffer, this method will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    DATA_API std::size_t _resize(const Size& size, const core::Type& type, PixelFormat format, bool realloc);
    /// @}

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

    //! Preferred window center/with
    ///@{
    std::vector<double> m_windowCenters;
    std::vector<double> m_windowWidths;
    ///@}

    //! Number of components
    std::size_t m_numComponents {1};

    //! type of image pixel
    core::Type m_type {core::Type::UINT8};

    //! image format
    PixelFormat m_pixelFormat {PixelFormat::UNDEFINED};

    //! image buffer
    Array::sptr m_dataArray;
};

//-----------------------------------------------------------------------------

inline std::vector<double> Image::getWindowCenter() const noexcept
{
    return m_windowCenters;
}

//-----------------------------------------------------------------------------

inline void Image::setWindowCenter(const std::vector<double>& windowCenters)
{
    m_windowCenters = windowCenters;
}

//-----------------------------------------------------------------------------

inline std::vector<double> Image::getWindowWidth() const noexcept
{
    return m_windowWidths;
}

//-----------------------------------------------------------------------------

inline void Image::setWindowWidth(const std::vector<double>& windowWidths)
{
    m_windowWidths = windowWidths;
}

//-----------------------------------------------------------------------------

inline std::size_t Image::numComponents() const
{
    return m_numComponents;
}

//-----------------------------------------------------------------------------

inline Image::PixelFormat Image::getPixelFormat() const
{
    return m_pixelFormat;
}

//------------------------------------------------------------------------------

inline const Image::Spacing& Image::getSpacing() const
{
    return m_spacing;
}

//------------------------------------------------------------------------------

inline void Image::setSpacing(const Spacing& spacing)
{
    m_spacing = spacing;
}

//------------------------------------------------------------------------------

inline const Image::Origin& Image::getOrigin() const
{
    return m_origin;
}

//------------------------------------------------------------------------------

inline void Image::setOrigin(const Origin& origin)
{
    m_origin = origin;
}

//------------------------------------------------------------------------------

inline const Image::Size& Image::getSize() const
{
    return m_size;
}

//------------------------------------------------------------------------------

template<typename T>
inline Image::iterator<T> Image::begin()
{
    return iterator<T>(static_cast<typename iterator<T>::pointer>(getBuffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline Image::iterator<T> Image::end()
{
    auto itr = begin<T>();
    itr += static_cast<typename iterator<T>::difference_type>(this->getSizeInBytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline Image::const_iterator<T> Image::begin() const
{
    return const_iterator<T>(static_cast<typename const_iterator<T>::pointer>(getBuffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline Image::const_iterator<T> Image::end() const
{
    auto itr = begin<T>();
    itr += static_cast<typename const_iterator<T>::difference_type>(this->getSizeInBytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline Image::const_iterator<T> Image::cbegin() const
{
    return const_iterator<T>(static_cast<typename const_iterator<T>::pointer>(getBuffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline Image::const_iterator<T> Image::cend() const
{
    auto itr = begin<T>();
    itr += static_cast<typename const_iterator<T>::difference_type>(this->getSizeInBytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
auto Image::range()
{
    auto b = begin<T>();
    auto e = end<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename T>
auto Image::crange() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename T>
inline T& Image::at(IndexType id)
{
    return *reinterpret_cast<T*>(this->getPixel(id));
}

//------------------------------------------------------------------------------

template<typename T>
inline T Image::at(IndexType id) const
{
    return *reinterpret_cast<const T*>(this->getPixel(id));
}

//------------------------------------------------------------------------------

template<typename T>
inline T& Image::at(IndexType x, IndexType y, IndexType z, IndexType c)
{
    const IndexType offset = x + m_size[0] * y + z * m_size[0] * m_size[1];
    return *(reinterpret_cast<T*>(this->getPixel(offset)) + c);
}

//------------------------------------------------------------------------------

template<typename T>
inline T Image::at(IndexType x, IndexType y, IndexType z, IndexType c) const
{
    const IndexType offset = x + m_size[0] * y + z * m_size[0] * m_size[1];
    return *(reinterpret_cast<const T*>(this->getPixel(offset)) + c);
}

} // namespace sight::data
