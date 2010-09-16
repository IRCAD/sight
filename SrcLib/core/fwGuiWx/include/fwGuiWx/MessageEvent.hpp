/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIWX_MESSAGEEVENT_HPP_
#define FWGUIWX_MESSAGEEVENT_HPP_

#include <wx/wx.h>

#include "fwGuiWX/config.hpp"

namespace fwGui
{

/**
 * @brief   This class define a message event, used to manage fwServices
 * notification in wx's events loop.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 * @todo Comment.
 */
class MessageEvent: public ::wxEvent
{
public:
    MessageEvent(wxEventType eventType)
        : wxEvent(0, eventType)
    {}

    // implement the base class pure virtual
    virtual wxEvent *Clone() const { return new MessageEvent(*this); }

};

wxDEFINE_EVENT(MESSAGE_EVENT, MessageEvent);


} // end namespace fwGui


#endif /* FWGUIWX_MESSAGEEVENT_HPP_ */
