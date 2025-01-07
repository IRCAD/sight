/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "data/array.hpp"
#include "data/iterator.hpp"

#include <core/memory/buffered.hpp>
#include <core/type.hpp>

#include <data/iterator.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <array>
#include <vector>

namespace sight::data
{

class point;
class point_list;

/**
 * @brief This class defines an image
 *
 * An image contains a buffer and is defined by some parameters (size, spacing, pixel type, ...)
 *
 * The buffer type is defined by core::type that provides the basic types ([u]int8, [u]int16, [u]int32,
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
 * You can get the allocated size using size_in_bytes() and getAllocatedSizeInBytes().
 *
 * To resize the image, you must pass the Type ([u]int[8|16|32|64], double, float), the size and the pixel
 * format of the buffer when calling resize(const Size& size, const core::type& type, PixelFormat format).
 *
 * @section Access Buffer access
 *
 * You can access voxel values using at<type>(index_t id) or
 * at<type>(index_t x, index_t y, index_t z, index_t c) methods. These methods are slow and should not be used
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
    const auto size = image->size();
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
 * that the iterator does iterate inside of the buffer bounds).
 *
 * \b Example :
 * @code{.cpp}
    image::sptr img = image::New();
    img->resize({1920, 1080, 1}, core::type::UINT8, image::pixel_format::rgba);
    auto iter    = img->begin<color>();
    const auto iterEnd = img->end<color>();

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
    const auto size = image->size();

    auto iter    = image->begin<color>();

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
class SIGHT_DATA_CLASS_API image : public virtual object,
                                   public core::memory::buffered
{
public:

    SIGHT_DECLARE_CLASS(image, object);
    SIGHT_ALLOW_SHARED_FROM_THIS()

    using size_t        = std::array<std::size_t, 3>;
    using origin_t      = std::array<double, 3>;
    using orientation_t = std::array<double, 9>;
    using spacing_t     = std::array<double, 3>;

    using index_t  = size_t::value_type;
    using buffer_t = std::uint8_t;

    /// image format
    enum pixel_format_t
    {
        undefined = 0, ///< Undefined pixel format
        rgb,           ///< image with 3 component RGB.
        rgba,          ///< image with 4 component RGBA.
        bgr,           ///< image with 3 component BGR.
        bgra,          ///< image with 4 component BGRA.
        gray_scale,    ///< image with 1 component.
        rg,            ///< image with 2 components RG.
        count
    };

    /**
     * @brief Constructor
     */
    SIGHT_DATA_API image();

    /**
     * @brief Destructor
     */
    SIGHT_DATA_API ~image() noexcept override = default;

    /// @brief get image information from source. Informations are spacing,origin,size ... expect Fields
    SIGHT_DATA_API void copy_information(image::csptr _source);

    /** @{
     *  @brief Get/set image spacing
     */
    const spacing_t& spacing() const;
    void set_spacing(const spacing_t& _spacing);
    /// @}

    /** @{
     *  @brief Get/set image origin
     */
    const origin_t& origin() const;
    virtual void set_origin(const origin_t& _origin);
    /// @}

    /** @{
     *  @brief Get/set image orientation.
     *
     *  @note The orientation is a 3x3 direction cosines matrix in row-major order.
     */
    const orientation_t& orientation() const;
    virtual void set_orientation(const orientation_t& _orientation);
    /// @}

    /// Get image size
    const image::size_t& size() const;

    /// Number of dimensions of the image (3 for 3D image)
    SIGHT_DATA_API std::size_t num_dimensions() const;

    /** @{
     *  @brief Get/set preferred window center
     */
    virtual std::vector<double> window_center() const noexcept;
    virtual void set_window_center(const std::vector<double>& _window_centers);
    /// @}

    /** @{
     *  @brief Get/set preferred window width
     */
    virtual std::vector<double> window_width() const noexcept;
    virtual void set_window_width(const std::vector<double>& _window_widths);
    /// @}

    /// Get the number of elements (ie: size[0]*size[1]*size[2]*nbComponents)
    SIGHT_DATA_API std::size_t num_elements() const;

    /// Get the number of components of an image pixel
    std::size_t num_components() const;

