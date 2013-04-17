/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/Port.hpp"

#include "fwData/Node.hpp"

fwDataRegisterMacro( ::fwData::Node );

namespace fwData
{
//------------------------------------------------------------------------------

Node::Node(::fwData::Object::Key key)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Node::~Node()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Node::addInputPort(::fwData::Port::sptr port)
{
    m_inputs.push_back(port);
}

//------------------------------------------------------------------------------

void Node::addOutputPort(::fwData::Port::sptr port)
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

void Node::setObject( ::fwData::Object::sptr object )
{
    m_object = object;
}

//------------------------------------------------------------------------------

::fwData::Object::sptr Node::getObject() const
{
    return m_object;
}

//------------------------------------------------------------------------------

Port::sptr Node::findPort(const std::string &identifier, /*const std::string &type,*/ bool modeInput) const
{
    if ( modeInput)
    {
        for ( PortContainer::const_iterator i = m_inputs.begin() ; i != m_inputs.end() ; ++i )
        {
            if ( (*i)->getIdentifier() == identifier)
            {
                return *i;
            }
        }
    }
    else
    {
        for ( PortContainer::const_iterator i = m_outputs.begin() ; i != m_outputs.end() ; ++i )
        {
            if ( (*i)->getIdentifier() == identifier)
            {
                return *i;
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
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_inputs.clear();
    m_outputs.clear();

    if( other->getObject())
    {
        ::fwTools::Object::sptr object = ::fwData::factory::New( other->getObject()->getClassname() );
        OSLM_ASSERT("Sorry, instantiate "<<other->getObject()->getClassname()<< " failed", object );
        m_object = ::fwData::Object::dynamicCast(object);
        m_object->shallowCopy( other->m_object );
    }
    BOOST_FOREACH(::fwData::Port::sptr port, other->m_inputs)
    {
        this->addInputPort( ::fwData::Object::copy(port) );
    }
    BOOST_FOREACH(::fwData::Port::sptr port, other->m_outputs)
    {
        this->addOutputPort( ::fwData::Object::copy(port) );
    }
}

//------------------------------------------------------------------------------

void Node::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Node::csptr other = Node::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_inputs.clear();
    m_outputs.clear();

    m_object = ::fwData::Object::copy( other->m_object, cache );

    BOOST_FOREACH(const ::fwData::Port::sptr &port, other->m_inputs)
    {
        ::fwData::Port::sptr newPort;
        newPort = ::fwData::Object::copy(port, cache);
        this->addInputPort(newPort);
    }
    BOOST_FOREACH(const ::fwData::Port::sptr &port, other->m_outputs)
    {
        ::fwData::Port::sptr newPort;
        newPort = ::fwData::Object::copy(port, cache);
        this->addOutputPort(newPort);
    }
}

//------------------------------------------------------------------------------

} // namespace fwData
