/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_MESSAGEHANDLER_HPP_
#define FWWX_MESSAGEHANDLER_HPP_

#include <boost/function.hpp>


#include <wx/timer.h>
#include <wx/event.h>

#include <fwCore/base.hpp>
#include <fwServices/IDeliveryDelegate.hpp>

#include "fwWX/config.hpp"

fwCorePredeclare( (fwServices)(GlobalEventManager) );
fwCorePredeclare( (fwWX)(MessageEvent) );

namespace fwWX
{

/**
 * @brief   This service handle fwServices message dispatching.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @todo    MessageHandler is not commented.
 */

class FWWX_CLASS_API MessageHandler : public ::fwServices::IDeliveryDelegate
{

public:
    /// Definitions
    fwCoreServiceClassDefinitionsMacro ( (MessageHandler)(::fwServices::IDeliveryDelegate::Baseclass) ) ;
    fwCoreAllowSharedFromThis();


     FWWX_API MessageHandler() throw() ;
     FWWX_API ~MessageHandler() throw() ;

     static void addNewMessageToWxQueue();

protected:
    /**
     * @brief If configuration is set, both subject (data) and observer (service) uuid are retrieved
     */
    FWWX_API virtual void configuring() throw( ::fwTools::Failed ) ;
    FWWX_API virtual void starting() throw( ::fwTools::Failed );
    FWWX_API virtual void stopping() throw( ::fwTools::Failed );
    FWWX_API virtual void updating() throw( ::fwTools::Failed );
    FWWX_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed );
    FWWX_API virtual void info( std::ostream &_sstream ) ;

private:

    /*
     * @brief   declares the wxWidgets event table
     */

    int      m_rate;
    wxTimer *m_wxTimer;

    SPTR(::fwServices::GlobalEventManager)         m_msgHandler;
    ::fwServices::GlobalEventManager::DeliveryType m_oldDeliveryType;

    ::boost::function1< void , ::fwWX::MessageEvent & > m_onMessageHandler;
    void onMessage(::fwWX::MessageEvent& event);
};

}

#endif /*FWWX_MESSAGEHANDLER_HPP_*/

