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
 * @brief	Management of UUID on objects.
 * @class	UUID.
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 *
 */
class FWTOOLS_CLASS_API UUID
{
public:

	/// @brief 	uuid version
	typedef enum  { SIMPLE,				//!<simple version
					RFC4122, 			//!< random based ( version 4 )
					EXTENDED = RFC4122 	//!< an alias
	} Version;

	/**
	 * @brief	Return true iff the given uuid is used
	 */
	FWTOOLS_API static bool exist( const std::string & uuid,const int &version);

//	/// return an uuid to the given object : if no one perviously set then generate a new one
//	template<class PTR>
//	std::string get(PTR *ptr);

	/**
	 * @brief	Return an uuid to the given object : if no one perviously set then generate a new one (shared_ptr version)
	 */
	template<class T>
	static std::string get(boost::shared_ptr<T> sptr,  const int &version = SIMPLE);

	/**
	 * @brief	Return an uuid to the given object : if no one perviously set then generate a new one (weak_ptr version)
	 */
	template<class T>
	static std::string get( boost::weak_ptr<T> wptr, const int &version = SIMPLE );


	/**
	 * @brief	Return a smart ptr on the object related to a given UUID : return null shared if not supervised
	 */
	template<class T>
	static boost::shared_ptr<T> get( const std::string & uuid,const int &version = SIMPLE);


	/**
	 * @brief 	Return true iff the ptr avec already an uuid assigned
	 */
	template<class T>
	static bool supervise(T *ptr);

	/**
	 * @brief 	Return true iff the weak ptr avec already an uuid assigned (weak_ptr version)
	 */
	template<class T>
	static bool supervise(boost::weak_ptr<T> wptr);

	/**
	 * @brief 	Return true iff the shared ptr avec already an uuid assigned (shared_ptr version)
	 */
	template<class T>
	static bool supervise(boost::shared_ptr<T> sptr);

	/**
	 * @brief 	Force  UUID for the given object an exception is throw iff UUID is already in use or object have already
	 * a setted one (shared_ptr version)
	 * @throw 	std::invalid_argument if UUID is already in use or object have already a setted one
	 **/
	template<class T>
	static void impose(boost::shared_ptr<T> sptr, const std::string &givenUUID,const int version) throw (std::invalid_argument);

	/** @brief 	Force  UUID for the given object an exception is throw iff UUID is already in use or object have already
	 * a setted one (weak_ptr version)
	 * @throw 	std::invalid_argument if UUID is already in use or object have already a setted one
	 **/
	template<class T>
	static void impose( boost::weak_ptr<T> wptr, const std::string &givenUUID,const int version) throw (std::invalid_argument);


	typedef std::pair<std::string,std::string> MultiUUID; // simple then RFC

	/**
	 * @brief	Return a new extended UUID;
	 */
	FWTOOLS_API static std::string generateExtendedUUID();


protected :




	/// Store association boost::weak_ptr adress <--> uuid as a string
	typedef std::map< boost::weak_ptr<void>, MultiUUID  > UUIDContainer;

	FWTOOLS_API static UUIDContainer m_uuids;

	/**
	 * @brief 	Return the next non expired weak ptr; erase all expired discovered
	 * @note 	To also be called before a for loop to remobe first element ( if necessary )
	 */
	FWTOOLS_API static void next( UUIDContainer::iterator &iter );

	/**
	 * @brief 	Return the first non expired element
	 */
	FWTOOLS_API static UUIDContainer::iterator begin(UUIDContainer &uuidContainer);

	/**
	 * @brief	Maintain lastest index available for a given typeinfo
	 */
	FWTOOLS_API static std::map< TypeInfo, ::boost::uint32_t > m_CategorizedCounter;

	/**
	 * @brief	Default constructor : does nothing.
	 */
	FWTOOLS_API UUID();

	/**
	 * @brief	Destructor : does nothing.
	 */
	FWTOOLS_API virtual ~UUID();

	/**
	 * @brief 	Generate an uuid from given type info (and store it).
	 *
	 * The generated uuid gave the form :
	 * \li CLASSNAME-NUM when NUM
	 * \li typeod()..).name()-NUM
	 *
	 * NUM is incremented at each request: NUMs a categorized
	 *
	 * @note 	This class is protected due to NUM incrementation
	 **/
	FWTOOLS_API static MultiUUID generate(const std::type_info &ti);

	/**
	 * @brief 	From a MultiUUID return the string corresponding to the wanted version
	 * @param 	muuid MultiUUID
	 * @param	version wanted version
	 * @return	Corresponding string
	 */
	FWTOOLS_API static std::string translateUUID(const MultiUUID &muuid, const int &version);

	/**
	 * @brief 	Helper to find a weak_ptr in the m_uuids
	 * @param	wp weeak_ptr to be find
	 * @return 	Iterator on the uuid associed to the weak_ptr
	 */
	UUIDContainer::iterator find( boost::weak_ptr<void> wp );

};





}

#include "fwTools/UUID.hxx"


#endif /* UUID_HPP_ */
