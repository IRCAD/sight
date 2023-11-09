/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/config.hpp"
#include "data/container.hpp"

#include <core/type.hpp>

#include <glm/vec4.hpp>

#include <span>

namespace sight::data
{

class DATA_CLASS_API transfer_function_base
{
public:

    /// Defines the type of values of each point.
    using value_t = double;

    /// Defines the color of each point.
    using color_t = glm::dvec4;

    /// Defines the type used to store transfer function data.
    using data_t = std::map<value_t, color_t>;

    using min_max_t = std::pair<value_t, value_t>;

    /// Defines the available modes {LINEAR, NEAREST} to interpolate color between two TF color points.
    enum class interpolation_mode
    {
        linear,
        nearest
    };
    DATA_API virtual ~transfer_function_base() = default;

    /// Scale the intensity value from the transfer function space to the window space.
    [[nodiscard]] DATA_API value_t map_value_to_window(value_t _value) const;

    /// Scale the intensity value from the window space space to the transfer function.
    [[nodiscard]] DATA_API value_t map_value_from_window(value_t _value) const;

    /// Gets the nearest color of a value, taking into account the window range
    [[nodiscard]] DATA_API color_t sample_nearest(value_t _value) const;

    /// Gets the color for a value (the color is computed with a linear interpolation), taking into account the window
    /// range
    [[nodiscard]] DATA_API color_t sample_linear(value_t _value) const;

    /// Gets the min/max of the window level.
    [[nodiscard]] DATA_API min_max_t window_min_max() const;

    /// Sets the min/max of the window level.
    DATA_API void set_window_min_max(const min_max_t& _min_max);

    /// Gets the level.
    [[nodiscard]] value_t level() const;

    /// Gets the window.
    [[nodiscard]] value_t window() const;

    /// Sets the level.
    DATA_API virtual void set_level(value_t _value) = 0;

    /// Sets the window.
    DATA_API virtual void set_window(value_t _value) = 0;

    /// Gets the first and last point values of the tf data.
    [[nodiscard]] DATA_API virtual min_max_t min_max() const = 0;

    /// Gets the interpolated color of the TF for a value, taking into account the window range
    /// @param _value input value in the curve
    /// @param _mode  interpolation mode, if not specified, use the mode specified by setInterpolationMode()
    [[nodiscard]] DATA_API virtual color_t sample(
        value_t _value,
        std::optional<enum interpolation_mode> _mode = std::nullopt
    ) const                                          = 0;

    /// Sets the current visualization level.
    double m_level {0.};

