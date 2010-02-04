/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_CONVERT_HPP_
#define _FWRUNTIME_CONVERT_HPP_

#include <boost/shared_ptr.hpp>

#include <libxml/tree.h>

#include "fwRuntime/config.hpp"
#include "fwRuntime/ConfigurationElement.hpp"

namespace fwRuntime
{

/**
 * @brief	Defines the convert class.
 * @class 	Convert
 * @date	2004-2009
 * @author 	IRCAD (Research and Development Team).
 */
class FWRUNTIME_CLASS_API Convert // Singleton
{
public:

	/**
	 * @brief	Destructor : does nothing.
	 */
	FWRUNTIME_API virtual  ~Convert();

	/**
	 * @brief	Build an xmlNodePtr from a ConfigurationElement
	 */
	FWRUNTIME_API static xmlNodePtr toXml( ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement) ;

	/**
	 * @brief	Build an xmlNodePtr with all running Bundles
	 */
	FWRUNTIME_API static xmlNodePtr runningBundlesToXml( ) ;

private :

	FWRUNTIME_API static void fromConfigurationElementToXml( ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement, xmlNodePtr _node) ; //_node is modified

	/**
	 * @brief	Constructor : does nothing.
	 */
	Convert();

};

} // namespace fwRuntime


#endif // _FWRUNTIME_CONVERT_HPP_
