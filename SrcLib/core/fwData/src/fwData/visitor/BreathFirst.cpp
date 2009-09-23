/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <list>
#include <algorithm>

#include "fwData/visitor/accept.hpp"
#include "fwData/visitor/BreathFirst.hpp"

namespace fwData
{
namespace visitor
{

void BreathFirst::next(::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &fields)
{
	// store fields+source : static variable !!
	typedef ::fwTools::Object::sptr  Source;
	typedef ::fwTools::Object::sptr  Child;

	static std::list< std::pair< Source ,Child > > fifo;

	// insert ONLY ONCE child in breathFirst order+source
	::fwTools::Object::ChildContainer::iterator f;
	for (f = fields.begin(); f != fields.end() ; ++f)
	{
		assert( ::fwTools::Object::dynamicCast( *f ) );
		std::pair< Source , Child > key( src, ::fwTools::Object::dynamicCast( *f ) );
		if ( std::find( fifo.begin(), fifo.end(),  key )==fifo.end()  )
		{
			// not already inserted
			fifo.push_back(key );
		}
	}

	// process list
	while ( !fifo.empty() )
	{
		m_source = fifo.front().first;
		Child achild= fifo.front().second;
		fifo.pop_front();
		accept( achild, this );
	}
}

} // namespace visitor

} // namespace fwData
