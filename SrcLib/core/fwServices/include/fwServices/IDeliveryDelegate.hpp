/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef FWSERVICES_IDELIVERYDELEGATE_HPP_
#define FWSERVICES_IDELIVERYDELEGATE_HPP_

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @brief   This interface defines messages delivery delegation services interface.
 *
 * @class   IDeliveryDelegate.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWSERVICES_CLASS_API IDeliveryDelegate : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IDeliveryDelegate)(::fwServices::IService::Baseclass) ) ;

    ///@brief IDeliveryDelegate constructor. Do nothing.
    FWSERVICES_API IDeliveryDelegate();

    ///@brief IDeliveryDelegate destructor. Do nothing.
    FWSERVICES_API virtual ~IDeliveryDelegate();

};

}

#endif // FWSERVICES_DELIVERYDELEGATER_HPP_