    /// Get image type
    SIGHT_DATA_API core::type type() const;

    /// Get pixel format
    pixel_format_t pixel_format() const;

    /**
       @{
     * @brief Resize the image and allocate the memory if needed.
     *
     * @param _size array of size in each direction (x,y,z)
     * @param _type type of a single pixel component value
     * @param _format specify the ordering and the meaning of a pixel components
     *
     * If the data array owns its buffer, this method will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    SIGHT_DATA_API virtual std::size_t resize(
        const image::size_t& _size,
        const core::type& _type,
        pixel_format_t _format
    );
    /// @}

    /// @brief return image size in bytes
    SIGHT_DATA_API std::size_t size_in_bytes() const;
    /// @brief return allocated image size in bytes
    SIGHT_DATA_API std::size_t allocated_size_in_bytes() const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when image's buffer is added
    using buffer_modified_signal_t = core::com::signal<void ()>;
    SIGHT_DATA_API static const core::com::signals::key_t BUFFER_MODIFIED_SIG;

    /// Type of signal when a landmark is added
    using landmark_added_signal_t = core::com::signal<void (std::shared_ptr<point>)>;
    SIGHT_DATA_API static const core::com::signals::key_t LANDMARK_ADDED_SIG;

    /// Type of signal when a landmark is removed
    using landmark_removed_signal_t = core::com::signal<void (std::shared_ptr<point>)>;
    SIGHT_DATA_API static const core::com::signals::key_t LANDMARK_REMOVED_SIG;

    /// Type of signal when a landmark is added
    using landmark_displayed_signal_t = core::com::signal<void (bool)>;
    SIGHT_DATA_API static const core::com::signals::key_t LANDMARK_DISPLAYED_SIG;

    /// Type of signal when slice index is modified (axial index, frontal index, sagittal index)
    using slice_index_modified_signal_t = core::com::signal<void (int, int, int)>;
    SIGHT_DATA_API static const core::com::signals::key_t SLICE_INDEX_MODIFIED_SIG;

    /// Type of signal when slice type is modified (from slice type, to slice type)
    using slice_type_modified_signal_t = core::com::signal<void (int, int)>;
    SIGHT_DATA_API static const core::com::signals::key_t SLICE_TYPE_MODIFIED_SIG;

    /// Type of signal when ruler fiducial is modified (the associated id of ruler fiducial and the new coordinates)
    using ruler_modified_signal_t =
        core::com::signal<void (std::optional<std::string>, std::array<double, 3>, std::array<double, 3>)>;
    SIGHT_DATA_API static const core::com::signals::key_t RULER_MODIFIED_SIG;

    /// Type of signal when fiducial is removed (the associated id of fiducial)
    using fiducial_removed_signal_t =
        core::com::signal<void (std::optional<std::string>)>;
    SIGHT_DATA_API static const core::com::signals::key_t FIDUCIAL_REMOVED_SIG;
    /**
     * @}
     */

