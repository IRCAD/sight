/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/TransferFunction.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwTools/Type.hpp>

#include <functional>

fwDataRegisterMacro( ::fwData::TransferFunction );

namespace fwData
{

//------------------------------------------------------------------------------

const std::string TransferFunction::s_DEFAULT_TF_NAME = "CT-GreyLevel";

const ::fwCom::Signals::SignalKeyType TransferFunction::s_POINTS_MODIFIED_SIG    = "pointsModified";
const ::fwCom::Signals::SignalKeyType TransferFunction::s_WINDOWING_MODIFIED_SIG = "windowingModified";

//------------------------------------------------------------------------------

TransferFunction::TransferFunction(::fwData::Object::Key key)
{
    newSignal< PointsModifiedSignalType >(s_POINTS_MODIFIED_SIG);
    newSignal< WindowingModifiedSignalType >(s_WINDOWING_MODIFIED_SIG);

    this->initTF();
}

//------------------------------------------------------------------------------

void TransferFunction::initTF()
{
    m_level  = 0;
    m_window = 100;

    m_name = "";

    m_interpolationMode = TransferFunction::LINEAR;
    m_isClamped         = true;

    m_tfData.clear();
}

//------------------------------------------------------------------------------

fwData::TransferFunction::sptr TransferFunction::createDefaultTF()
{
    TransferFunction::sptr tf = TransferFunction::New();

    tf->setName(TransferFunction::s_DEFAULT_TF_NAME);
    tf->addTFColor(0.0, TFColor());
    tf->addTFColor(1.0, TFColor(1.0, 1.0, 1.0, 1.0));
    tf->setIsClamped(false);
    tf->setWindow( 50. );
    tf->setLevel( 500. );
    return tf;
}

//------------------------------------------------------------------------------

TransferFunction::~TransferFunction()
{
}

//------------------------------------------------------------------------------

TransferFunction::TFValueVectorType TransferFunction::getTFValues() const
{
    TFValueVectorType values;
    values.reserve(m_tfData.size());
    std::transform( m_tfData.begin(), m_tfData.end(),
                    std::back_inserter(values),
                    std::bind(&TFDataType::value_type::first, std::placeholders::_1) );
    return values;
}

//------------------------------------------------------------------------------

TransferFunction::TFValueVectorType TransferFunction::getScaledValues() const
{
    TFValueVectorType values;
    values.reserve(m_tfData.size());
    TFValuePairType minMax       = this->getMinMaxTFValues();
    TFValuePairType windowMinMax = this->getWLMinMax();

    const double scale = m_window / (minMax.second - minMax.first);

    for(const TFDataType::value_type& data : m_tfData)
    {
        const double value = (data.first - minMax.first) * scale + windowMinMax.first;
        values.push_back(value);
    }

    return values;
}

//------------------------------------------------------------------------------

TransferFunction::TFValuePairType
TransferFunction::getMinMaxTFValues() const
{
    SLM_ASSERT("It must have at least one value.", m_tfData.size() >= 1);
    TFValuePairType minMax;
    minMax.first  = m_tfData.begin()->first;
    minMax.second = (m_tfData.rbegin())->first;
    return minMax;
}

//------------------------------------------------------------------------------

TransferFunction::TFValuePairType
TransferFunction::getWLMinMax() const
{
    TFValuePairType minMax;
    double halfWindow = m_window/2.f;
    minMax.first  = m_level - halfWindow;
    minMax.second = m_level + halfWindow;
    return minMax;
}

//------------------------------------------------------------------------------

void TransferFunction::setWLMinMax(const TFValuePairType& minMax)
{
    m_window = minMax.second - minMax.first;
    double halfWindow = m_window/2.f;
    m_level = halfWindow + minMax.first;
}

//------------------------------------------------------------------------------

TransferFunction::TFValueType TransferFunction::getNearestValue( TFValueType value ) const
{
    OSLM_ASSERT("It must have at least one value.", m_tfData.size() >= 1);
    std::pair<double, double> minMax = ::fwTools::Type::s_DOUBLE.minMax<double>();
    double previousValue             = minMax.first;
    double nextValue                 = minMax.second;

    TFValueType val;
    for(const TFDataType::value_type& data : m_tfData)
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

void TransferFunction::setTFData( const TFDataType& tfData )
{
    m_tfData = tfData;
}

//------------------------------------------------------------------------------

void TransferFunction::addTFColor( TFValueType value, const TFColor& color )
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
                    std::bind(&TFDataType::value_type::second, std::placeholders::_1) );
    return colors;
}

