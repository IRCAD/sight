/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"

#include "fwData/ProcessObject.hpp"

fwDataRegisterMacro( ::fwData::ProcessObject );

namespace fwData
{

//------------------------------------------------------------------------------

ProcessObject::ProcessObject()
{}

//------------------------------------------------------------------------------

ProcessObject::~ProcessObject()
{}

//------------------------------------------------------------------------------

::fwData::Object::sptr ProcessObject::getValue(const ParamNameType& name, const ProcessObjectMapType& params)
{
    ::fwData::Object::sptr object;
    ProcessObjectMapType::const_iterator iter = params.find(name);
    if(iter != params.end())
    {
        object = iter->second;
    }
    return object;
}

//------------------------------------------------------------------------------

::fwData::Object::sptr ProcessObject::getInput(const ParamNameType& name)
{
    return this->getValue(name, m_attrInputs);
}

//------------------------------------------------------------------------------

::fwData::Object::sptr ProcessObject::getOutput(const ParamNameType& name)
{
    return this->getValue(name, m_attrOutputs);
}

//------------------------------------------------------------------------------

void ProcessObject::setValue(const ParamNameType& name, ::fwData::Object::sptr object, ProcessObjectMapType& params)
{
    std::pair<ProcessObjectMapType::iterator, bool> res;
    res = params.insert(ProcessObjectMapType::value_type(name, object));
    if( !res.second )
    {
        res.first->second = object;
    }
}

//------------------------------------------------------------------------------

void ProcessObject::setInputValue(const ParamNameType& name, ::fwData::Object::sptr object)
{
    this->setValue(name, object, m_attrInputs);
}

//------------------------------------------------------------------------------

void ProcessObject::setOutputValue(const ParamNameType& name, ::fwData::Object::sptr object)
{
    this->setValue(name, object, m_attrOutputs);
}

//------------------------------------------------------------------------------

ProcessObject::ParamNameVectorType ProcessObject::getParamNames(const ProcessObjectMapType& params) const
{
    ParamNameVectorType names;
    std::transform( params.begin(), params.end(),
            std::back_inserter(names),
            ::boost::bind(& ProcessObjectMapType::value_type::first, _1) );
    return names;
}

//------------------------------------------------------------------------------

ProcessObject::ParamNameVectorType ProcessObject::getInputsParamNames() const
{
    return this->getParamNames(m_attrInputs);
}

//------------------------------------------------------------------------------

ProcessObject::ParamNameVectorType ProcessObject::getOutputsParamNames() const
{
    return this->getParamNames(m_attrOutputs);
}

//------------------------------------------------------------------------------

void ProcessObject::clearInputs()
{
    this->clearParams(m_attrOutputs);
}

//------------------------------------------------------------------------------

void ProcessObject::clearOutputs()
{
    this->clearParams(m_attrInputs);
}

//------------------------------------------------------------------------------

void ProcessObject::clearParams(ProcessObjectMapType& params)
{
    params.clear();
}

//-----------------------------------------------------------------------------

void ProcessObject::shallowCopy( ::fwData::ProcessObject::csptr source )
{
    this->fieldShallowCopy( source );

    m_attrInputs = source->m_attrInputs;
    m_attrOutputs = source->m_attrOutputs;
}

//-----------------------------------------------------------------------------

void ProcessObject::deepCopy( ::fwData::ProcessObject::csptr source )
{
    this->fieldDeepCopy( source );

    this->clearInputs();
    this->clearOutputs();

    BOOST_FOREACH(ProcessObjectMapType::value_type elt, source->m_attrInputs)
    {
        m_attrInputs[elt.first] = ::fwData::Object::copy(elt.second);
    }

    BOOST_FOREACH(ProcessObjectMapType::value_type elt, source->m_attrOutputs)
    {
        m_attrOutputs[elt.first] = ::fwData::Object::copy(elt.second);
    }
}

//------------------------------------------------------------------------------

} // namespace fwData

