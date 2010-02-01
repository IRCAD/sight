/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/ConfigurationElementContainer.hpp"

#include <algorithm>

#include "fwRuntime/ConfigurationElement.hpp"


namespace fwRuntime
{

namespace
{

	/**
	 * @brief	Implements a STL compliant predicate that test if a configuration
	 * 			element has the given name.
	 *
	 * @author	IRCAD (Research and Development Team).
	 */
	struct HasName
	{
		HasName( const std::string & name )
		:	m_name( name )
		{}

		const bool operator() ( const ::boost::shared_ptr< ConfigurationElement > element ) const
		{
			return element->getName() == m_name;
		}

	private:

		std::string	m_name;
	};

}


void ConfigurationElementContainer::addConfigurationElement( ::boost::shared_ptr< ConfigurationElement > element )
{
	m_elements.push_back( element );
}



ConfigurationElementContainer::Iterator ConfigurationElementContainer::begin()
{
	return m_elements.begin();
}



ConfigurationElementContainer::Iterator ConfigurationElementContainer::end()
{
	return m_elements.end();
}



const ::boost::shared_ptr<ConfigurationElement> ConfigurationElementContainer::findConfigurationElement( const std::string & name ) const
{
	Container::const_iterator	found = std::find_if( m_elements.begin(), m_elements.end(), HasName(name) );

	return ( found == m_elements.end() ) ? ::boost::shared_ptr< ConfigurationElement >() : *found;
}

//-----------------------------------------------------------------------------

ConfigurationElementContainer ConfigurationElementContainer::findAllConfigurationElement( const std::string & _name ) const
{
	ConfigurationElementContainer container;

	for( 	Container::const_iterator itCfgElem = m_elements.begin();
			itCfgElem != m_elements.begin();
			++itCfgElem )
	{
		if ( (*itCfgElem)->getName() == _name )
		{
			container.addConfigurationElement( (*itCfgElem) );
		}
	}

	return container;
}

//-----------------------------------------------------------------------------

unsigned int ConfigurationElementContainer::size() const
{
	return m_elements.size();
}

//-----------------------------------------------------------------------------

} // namespace fwRuntime
