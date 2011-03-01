/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_FWID_HPP_
#define _FWTOOLS_FWID_HPP_

#include <string>

#include <boost/unordered_map.hpp>

#include <fwCore/base.hpp>


#include "fwTools/config.hpp"
#include "fwTools/macros.hpp"

namespace fwTools
{

class Object;
/**
 * @brief   Defines the abstract interface for deletion.
 * @class   IDeleter
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @note    To be specialized to extend object destruction procedure.
 * @note    Typical use: for service unregistration while keeping data independant with respect to fwServices library and keeping the benefits of smart pointers use.
 */
class FWTOOLS_CLASS_API fwID
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((fwID), (()), new fwID );

    typedef std::string IDType;


    typedef enum {
        EMPTY    = 1, // return a empty id if no one set
        GENERATE    , // generate a new id if necessary
        MUST_EXIST    // throw an exception if object not have an id
    } Policy;

    /**
     * Test if the given id exist (i.e recorded in fwID dictionary)
     * @param[in] _id : the id to test
     * @return true iff the given id is recorded in fwID dictionary
     */
    FWTOOLS_API static bool exist( IDType _id);

    /**
     * @brief retrieve the object attached to the given id. Return a null sptr if no correspondence exist
     */
    FWTOOLS_API static SPTR(::fwTools::Object ) getObject( IDType requestID );

    FWTOOLS_API virtual ~fwID() ;

protected :

    // API to expose in fwToolsObject
    /// return true if object have an id set
    FWTOOLS_API bool   hasID() const;
    /**
     * @brief retun the id of the object. If no set and the policy value =
     * \li EMPTY then a empty id is returned
     * \li GENERATE (default) a new ID will be generated (and recorded ) using the pattern "CLASSNAME-NUM". NUM is always increasing
     * \li MUST_EXIST  an exception Failed is raised
     * @note we consider a object be constant whatever if its id is generated
     */
    FWTOOLS_API IDType getID( Policy  policy=GENERATE ) const;
    /// set a newID  for the object, (newID must not exist in fwID), the oldest one is released
    FWTOOLS_API void   setID( IDType newID ); // cannot set a empty one

    /** @brief swap the id of the 2 object, if a object do not have a id then getID(...,GENERATE) will be used **/
    FWTOOLS_API void swapID( SPTR(::fwTools::Object )   );

    /// release the id for the object
    FWTOOLS_API void   resetID();

     /**
      * @brief   Constructor : does nothing.
      */
    FWTOOLS_API fwID() {}; // cannot be instantiated

    /// will generate a new ID using the pattern "CLASSNAME-NUM". NUM is always increasing
    IDType generate() const;


   IDType m_id;

   typedef ::boost::unordered_map< IDType, WPTR(::fwTools::Object ) > Dictionary;
   typedef ::boost::unordered_map< std::string , ::boost::uint32_t >  CategorizedCounter;

   static  Dictionary m_dictionary;
   static  CategorizedCounter m_CategorizedCounter;
};

}


#endif /* FWID_HPP_ */
