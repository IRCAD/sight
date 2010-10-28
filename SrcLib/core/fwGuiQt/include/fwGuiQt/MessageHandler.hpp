/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIQT_MESSAGEHANDLER_HPP_
#define FWGUIQT_MESSAGEHANDLER_HPP_

#include <QObject>
#include <QAbstractEventDispatcher>
#include <QPointer>

#include <fwCore/base.hpp>
#include <fwServices/IDeliveryDelegate.hpp>

#include "fwGuiQt/config.hpp"

fwCorePredeclare( (fwServices)(GlobalEventManager) );

QT_BEGIN_NAMESPACE
class QEvent;
QT_END_NAMESPACE

namespace fwGui
{

/**
 * @brief   This service handle fwServices message dispatching.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @todo    MessageHandler is not commented.
 */

class FWGUIQT_CLASS_API MessageHandler : public QObject, public ::fwServices::IDeliveryDelegate
{

public:
    /// Definitions
    fwCoreServiceClassDefinitionsMacro ( (MessageHandler)(::fwServices::IDeliveryDelegate) ) ;
    fwCoreAllowSharedFromThis();


     FWGUIQT_API MessageHandler() throw() ;
     FWGUIQT_API ~MessageHandler() throw() ;

     FWGUIQT_API virtual bool event ( QEvent * e );

protected:
    /**
     * @brief If configuration is set, both subject (data) and observer (service) uuid are retrieved
     */
    FWGUIQT_API virtual void configuring() throw( ::fwTools::Failed ) ;
    FWGUIQT_API virtual void starting() throw( ::fwTools::Failed );
    FWGUIQT_API virtual void stopping() throw( ::fwTools::Failed );
    FWGUIQT_API virtual void updating() throw( ::fwTools::Failed );
    FWGUIQT_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed );
    FWGUIQT_API virtual void info( std::ostream &_sstream ) ;

    void addNewMessageToQtQueue();

private:

    bool m_processingEvent;
    unsigned int m_filteredMessagesCount;

    SPTR(::fwServices::GlobalEventManager)         m_msgHandler;
    ::fwServices::GlobalEventManager::DeliveryType m_oldDeliveryType;

    QPointer<QAbstractEventDispatcher> m_qtDispatcher;

    static int s_qtMessageHandlerEventType;
};

}

#endif /*FWGUIQT_MESSAGEHANDLER_HPP_*/

