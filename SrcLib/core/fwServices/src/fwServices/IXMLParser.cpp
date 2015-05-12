/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IXMLParser.hpp"



namespace fwServices
{

IXMLParser::IXMLParser( )
{
}

IXMLParser::~IXMLParser()
{
}

void IXMLParser::starting() throw(::fwTools::Failed)
{
}

void IXMLParser::stopping() throw(::fwTools::Failed)
{
}

void IXMLParser::configuring() throw(::fwTools::Failed)
{
}

void IXMLParser::receiving( fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

void IXMLParser::updating() throw(::fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::Iterator configEltIter;
    for(configEltIter = this->m_configuration->begin(); !(configEltIter == this->m_configuration->end());
        ++configEltIter)
    {
        SLM_ASSERT("ACH => still used ?", (*configEltIter)->getName() != "object" );
    }
}


//-----------------------------------------------------------------------------

void IXMLParser::setObjectConfig( ::fwRuntime::ConfigurationElement::csptr _cfgElem )
{
    m_cfg = _cfgElem;
}

//-----------------------------------------------------------------------------

void IXMLParser::createConfig( ::fwTools::Object::sptr _obj )
{
}

//-----------------------------------------------------------------------------

void IXMLParser::startConfig()
{
}

//-----------------------------------------------------------------------------

void IXMLParser::updateConfig()
{
}

//-----------------------------------------------------------------------------

void IXMLParser::stopConfig()
{
}

//-----------------------------------------------------------------------------

void IXMLParser::destroyConfig()
{
}

//-----------------------------------------------------------------------------

}

