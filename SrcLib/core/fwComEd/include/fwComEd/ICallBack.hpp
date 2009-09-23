/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_ICALLBACK_HPP_
#define _FWCOMED_ICALLBACK_HPP_


#include <fwTools/Failed.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{

/**
 * @brief	Interface for callback functions to define and execute and messages
 * @class 	ICallback
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWCOMED_CLASS_API ICallback
{
public:

	/**
	 * @brief	Constructor : does nothing.
	 */
	FWCOMED_API ICallback() throw();

	/**
	 * @brief	Destuctor : does nothing.
	 */
	FWCOMED_API virtual ~ICallback() throw();

	/// Define modif status
	FWCOMED_API virtual void operator()
		( ::boost::shared_ptr< const ::fwServices::ObjectMsg >
		_msg, void *_client) throw( ::fwTools::Failed ) = 0 ;

};

/**
 * @brief 	Template filter for callback functions to define and execute and messages.
 * @class 	ICallbackFilter
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 * Filter is performed with respect to CLIENT template definition. client is the
 * object to which modifications will be applied.
 */
template< class CLIENT >
class ICallbackFilter : public ICallback
{
public:

	/**
	 * @brief	Constructor : does nothing.
	 */
	ICallbackFilter() throw() {};

	/**
	 * @brief	Destuctor : does nothing.
	 */
	virtual ~ICallbackFilter() throw() {};

	/// Define modif status
	virtual void operator()
		( ::boost::shared_ptr< const ::fwServices::ObjectMsg >
		_msg, void *_client) throw( ::fwTools::Failed ) = 0 ;

};

}


#endif // _FWCOMED_ICALLBACK_HPP_
