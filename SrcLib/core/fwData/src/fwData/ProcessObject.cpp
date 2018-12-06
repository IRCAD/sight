/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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


#include "fwData/Exception.hpp"
#include "fwData/ProcessObject.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCore/base.hpp>

#include <functional>

fwDataRegisterMacro( ::fwData::ProcessObject );

namespace fwData
{

//------------------------------------------------------------------------------

ProcessObject::ProcessObject(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

ProcessObject::~ProcessObject()
{
}

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
    return this->getValue(name, m_inputs);
}

//------------------------------------------------------------------------------

::fwData::Object::sptr ProcessObject::getOutput(const ParamNameType& name)
{
    return this->getValue(name, m_outputs);
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
    this->setValue(name, object, m_inputs);
}

//------------------------------------------------------------------------------

void ProcessObject::setOutputValue(const ParamNameType& name, ::fwData::Object::sptr object)
{
    this->setValue(name, object, m_outputs);
}

//------------------------------------------------------------------------------

ProcessObject::ParamNameVectorType ProcessObject::getParamNames(const ProcessObjectMapType& params) const
{
    ParamNameVectorType names;
    std::transform( params.begin(), params.end(),
                    std::back_inserter(names),
                    std::bind(&ProcessObjectMapType::value_type::first, std::placeholders::_1) );
    return names;
}

//------------------------------------------------------------------------------

ProcessObject::ParamNameVectorType ProcessObject::getInputsParamNames() const
{
    return this->getParamNames(m_inputs);
}

//------------------------------------------------------------------------------

ProcessObject::ParamNameVectorType ProcessObject::getOutputsParamNames() const
{
    return this->getParamNames(m_outputs);
}

//------------------------------------------------------------------------------

void ProcessObject::clearInputs()
{
    this->clearParams(m_outputs);
}

//------------------------------------------------------------------------------

void ProcessObject::clearOutputs()
{
    this->clearParams(m_inputs);
}

//------------------------------------------------------------------------------

void ProcessObject::clearParams(ProcessObjectMapType& params)
{
    params.clear();
}

//-----------------------------------------------------------------------------

void ProcessObject::shallowCopy(const Object::csptr &source )
{
    ProcessObject::csptr other = ProcessObject::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( source );

    m_inputs  = other->m_inputs;
    m_outputs = other->m_outputs;
}

//-----------------------------------------------------------------------------

void ProcessObject::cachedDeepCopy(const Object::csptr &source, DeepCopyCacheType &cache)
{
    ProcessObject::csptr other = ProcessObject::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );

    this->clearInputs();
    this->clearOutputs();

    for(const ProcessObjectMapType::value_type& elt : other->m_inputs)
    {
        m_inputs[elt.first] = ::fwData::Object::copy(elt.second, cache);
    }

    for(const ProcessObjectMapType::value_type& elt : other->m_outputs)
    {
        m_outputs[elt.first] = ::fwData::Object::copy(elt.second, cache);
    }
}

//------------------------------------------------------------------------------

} // namespace fwData

