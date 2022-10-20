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

#include "data/config.hpp"
#include "data/IContainer.hpp"

#include <glm/vec4.hpp>
namespace sight::data
{

class DATA_CLASS_API ITransferFunction
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
    enum class InterpolationMode
    {
        LINEAR,
        NEAREST
    };
    DATA_API virtual ~ITransferFunction() = default;

    /// Scale the intensity value from the transfer function space to the window space.
    [[nodiscard]] DATA_API value_t mapValueToWindow(value_t _value) const;

    /// Scale the intensity value from the window space space to the transfer function.
    [[nodiscard]] DATA_API value_t mapValueFromWindow(value_t _value) const;

    /// Gets the nearest color of a value, taking into account the window range
    [[nodiscard]] DATA_API color_t sampleNearest(value_t _value) const;

    /// Gets the color for a value (the color is computed with a linear interpolation), taking into account the window
    /// range
    [[nodiscard]] DATA_API color_t sampleLinear(value_t _value) const;

    /// Gets the min/max of the window level.
    [[nodiscard]] DATA_API min_max_t windowMinMax() const;

    /// Sets the min/max of the window level.
    DATA_API void setWindowMinMax(const min_max_t& _minMax);

    /// Gets the level.
    [[nodiscard]] value_t level() const;

    /// Gets the window.
    [[nodiscard]] value_t window() const;

    /// Sets the level.
    DATA_API virtual void setLevel(value_t _value) = 0;

    /// Sets the window.
    DATA_API virtual void setWindow(value_t _value) = 0;

    /// Gets the first and last point values of the tf data.
    [[nodiscard]] DATA_API virtual min_max_t minMax() const = 0;

    /// Gets the interpolated color of the TF for a value, taking into account the window range
    /// @param _value input value in the curve
    /// @param _mode  interpolation mode, if not specified, use the mode specified by setInterpolationMode()
    [[nodiscard]] DATA_API virtual color_t sample(
        value_t _value,
        std::optional<InterpolationMode> _mode = std::nullopt
    ) const                                    = 0;

    /// Sets the current visualization level.
    double m_level {0.};

    /// Sets the current visualization window.
    double m_window {2.};
};

/**
 * @brief Stores a transfer function properties. A list of points associating a value to a RGBA color, and windowing
 * parameters which simplifies the scaling of the function.
 */
class DATA_CLASS_API TransferFunctionPiece final : public ContainerWrapper<std::map<ITransferFunction::value_t,
                                                                                    ITransferFunction::color_t> >,
                                                   public ITransferFunction
{
public:

    SIGHT_DECLARE_CLASS(TransferFunctionPiece);

    using ContainerWrapper<data_t>::ContainerWrapper;

    DATA_API ~TransferFunctionPiece() final = default;

    static TransferFunctionPiece::sptr New();

    /// Equality comparison operators
    /// @{
    bool operator==(const TransferFunctionPiece& _other) const noexcept;
    bool operator!=(const TransferFunctionPiece& _other) const noexcept;
    /// @}

    TransferFunctionPiece& operator=(const TransferFunctionPiece& _other);

    /// Sets the level.
    DATA_API void setLevel(value_t _value) final;

    /// Sets the window.
    DATA_API void setWindow(value_t _value) final;

    /// Gets the first and last point values of the tf data.
    [[nodiscard]] DATA_API min_max_t minMax() const final;

    /// Gets the interpolated color of the TF for a value, taking into account the window range
    /// @param _value input value in the curve
    /// @param _mode  interpolation mode, if not specified, use the mode specified by setInterpolationMode()
    [[nodiscard]] DATA_API color_t sample(
        value_t _value,
        std::optional<InterpolationMode> _mode = std::nullopt
    ) const final;

    /// Gets the interpolation mode.
    [[nodiscard]] InterpolationMode interpolationMode() const;

    /// Sets the interpolation mode.
    void setInterpolationMode(InterpolationMode _value);

    /// Gets if the TF is clamped.
    [[nodiscard]] bool clamped() const;

    /// Sets if the TF is clamped.
    void setClamped(bool _value);

private:

    /// Defines the current interpolation mode.
    InterpolationMode m_interpolationMode {InterpolationMode::LINEAR};

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
 * function that returns a vector of TransferFunctionPiece.
 */
class DATA_CLASS_API TransferFunction final : public Object,
                                              public ITransferFunction
{
public:

    SIGHT_DECLARE_CLASS(TransferFunction, Object, factory::New<TransferFunction>);

    /// Defines the type of values of each point.
    using value_t = TransferFunctionPiece::value_t;

    /// Defines the color of each point.
    using color_t = TransferFunctionPiece::color_t;

    /// Defines the type used to store transfer function data.
    using data_t = TransferFunctionPiece::data_t;

    using min_max_t = TransferFunctionPiece::min_max_t;

    using InterpolationMode = TransferFunctionPiece::InterpolationMode;

    /// Constructors / Destructor / Assignment operators
    /// @{
    DATA_API TransferFunction(Object::Key /*unused*/);
    DATA_API ~TransferFunction() final = default;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const TransferFunction& _other) const noexcept;
    DATA_API bool operator!=(const TransferFunction& _other) const noexcept;
    /// @}

    /// Sets the defaults transfer function name.
    DATA_API static const std::string s_DEFAULT_TF_NAME;

    /// Creates a default TF.
    DATA_API static TransferFunction::sptr createDefaultTF();

    /// Gets the transfert function name.
    [[nodiscard]] const std::string& name() const;

    /// Sets the transfert function name.
    void setName(const std::string& _value);

    /// Gets the TF background color when tf 'IsClamped' is true.
    [[nodiscard]] const color_t& backgroundColor() const;

    /// Set the TF background color when tf 'IsClamped' is true.
    void setBackgroundColor(const color_t& _value);

    /// Returns all the pieces of the piecewise function
    std::vector<TransferFunctionPiece::sptr>& pieces();
    const std::vector<TransferFunctionPiece::sptr>& pieces() const;

    /// Returns true if the function is empty
    [[nodiscard]] bool empty() const;

    /// Recompute the window parameters by taking the minimum and the maximum of the pieces.
    DATA_API void fitWindow();

    /// Sets the level of the transfer function and its pieces.
    DATA_API void setLevel(value_t _value) final;

    /// Sets the window of the transfer function and its pieces.
    DATA_API void setWindow(value_t _value) final;

    /// Gets the first and last point values of the tf data.
    [[nodiscard]] DATA_API min_max_t minMax() const final;

    /// Gets the interpolated color of the TF for a value, taking into account the window range
    /// @param _value input value in the curve
    /// @param _mode  interpolation mode, if not specified, use the mode specified by setInterpolationMode()
    [[nodiscard]] DATA_API color_t sample(
        value_t _value,
        std::optional<InterpolationMode> _mode = std::nullopt
    ) const final;

    /// @name Signals
    /// @{
    /// Defines the type of signal sent when points are modified.
    typedef core::com::Signal<void ()> PointsModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINTS_MODIFIED_SIG;

    /// Defines the type of signal sent when window-level is modified (window, level).
    typedef core::com::Signal<void (value_t, value_t)> WindowingModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_WINDOWING_MODIFIED_SIG;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) final;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) final;

