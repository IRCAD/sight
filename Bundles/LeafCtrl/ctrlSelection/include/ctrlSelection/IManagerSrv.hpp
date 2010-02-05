/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IMANAGERSRV_HPP_
#define _CTRLSELECTION_IMANAGERSRV_HPP_

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class	IManagerSrv.
 * @author  IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class CTRLSELECTION_CLASS_API IManagerSrv : public ::fwServices::IService
{

public:

	fwCoreServiceClassDefinitionsMacro ( (IManagerSrv)(::fwServices::IService::Baseclass) ) ;

	///@brief IManagerSrv constructor. Do nothing.
	CTRLSELECTION_API IManagerSrv();

	///@brief IManagerSrv destructor. Do nothing.
	CTRLSELECTION_API virtual ~IManagerSrv();

};

}

#endif // _CTRLSELECTION_IMANAGERSRV_HPP_
