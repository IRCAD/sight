/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

//#include <cmath>
#include <boost/bind.hpp>

#include <fwCore/base.hpp>
#include <fwTools/Type.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/TransferFunction.hpp"


fwDataRegisterMacro( ::fwData::TransferFunction );

namespace fwData
{

//------------------------------------------------------------------------------

const std::string TransferFunction::s_DEFAULT_TF_NAME = "CT-GreyLevel";

//------------------------------------------------------------------------------

TransferFunction::TransferFunction()
{
    SLM_TRACE_FUNC();
    this->initTF();
}

//------------------------------------------------------------------------------

void TransferFunction::initTF()
{

    m_attrLevel = 0;
    m_attrWindow = 100;

    m_attrName = "";

    m_attrInterpolationMode = TransferFunction::LINEAR;
    m_attrIsClamped = true;

    m_tfData.clear();

}

//------------------------------------------------------------------------------

fwData::TransferFunction::sptr TransferFunction::createDefaultTF()
{
    TransferFunction::NewSptr tf;

    tf->setName(TransferFunction::s_DEFAULT_TF_NAME);
    tf->addTFColor(0.0, TFColor());
    tf->addTFColor(1.0, TFColor(1.0,1.0,1.0,1.0));
    tf->setIsClamped(false);
    return tf;
}

//------------------------------------------------------------------------------

TransferFunction::~TransferFunction()
{}

//------------------------------------------------------------------------------

TransferFunction::TFValueVectorType TransferFunction::getTFValues() const
{
    TFValueVectorType values;
    values.reserve(m_tfData.size());
    std::transform( m_tfData.begin(), m_tfData.end(),
            std::back_inserter(values),
            ::boost::bind(& TFDataType::value_type::first, _1) );
    return values;
}

//------------------------------------------------------------------------------

TransferFunction::TFValueVectorType TransferFunction::getScaledValues() const
{
    TFValueVectorType values;
    values.reserve(m_tfData.size());
    TFValuePairType minMax = this->getMinMaxTFValues();
    TFValuePairType windowMinMax = this->getWLMinMax();

    const double shift =  windowMinMax.first - minMax.first;
    const double scale = m_attrWindow / (minMax.second - minMax.first);

    BOOST_FOREACH(const TFDataType::value_type &data, m_tfData)
    {
        values.push_back( (data.first * scale) + shift );
    }

    return values;
}

//------------------------------------------------------------------------------

TransferFunction::TFValuePairType
TransferFunction::getMinMaxTFValues() const
{
    OSLM_ASSERT("It must have at least one value.", m_tfData.size()>= 1);
    TFValuePairType minMax;
    minMax.first = m_tfData.begin()->first;
    minMax.second = (m_tfData.rbegin())->first;
    return minMax;
}

//------------------------------------------------------------------------------

TransferFunction::TFValuePairType
TransferFunction::getWLMinMax() const
{
    TFValuePairType minMax;
    double halfWindow = m_attrWindow/2.f;
    minMax.first = m_attrLevel - halfWindow;
    minMax.second = m_attrLevel + halfWindow;;
    return minMax;
}

//------------------------------------------------------------------------------

void TransferFunction::setWLMinMax(const TFValuePairType& minMax)
{
    m_attrWindow = minMax.second - minMax.first;
    m_attrLevel = m_attrWindow/2.f;
}

//------------------------------------------------------------------------------

TransferFunction::TFValueType TransferFunction::getNearestValue( TFValueType value ) const
{
    OSLM_ASSERT("It must have at least one value.", m_tfData.size()>= 1);
    std::pair<double, double> minMax  = ::fwTools::Type::s_DOUBLE.minMax<double>();
    double previousValue = minMax.first;
    double nextValue = minMax.second;

    TFValueType val;
    BOOST_FOREACH(const TFDataType::value_type &data, m_tfData)
    {
        if(value < data.first )
        {
            nextValue = data.first;
            break;
        }
        else
        {
            previousValue = data.first;
        }
    }
    if(previousValue == minMax.first)
    {
        val = nextValue;
    }
    else if(nextValue == minMax.second)
    {
        val = previousValue;
    }
    else
    {
        if((value - previousValue) < (nextValue - value))
        {
            val = previousValue;
        }
        else
        {
            val = nextValue;
        }
    }
    return val;
}

//------------------------------------------------------------------------------

const TransferFunction::TFDataType& TransferFunction::getTFData() const
{
    return m_tfData;
}

//------------------------------------------------------------------------------

void TransferFunction::setTFData( const TFDataType & tfData )
{
    m_tfData = tfData;
}

//------------------------------------------------------------------------------

void TransferFunction::addTFColor( TFValueType value, const TFColor & color )
{
    m_tfData[value] = color;
}

//------------------------------------------------------------------------------
void TransferFunction::eraseTFValue( TFValueType value)
{
    m_tfData.erase(value);
}

//------------------------------------------------------------------------------
void TransferFunction::clear()
{
    m_tfData.clear();
}
//------------------------------------------------------------------------------

TransferFunction::TFColorVectorType TransferFunction::getTFColors() const
{
    TFColorVectorType colors;
    std::transform( m_tfData.begin(), m_tfData.end(),
            std::back_inserter(colors),
            ::boost::bind(& TFDataType::value_type::second, _1) );
    return colors;
}

//------------------------------------------------------------------------------

TransferFunction::TFColor TransferFunction::getNearestColor( TFValueType value ) const
{
    OSLM_ASSERT("It must have at least one value.", m_tfData.size()>= 1);
    std::pair<double, double> minMax  = ::fwTools::Type::s_DOUBLE.minMax<double>();
    double previousValue = minMax.first;
    double nextValue = minMax.second;

    TFColor blackColor(0.0, 0.0, 0.0, 0.0);
    TFColor color;
    TFColor previousColor = blackColor;
    TFColor nextColor = blackColor;

    BOOST_FOREACH(const TFDataType::value_type &data, m_tfData)
    {
        if(value < data.first )
        {
            nextValue = data.first;
            nextColor = data.second;
            break;
        }
        else
        {
            previousValue = data.first;
            previousColor = data.second;
        }
    }
    if(previousValue == minMax.first)
    {
        if(m_attrIsClamped)
        {
            color = blackColor;
        }
        else
        {
            color = nextColor;
        }
    }
    else if(nextValue == minMax.second)
    {
        if(m_attrIsClamped && (value != previousValue))
        {
            color = blackColor;
        }
        else
        {
            color = previousColor;
        }
    }
    else
    {
        if((value - previousValue) < (nextValue - value))
        {
            color = previousColor;
        }
        else
        {
            color = nextColor;
        }
    }

    return color;
}

//------------------------------------------------------------------------------

TransferFunction::TFColor TransferFunction::getLinearColor( TFValueType value ) const
{
    OSLM_ASSERT("It must have at least one value.", m_tfData.size()>= 1);
    std::pair<double, double> minMax  = ::fwTools::Type::s_DOUBLE.minMax<double>();
    double previousValue = minMax.first;
    double nextValue = minMax.second;

    TFColor blackColor(0.0, 0.0, 0.0, 0.0);
    TFColor color;
    TFColor previousColor = blackColor;
    TFColor nextColor = blackColor;

    BOOST_FOREACH(const TFDataType::value_type &data, m_tfData)
    {
        if(value < data.first )
        {
            nextValue = data.first;
            nextColor = data.second;
            break;
        }
        else
        {
            previousValue = data.first;
            previousColor = data.second;
        }
    }
    if(previousValue == minMax.first)
    {
        if(m_attrIsClamped)
        {
            color = blackColor;
        }
        else
        {
            color = nextColor;
        }
    }
    else if(nextValue == minMax.second)
    {
        if(m_attrIsClamped && (value != previousValue))
        {
            color = blackColor;
        }
        else
        {
            color = previousColor;
        }
    }
    else
    {
        // Interpolate the color.
        double distanceToNextValue = nextValue - value;
        double distanceToPreviousValue = value - previousValue;
        double distance = nextValue - previousValue;
        double coefPrevious = 1.0 - (distanceToPreviousValue/distance);
        double coefNext = 1.0 - (distanceToNextValue/distance);

        color.r =  coefPrevious*previousColor.r + coefNext*nextColor.r;
        color.g =  coefPrevious*previousColor.g + coefNext*nextColor.g;
        color.b =  coefPrevious*previousColor.b + coefNext*nextColor.b;
        color.a =  coefPrevious*previousColor.a + coefNext*nextColor.a;
    }
    return color;
}

//------------------------------------------------------------------------------

TransferFunction::TFColor TransferFunction::getInterpolatedColor( TFValueType value ) const
{
    TFColor color;

    if(m_attrInterpolationMode == LINEAR)
    {
        color = this->getLinearColor(value);
    }
    else if(m_attrInterpolationMode == NEAREST)
    {
        color = this->getNearestColor(value);
    }
    return color;
}

//------------------------------------------------------------------------------

const TransferFunction::TFColor& TransferFunction::getTFColor( TFValueType value ) const
{
    TFDataType::const_iterator itr = m_tfData.find(value);
    SLM_ASSERT("Sorry not defined in th TF.", itr != m_tfData.end());
    return itr->second;
}
//------------------------------------------------------------------------------

void TransferFunction::shallowCopy( TransferFunction::csptr _source )
{
    this->fieldShallowCopy( _source );
    this->m_attrLevel = _source->m_attrLevel;
    this->m_attrWindow = _source->m_attrWindow;
    this->m_attrName =_source->m_attrName;
    this->m_attrBackgroundColor = _source->m_attrBackgroundColor;
    this->m_tfData = _source->m_tfData;
    this->m_attrInterpolationMode = _source->m_attrInterpolationMode;
    this->m_attrIsClamped = _source->m_attrIsClamped;
}

//------------------------------------------------------------------------------

void TransferFunction::deepCopy( TransferFunction::csptr _source )
{
    this->fieldDeepCopy( _source );
    this->m_attrLevel = _source->m_attrLevel;
    this->m_attrWindow = _source->m_attrWindow;
    this->m_attrName =_source->m_attrName;
    this->m_attrBackgroundColor = _source->m_attrBackgroundColor;
    this->m_tfData = _source->m_tfData;
    this->m_attrInterpolationMode = _source->m_attrInterpolationMode;
    this->m_attrIsClamped = _source->m_attrIsClamped;
}

//------------------------------------------------------------------------------

} // end namespace fwData
