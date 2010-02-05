/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IUPDATER_HPP_
#define _CTRLSELECTION_IUPDATER_HPP_

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class	IUpdater.
 * @author  IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class CTRLSELECTION_CLASS_API IUpdater : public ::fwServices::IService
{

public:

	fwCoreServiceClassDefinitionsMacro ( (IUpdater)(::fwServices::IService::Baseclass) ) ;

	///@brief IUpdater constructor. Do nothing.
	CTRLSELECTION_API IUpdater();

	///@brief IUpdater destructor. Do nothing.
	CTRLSELECTION_API virtual ~IUpdater();

};

}

#endif // _CTRLSELECTION_IUPDATER_HPP_
