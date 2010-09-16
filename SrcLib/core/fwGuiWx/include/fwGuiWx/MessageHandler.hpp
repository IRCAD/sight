/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIWX_MESSAGEHANDLER_HPP_
#define FWGUIWX_MESSAGEHANDLER_HPP_

#include <boost/function.hpp>

#include <wx/event.h>

#include <fwCore/base.hpp>
#include <fwServices/IDeliveryDelegate.hpp>

#include "fwGuiWx/config.hpp"

fwCorePredeclare( (fwServices)(GlobalEventManager) );
fwCorePredeclare( (fwGui)(MessageEvent) );

namespace fwGui
{

/**
 * @brief   This service handle fwServices message dispatching.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @todo    MessageHandler is not commented.
 */

class FWGUIWX_CLASS_API MessageHandler : public ::fwServices::IDeliveryDelegate
{

public:
    /// Definitions
    fwCoreServiceClassDefinitionsMacro ( (MessageHandler)(::fwServices::IDeliveryDelegate) ) ;
    fwCoreAllowSharedFromThis();


     FWGUIWX_API MessageHandler() throw() ;
     FWGUIWX_API ~MessageHandler() throw() ;

     static void addNewMessageToWxQueue();

protected:
    /**
     * @brief If configuration is set, both subject (data) and observer (service) uuid are retrieved
     */
    FWGUIWX_API virtual void configuring() throw( ::fwTools::Failed ) ;
    FWGUIWX_API virtual void starting() throw( ::fwTools::Failed );
    FWGUIWX_API virtual void stopping() throw( ::fwTools::Failed );
    FWGUIWX_API virtual void updating() throw( ::fwTools::Failed );
    FWGUIWX_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed );
    FWGUIWX_API virtual void info( std::ostream &_sstream ) ;

private:

    SPTR(::fwServices::GlobalEventManager)         m_msgHandler;
    ::fwServices::GlobalEventManager::DeliveryType m_oldDeliveryType;

    ::boost::function1< void , ::fwGui::MessageEvent & > m_onMessageHandler;
    void onMessage(::fwGui::MessageEvent& event);
};

}

#endif /*FWGUIWX_MESSAGEHANDLER_HPP_*/

