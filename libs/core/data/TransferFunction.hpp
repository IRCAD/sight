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

/**
 * @brief Stores a transfer function properties. A list of points associating a value to a RGBA color, and windowing
 * parameters which simplifies the scaling of the function.
 */
class DATA_CLASS_API TransferFunctionData : public ContainerWrapper<std::map<double,
                                                                             glm::dvec4> >
{
public:

    SIGHT_DECLARE_CLASS(TransferFunctionData);

    /// Defines the type of values of each point.
    using value_t = key_type;

    /// Defines the color of each point.
    using color_t = mapped_type;

    /// Defines the type used to store transfer function data.
    using data_t = std::map<value_t, color_t>;

    using min_max_t = std::pair<value_t, value_t>;

    using ContainerWrapper<data_t>::ContainerWrapper;

    /// Defines the available modes {LINEAR, NEAREST} to interpolate color between two TF color points.
    enum class InterpolationMode
    {
        LINEAR,
        NEAREST
    };

    DATA_API virtual ~TransferFunctionData() = default;

    static TransferFunctionData::sptr New();

    /// Equality comparison operators
    /// @{
    bool operator==(const TransferFunctionData& _other) const noexcept;
    bool operator!=(const TransferFunctionData& _other) const noexcept;
    /// @}

    TransferFunctionData& operator=(const TransferFunctionData& _other);

    /// Scale the intensity value from the transfer function space to the window space.
    DATA_API value_t mapValueToWindow(value_t _value) const;

    /// Scale the intensity value from the window space space to the transfer function.
    DATA_API value_t mapValueFromWindow(value_t _value) const;

    /// Gets the first and last point values of the tf data.
    DATA_API min_max_t minMax() const;

    /// Gets the min/max of the window level.
    DATA_API min_max_t windowMinMax() const;

    /// Sets the min/max of the window level.
    DATA_API void setWindowMinMax(const min_max_t& _minMax);

    /// Gets the nearest color of a value, taking into account the window range
    DATA_API color_t sampleNearest(value_t _value) const;

    /// Gets the color for a value (the color is computed with a linear interpolation), taking into account the window
    /// range
    DATA_API color_t sampleLinear(value_t _value) const;

    /// Gets the interpolated color of the TF for a value, taking into account the window range
    /// @param _value input value in the curve
    /// @param _mode  interpolation mode, if not specified, use the mode specified by setInterpolationMode()
    DATA_API color_t sample(value_t _value, std::optional<InterpolationMode> _mode = std::nullopt) const;

    /// Gets the interpolation mode.
    InterpolationMode interpolationMode() const;

    /// Sets the interpolation mode.
    void setInterpolationMode(InterpolationMode _value);

    /// Gets the level.
    [[nodiscard]] value_t level() const;

    /// Sets the level.
    DATA_API virtual void setLevel(value_t _value);

    /// Gets the window.
    [[nodiscard]] value_t window() const;

    /// Sets the window.
    DATA_API virtual void setWindow(value_t _value);

    /// Gets if the TF is clamped.
    [[nodiscard]] bool clamped() const;

    /// Sets if the TF is clamped.
    void setClamped(bool _value);

protected:

    /// Sets the current visualization level.
    double m_level {0.};

    /// Sets the current visualization window.
    double m_window {100.};

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
 * function that returns a vector of TransferFunctionData. When the pieces are modified, the function mergePieces() must
 * be called to mix the pieces together with additive alpha blending.
 */
class DATA_CLASS_API TransferFunction final : public Object,
                                              public TransferFunctionData
{
public:

    SIGHT_DECLARE_CLASS(TransferFunction, Object, factory::New<TransferFunction>);

    /// Constructors / Destructor / Assignment operators
    /// @{
    DATA_API TransferFunction(Object::Key);
    DATA_API ~TransferFunction() override = default;
    /// @}

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    DATA_API void shallowCopy(const Object::csptr& _source) override;

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
    std::vector<TransferFunctionData::sptr>& pieces();
    const std::vector<TransferFunctionData::sptr>& pieces() const;

    /// Blends all pieces together with additive alpha blending. Additional points may be added according to the
    /// clamping settings of the pieces.
    DATA_API void mergePieces();

    /// Sets the level of the transfer function and its pieces.
    DATA_API void setLevel(value_t _value) override;

    /// Sets the window of the transfer function and its pieces.
    DATA_API void setWindow(value_t _value) override;

    /**
     * @name Signals
     * @{
     */
    /// Defines the type of signal sent when points are modified.
    typedef core::com::Signal<void ()> PointsModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINTS_MODIFIED_SIG;

    /// Defines the type of signal sent when window-level is modified (window, level).
    typedef core::com::Signal<void (value_t, value_t)> WindowingModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_WINDOWING_MODIFIED_SIG;
/**
 * @}
 */

protected:

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache) override;

private:

    /// Sets the transfer function name.
    std::string m_name;

    /// Sets the recommended background color to use this TF.
    color_t m_backgroundColor {0., 0., 0., 0.};

    /// The transfer function object is a piecewise function
    std::vector<TransferFunctionData::sptr> m_pieces;
};
//------------------------------------------------------------------------------

