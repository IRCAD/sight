/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_TRANSFERTFUNCTION_VERSION_II_HPP_
#define _FWDATA_TRANSFERTFUNCTION_VERSION_II_HPP_

#include <vector>
#include <map>
#include <limits>

#include "fwData/config.hpp"
#include "fwData/macros.hpp"
#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{
/**
 * @class   TransfertFunction
 * @brief   This class defines a transfert function.
 *
 * A transfert function is composed of some points which have a color and a value.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API TransfertFunction_VERSION_II : public Object
{
public :

    fwCoreClassDefinitionsWithFactoryMacro( (TransfertFunction_VERSION_II)(::fwData::Object), (()), ::fwData::Factory::New< TransfertFunction_VERSION_II >) ;

    /// Macro for deep and shallow copies
    fwDataObjectMacro();


    /// Defines color structure for TF
    struct TFColor
    {
        /// red color (value [0,1])
        float r;
        /// green color (value [0,1])
        float g;
        /// blue color (value [0,1])
        float b;
        /// alpha (value [0,1])
        float a;

        // Default constructor
        TFColor()
        {
            r = 0.0;
            g = 0.0;
            b = 0.0;
            a = 0.0;
        };

        TFColor( float _r, float _g, float _b, float _a )
        {
            r = _r;
            g = _g;
            b = _b;
            a = _a;
        };

        inline bool operator== (const TFColor& _color) const
        {
            return (r == _color.r && g == _color.g && b == _color.b && a == _color.a);
        };

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


    // Initialize a default TF.
    FWDATA_API void initTF();

    // Create a default TF
    FWDATA_API static TransfertFunction_VERSION_II::sptr createDefaultTF();

    /// Shallow copy method
    FWDATA_API void shallowCopy( TransfertFunction_VERSION_II::csptr _source );

    /// Deep copy method
    FWDATA_API void deepCopy( TransfertFunction_VERSION_II::csptr _source );

    /// Get all the point values of the TF (keys of the map m_tfData)
    FWDATA_API TFValueVectorType getTFValues() const;

    /// Return the nearest point value of a value.
    FWDATA_API TFValueType getNearestValue( TFValueType value ) const;

    /// Get all the TF data .
    FWDATA_API const TFDataType& getTFData() const;

    /// Add a new TF color point.
    FWDATA_API void addTFColor( TFValueType value, const TFColor & color );

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
    fwDataGetSetMacro(InterpolationMode, InterpolationMode);

    /// Level
    fwDataGetSetMacro(Level, double);

    /// Window
    fwDataGetSetMacro(Window, double);

    /// Transfert function name
    fwDataGetSetCRefMacro(Name, std::string);

    /// is TF clamped
    fwDataGetSetMacro(IsClamped, bool);

    /// set the color of the recommanded background for the TF
    fwDataGetSetCRefMacro(BackgroundColor, TFColor);

    /// Default transfert function name
    FWDATA_API static const std::string s_DEFAULT_TF_NAME;

protected :

    /// Constructor
    FWDATA_API TransfertFunction_VERSION_II();

    /// Destructor
    FWDATA_API virtual ~TransfertFunction_VERSION_II();


private :

    /// Current visualisation level
    double m_attrLevel;

    /// Current visualisation window
    double m_attrWindow;

    ///  Funtion transfert name.
    std::string m_attrName;

    /// The recommended background color to use this TF.
    TFColor m_attrBackgroundColor;

    /// The Transfert function data.
    TFDataType m_attrTfData;

    /// The current interpolation mode.
    InterpolationMode m_attrInterpolationMode;

    /**
     *  @brief Defines interpolation mode on extremities
     *
     *  if m_isClamped == true then after extremity point, the returned TF color is TFColor(0,0,0,0).
     *  if m_isClamped == false then after extremity point, the returned TF color is one of the extremity color value.
    **/
    bool m_attrIsClamped;

}; // end class TransfertFunction_VERSION_II

} // end namespace fwData

#endif // _FWDATA_TRANSFERTFUNCTION_VERSION_II_HPP_
