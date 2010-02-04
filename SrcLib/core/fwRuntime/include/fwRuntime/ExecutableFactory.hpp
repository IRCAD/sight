/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_EXECUTABLEFACTORY_HPP
#define _FWRUNTIME_EXECUTABLEFACTORY_HPP

#include <string>

#include "fwRuntime/config.hpp"
#include "fwRuntime/BundleElement.hpp"
#include "fwRuntime/RuntimeException.hpp"

namespace fwRuntime
{
	struct IExecutable;
}



namespace fwRuntime
{


/**
 * @brief	Defines the abstract executable factory class.
 * @struct	ExecutableFactory
 * @date	2004-2009
 * @author 	IRCAD (Research and Development Team).
 */
struct FWRUNTIME_CLASS_API ExecutableFactory : public BundleElement
{

	/**
	 * @name	Construction & Destruction
	 *
	 * @{
	 */

	/**
	 * @brief		Constructor
	 *
	 * @param[in]	type	a string containing a type identifier
	 */
	FWRUNTIME_API ExecutableFactory( const std::string & type );

	/**
	 * @brief	Destructor : does nothing
	 */
	FWRUNTIME_API virtual ~ExecutableFactory();
	///@}

	/**
	 * @brief	Creates an executable object instance.
	 *
	 * @return	a pointer to an executable instance
	 */
	virtual IExecutable* createExecutable() const throw(RuntimeException) = 0;


	/**
	 * @brief	Retrieves the type of executable the factory is able to create.
	 *
	 * @return	a string containing an executable type
	 */
	FWRUNTIME_API const std::string getType() const;


private:


	std::string	m_type;	///< a string containing the type identifier managed by the factory

};


} // namespace fwRuntime


#endif // #define _FWRUNTIME_EXECUTABLEFACTORY_HPP
