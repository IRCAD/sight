/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/profile/Profile.hpp"

#include <algorithm>

#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/profile/Activater.hpp"
#include "fwRuntime/profile/Starter.hpp"



namespace fwRuntime
{

namespace profile
{

namespace
{

	template< typename E >
	struct Apply
	{
		void operator() ( E e )
		{
			e->apply();
		}
	};

}



Profile::Profile()
{}



void Profile::add( ::boost::shared_ptr< Activater > activater )
{
	m_activaters.push_back( activater );
}



void Profile::add( ::boost::shared_ptr< Starter > starter )
{
	m_starters.push_back( starter );
}



void Profile::start()
{
	std::for_each( m_activaters.begin(), m_activaters.end(), Apply< ActivaterContainer::value_type >() );
	std::for_each( m_starters.begin(), m_starters.end(), Apply< StarterContainer::value_type >() );
}



} // namespace profile

} // namespace fwRuntime
