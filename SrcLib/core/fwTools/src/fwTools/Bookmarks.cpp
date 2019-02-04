/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwTools/Bookmarks.hpp"

#include "fwTools/Object.hpp"

namespace fwTools
{
Bookmarks::Dictionary Bookmarks::m_dictionary;

//-----------------------------------------------------------------------------

Bookmarks::Bookmarks()
{
}

//-----------------------------------------------------------------------------

Bookmarks::~Bookmarks()
{
}

//-----------------------------------------------------------------------------

bool Bookmarks::exist( Bookmarks::BookmarkName _id)
{
    FW_DEPRECATED_MSG("This class is no longer supported", "20.0");

    return m_dictionary.find( _id ) != m_dictionary.end();
}

//-----------------------------------------------------------------------------

void Bookmarks::add( Bookmarks::BookmarkName _bookmark,  ::fwTools::Object::sptr obj )
{
    FW_DEPRECATED_MSG("This class is no longer supported", "20.0");

    OSLM_FATAL_IF("Try to set a  Bookmarks on a null object ", !obj );
    m_dictionary[ _bookmark ] = obj;
}

//-----------------------------------------------------------------------------

void Bookmarks::remove( Bookmarks::BookmarkName _bookmark  )
{
    FW_DEPRECATED_MSG("This class is no longer supported", "20.0");

    m_dictionary.erase( m_dictionary.find( _bookmark ) );
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr Bookmarks::getObject( Bookmarks::BookmarkName _bookmark )
{
    FW_DEPRECATED_MSG("This class is no longer supported", "20.0");

    ::fwTools::Object::sptr bookmark;
    Dictionary::iterator iter = m_dictionary.find( _bookmark );
    if ( iter != m_dictionary.end()  && !iter->second.expired() )
    {
        bookmark = iter->second.lock();
    }
    return bookmark;
}

//-----------------------------------------------------------------------------

std::list<Bookmarks::BookmarkName> Bookmarks::getBookmarks( ::fwTools::Object::sptr obj )
{
    FW_DEPRECATED_MSG("This class is no longer supported", "20.0");

    std::list<Bookmarks::BookmarkName> result;
    for( Bookmarks::Dictionary::value_type elt :  m_dictionary)
    {
        if ( !elt.second.expired() && elt.second.lock() == obj )
        {
            result.push_back(  elt.first );
        }
    }
    return result;
}

//-----------------------------------------------------------------------------

std::list<Bookmarks::BookmarkName> Bookmarks::getBookmarks()
{
    FW_DEPRECATED_MSG("This class is no longer supported", "20.0");

    std::list<Bookmarks::BookmarkName> result;
    for( Bookmarks::Dictionary::value_type elt :  m_dictionary)
    {
        result.push_back( elt.first );
    }
    return result;
}

//-----------------------------------------------------------------------------

}
