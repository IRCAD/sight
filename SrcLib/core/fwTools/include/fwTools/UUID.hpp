/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef UUID_HPP_
#define UUID_HPP_

#include <string>
#include <map>

#include <stdexcept>

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits.hpp>

#include <fwCore/base.hpp>
#include "fwTools/config.hpp"
#include "fwTools/TypeInfo.hpp"

namespace fwTools
{
/**
 * @brief   Management of UUID on objects.
 * @class   UUID.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 */
class FWTOOLS_CLASS_API UUID
{
public:


    /**
     * @brief   Return true iff the given uuid is used
     */
    FWTOOLS_API static bool exist( const std::string & uuid);

//  /// return an uuid to the given object : if no one perviously set then generate a new one
//  template<class PTR>
//  std::string get(PTR *ptr);

    /**
     * @brief   Return an uuid to the given object : if no one perviously set then generate a new one (shared_ptr version)
     */
    template<class T>
    static std::string get(boost::shared_ptr<T> sptr);

    /**
     * @brief   Return an uuid to the given object : if no one perviously set then generate a new one (weak_ptr version)
     */
    template<class T>
    static std::string get( boost::weak_ptr<T> wptr );


    /**
     * @brief   Return a smart ptr on the object related to a given UUID : return null shared if not supervised
     */
    template<class T>
    static boost::shared_ptr<T> get( const std::string & uuid );


    /**
     * @brief   Return true iff the ptr avec already an uuid assigned
     */
    template<class T>
    static bool supervise(T *ptr);

    /**
     * @brief   Return true iff the weak ptr avec already an uuid assigned (weak_ptr version)
     */
    template<class T>
    static bool supervise(boost::weak_ptr<T> wptr);

    /**
     * @brief   Return true iff the shared ptr avec already an uuid assigned (shared_ptr version)
     */
    template<class T>
    static bool supervise(boost::shared_ptr<T> sptr);


    /**
     * @brief   Return a new extended UUID;
     */
    FWTOOLS_API static std::string generateUUID();


protected :




    /// Store association boost::weak_ptr adress <--> uuid as a string
    typedef std::string UUIDType;
    typedef std::map< boost::weak_ptr<void>, UUIDType  > UUIDContainer;

    FWTOOLS_API static UUIDContainer m_uuids;

    /**
     * @brief   Return the next non expired weak ptr; erase all expired discovered
     * @note    To also be called before a for loop to remobe first element ( if necessary )
     */
    FWTOOLS_API static void next( UUIDContainer::iterator &iter );

    /**
     * @brief   Return the first non expired element
     */
    FWTOOLS_API static UUIDContainer::iterator begin(UUIDContainer &uuidContainer);

    /**
     * @brief   Maintain lastest index available for a given typeinfo
     */
    FWTOOLS_API static std::map< TypeInfo, ::boost::uint32_t > m_CategorizedCounter;

    /**
     * @brief   Default constructor : does nothing.
     */
    FWTOOLS_API UUID();

    /**
     * @brief   Destructor : does nothing.
     */
    FWTOOLS_API virtual ~UUID();



    /**
     * @brief   Helper to find a weak_ptr in the m_uuids
     * @param   wp weeak_ptr to be find
     * @return  Iterator on the uuid associed to the weak_ptr
     */
    UUIDContainer::iterator find( boost::weak_ptr<void> wp );

};





}

#include "fwTools/UUID.hxx"


#endif /* UUID_HPP_ */
