/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_MESSAGEHANDLER_HPP_
#define FWWX_MESSAGEHANDLER_HPP_

//#include <boost/preprocessor/repetition/enum_params.hpp>
//#include <boost/preprocessor/repetition/repeat.hpp>

#include <wx/timer.h>
#include <wx/event.h>

#include "fwCore/base.hpp"

#include "fwWX/config.hpp"

fwCorePredeclare( (fwServices)(GlobalEventManager) );
fwCorePredeclare( (fwWX)(MessageEvent) );

namespace fwWX
{

/**
 * @brief   This class handle fwServices message dispatching.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @todo    MessageHandler is not commented.
 */
class FWWX_CLASS_API MessageHandler : public wxEvtHandler
{

public:
     FWWX_API MessageHandler() throw() ;
     FWWX_API ~MessageHandler() throw() ;

     static void addNewMessageToWxQueue();
private:

    /*
     * @brief   declares the wxWidgets event table
     */

    int      m_rate;
	wxTimer *m_wxTimer;

    SPTR(::fwServices::GlobalEventManager) m_msgHandler;

    void OnMessage(::fwWX::MessageEvent& event);
};

}

#endif /*FWWX_MESSAGEHANDLER_HPP_*/

