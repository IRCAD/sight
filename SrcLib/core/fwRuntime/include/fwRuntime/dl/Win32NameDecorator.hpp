/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_DL_WIN32NAMEDECORATOR_HPP_
#define _FWRUNTIME_DL_WIN32NAMEDECORATOR_HPP_



#include "fwRuntime/dl/NameDecorator.hpp"



namespace fwRuntime
{

namespace dl
{



/**
 * @brief	Implements a win32 specific name decorator.
 * @struct	Win32NameDecorator
 * @date	2004-2009
 * @author 	IRCAD (Research and Development Team).
 */
struct Win32NameDecorator : public NameDecorator
{


protected:

	/**
	 * @name		Ovrerides
	 */
	//@{
	const std::string getNativeExtension() const throw();
	//@}

};



} // namespace dl

} // namespace fwRuntime



#endif /*_FWRUNTIME_DL_WIN32NAMEDECORATOR_HPP_*/
