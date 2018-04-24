/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ctrlSelection/config.hpp"

#include <fwData/Composite.hpp>

#include <fwServices/IService.hpp>

#include <tuple>

namespace ctrlSelection
{

/**
 * @brief   An updater is use to register/unregister object in the OSR when it receives specific signals (declared in
 * configuration).
 */
class CTRLSELECTION_CLASS_API IUpdaterSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (IUpdaterSrv)(::fwServices::IService) );

    ///@brief IUpdaterSrv constructor. Do nothing.
    CTRLSELECTION_API IUpdaterSrv();

    ///@brief IUpdaterSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IUpdaterSrv();
};

}
