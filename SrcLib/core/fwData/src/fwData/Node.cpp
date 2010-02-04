/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Port.hpp"

#include "fwData/Node.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Node, ::fwData::Node );
namespace fwData {
//------------------------------------------------------------------------------

Node::Node() {
	// TODO Auto-generated constructor stub

}

//------------------------------------------------------------------------------

Node::~Node() {
	// TODO Auto-generated destructor stub
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

Port::sptr Node::findPort(const std::string &identifier, /*const std::string &type,*/ bool modeInput)
{
	if ( modeInput)
	{
		for ( PortContainer::iterator i = m_inputs.begin() ; i != m_inputs.end() ; ++i )
		{
			if ( (*i)->getIdentifier() == identifier)
			{
				return *i;
			}
		}
	}
	else
	{
		for ( PortContainer::iterator i = m_outputs.begin() ; i != m_outputs.end() ; ++i )
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

} // namespace fwData