//------------------------------------------------------------------------------

TransferFunction::TFColor TransferFunction::getNearestColor( TFValueType value ) const
{
    OSLM_ASSERT("It must have at least one value.", m_tfData.size() >= 1);

    double min = std::numeric_limits<double>::min();
    double max = std::numeric_limits<double>::max();

    double previousValue = min;
    double nextValue     = max;

    TFColor blackColor(0.0, 0.0, 0.0, 0.0);
    TFColor color;
    TFColor previousColor = blackColor;
    TFColor nextColor     = blackColor;

    for(const TFDataType::value_type& data : m_tfData)
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
    if(previousValue == min)
    {
        if(m_isClamped)
        {
            color = blackColor;
        }
        else
        {
            color = nextColor;
        }
    }
    else if(nextValue == max)
    {
        if(m_isClamped && (value != previousValue))
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
    OSLM_ASSERT("It must have at least one value.", m_tfData.size() >= 1);

    double min           = std::numeric_limits<double>::min();
    double max           = std::numeric_limits<double>::max();
    double previousValue = min;
    double nextValue     = max;

    TFColor blackColor(0.0, 0.0, 0.0, 0.0);
    TFColor color;
    TFColor previousColor = blackColor;
    TFColor nextColor     = blackColor;

    for(const TFDataType::value_type& data : m_tfData)
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
    if(previousValue == min)
    {
        if(m_isClamped)
        {
            color = blackColor;
        }
        else
        {
            color = nextColor;
        }
    }
    else if(nextValue == max)
    {
        if(m_isClamped && (value != previousValue))
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
        const double distanceToNextValue     = nextValue - value;
        const double distanceToPreviousValue = value - previousValue;
        const double distance                = 1.0 / (nextValue - previousValue);
        const double coefPrevious            = 1.0 - (distanceToPreviousValue * distance);
        const double coefNext                = 1.0 - (distanceToNextValue * distance);

        color.r = coefPrevious*previousColor.r + coefNext*nextColor.r;
        color.g = coefPrevious*previousColor.g + coefNext*nextColor.g;
        color.b = coefPrevious*previousColor.b + coefNext*nextColor.b;
        color.a = coefPrevious*previousColor.a + coefNext*nextColor.a;
    }
    return color;
}

//------------------------------------------------------------------------------

const TransferFunction::TFColor& TransferFunction::getTFColor( TFValueType value ) const
{
    TFDataType::const_iterator itr = m_tfData.find(value);
    OSLM_ASSERT("The value "<< value <<" is not defined in the transfer function.", itr != m_tfData.end());
    return itr->second;
}
//------------------------------------------------------------------------------

void TransferFunction::shallowCopy(const Object::csptr& _source )
{
    TransferFunction::csptr other = TransferFunction::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    this->m_level             = other->m_level;
    this->m_window            = other->m_window;
    this->m_name              = other->m_name;
    this->m_backgroundColor   = other->m_backgroundColor;
    this->m_tfData            = other->m_tfData;
    this->m_interpolationMode = other->m_interpolationMode;
    this->m_isClamped         = other->m_isClamped;
}

//------------------------------------------------------------------------------

void TransferFunction::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    TransferFunction::csptr other = TransferFunction::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    this->m_level             = other->m_level;
    this->m_window            = other->m_window;
    this->m_name              = other->m_name;
    this->m_backgroundColor   = other->m_backgroundColor;
    this->m_tfData            = other->m_tfData;
    this->m_interpolationMode = other->m_interpolationMode;
    this->m_isClamped         = other->m_isClamped;
}

//------------------------------------------------------------------------------

} // end namespace fwData