inline TransferFunctionData::sptr TransferFunctionData::New()
{
    return std::make_shared<TransferFunctionData>();
}

//------------------------------------------------------------------------------

inline TransferFunctionData& TransferFunctionData::operator=(const TransferFunctionData& _other)
{
    this->clear();

    ContainerWrapper<data_t>::operator=(_other);

    this->m_level             = _other.m_level;
    this->m_window            = _other.m_window;
    this->m_interpolationMode = _other.m_interpolationMode;
    this->m_clamped           = _other.m_clamped;

    return *this;
}

//------------------------------------------------------------------------------

inline bool TransferFunctionData::operator==(const TransferFunctionData& _other) const noexcept
{
    if(!core::tools::is_equal(m_level, _other.m_level)
       || !core::tools::is_equal(m_window, _other.m_window)
       || m_interpolationMode != _other.m_interpolationMode
       || m_clamped != _other.m_clamped)
    {
        return false;
    }

    // Super class last
    return !core::tools::is_equal(
        static_cast<ContainerWrapper<data_t> >(*this),
        static_cast<ContainerWrapper<data_t> >(_other)
    );
}

//------------------------------------------------------------------------------

inline TransferFunctionData::value_t TransferFunctionData::mapValueToWindow(value_t _value) const
{
    const min_max_t minMax       = this->minMax();
    const min_max_t windowMinMax = this->windowMinMax();

    const value_t scale = m_window / (minMax.second - minMax.first);
    const value_t value = (_value - minMax.first) * scale + windowMinMax.first;

    return value;
}

//------------------------------------------------------------------------------

inline TransferFunctionData::value_t TransferFunctionData::mapValueFromWindow(value_t _value) const
{
    const min_max_t minMax       = this->minMax();
    const min_max_t windowMinMax = this->windowMinMax();

    const value_t scale = (minMax.second - minMax.first) / m_window;
    const value_t value = (_value - windowMinMax.first) * scale + minMax.first;

    return value;
}

//-----------------------------------------------------------------------------

inline TransferFunction::InterpolationMode TransferFunctionData::interpolationMode() const
{
    return m_interpolationMode;
}

//-----------------------------------------------------------------------------

inline void TransferFunctionData::setInterpolationMode(InterpolationMode _value)
{
    m_interpolationMode = _value;
}

//-----------------------------------------------------------------------------

inline TransferFunctionData::value_t TransferFunctionData::level() const
{
    return m_level;
}

//-----------------------------------------------------------------------------

inline TransferFunctionData::value_t TransferFunctionData::window() const
{
    return m_window;
}

//-----------------------------------------------------------------------------

inline bool TransferFunctionData::clamped() const
{
    return m_clamped;
}

//-----------------------------------------------------------------------------

inline void TransferFunctionData::setClamped(bool _value)
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

inline std::vector<TransferFunctionData::sptr>& TransferFunction::pieces()
{
    return m_pieces;
}

//-----------------------------------------------------------------------------

inline const std::vector<TransferFunctionData::sptr>& TransferFunction::pieces() const
{
    return m_pieces;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
