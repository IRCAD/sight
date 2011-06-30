/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_BOOKMARKS_HPP_
#define _FWTOOLS_BOOKMARKS_HPP_

#include <string>
#include <list>

#include <boost/unordered_map.hpp>

#include <fwCore/base.hpp>


#include "fwTools/config.hpp"
#include "fwTools/macros.hpp"

namespace fwTools
{

class Object;
/**
 * @brief   Allow to add a bookmark to an object, the bookmark name is unique (in time), an object can have several bookmarks
 * @class   Bookmarks
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2011.
 * @note    expired object do not erase its bookmarks
 **/
class FWTOOLS_CLASS_API Bookmarks
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((Bookmarks), (()), new Bookmarks );

    typedef std::string BookmarkName;

    /**
     * Test if the given id exist (i.e recorded in Bookmarks dictionary)
     * @param[in] _id : the id to test
     * @return true iff the given id is recorded in Bookmarks dictionary
     */
    FWTOOLS_API static bool exist( BookmarkName _bookmark );

    FWTOOLS_API static void add( BookmarkName _bookmark,  SPTR(::fwTools::Object ) ); // cannot set a empty one

    /// silently remove given bookmark
    FWTOOLS_API static void remove( Bookmarks::BookmarkName _bookmark  );

    /**
     * @brief retrieve the object attached to the given id. Return a null sptr if no correspondence exist
     */
    FWTOOLS_API static SPTR(::fwTools::Object ) getObject( BookmarkName _bookmark );


    /**
     * @brief retrieve the list of bookmarks attach to an object
     */
    FWTOOLS_API static std::list<BookmarkName> getBookmarks(  SPTR(::fwTools::Object ) );

    /**
     * @brief retrieve the list of bookmarks
     */
    FWTOOLS_API static std::list<Bookmarks::BookmarkName> getBookmarks();

protected :

    FWTOOLS_API Bookmarks();

    FWTOOLS_API virtual ~Bookmarks() ;

   typedef ::boost::unordered_map< BookmarkName, WPTR(::fwTools::Object ) > Dictionary;

   static  Dictionary m_dictionary;
};

}


#endif /* BOOKMARKS_HPP_ */