    /**
     * @name Iteration typedefs
     * @{
     */
    /// image iterator
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
        image::sptr img = image::New();
        img->resize(1920, 1080, 0, core::type::UINT8, image::pixel_format::rgba);
        img->resize({1920, 1080, 0}, core::type::UINT8, image::pixel_format::rgba);
        image::iterator< Color > iter    = img->begin< Color >();
        const image::iterator< Color > iterEnd = img->end< Color >();

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
     * (in debug) that the iterator does iterate inside of the buffer bounds).
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
    SIGHT_DATA_API iterator<char> begin();
    SIGHT_DATA_API iterator<char> end();
    SIGHT_DATA_API const_iterator<char> begin() const;
    SIGHT_DATA_API const_iterator<char> end() const;
    /// @}

    ///
    /// @{
    /// Returns image buffer
    SIGHT_DATA_API void* buffer();
    SIGHT_DATA_API const void* buffer() const;
    /// @}

    /**
     * @brief Setter for the array buffer.
     *
     * An existing buffer will be released if the array own it.
     *
     * @param _buf             Buffer to set as Array's buffer
     * @param _take_ownership  if true, the Array will manage allocation and destroy the buffer when needed.
     * @param _type            Type of the array view
     * @param _size            Size of the array view
     * @param _format          Specify the ordering and the meaning of a pixel components
     * @param _policy          If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    SIGHT_DATA_API void set_buffer(
        void* _buf,
        bool _take_ownership,
        const core::type& _type,
        const image::size_t& _size,
        pixel_format_t _format,
        core::memory::buffer_allocation_policy::sptr _policy = std::make_shared<core::memory::buffer_malloc_policy>()
    );

    /**
     * @{
     * @brief Get the value of an element
     *
     * @param T Type in which the pointer will be returned
     * @param _id Item image index
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     * @throw Exception Index out of bounds
     */
    template<typename T>
    T& at(index_t _id);
    template<typename T>
    T at(index_t _id) const;
    /// @}
    /**
     * @{
     * @brief Get the value of an element
     *
     * @param T Type in which the pointer will be returned
     * @param _x x index
     * @param _y y index
     * @param _z z index
     * @param _c component index
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     * @throw Exception Index out of bounds
     */
    template<typename T>
    T& at(index_t _x, index_t _y, index_t _z, index_t _c = 0);
    template<typename T>
    T at(index_t _x, index_t _y, index_t _z, index_t _c = 0) const;
    /// @}
    ///

    /**
     * @brief Return a pointer on a image pixel
     * @param _index offset of the pixel
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     */
    SIGHT_DATA_API void* get_pixel(index_t _index);

    /**
     * @brief Return a pointer on a image pixel
     * @param _index offset of the pixel
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     */
    SIGHT_DATA_API const void* get_pixel(index_t _index) const;

    /**
     * @brief Set pixel value represented as a void* buffer
     * @param _index    offset of the pixel
     * @param _pix_buf  pixel value represented as a void* buffer
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl())
     */
    SIGHT_DATA_API void set_pixel(index_t _index, const buffer_t* _pix_buf);

    /// Return the pixel value in a std::string
    SIGHT_DATA_API std::string get_pixel_as_string(
        index_t _x,
        index_t _y,
        index_t _z
    ) const;

    /// Return the buffer object
    SIGHT_DATA_API core::memory::buffer_object::sptr get_buffer_object();

    /// Return the buffer object
    SIGHT_DATA_API core::memory::buffer_object::csptr get_buffer_object() const;

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const image& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const image& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    /// Add a lock on the image in the given vector to prevent from dumping the buffer on the disk
    /// This is needed for IBuffered interface implementation
    SIGHT_DATA_API void dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& _locks) const override;

