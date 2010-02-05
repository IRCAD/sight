/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_PROFILE_PROFILE_HPP_
#define _FWRUNTIME_PROFILE_PROFILE_HPP_

#include <vector>
#include <boost/utility.hpp>

#include "fwCore/base.hpp"

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
struct Stopper;



/**
 * @brief	Implements a bundle set profile.
 * @struct	Profile
 * @date	2007-2009
 * @author 	IRCAD (Research and Development Team).
 */
struct Profile : public ::fwCore::BaseObject
{
	typedef std::vector< std::string >	ParamsContainer;

	fwCoreClassDefinitionsWithFactoryMacro( (Profile)(BaseObject::Baseclass), (()), new Profile) ;

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
	 * @param[in]	starter	a shared pointer to a starter
	 */
	FWRUNTIME_API void add( ::boost::shared_ptr< Starter > starter );

	/**
	 * @brief	Starts the profile.
	 */
	FWRUNTIME_API void start();
	FWRUNTIME_API void stop();

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


	/**
	 * @brief	Return profile version.
	 */
	FWRUNTIME_API std::string getVersion() { return m_sVersion; }

	/**
	 * @brief		Set profile version.
	 *
	 * @param[in]	_sVersion	profile version
	 */
	FWRUNTIME_API void setVersion(std::string _sVersion) { m_sVersion = _sVersion; }



	/**
	 * @brief	Return profile CheckSingleInstance.
	 */
	FWRUNTIME_API bool getCheckSingleInstance() { return m_checkSingleInstance; }

	/**
	 * @brief		Set profile CheckSingleInstance.
	 *
	 * @param[in]	_checkSingleInstance	profile CheckSingleInstance
	 */
	FWRUNTIME_API void setCheckSingleInstance(bool _checkSingleInstance) { m_checkSingleInstance = _checkSingleInstance; }

	FWRUNTIME_API ParamsContainer getParams();

	FWRUNTIME_API void setParams(int argc, char** argv);

	FWRUNTIME_API char** getRawParams();

private:

	typedef std::vector< ::boost::shared_ptr< Activater > >	ActivaterContainer;
	typedef std::vector< ::boost::shared_ptr< Starter > >	StarterContainer;
	typedef std::vector< ::boost::shared_ptr< Stopper > >	StopperContainer;

	ActivaterContainer	m_activaters;	///< all managed activators
	StarterContainer	m_starters;		///< all managed starters
	StopperContainer	m_stoppers;		///< all managed stoppers
	std::string			m_sName;		///< name profile
	std::string			m_sVersion;		///< profile app version
	bool				m_checkSingleInstance;
	ParamsContainer		m_params;
};


/**
 * @brief       Set current profile.
 *
 * @param       prof profile
 */
FWRUNTIME_API void setCurrentProfile(Profile::sptr prof);

/**
 * @brief       Get current profile.
 */
FWRUNTIME_API Profile::sptr getCurrentProfile();

} // namespace profile

} // namespace fwRuntime




#endif /*_FWRUNTIME_PROFILE_PROFILE_HPP_*/
