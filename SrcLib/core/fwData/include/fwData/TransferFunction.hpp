/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_TRANSFERFUNCTION_HPP__
#define __FWDATA_TRANSFERFUNCTION_HPP__

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <limits>
#include <map>
#include <vector>

fwCampAutoDeclareDataMacro((fwData)(TransferFunction), FWDATA_API);

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

    fwCoreClassDefinitionsWithFactoryMacro( (TransferFunction)(::fwData::Object), (()),
                                            ::fwData::factory::New< TransferFunction >);

    /// Macro for deep and shallow copies

    fwCampMakeFriendDataMacro((fwData)(TransferFunction));

    /// Defines color structure for TF
    struct TFColor
    {
        typedef double ColorType;

        /// red color (value [0,1])
        ColorType r;
        /// green color (value [0,1])
        ColorType g;
        /// blue color (value [0,1])
        ColorType b;
        /// alpha (value [0,1])
        ColorType a;

        // Default constructor
        TFColor()
        {
            r = 0.0;
            g = 0.0;
            b = 0.0;
            a = 0.0;
        }

        TFColor( ColorType _r, ColorType _g, ColorType _b, ColorType _a )
        {
            r = _r;
            g = _g;
            b = _b;
            a = _a;
        }

        //------------------------------------------------------------------------------

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

    typedef double TFValueType;
    typedef std::vector<TFValueType> TFValueVectorType;

    typedef std::vector<TFColor> TFColorVectorType;
    typedef std::map< TFValueType, TFColor > TFDataType;

    typedef std::pair< TFValueType, TFValueType > TFValuePairType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API TransferFunction(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~TransferFunction();

    // Initialize a default TF.
    FWDATA_API void initTF();

    // Create a default TF
    FWDATA_API static TransferFunction::sptr createDefaultTF();

    /// Shallow copy method
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Deep copy method
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Get all the point values of the TF (keys of the map m_tfData)
    FWDATA_API TFValueVectorType getTFValues() const;

    /// Get all the point values of the TF scaled and shifted with the window level
    FWDATA_API TFValueVectorType getScaledValues() const;

    /// Get the first and last point values of the tf data
    FWDATA_API TFValuePairType getMinMaxTFValues() const;

    /// Get the min/max of the window level
    FWDATA_API TFValuePairType getWLMinMax() const;

    /// Set the min/max of the window level
    FWDATA_API void setWLMinMax(const TFValuePairType& minMax);

    /// Return the nearest point value of a value.
    FWDATA_API TFValueType getNearestValue( TFValueType value ) const;

    /// Get all the TF data .
    FWDATA_API const TFDataType& getTFData() const;

    /// Set all the TF data (clear previous tf data).
    FWDATA_API void setTFData( const TFDataType& tfData );

    /// Add a new TF color point.
    FWDATA_API void addTFColor( TFValueType value, const TFColor& color );

    /// Erase a TF color point.
    FWDATA_API void eraseTFValue( TFValueType value);

    /// Clear all the TF data.
    FWDATA_API void clear();

    /// Get all the colors of the TF
    FWDATA_API TFColorVectorType getTFColors() const;

    /// Get the nearest color of a value.
    FWDATA_API TFColor getNearestColor( TFValueType value ) const;

    /// Get the color for a value (the color is computed with a linear interpolation).
    FWDATA_API TFColor getLinearColor( TFValueType value ) const;

    /// Get the interpolated color of the TF for a value.The result depends of the current interpolation mode.
    FWDATA_API TFColor getInterpolatedColor( TFValueType value ) const;

    // Get the color associated to the value.
    FWDATA_API const TFColor& getTFColor( TFValueType value ) const;

    /// Interpolation mode
    InterpolationMode  getInterpolationMode () const;

    void setInterpolationMode (InterpolationMode val);

    /// Level
    double  getLevel () const;
    void setLevel (double val);

    /// Window
    double getWindow () const;
    void setWindow (double val);

    /// Transfert function name
    const std::string& getName () const;
    void setName (const std::string& val);

    /// is TF clamped
    bool  getIsClamped () const;
    void setIsClamped (bool val);

    /// set the TF background color when tf 'IsClamped' is true
    const TFColor& getBackgroundColor () const;
    void setBackgroundColor (const TFColor& val);

    /// Default transfer function name
    FWDATA_API static const std::string s_DEFAULT_TF_NAME;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when points are modified
    typedef ::fwCom::Signal< void () > PointsModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_POINTS_MODIFIED_SIG;

    /// Type of signal when window-level is modified (window, level)
    typedef ::fwCom::Signal< void (double, double) > WindowingModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_WINDOWING_MODIFIED_SIG;
    /**
     * @}
     */

private:

    /// Current visualization level
    double m_level;

    /// Current visualization window
    double m_window;

    ///  Function transfer name.
    std::string m_name;

    /// The recommended background color to use this TF.
    TFColor m_backgroundColor;

    /// The Transfer function data.
    TFDataType m_tfData;

    /// The current interpolation mode.
    InterpolationMode m_interpolationMode;

    /**
     *  @brief Defines interpolation mode on extremities
     *
     *  if m_isClamped == true then after extremity point, the returned TF color is TFColor(0,0,0,0).
     *  if m_isClamped == false then after extremity point, the returned TF color is one of the extremity color value.
     **/
    bool m_isClamped;

}; // class TransferFunction

//-----------------------------------------------------------------------------

inline TransferFunction::TFColor TransferFunction::getInterpolatedColor( TFValueType value ) const
{
    TFColor color;

    if(m_interpolationMode == LINEAR)
    {
        color = this->getLinearColor(value);
    }
    else if(m_interpolationMode == NEAREST)
    {
        color = this->getNearestColor(value);
    }
    return color;
}

//-----------------------------------------------------------------------------

inline TransferFunction::InterpolationMode TransferFunction::getInterpolationMode () const
{
    return m_interpolationMode;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setInterpolationMode (InterpolationMode val)
{
    m_interpolationMode = val;
}

//-----------------------------------------------------------------------------

inline double TransferFunction::getLevel () const
{
    return m_level;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setLevel (double val)
{
    m_level = val;
}

//-----------------------------------------------------------------------------

inline double TransferFunction::getWindow () const
{
    return m_window;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setWindow (double val)
{
    m_window = val;
}

//-----------------------------------------------------------------------------

inline const std::string& TransferFunction::getName () const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setName (const std::string& val)
{
    m_name = val;
}

//-----------------------------------------------------------------------------

inline bool TransferFunction::getIsClamped () const
{
    return m_isClamped;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setIsClamped (bool val)
{
    m_isClamped = val;
}

//-----------------------------------------------------------------------------

inline const TransferFunction::TFColor& TransferFunction::getBackgroundColor () const
{
    return m_backgroundColor;
}

//-----------------------------------------------------------------------------

inline void TransferFunction::setBackgroundColor (const TFColor& val)
{
    m_backgroundColor = val;
}

//-----------------------------------------------------------------------------

} // namespace fwData

#endif // __FWDATA_TRANSFERFUNCTION_HPP__