private:

    /**
     * @{
     * @brief Resize the image and allocate the memory if needed.
     *
     * @param _size array of size in each direction (x,y,z)
     * @param _type type of a single pixel component value
     * @param _format specify the ordering and the meaning of a pixel components
     * @param _realloc allows to not reallocate, for instance when importing directly the buffer with setBuffer()
     *
     * If the data array owns its buffer, this method will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    SIGHT_DATA_API std::size_t resize(
        const image::size_t& _size,
        const core::type& _type,
        pixel_format_t _format,
        bool _realloc
    );
    /// @}

    /**
     * @brief Protected setter for the array buffer.
     * An existing buffer will be released if the array own it.
     *
     * @param _buf              Buffer to set as Array's buffer
     * @param _take_ownership   If true, the Array will manage allocation and destroy the buffer when needed.
     * @param _policy           If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    void set_buffer(
        void* _buf,
        bool _take_ownership                                 = false,
        core::memory::buffer_allocation_policy::sptr _policy = std::make_shared<core::memory::buffer_malloc_policy>()
    );

    //! Size of the image (in terms of points)
    size_t m_size {0, 0, 0};

    //! An array on the voxel size of the image
    spacing_t m_spacing {0., 0., 0.};

    //! origin_t of the image in 3D coordinate system
    origin_t m_origin {0., 0., 0.};

    //! orientation_t of the image in 3D coordinate system (row-major order)
    orientation_t m_orientation {1., 0., 0., 0., 1., 0., 0., 0., 1.};

    //! Preferred window center/width
    ///@{
    std::vector<double> m_window_centers;
    std::vector<double> m_window_widths;
    ///@}

    //! Number of components
    std::size_t m_num_components {1};

    //! type of image pixel
    core::type m_type {core::type::UINT8};

    //! number of bytes between two pixels - not serialized
    std::size_t m_stride {1};

    //! image format
    pixel_format_t m_pixel_format {pixel_format_t::undefined};

    //! image buffer
    array::sptr m_data_array;
};

//-----------------------------------------------------------------------------

inline std::vector<double> image::window_center() const noexcept
{
    return m_window_centers;
}

//-----------------------------------------------------------------------------

inline void image::set_window_center(const std::vector<double>& _window_centers)
{
    m_window_centers = _window_centers;
}

//-----------------------------------------------------------------------------

inline std::vector<double> image::window_width() const noexcept
{
    return m_window_widths;
}

//-----------------------------------------------------------------------------

inline void image::set_window_width(const std::vector<double>& _window_widths)
{
    m_window_widths = _window_widths;
}

//-----------------------------------------------------------------------------

inline std::size_t image::num_components() const
{
    return m_num_components;
}

//-----------------------------------------------------------------------------

inline enum image::pixel_format_t image::pixel_format() const
{
    return m_pixel_format;
}

//------------------------------------------------------------------------------

inline const image::spacing_t& image::spacing() const
{
    return m_spacing;
}

//------------------------------------------------------------------------------

inline void image::set_spacing(const spacing_t& _spacing)
{
    m_spacing = _spacing;
}

//------------------------------------------------------------------------------

inline const image::origin_t& image::origin() const
{
    return m_origin;
}

//------------------------------------------------------------------------------

inline void image::set_origin(const origin_t& _origin)
{
    m_origin = _origin;
}

//------------------------------------------------------------------------------

inline const image::orientation_t& image::orientation() const
{
    return m_orientation;
}

//------------------------------------------------------------------------------

inline void image::set_orientation(const orientation_t& _orientation)
{
    m_orientation = _orientation;
}

//------------------------------------------------------------------------------

inline const image::size_t& image::size() const
{
    return m_size;
}

//------------------------------------------------------------------------------

template<typename T>
inline image::iterator<T> image::begin()
{
    return iterator<T>(static_cast<typename iterator<T>::pointer_t>(buffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline image::iterator<T> image::end()
{
    auto itr = begin<T>();
    itr += static_cast<typename iterator<T>::difference_type>(this->size_in_bytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline image::const_iterator<T> image::begin() const
{
    return const_iterator<T>(static_cast<typename const_iterator<T>::pointer_t>(buffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline image::const_iterator<T> image::end() const
{
    auto itr = begin<T>();
    itr += static_cast<typename const_iterator<T>::difference_type>(this->size_in_bytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline image::const_iterator<T> image::cbegin() const
{
    return const_iterator<T>(static_cast<typename const_iterator<T>::pointer_t>(buffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline image::const_iterator<T> image::cend() const
{
    auto itr = begin<T>();
    itr += static_cast<typename const_iterator<T>::difference_type>(this->size_in_bytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
auto image::range()
{
    auto b = begin<T>();
    auto e = end<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename T>
auto image::crange() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename T>
inline T& image::at(index_t _id)
{
    return *reinterpret_cast<T*>(this->get_pixel(_id));
}

//------------------------------------------------------------------------------

template<typename T>
inline T image::at(index_t _id) const
{
    return *reinterpret_cast<const T*>(this->get_pixel(_id));
}

//------------------------------------------------------------------------------

template<typename T>
inline T& image::at(index_t _x, index_t _y, index_t _z, index_t _c)
{
    const index_t offset = _x + m_size[0] * _y + _z * m_size[0] * m_size[1];
    return *(reinterpret_cast<T*>(this->get_pixel(offset)) + _c);
}

//------------------------------------------------------------------------------

template<typename T>
inline T image::at(index_t _x, index_t _y, index_t _z, index_t _c) const
{
    const index_t offset = _x + m_size[0] * _y + _z * m_size[0] * m_size[1];
    return *(reinterpret_cast<const T*>(this->get_pixel(offset)) + _c);
}

} // namespace sight::data
