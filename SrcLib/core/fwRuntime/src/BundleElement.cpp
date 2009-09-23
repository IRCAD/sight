/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/BundleElement.hpp"

#include <cassert>

#include "fwRuntime/Bundle.hpp"



namespace fwRuntime
{



BundleElement::BundleElement()
:	m_bundle( Bundle::getLoadingBundle() ), m_enable(true)
{
	// Post-condition
	assert( m_bundle != 0 );
}



BundleElement::BundleElement( ::boost::shared_ptr< Bundle > bundle )
:	m_bundle( bundle ), m_enable(true)
{
	// Post-condition
	assert( m_bundle != 0 );
}



::boost::shared_ptr<Bundle> BundleElement::getBundle() const
{
	return m_bundle;
}



const bool BundleElement::isEnable() const
{
	// Pre-condition.
	assert( m_bundle != 0 );

	return m_bundle->isEnable() && m_enable;
}



void BundleElement::setEnable(const bool enable)
{
	m_enable = enable;
}



} // namespace fwRuntime
