/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_EXCEPTION_HPP
#define _FWRUNTIME_EXCEPTION_HPP

#include <exception>
#include <string>

#include "fwRuntime/config.hpp"


namespace fwRuntime
{


/**
 * @brief	Defines the runtime exception class.
 * @struct 	RuntimeException
 * @date	2004-2009
 * @author 	IRCAD (Research and Development Team).
 */
struct FWRUNTIME_CLASS_API RuntimeException : public std::exception
{

	/**
	 * @brief		Copy constructor.
	 *
	 * @param[in]	exception	an exception instance to copy
	 */
	FWRUNTIME_API RuntimeException(const RuntimeException& exception) throw();

	/**
	 * @brief		Constructor.
	 *
	 * @param[in]	message	a string containing the exception message
	 */
	FWRUNTIME_API RuntimeException(const std::string& message) throw();

	/**
	 * @brief	Destructor : does nothing.
	 */
	FWRUNTIME_API virtual ~RuntimeException() throw();

	/**
	 * @brief	Retrieves the exception message.
	 *
	 * @return	a pointer to a string containing the exception message.
	 */
	FWRUNTIME_API virtual const char* what() const throw();


private:

	/**
	 * A string containing the exception message.
	 */
	std::string m_message;
};


} // namespace fwRuntime


#endif // #ifndef _FWRUNTIME_EXCEPTION_HPP
