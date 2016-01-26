/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

//FIXME :This needs to be include first for GCC
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
//
#include "fwData/Node.hpp"
#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Port.hpp"

fwDataRegisterMacro( ::fwData::Node );

namespace fwData
{

const ::fwCom::Signals::SignalKeyType Node::s_UPDATED_SIG = "updated";

//------------------------------------------------------------------------------

Node::Node(::fwData::Object::Key key) :
    m_sigUpdated(UpdatedSignalType::New())
{
    m_signals( s_UPDATED_SIG,  m_sigUpdated);


}

//------------------------------------------------------------------------------

Node::~Node()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Node::addInputPort(const ::fwData::Port::sptr& port)
{
    m_inputs.push_back(port);
}

//------------------------------------------------------------------------------

void Node::addOutputPort(const ::fwData::Port::sptr& port)
{
    m_outputs.push_back(port);
}

//------------------------------------------------------------------------------

Node::PortContainer & Node::getInputPorts()
{
    return m_inputs;
}

//------------------------------------------------------------------------------

Node::PortContainer & Node::getOutputPorts()
{
    return m_outputs;
}

//------------------------------------------------------------------------------

void Node::setObject(const ::fwData::Object::sptr& object )
{
    m_object = object;
}

//------------------------------------------------------------------------------

::fwData::Object::sptr Node::getObject() const
{
    return m_object;
}

//------------------------------------------------------------------------------

Port::sptr Node::findPort(const std::string &identifier, bool modeInput) const
{
    if ( modeInput)
    {
        for ( const auto& input : m_inputs)
        {
            if ( input->getIdentifier() == identifier)
            {
                return input;
            }
        }
    }
    else
    {
        for ( const auto& output : m_outputs )
        {
            if ( output->getIdentifier() == identifier)
            {
                return output;
            }
        }
    }
    return Port::sptr();
}

//------------------------------------------------------------------------------

void Node::shallowCopy(const Object::csptr &_source )
{
    Node::csptr other = Node::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !other );
    this->fieldShallowCopy( _source );

    m_inputs.clear();
    m_outputs.clear();

    if( other->getObject())
    {
        ::fwTools::Object::sptr object = ::fwData::factory::New( other->getObject()->getClassname() );
        OSLM_ASSERT("The instantiation of '"<<other->getObject()->getClassname()<< "' failed", object );
        m_object = ::fwData::Object::dynamicCast(object);
        m_object->shallowCopy( other->m_object );
    }
    for(const ::fwData::Port::sptr& port : other->m_inputs)
    {
        this->addInputPort( ::fwData::Object::copy(port) );
    }
    for(const ::fwData::Port::sptr& port : other->m_outputs)
    {
        this->addOutputPort( ::fwData::Object::copy(port) );
    }
}

//------------------------------------------------------------------------------

void Node::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Node::csptr other = Node::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_inputs.clear();
    m_outputs.clear();

    m_object = ::fwData::Object::copy( other->m_object, cache );

    for(const ::fwData::Port::sptr &port : other->m_inputs)
    {
        ::fwData::Port::sptr newPort;
        newPort = ::fwData::Object::copy(port, cache);
        this->addInputPort(newPort);
    }
    for(const ::fwData::Port::sptr &port : other->m_outputs)
    {
        ::fwData::Port::sptr newPort;
        newPort = ::fwData::Object::copy(port, cache);
        this->addOutputPort(newPort);
    }
}

//------------------------------------------------------------------------------

} // namespace fwData
