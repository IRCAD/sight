/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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
	// insert ONLY ONCE child in breathFirst order+source
	::fwTools::Object::ChildContainer::iterator f;
	for (f = fields.begin(); f != fields.end() ; ++f)
	{
		assert( ::fwTools::Object::dynamicCast( *f ) );
		std::pair< Source , Child > key( src, ::fwTools::Object::dynamicCast( *f ) );
		if ( std::find( m_fifo.begin(), m_fifo.end(),  key )==m_fifo.end()  )
		{
			// not already inserted
			m_fifo.push_back(key );
		}
	}

	// process list
	while ( !m_fifo.empty() )
	{
		m_source = m_fifo.front().first;
		Child achild= m_fifo.front().second;
		m_fifo.pop_front();
		accept( achild, this );
	}
}

} // namespace visitor

} // namespace fwData
