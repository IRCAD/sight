/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/visitor/accept.hpp"

#include "fwData/visitor/DepthFirst.hpp"

namespace fwData
{
namespace visitor
{

void DepthFirst::next( ::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &fields)
{
    ::fwTools::Object::ChildContainer::iterator f;
    for ( f = fields.begin();   f != fields.end() ; ++f )
    {
        assert( ::fwTools::Object::dynamicCast( *f ) ) ;
        m_source = src;
        accept( ::fwTools::Object::dynamicCast( *f ) , this );
    }
}

} // namespace visitor

} // namespace fwData