    /// Sets the current visualization window.
    double m_window {2.};
};

/**
 * @brief Stores a transfer function properties. A list of points associating a value to a RGBA color, and windowing
 * parameters which simplifies the scaling of the function.
 */
class DATA_CLASS_API transfer_function_piece final : public container_wrapper<std::map<transfer_function_base::value_t,
                                                                                       transfer_function_base::color_t> >,
                                                     public transfer_function_base
{
public:

    SIGHT_DECLARE_CLASS(transfer_function_piece);

    using container_wrapper<data_t>::container_wrapper;

    DATA_API ~transfer_function_piece() final = default;

    static transfer_function_piece::sptr make();

    /// Equality comparison operators
    /// @{
    bool operator==(const transfer_function_piece& _other) const noexcept;
    bool operator!=(const transfer_function_piece& _other) const noexcept;
    /// @}

    transfer_function_piece& operator=(const transfer_function_piece& _other);

    /// Sets the level.
    DATA_API void set_level(value_t _value) final;

    /// Sets the window.
    DATA_API void set_window(value_t _value) final;

    /// Gets the first and last point values of the tf data.
    [[nodiscard]] DATA_API min_max_t min_max() const final;

    /// Gets the interpolated color of the TF for a value, taking into account the window range
    /// @param _value input value in the curve
    /// @param _mode  interpolation mode, if not specified, use the mode specified by setInterpolationMode()
    [[nodiscard]] DATA_API color_t sample(
        value_t _value,
        std::optional<enum interpolation_mode> _mode = std::nullopt
    ) const final;

    /// Gets the interpolation mode.
    [[nodiscard]] enum interpolation_mode get_interpolation_mode() const;

    /// Sets the interpolation mode.
    void set_interpolation_mode(enum interpolation_mode _value);

    /// Gets if the TF is clamped.
    [[nodiscard]] bool clamped() const;

    /// Sets if the TF is clamped.
    void set_clamped(bool _value);

private:

    /// Defines the current interpolation mode.
    enum interpolation_mode m_interpolation_mode {interpolation_mode::linear};

    /**
     *  @brief Defines interpolation mode on extremities.
     *
     *  if m_clamped == true then after extremity point, the returned TF color is color_t(0,0,0,0).
     *  if m_clamped == false then after extremity point, the returned TF color is one of the extremity color value.
     **/
    bool m_clamped {true};
};

/**
 * @brief This class defines a piecewise transfer function.
 *
 * A transfer function is composed of a list of points that associates a value, typically an image intensity,
 * to a RGBA color.
 *
 * The function is composed of individual pieces that can be accessed and edited independently thanks to the pieces()
 * function that returns a vector of transfer_function_piece.
 */
class DATA_CLASS_API transfer_function final : public object,
                                               public transfer_function_base
{
public:

    SIGHT_DECLARE_CLASS(transfer_function, object);

    /// Defines the type of values of each point.
    using value_t = transfer_function_piece::value_t;

    /// Defines the color of each point.
    using color_t = transfer_function_piece::color_t;

    /// Defines the type used to store transfer function data.
    using data_t = transfer_function_piece::data_t;

    using min_max_t = transfer_function_piece::min_max_t;

    using interpolation_mode_t = transfer_function_piece::interpolation_mode;

    /// Constructors / Destructor / Assignment operators
    /// @{
    DATA_API transfer_function();
    DATA_API ~transfer_function() final = default;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const transfer_function& _other) const noexcept;
    DATA_API bool operator!=(const transfer_function& _other) const noexcept;
    /// @}

    /// Sets the defaults transfer function name.
    DATA_API static const std::string DEFAULT_TF_NAME;

    /// Creates a default TF.
    DATA_API static transfer_function::sptr create_default_tf();

    /// Creates a default TF according to the pixel type.
    DATA_API static transfer_function::sptr create_default_tf(core::type _type);

    /**
     * Merges a range of transfer functions into one.
     * @param _dst Destination transfer function. Its pieces are conserved too.
     * @param _src Source transfer function.
     * @pre None of the pieces of _src must be nullptr.
     */
    DATA_API
    static void merge(
        sight::data::transfer_function& _dst,
        const sight::data::transfer_function& _src
    );

    /// Gets the transfert function name.
    [[nodiscard]] const std::string& name() const;

    /// Sets the transfert function name.
    void set_name(const std::string& _value);

    /// Gets the TF background color when tf 'IsClamped' is true.
    [[nodiscard]] const color_t& background_color() const;

    /// Set the TF background color when tf 'IsClamped' is true.
    void set_background_color(const color_t& _value);

    /// Returns all the pieces of the piecewise function
    std::vector<transfer_function_piece::sptr>& pieces();
    const std::vector<transfer_function_piece::sptr>& pieces() const;

    /// Returns true if the function is empty
    [[nodiscard]] bool empty() const;

    /// Recompute the window parameters by taking the minimum and the maximum of the pieces.
    DATA_API void fit_window();

    /// Sets the level of the transfer function and its pieces.
    DATA_API void set_level(value_t _value) final;

    /// Sets the window of the transfer function and its pieces.
    DATA_API void set_window(value_t _value) final;

    /// Gets the first and last point values of the tf data.
    [[nodiscard]] DATA_API min_max_t min_max() const final;

    /// Gets the interpolated color of the TF for a value, taking into account the window range
    /// @param _value input value in the curve
    /// @param _mode  interpolation mode, if not specified, use the mode specified by setInterpolationMode()
    [[nodiscard]] DATA_API color_t sample(
        value_t _value,
        std::optional<interpolation_mode_t> _mode = std::nullopt
    ) const final;

