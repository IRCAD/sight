/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORY_HPP
#define _FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORY_HPP

#include <sstream>
#include <string>
#include <boost/shared_ptr.hpp>

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/ExecutableFactory.hpp"


namespace fwRuntime
{

/**
 * @namespace 	::fwRuntime::utils
 * @brief 		Namespace ::fwRuntime::utils
 * @date	2004-2009
 * @author 	IRCAD (Research and Development Team).
 */
namespace utils
{


/**
 * @brief	Defines a generic template executable factory class.
 * @struct	GenericExecutableFactory
 * @date	2004-2009
 * @author 	IRCAD (Research and Development Team).
 */
template<typename E>
struct GenericExecutableFactory : public ExecutableFactory
{
	/**
	 * @brief		Constructor.
	 *
	 * @param[in]	type	a string containing the type supported by the factory
	 */
	GenericExecutableFactory(const std::string& type)
	:	ExecutableFactory( type )
	{}

	/**
	 * @brief	Destructor : does nothing.
	 */
	virtual ~GenericExecutableFactory()
	{}


	/**
	 * @brief	Creates an executable object instance.
	 *
	 * @return	a pointer to an executable instance
	 */
	IExecutable* createExecutable() const throw(RuntimeException)
	{
		IExecutable	* result( 0 );
		try
		{
			result = new E();
		}
		catch( std::exception & exception )
		{
			std::ostringstream	buffer;
			buffer << "Unexpected exception caught while creating an ::fwRuntime::IExecutable instance. ";
			buffer << exception.what();
			throw RuntimeException( buffer.str() );
		}
		catch( ... )
		{
			throw RuntimeException( "Unexpected exception caught while creating an ::fwRuntime::IExecutable instance." );
		}
		return result;
	}


private:

	/**
	 * @brief	Assignment operator.
	 *
	 * @remark	Assignment forbidden for this class.
	 */
	void operator=(const GenericExecutableFactory&) throw()
	{}

};


} // namespace utils

} // namespace fwRuntime


#endif // #define _FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORY_HPP
