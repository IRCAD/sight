/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_ACTIONCALLBACK_HPP_
#define _FWGUIWX_ACTIONCALLBACK_HPP_

#include <wx/event.h>

#include <fwGui/ActionCallbackBase.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{

/**
 * @brief   Defines the menu item callback.
 * @class   ActionCallback.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API ActionCallback : public ::fwGui::ActionCallbackBase
{

public :

    fwCoreClassDefinitionsWithFactoryMacro( (ActionCallback)(::fwGui::ActionCallbackBase), (()), new ActionCallback );

    /**
     * @brief   Constructor.
     */
    FWGUIWX_API ActionCallback() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIWX_API virtual ~ActionCallback() ;

    /**
     * @brief  wxWidget called method when callback is executed.
     */
    FWGUIWX_API virtual void executeWx(wxCommandEvent& event);

};

} // namespace fwGuiWx

#endif /*_FWGUIWX_ACTIONCALLBACK_HPP_*/


