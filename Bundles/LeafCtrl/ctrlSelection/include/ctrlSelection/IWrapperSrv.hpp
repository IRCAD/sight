/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_IWRAPPERSRV_HPP_
#define _CTRLSELECTION_IWRAPPERSRV_HPP_

#include <fwServices/IService.hpp>

#include "ctrlSelection/config.hpp"


namespace ctrlSelection
{

/**
 * @class   IWrapperSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
/**
 * @brief the purpose of this service is to wrap messages incoming to the object to new messages.
 * Typically when a generic message as ::ObjectMsg::UPDATED_OBJECT is receive ) by the object then a
 * specific service can translate it to "NEW_BUFFER" for fwData::Image, "CAMERA_MOVING" for camera ....
 * this service was created because we do not yet have a factory on msg
 */


class CTRLSELECTION_CLASS_API IWrapperSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IWrapperSrv)(::fwServices::IService) ) ;

    ///@brief IWrapperSrv constructor. Do nothing.
    CTRLSELECTION_API IWrapperSrv();

    ///@brief IWrapperSrv destructor. Do nothing.
    CTRLSELECTION_API virtual ~IWrapperSrv();

};

}

#endif // _CTRLSELECTION_IWRAPPERSRV_HPP_
