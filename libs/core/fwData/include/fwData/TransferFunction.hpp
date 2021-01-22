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

#include "fwData/config.hpp"
#include "fwData/Object.hpp"

fwCampAutoDeclareDataMacro((fwData)(TransferFunction))

namespace fwData
{

/**
 * @brief This class defines a transfer function.
 *
 * A transfer function is composed of some points which have a color and a value.
 */
class FWDATA_CLASS_API TransferFunction : public Object
{

public:

    fwCoreClassMacro(TransferFunction, ::fwData::Object, ::fwData::factory::New< TransferFunction >)

    /// Defines the deep and shallow copies.
    fwCampMakeFriendDataMacro((fwData)(TransferFunction))

    /// Defines color structure for TF.
    struct TFColor
    {
        typedef double ColorType;

        /// Sets red color (value [0,1]).
        ColorType r { 0.0 };

        /// Sets green color (value [0,1]).
        ColorType g { 0.0 };

        /// Sets blue color (value [0,1]).
        ColorType b { 0.0 };

        /// Sets alpha (value [0,1]).
        ColorType a { 0.0 };

        /// Creates a default black color.
        TFColor()
        {
        }

        /**
         * @brief Creates a color from rgba values.
         * @param _r the red channel value.
         * @param _g the blue channel value.
         * @param _b the red channel value.
         * @param _a the alpha channel value.
         */
        TFColor( ColorType _r, ColorType _g, ColorType _b, ColorType _a )
        {
            r = _r;
            g = _g;
            b = _b;
            a = _a;
        }

        /**
         * @brief Compares this color with another one.
         * @param _color the color to compare.
         * @return True if this color have the same rgba value than the compared one.
         */
        inline bool operator== (const TFColor& _color) const
        {
            return (r == _color.r && g == _color.g && b == _color.b && a == _color.a);
        }

    };

    /// Defines the available modes {LINEAR, NEAREST} to interpolate color between two TF color points.
    typedef enum
    {
        LINEAR,
        NEAREST
    } InterpolationMode;

    /// Defines the type of values of each point.
    typedef double TFValueType;

    /// Defines a vector of values.
    typedef std::vector<TFValueType> TFValueVectorType;

    /// Defines a vector of colors.
    typedef std::vector<TFColor> TFColorVectorType;

    /// Defines a point type, affects a color for each value.
    typedef std::map< TFValueType, TFColor > TFDataType;

    typedef std::pair< TFValueType, TFValueType > TFValuePairType;

    /// Sets the defaults transfer function name.
    FWDATA_API static const std::string s_DEFAULT_TF_NAME;

    /// Creates a default TF.
    FWDATA_API static TransferFunction::sptr createDefaultTF();

    /// Initializes signals.
    FWDATA_API TransferFunction(::fwData::Object::Key);

    /// Destroys the TF.
    FWDATA_API virtual ~TransferFunction();

    /// Initializes a default TF.
    FWDATA_API void initTF();

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    FWDATA_API void shallowCopy(const Object::csptr& _source) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Gets all point values of the TF (keys of the map m_tfData).
    FWDATA_API TFValueVectorType getTFValues() const;

    /// Gets all point values of the TF scaled and shifted with the window level.
    FWDATA_API TFValueVectorType getScaledValues() const;

    /// Gets the first and last point values of the tf data.
    FWDATA_API TFValuePairType getMinMaxTFValues() const;

    /// Gets the min/max of the window level.
    FWDATA_API TFValuePairType getWLMinMax() const;

    /// Sets the min/max of the window level.
    FWDATA_API void setWLMinMax(const TFValuePairType& _minMax);

    /// Gets the nearest point value of a value.
    FWDATA_API TFValueType getNearestValue(TFValueType _value) const;

    /// Gets all the TF data.
    const TFDataType& getTFData() const;

    /// Sets all the TF data (clear previous tf data).
    void setTFData(const TFDataType& _tfData);

    /// Adds a new TF color point.
    void addTFColor(TFValueType _value, const TFColor& _color);

    /// Erases a TF color point.
    void eraseTFValue(TFValueType _value);

    /// Clears all the TF data.
    void clear();

    /// Gets all the colors of the TF.
    FWDATA_API TFColorVectorType getTFColors() const;

    /// Gets the nearest color of a value.
    FWDATA_API TFColor getNearestColor(TFValueType _value) const;

    /// Gets the color for a value (the color is computed with a linear interpolation).
    FWDATA_API TFColor getLinearColor(TFValueType _value) const;

