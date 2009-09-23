/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_PROFILE_PROFILE_HPP_
#define _FWRUNTIME_PROFILE_PROFILE_HPP_

#include <vector>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "fwRuntime/config.hpp"


namespace fwRuntime
{

/**
 * @namespace 	::fwRuntime::profile
 * @brief 		Namespace ::fwRuntime::profile
 * @date	2007-2009
 * @author 	IRCAD (Research and Development Team).
 */
namespace profile
{



struct Activater;
struct Starter;



/**
 * @brief	Implements a bundle set profile.
 * @struct	Profile
 * @date	2007-2009
 * @author 	IRCAD (Research and Development Team).
 */
struct Profile
{

	/**
	 * @brief	Constructor : does nothing.
	 */
	FWRUNTIME_API Profile();

	/**
	 * @brief		Adds a new activator.
	 *
	 * @param[in]	activater	a shared pointer to an activator
	 */
	FWRUNTIME_API void add( ::boost::shared_ptr< Activater > activater );

	/**
	 * @brief		Adds a new starter.
	 *
	 * @param[in]	starter	a shared pointer to an starter
	 */
	FWRUNTIME_API void add( ::boost::shared_ptr< Starter > starter );

	/**
	 * @brief	Starts the profile.
	 */
	FWRUNTIME_API void start();

	/**
	 * @brief	Return profile name.
	 */
	FWRUNTIME_API std::string getName() { return m_sName; }

	/**
	 * @brief		Set profile name.
	 *
	 * @param[in]	_sName	profile name
	 */
	FWRUNTIME_API void setName(std::string _sName) { m_sName = _sName; }


private:

	typedef std::vector< ::boost::shared_ptr< Activater > >	ActivaterContainer;
	typedef std::vector< ::boost::shared_ptr< Starter > >	StarterContainer;

	ActivaterContainer	m_activaters;	///< all managed activators
	StarterContainer	m_starters;		///< all managed starters
	std::string			m_sName;		///< name profile

};



} // namespace profile

} // namespace fwRuntime



#endif /*_FWRUNTIME_PROFILE_PROFILE_HPP_*/
