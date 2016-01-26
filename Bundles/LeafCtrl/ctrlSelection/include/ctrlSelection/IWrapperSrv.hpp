/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_IWRAPPERSRV_HPP__
#define __CTRLSELECTION_IWRAPPERSRV_HPP__

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class   IWrapperSrv
 * @brief The purpose of this service is to wrap messages incoming to the object to new messages.
 */


class CTRLSELECTION_CLASS_API IWrapperSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IWrapperSrv)(::fwServices::IService) );

    ///@brief IWrapperSrv constructor. Do nothing.
    CTRLSELECTION_API IWrapperSrv();

    ///@brief IWrapperSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IWrapperSrv();

};

}

#endif // __CTRLSELECTION_IWRAPPERSRV_HPP__
