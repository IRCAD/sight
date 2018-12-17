/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWTOOLS_BOOKMARKS_HPP__
#define __FWTOOLS_BOOKMARKS_HPP__

#include "fwTools/config.hpp"

#include <fwCore/base.hpp>

#include <list>
#include <string>
#include <unordered_map>

namespace fwTools
{

class Object;
/**
 * @brief   Allows to add a bookmark to an object, the bookmark name is unique (in time), an object can have several
 * bookmarks
 * @note    expired object do not erase its bookmarks
 **/
class FWTOOLS_CLASS_API Bookmarks
{
public:

    FWTOOLS_API Bookmarks();

    typedef std::string BookmarkName;

    /**
     * @brief Tests if the given id exist (i.e recorded in Bookmarks dictionary)
     * @param[in] _id : the id to test
     * @return true iff the given id is recorded in Bookmarks dictionary
     */
    FWTOOLS_API static bool exist( BookmarkName _bookmark );

    FWTOOLS_API static void add( BookmarkName _bookmark,  SPTR(::fwTools::Object ) ); // cannot set a empty one

    /// silently remove given bookmark
    FWTOOLS_API static void remove( Bookmarks::BookmarkName _bookmark  );

    /**
     * @brief Retrieves the object attached to the given id. Return a null sptr if no correspondence exist
     */
    FWTOOLS_API static SPTR(::fwTools::Object ) getObject( BookmarkName _bookmark );

    /**
     * @brief Retrieves the list of bookmarks attach to an object
     */
    FWTOOLS_API static std::list<BookmarkName> getBookmarks(  SPTR(::fwTools::Object ) );

    /**
     * @brief Retrieves the list of bookmarks
     */
    FWTOOLS_API static std::list<BookmarkName> getBookmarks();

    FWTOOLS_API virtual ~Bookmarks();
protected:

    typedef ::std::unordered_map< BookmarkName, WPTR(::fwTools::Object ) > Dictionary;

    static Dictionary m_dictionary;
};

}

#endif /* __FWTOOLS_BOOKMARKS_HPP__ */
