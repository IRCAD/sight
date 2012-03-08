/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>
#include <boost/bind.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Type.hpp>

#include "fwData/registry/macros.hpp"
#include <fwData/Factory.hpp>

#include "fwData/Color.hpp"
#include "fwData/TransfertFunctionPoint.hpp"
#include "fwData/TransfertFunction_VERSION_II.hpp"

REGISTER_DATA( ::fwData::TransfertFunction_VERSION_II );

namespace fwData
{

//------------------------------------------------------------------------------

const std::string TransfertFunction_VERSION_II::s_DEFAULT_TF_NAME = "CT-GreyLevel";

//------------------------------------------------------------------------------

TransfertFunction_VERSION_II::TransfertFunction_VERSION_II()
{
    SLM_TRACE_FUNC();
    this->initTF();
}

//------------------------------------------------------------------------------

void TransfertFunction_VERSION_II::initTF()
{

    m_level = 0;
    m_window = 100;

    m_refLevel = 0.5;
    m_refWindow = 1;

    m_name = "";

    m_interpolationMode = TransfertFunction_VERSION_II::LINEAR;
    m_isClamped = true;

    m_backgroundColor = TFColor();

    m_tfData.clear();

}

//------------------------------------------------------------------------------

fwData::TransfertFunction_VERSION_II::sptr TransfertFunction_VERSION_II::createDefaultTF()
{
    TransfertFunction_VERSION_II::NewSptr tf;

    tf->setName(TransfertFunction_VERSION_II::s_DEFAULT_TF_NAME);
    tf->addTFColor(0.0, TFColor());
    tf->addTFColor(1.0, TFColor(1.0,1.0,1.0,1.0));
    return tf;
}

//------------------------------------------------------------------------------

TransfertFunction_VERSION_II::~TransfertFunction_VERSION_II ()
{}

//------------------------------------------------------------------------------

TransfertFunction_VERSION_II::TFValueVectorType TransfertFunction_VERSION_II::getTFValues() const
{
    TFValueVectorType values;
    std::transform( m_tfData.begin(), m_tfData.end(),
            std::back_inserter(values),
            ::boost::bind(& TFDataType::value_type::first, _1) );
    return values;

}

//------------------------------------------------------------------------------

TransfertFunction_VERSION_II::TFValueType TransfertFunction_VERSION_II::getNearestValue( TFValueType value ) const
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

const TransfertFunction_VERSION_II::TFDataType& TransfertFunction_VERSION_II::getTFData() const
{
    return m_tfData;
}

//------------------------------------------------------------------------------

void TransfertFunction_VERSION_II::addTFColor( TFValueType value, const TFColor & color )
{
    m_tfData[value] = color;
}

//------------------------------------------------------------------------------
void TransfertFunction_VERSION_II::eraseTFValue( TFValueType value)
{
    m_tfData.erase(value);
}

//------------------------------------------------------------------------------
void TransfertFunction_VERSION_II::clear()
{
    m_tfData.clear();
}
//------------------------------------------------------------------------------

TransfertFunction_VERSION_II::TFColorVectorType TransfertFunction_VERSION_II::getTFColors() const
{
    TFColorVectorType colors;
    std::transform( m_tfData.begin(), m_tfData.end(),
            std::back_inserter(colors),
            ::boost::bind(& TFDataType::value_type::second, _1) );
    return colors;
}

//------------------------------------------------------------------------------
TransfertFunction_VERSION_II::TFColor TransfertFunction_VERSION_II::getNearestColor( const TFColor& _color) const
{
    TFColor color;

    return color;
}

//------------------------------------------------------------------------------

TransfertFunction_VERSION_II::TFColor TransfertFunction_VERSION_II::getLinearColor( const TFColor& _color ) const
{
    TFColor color;

    return color;
}

//------------------------------------------------------------------------------

TransfertFunction_VERSION_II::TFColor TransfertFunction_VERSION_II::getInterpolatedColor( TFValueType value ) const
{
    TFColor color;

    return color;
}

//------------------------------------------------------------------------------

const TransfertFunction_VERSION_II::TFColor& TransfertFunction_VERSION_II::getTFColor( TFValueType value ) const
{
    TFDataType::const_iterator itr = m_tfData.find(value);
    SLM_ASSERT("Sorry not defined in th TF.", itr != m_tfData.end());
    return itr->second;
}
//------------------------------------------------------------------------------

void TransfertFunction_VERSION_II::shallowCopy( TransfertFunction_VERSION_II::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    this->m_level = _source->m_level;
    this->m_window = _source->m_window;
    this->m_refLevel = _source->m_refLevel;
    this->m_refWindow = _source->m_refWindow;
    this->m_name =_source->m_name;
    this->m_backgroundColor = _source->m_backgroundColor;
    this->m_tfData = _source->m_tfData;
    this->m_interpolationMode = _source->m_interpolationMode;
    this->m_isClamped = _source->m_isClamped;
}

//------------------------------------------------------------------------------

void TransfertFunction_VERSION_II::deepCopy( TransfertFunction_VERSION_II::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
    this->m_level = _source->m_level;
    this->m_window = _source->m_window;
    this->m_refLevel = _source->m_refLevel;
    this->m_refWindow = _source->m_refWindow;
    this->m_name =_source->m_name;
    this->m_backgroundColor = _source->m_backgroundColor;
    this->m_tfData = _source->m_tfData;
    this->m_interpolationMode = _source->m_interpolationMode;
    this->m_isClamped = _source->m_isClamped;
}

//------------------------------------------------------------------------------

} // end namespace fwData