    /// Gets the interpolated color of the TF for a value.The result depends of the current interpolation mode.
    FWDATA_API TFColor getInterpolatedColor(TFValueType _value) const;

    /// Gets the color associated to the value.
    FWDATA_API const TFColor& getTFColor(TFValueType _value) const;

    /// Gets the interpolation mode.
    InterpolationMode getInterpolationMode() const;

    /// Sets the interpolation mode.
    void setInterpolationMode(InterpolationMode _value);

    /// Gets the level.
    double getLevel() const;

    /// Sets the level.
    void setLevel(double _value);

    /// Gets the window level.
    double getWindow() const;

    /// Sets the window level.
    void setWindow(double _value);

    /// Gets the transfert function name.
    const std::string& getName() const;

    /// Sets the transfert function name.
    void setName(const std::string& _value);

    /// Gets if the TF is clamped.
    bool getIsClamped() const;

    /// Sets if the TF is clamped.
    void setIsClamped(bool _value);

    /// Gets the TF background color when tf 'IsClamped' is true.
    const TFColor& getBackgroundColor() const;

    /// Set the TF background color when tf 'IsClamped' is true.
    void setBackgroundColor(const TFColor& _value);

    /**
     * @name Signals
     * @{
     */
    /// Defines the type of signal sent when points are modified.
    typedef core::com::Signal< void () > PointsModifiedSignalType;
    FWDATA_API static const core::com::Signals::SignalKeyType s_POINTS_MODIFIED_SIG;

    /// Defines the type of signal sent when window-level is modified (window, level).
    typedef core::com::Signal< void (double, double) > WindowingModifiedSignalType;
    FWDATA_API static const core::com::Signals::SignalKeyType s_WINDOWING_MODIFIED_SIG;
    /**
     * @}
     */

private:

    /// Sets the current visualization level.
    double m_level;

    /// Sets the current visualization window.
    double m_window;

    /// Sets the transfer function name.
    std::string m_name;

    /// Sets the recommended background color to use this TF.
    TFColor m_backgroundColor;

    /// Stores the transfer function data.
    TFDataType m_tfData;

    /// Defines the current interpolation mode.
    InterpolationMode m_interpolationMode;

    /**
     *  @brief Defines interpolation mode on extremities.
     *
     *  if m_isClamped == true then after extremity point, the returned TF color is TFColor(0,0,0,0).
     *  if m_isClamped == false then after extremity point, the returned TF color is one of the extremity color value.
     **/
    bool m_isClamped;

};

//------------------------------------------------------------------------------

inline const TransferFunction::TFDataType& TransferFunction::getTFData() const
{
    return m_tfData;
}

//------------------------------------------------------------------------------

inline void TransferFunction::setTFData(const TFDataType& _tfData)
{
    m_tfData = _tfData;
}

//------------------------------------------------------------------------------

inline void TransferFunction::addTFColor( TFValueType _value, const TFColor& _color)
{
    m_tfData[_value] = _color;
}

//------------------------------------------------------------------------------

inline void TransferFunction::eraseTFValue(TFValueType _value)
{
    m_tfData.erase(_value);
}

//------------------------------------------------------------------------------

inline void TransferFunction::clear()
{
    m_tfData.clear();
}

//-----------------------------------------------------------------------------

inline TransferFunction::InterpolationMode TransferFunction::getInterpolationMode() const
{
    return m_interpolationMode;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setInterpolationMode(InterpolationMode _value)
{
    m_interpolationMode = _value;
}

//-----------------------------------------------------------------------------

inline double TransferFunction::getLevel() const
{
    return m_level;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setLevel(double _value)
{
    m_level = _value;
}

//-----------------------------------------------------------------------------

inline double TransferFunction::getWindow() const
{
    return m_window;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setWindow(double _value)
{
    m_window = _value;
}

//-----------------------------------------------------------------------------

inline const std::string& TransferFunction::getName() const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setName(const std::string& _value)
{
    m_name = _value;
}

//-----------------------------------------------------------------------------

inline bool TransferFunction::getIsClamped() const
{
    return m_isClamped;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setIsClamped(bool _value)
{
    m_isClamped = _value;
}

//-----------------------------------------------------------------------------

inline const TransferFunction::TFColor& TransferFunction::getBackgroundColor() const
{
    return m_backgroundColor;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setBackgroundColor(const TFColor& _value)
{
    m_backgroundColor = _value;
}

//-----------------------------------------------------------------------------

} // namespace fwData
