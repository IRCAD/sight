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

    /**
     * @brief Add the given pixel value into the histogram. 
     * 
     * This void will chose the right bins where the pixel must be added (if the bins width parameter is greater than 1).
     *
     * @param pixel the pixel value to be added into the histogram.
     */
    FWDATA_API void addPixel( float pixel );

    /**
     * @brief Return the number of pixels of the histogram that are within the range defined by the given parameters min and max.
     *
     * @param _min lower value of the range
     * @param _max upper value of the range
     *
     * @return number of pixels that are within [_min, _max]
     */ 
    FWDATA_API long getNbPixels( float _min, float _max );

    /**
     * @brief Initialize the histogram.
     * 
     * @param min Defines the lower pixel value of the image that this histogram belongs to.
     * @param max Defines the upper pixel value of the image that this histogram belongs to.
     * @param binsWidth Defines the desired bins' width in order to classify pixels.
     */
    FWDATA_API void initialize( float min, float max, float binsWidth );

    /**
     * @brief Return true if the given pixel value is set within histogram's boudaries.
     * 
     * @param _pixel the pixel value
     *
     * @return true if the pixel value is set within histogram's boundaries.
     */
    FWDATA_API bool isInRange( float _pixel );

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
     * @brief The pixel width of the bins (a default value is set to 1).
     */
    float m_binsWidth;

    /**
     * @brief The minimum pixel value within the histogram (a default value is set to 0).
     */
    float m_minValue;

    /**
     * @brief The maximum pixel value within the histogram (a default value is set to 100).
     */
    float m_maxValue;
};

}

#endif // _FWDATA_HISTOGRAM_HPP_

