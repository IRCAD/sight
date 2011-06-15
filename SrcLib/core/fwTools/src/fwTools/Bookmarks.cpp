/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <assert.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <fwCore/Demangler.hpp>

#include "fwTools/Object.hpp"
#include "fwTools/Bookmarks.hpp"
#include "fwTools/Failed.hpp"

namespace fwTools
{
Bookmarks::Dictionary Bookmarks::m_dictionary;

//-----------------------------------------------------------------------------

Bookmarks::Bookmarks()
{}

//-----------------------------------------------------------------------------

Bookmarks::~Bookmarks()
{}

//-----------------------------------------------------------------------------

bool Bookmarks::exist( Bookmarks::BookmarkName _id)
{
    return m_dictionary.find( _id ) != m_dictionary.end();
}

//-----------------------------------------------------------------------------

void Bookmarks::add( Bookmarks::BookmarkName _bookmark,  ::fwTools::Object::sptr obj )
{
    OSLM_FATAL_IF("Try to set a  Bookmarks on a null object ", !obj );
    m_dictionary[ _bookmark ] = obj;
}

//-----------------------------------------------------------------------------

void Bookmarks::remove( Bookmarks::BookmarkName _bookmark  )
{
    m_dictionary.erase( m_dictionary.find( _bookmark ) );
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr Bookmarks::getObject( Bookmarks::BookmarkName _bookmark )
{
    ::fwTools::Object::sptr bookmark;
    Dictionary::iterator iter = m_dictionary.find( _bookmark );
    if ( iter != m_dictionary.end()  && !iter->second.expired() )
    {
        bookmark = iter->second.lock();
    }
    return  bookmark;
}

//-----------------------------------------------------------------------------

std::list<Bookmarks::BookmarkName> Bookmarks::getBookmarks( ::fwTools::Object::sptr obj )
{
    std::list<Bookmarks::BookmarkName> result;
    BOOST_FOREACH( Bookmarks::Dictionary::value_type elt, m_dictionary)
    {
        if ( !elt.second.expired() && elt.second.lock() == obj )
        {
            result.push_back(  elt.first );
        }
    }
    return  result;
}

//-----------------------------------------------------------------------------

std::list<Bookmarks::BookmarkName> Bookmarks::getBookmarks()
{
    std::list<Bookmarks::BookmarkName> result;
    BOOST_FOREACH( Bookmarks::Dictionary::value_type elt, m_dictionary)
    {
        result.push_back( elt.first );
    }
    return  result;
}

//-----------------------------------------------------------------------------

}
