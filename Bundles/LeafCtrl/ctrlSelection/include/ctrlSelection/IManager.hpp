/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IMANAGER_HPP_
#define _CTRLSELECTION_IMANAGER_HPP_

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class	IManager.
 * @author  IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class CTRLSELECTION_CLASS_API IManager : public ::fwServices::IService
{

public:

	fwCoreServiceClassDefinitionsMacro ( (IManager)(::fwServices::IService::Baseclass) ) ;

	///@brief IManager constructor. Do nothing.
	CTRLSELECTION_API IManager();

	///@brief IManager destructor. Do nothing.
	CTRLSELECTION_API virtual ~IManager();

};

}

#endif // _CTRLSELECTION_IMANAGER_HPP_
