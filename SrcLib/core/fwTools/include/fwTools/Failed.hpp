/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_FAILED_HPP_
#define _FWTOOLS_FAILED_HPP_

#include <exception>
#include <string>

#include "fwTools/config.hpp"

namespace fwTools
{

/**
 * @brief	Implements a failed exception class
 * @class 	Failed
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
struct FWTOOLS_CLASS_API Failed : public std::exception
{

	/**
	 * @brief		Constructor
	 *
	 * @param[in]	message	a string containing the failure message
	 */
	FWTOOLS_API Failed(const std::string & message) throw();

	/**
	 * @brief	Destructor : do nothing.
	 */
	FWTOOLS_API ~Failed() throw();


	/**
	 * @brief	Retrieves the failuer message
	 *
	 * @return	a pointer to the string containing the failure message
	 */
	FWTOOLS_API const char* what() const throw();


private:

	/// A string containing the failure message
	std::string m_message;

};

} // namespace fwTools

#endif // _FWTOOLS_FAILED_HPP_
