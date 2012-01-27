/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/EConfigurationElement.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/RuntimeException.hpp"


namespace fwRuntime
{

EConfigurationElement::EConfigurationElement(const std::string& name) :
::fwRuntime::ConfigurationElement( ::boost::shared_ptr< ::fwRuntime::Bundle >() , name )
{}

EConfigurationElement::~EConfigurationElement()
{}

EConfigurationElement::sptr EConfigurationElement::addConfigurationElement( const std::string& name )
{
    EConfigurationElement::sptr cfg = EConfigurationElement::New(name) ;
    this->::fwRuntime::ConfigurationElement::addConfigurationElement( cfg );
    return cfg ;
}

void EConfigurationElement::addConfigurationElement( EConfigurationElement::sptr element )
{
    this->::fwRuntime::ConfigurationElement::addConfigurationElement( element );
}

void EConfigurationElement::setAttributeValue(const std::string& name, const std::string& value) throw()
{
    this->::fwRuntime::ConfigurationElement::setAttributeValue( name , value ) ;
}

void EConfigurationElement::setValue(const std::string& value) throw()
{
    this->::fwRuntime::ConfigurationElement::setValue(value);
}

}
