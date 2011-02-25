/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_HISTOGRAM_HPP_
#define _FWDATA_HISTOGRAM_HPP_

#include <fwData/Object.hpp>
#include "fwData/config.hpp"

#include <boost/shared_ptr.hpp>


namespace fwData
{


/**
 * @class   Histogram
 * @brief   This class defines the histogram of an image.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class FWDATA_CLASS_API Histogram : public Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Histogram)(::fwData::Object), (()), ::fwTools::Factory::New< Histogram > );

    typedef std::vector< long > fwHistogramValues;

    /// Constructor
    FWDATA_API Histogram();

    /// Destructor
    FWDATA_API virtual ~Histogram();

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Histogram::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Histogram::csptr _source );

    FWDATA_API void addPixel( float pixel );

    FWDATA_API long getNbPixels( float min, float max );

    FWDATA_API bool isInRange( float pixel );

    FWDATA_API void initialize( float min, float max, float binsWidth );

    fwGettersSettersDocMacro(BinsWidth, binsWidth, float, bins width);
    fwGettersSettersDocMacro(Values, values, fwHistogramValues, histogram values);
    fwGettersSettersDocMacro(MinValue, minValue, float, minimum value within the histogram);
    fwGettersSettersDocMacro(MaxValue, maxValue, float, maximum value within the histogram);

protected:

    /**
     * @brief Histogram values.
     */
    fwHistogramValues m_values;

    /**
     * @brief The pixel width of the bins.
     */
    float m_binsWidth;

    /**
     * @brief The minimum pixel value within the histogram.
     */
    float m_minValue;

    /**
     * @brief The maximum pixel value within the histogram.
     */
    float m_maxValue;
};

}

#endif // _FWDATA_HISTOGRAM_HPP_

