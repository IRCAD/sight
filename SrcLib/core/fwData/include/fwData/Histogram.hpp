/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

fwCampAutoDeclareDataMacro((fwData)(Histogram), FWDATA_API);

namespace fwData
{

/**
 * @brief This class defines the histogram of an image.
 */
class FWDATA_CLASS_API Histogram : public Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Histogram)(::fwData::Object), (()), ::fwData::factory::New< Histogram > );

    fwCampMakeFriendDataMacro((fwData)(Histogram));

    typedef std::vector< long > fwHistogramValues;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Histogram(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Histogram();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Add the given pixel value into the histogram.
     *
     * This void will chose the right bins where the pixel must be added (if the bins width parameter is greater than
     * 1).
     *
     * @param pixel the pixel value to be added into the histogram.
     */
    FWDATA_API void addPixel( float pixel );

    /**
     * @brief Return the number of pixels of the histogram that are within the range defined by the given parameters min
     * and max.
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

    /** @name bins width accessor
     *  @brief Get/Set bins width
     *  @{
     */
    const float  getBinsWidth() const;
    void setBinsWidth(float _binsWidth);
    /// @}

    /** @name histogram values accessor
     *  @brief Get/Set histogram values
     *  @{
     */
    fwHistogramValues& getRefValues();
    const fwHistogramValues& getValues() const;
    void setValues(const fwHistogramValues& _values);
    /// @}

    /** @name minimum value within the histogram
     *  @brief Get/Set minimum value within the histogram
     *  @{
     */
    float& getRefMinValue ();
    const float& getMinValue () const;
    void setMinValue (float _minValue);
    /// @}

    /** @name maximum value within the histogram
     *  @brief Get/Set maximum value within the histogram
     *  @{
     */
    float& getRefMaxValue();
    const float& getMaxValue() const;
    void setMaxValue(float _maxValue);
    /// @}

protected:

    /// Histogram values.
    fwHistogramValues m_values;

    /// The pixel width of the bins (a default value is set to 1).
    float m_binsWidth;

    /// The minimum pixel value within the histogram (a default value is set to 0).
    float m_minValue;

    /// The maximum pixel value within the histogram (a default value is set to 100).
    float m_maxValue;
};

//-----------------------------------------------------------------------------

inline const float Histogram::getBinsWidth() const
{
    return m_binsWidth;
}

//-----------------------------------------------------------------------------

inline void Histogram::setBinsWidth(float _binsWidth)
{
    this->m_binsWidth = _binsWidth;
}

//-----------------------------------------------------------------------------

inline Histogram::fwHistogramValues& Histogram::getRefValues()
{
    return this->m_values;
}

//-----------------------------------------------------------------------------

inline const Histogram::fwHistogramValues& Histogram::getValues() const
{
    return this->m_values;
}

//-----------------------------------------------------------------------------

inline void Histogram::setValues(const Histogram::fwHistogramValues& _values)
{
    this->m_values = _values;
}

//-----------------------------------------------------------------------------

inline float& Histogram::getRefMinValue ()
{
    return this->m_minValue;
}

//-----------------------------------------------------------------------------

inline const float& Histogram::getMinValue () const
{
    return this->m_minValue;
}

//-----------------------------------------------------------------------------

inline void Histogram::setMinValue (float _minValue)
{
    this->m_minValue = _minValue;
}

//-----------------------------------------------------------------------------

inline float& Histogram::getRefMaxValue()
{
    return this->m_maxValue;
}

//-----------------------------------------------------------------------------

inline const float& Histogram::getMaxValue() const
{
    return this->m_maxValue;
}

//-----------------------------------------------------------------------------

inline void Histogram::setMaxValue(float _maxValue)
{
    this->m_maxValue = _maxValue;
}

//-----------------------------------------------------------------------------

} // namespace fwData