    /// @name Signals
    /// @{
    /// Defines the type of signal sent when points are modified.
    using points_modified_signal_t = core::com::signal<void ()>;
    DATA_API static const core::com::signals::key_t POINTS_MODIFIED_SIG;

    /// Defines the type of signal sent when window-level is modified (window, level).
    using windowing_modified_signal_t = core::com::signal<void (value_t, value_t)>;
    DATA_API static const core::com::signals::key_t WINDOWING_MODIFIED_SIG;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) final;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) final;

private:

    /// Sets the transfer function name.
    std::string m_name;

    /// Sets the recommended background color to use this TF.
    color_t m_background_color {0., 0., 0., 0.};

    /// The transfer function object is a piecewise function
    std::vector<transfer_function_piece::sptr> m_pieces;
};

//------------------------------------------------------------------------------

inline transfer_function_piece::sptr transfer_function_piece::make()
{
    return std::make_shared<transfer_function_piece>();
}

//------------------------------------------------------------------------------

inline transfer_function_piece& transfer_function_piece::operator=(const transfer_function_piece& _other)
{
    if(&_other != this)
    {
        this->clear();

        container_wrapper<data_t>::operator=(_other);

        this->m_level              = _other.m_level;
        this->m_window             = _other.m_window;
        this->m_interpolation_mode = _other.m_interpolation_mode;
        this->m_clamped            = _other.m_clamped;
    }

    return *this;
}

//------------------------------------------------------------------------------

inline bool transfer_function_piece::operator==(const transfer_function_piece& _other) const noexcept
{
    if(!core::tools::is_equal(m_level, _other.m_level)
       || !core::tools::is_equal(m_window, _other.m_window)
       || m_interpolation_mode != _other.m_interpolation_mode
       || m_clamped != _other.m_clamped)
    {
        return false;
    }

    // Super class last
    return core::tools::is_equal(
        *this,
        _other
    );
}

//-----------------------------------------------------------------------------

inline bool transfer_function_piece::operator!=(const transfer_function_piece& _other) const noexcept
{
    return !(*this == _other);
}

//-----------------------------------------------------------------------------

inline transfer_function_base::value_t transfer_function_base::level() const
{
    return m_level;
}

//-----------------------------------------------------------------------------

inline transfer_function_base::value_t transfer_function_base::window() const
{
    return m_window;
}

//-----------------------------------------------------------------------------

inline enum transfer_function::interpolation_mode transfer_function_piece::get_interpolation_mode() const
{
    return m_interpolation_mode;
}

//-----------------------------------------------------------------------------

inline void transfer_function_piece::set_interpolation_mode(enum interpolation_mode _value)
{
    m_interpolation_mode = _value;
}

//-----------------------------------------------------------------------------

inline bool transfer_function_piece::clamped() const
{
    return m_clamped;
}

//-----------------------------------------------------------------------------

inline void transfer_function_piece::set_clamped(bool _value)
{
    m_clamped = _value;
}

//-----------------------------------------------------------------------------

inline const std::string& transfer_function::name() const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void transfer_function::set_name(const std::string& _value)
{
    m_name = _value;
}

//-----------------------------------------------------------------------------

inline const transfer_function::color_t& transfer_function::background_color() const
{
    return m_background_color;
}

//-----------------------------------------------------------------------------

inline void transfer_function::set_background_color(const color_t& _value)
{
    m_background_color = _value;
}

//-----------------------------------------------------------------------------

inline std::vector<transfer_function_piece::sptr>& transfer_function::pieces()
{
    return m_pieces;
}

//-----------------------------------------------------------------------------

inline const std::vector<transfer_function_piece::sptr>& transfer_function::pieces() const
{
    return m_pieces;
}

//-----------------------------------------------------------------------------

inline bool transfer_function::empty() const
{
    return m_pieces.empty();
}

} // namespace sight::data
