/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IUPDATERSRV_HPP_
#define _CTRLSELECTION_IUPDATERSRV_HPP_

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class   IUpdaterSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class CTRLSELECTION_CLASS_API IUpdaterSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IUpdaterSrv)(::fwServices::IService::Baseclass) ) ;

    ///@brief IUpdaterSrv constructor. Do nothing.
    CTRLSELECTION_API IUpdaterSrv();

    ///@brief IUpdaterSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IUpdaterSrv();

};

}

#endif // _CTRLSELECTION_IUPDATERSRV_HPP_