private:

    /// Sets the transfer function name.
    std::string m_name;

    /// Sets the recommended background color to use this TF.
    color_t m_backgroundColor {0., 0., 0., 0.};

    /// The transfer function object is a piecewise function
    std::vector<TransferFunctionPiece::sptr> m_pieces;
};

//------------------------------------------------------------------------------

inline TransferFunctionPiece::sptr TransferFunctionPiece::New()
{
    return std::make_shared<TransferFunctionPiece>();
}

//------------------------------------------------------------------------------

inline TransferFunctionPiece& TransferFunctionPiece::operator=(const TransferFunctionPiece& _other)
{
    if(&_other != this)
    {
        this->clear();

        ContainerWrapper<data_t>::operator=(_other);

        this->m_level             = _other.m_level;
        this->m_window            = _other.m_window;
        this->m_interpolationMode = _other.m_interpolationMode;
        this->m_clamped           = _other.m_clamped;
    }

    return *this;
}

//------------------------------------------------------------------------------

inline bool TransferFunctionPiece::operator==(const TransferFunctionPiece& _other) const noexcept
{
    if(!core::tools::is_equal(m_level, _other.m_level)
       || !core::tools::is_equal(m_window, _other.m_window)
       || m_interpolationMode != _other.m_interpolationMode
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

inline bool TransferFunctionPiece::operator!=(const TransferFunctionPiece& _other) const noexcept
{
    return !(*this == _other);
}

//-----------------------------------------------------------------------------

inline ITransferFunction::value_t ITransferFunction::level() const
{
    return m_level;
}

//-----------------------------------------------------------------------------

inline ITransferFunction::value_t ITransferFunction::window() const
{
    return m_window;
}

//-----------------------------------------------------------------------------

inline TransferFunction::InterpolationMode TransferFunctionPiece::interpolationMode() const
{
    return m_interpolationMode;
}

//-----------------------------------------------------------------------------

inline void TransferFunctionPiece::setInterpolationMode(InterpolationMode _value)
{
    m_interpolationMode = _value;
}

//-----------------------------------------------------------------------------

inline bool TransferFunctionPiece::clamped() const
{
    return m_clamped;
}

//-----------------------------------------------------------------------------

inline void TransferFunctionPiece::setClamped(bool _value)
{
    m_clamped = _value;
}

//-----------------------------------------------------------------------------

inline const std::string& TransferFunction::name() const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setName(const std::string& _value)
{
    m_name = _value;
}

//-----------------------------------------------------------------------------

inline const TransferFunction::color_t& TransferFunction::backgroundColor() const
{
    return m_backgroundColor;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setBackgroundColor(const color_t& _value)
{
    m_backgroundColor = _value;
}

//-----------------------------------------------------------------------------

inline std::vector<TransferFunctionPiece::sptr>& TransferFunction::pieces()
{
    return m_pieces;
}

//-----------------------------------------------------------------------------

inline const std::vector<TransferFunctionPiece::sptr>& TransferFunction::pieces() const
{
    return m_pieces;
}

//-----------------------------------------------------------------------------

inline bool TransferFunction::empty() const
{
    return m_pieces.empty();
}

//-----------------------------------------------------------------------------

} // namespace sight::data
