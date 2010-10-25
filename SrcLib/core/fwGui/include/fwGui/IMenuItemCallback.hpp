/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IMENUITEMCALLBACK_HPP_
#define _FWGUI_IMENUITEMCALLBACK_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"

/**
 * @brief   The namespace fwGui contains the base interface for IHM services.
 * @namespace   fwGui
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
namespace fwGui
{

/**
 * @brief   Defines the menu item callback.
 * @class   IMenuItemCallback.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IMenuItemCallback : public ::fwCore::BaseObject
{

public :

    fwCoreNonInstanciableClassDefinitionsMacro ( (IMenuItemCallback)(::fwCore::BaseObject) ) ;

    /**
     * @brief  Called method when callback is executed.
     */
    FWGUI_API virtual void execute() =0;

};

} // namespace fwGui

#endif /*_FWGUI_IMENUITEMCALLBACK_HPP_*/


