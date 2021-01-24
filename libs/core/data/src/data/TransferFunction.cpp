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

#include "data/TransferFunction.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/Signal.hxx>
#include <core/tools/Type.hpp>

fwDataRegisterMacro( sight::data::TransferFunction )

namespace sight::data
{

//------------------------------------------------------------------------------

const std::string TransferFunction::s_DEFAULT_TF_NAME = "CT-GreyLevel";

const core::com::Signals::SignalKeyType TransferFunction::s_POINTS_MODIFIED_SIG    = "pointsModified";
const core::com::Signals::SignalKeyType TransferFunction::s_WINDOWING_MODIFIED_SIG = "windowingModified";

//------------------------------------------------------------------------------

data::TransferFunction::sptr TransferFunction::createDefaultTF()
{
    TransferFunction::sptr tf = TransferFunction::New();

    tf->setName(TransferFunction::s_DEFAULT_TF_NAME);
    tf->addTFColor(0.0, TFColor());
    tf->addTFColor(1.0, TFColor(1.0, 1.0, 1.0, 1.0));
    tf->setIsClamped(false);
    tf->setWindow( 500. );
    tf->setLevel( 50. );
    return tf;
}

//------------------------------------------------------------------------------

TransferFunction::TransferFunction(data::Object::Key)
{
    newSignal< PointsModifiedSignalType >(s_POINTS_MODIFIED_SIG);
    newSignal< WindowingModifiedSignalType >(s_WINDOWING_MODIFIED_SIG);

    this->initTF();
}

//------------------------------------------------------------------------------

TransferFunction::~TransferFunction()
{
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

void TransferFunction::shallowCopy(const Object::csptr& _source)
{
    TransferFunction::csptr other = TransferFunction::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
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

void TransferFunction::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache)
{
    TransferFunction::csptr other = TransferFunction::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, _cache );
    this->m_level             = other->m_level;
    this->m_window            = other->m_window;
    this->m_name              = other->m_name;
    this->m_backgroundColor   = other->m_backgroundColor;
    this->m_tfData            = other->m_tfData;
    this->m_interpolationMode = other->m_interpolationMode;
    this->m_isClamped         = other->m_isClamped;
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

TransferFunction::TFValuePairType TransferFunction::getMinMaxTFValues() const
{
    SLM_ASSERT("It must have at least one value.", m_tfData.size() >= 1);
    TFValuePairType minMax;
    minMax.first  = m_tfData.begin()->first;
    minMax.second = (m_tfData.rbegin())->first;
    return minMax;
}

//------------------------------------------------------------------------------

TransferFunction::TFValuePairType TransferFunction::getWLMinMax() const
{
    TFValuePairType minMax;
    const double halfWindow = m_window/2.;
    minMax.first  = m_level - halfWindow;
    minMax.second = m_level + halfWindow;
    return minMax;
}

//------------------------------------------------------------------------------

void TransferFunction::setWLMinMax(const TFValuePairType& _minMax)
{
    m_window = _minMax.second - _minMax.first;
    const double halfWindow = m_window/2.;
    m_level = halfWindow + _minMax.first;
}

//------------------------------------------------------------------------------

TransferFunction::TFValueType TransferFunction::getNearestValue(TFValueType _value) const
{
    SLM_ASSERT("It must have at least one value.", m_tfData.size() >= 1);
    const std::pair<double, double> minMax = core::tools::Type::s_DOUBLE.minMax<double>();
    double previousValue                   = minMax.first;
    double nextValue                       = minMax.second;

    TFValueType val;
    for(const TFDataType::value_type& data : m_tfData)
    {
        if(_value < data.first )
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
        if((_value - previousValue) < (nextValue - _value))
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

TransferFunction::TFColorVectorType TransferFunction::getTFColors() const
{
    TFColorVectorType colors;
    std::transform( m_tfData.begin(), m_tfData.end(),
                    std::back_inserter(colors),
                    std::bind(&TFDataType::value_type::second, std::placeholders::_1) );
    return colors;
}

//------------------------------------------------------------------------------

TransferFunction::TFColor TransferFunction::getNearestColor(TFValueType _value) const
{
    SLM_ASSERT("It must have at least one value.", m_tfData.size() >= 1);

    const double min = std::numeric_limits<double>::min();
    const double max = std::numeric_limits<double>::max();

    double previousValue = min;
    double nextValue     = max;

    const TFColor blackColor(0.0, 0.0, 0.0, 0.0);
    TFColor color;
    TFColor previousColor = blackColor;
    TFColor nextColor     = blackColor;

    for(const TFDataType::value_type& data : m_tfData)
    {
        if(_value < data.first )
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
        if(m_isClamped && (_value != previousValue))
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
        if((_value - previousValue) < (nextValue - _value))
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

TransferFunction::TFColor TransferFunction::getLinearColor(TFValueType _value) const
{
    SLM_ASSERT("It must have at least one value.", m_tfData.size() >= 1);

    const double min     = std::numeric_limits<double>::min();
    const double max     = std::numeric_limits<double>::max();
    double previousValue = min;
    double nextValue     = max;

    const TFColor blackColor(0.0, 0.0, 0.0, 0.0);
    TFColor color;
    TFColor previousColor = blackColor;
    TFColor nextColor     = blackColor;

    for(const TFDataType::value_type& data : m_tfData)
    {
        if(_value < data.first )
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
        if(m_isClamped && (_value != previousValue))
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
        const double distanceToNextValue     = nextValue - _value;
        const double distanceToPreviousValue = _value - previousValue;
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

//-----------------------------------------------------------------------------

TransferFunction::TFColor TransferFunction::getInterpolatedColor(TFValueType value) const
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

//------------------------------------------------------------------------------

const TransferFunction::TFColor& TransferFunction::getTFColor(TFValueType _value) const
{
    TFDataType::const_iterator itr = m_tfData.find(_value);
    SLM_ASSERT("The value "<< _value <<" is not defined in the transfer function.", itr != m_tfData.end());
    return itr->second;
}

//------------------------------------------------------------------------------

} // end namespace sight::data
