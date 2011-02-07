/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_FWMENUITEM_HPP_
#define _FWGUI_FWMENUITEM_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"

namespace fwGui
{
namespace container
{
/**
 * @brief   Defines the menu item for IHM.
 * @class   fwMenuItemBar.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API fwMenuItem : public ::fwCore::BaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro ( (fwMenuItem)(::fwCore::BaseObject) ) ;

    FWGUI_API virtual void clean() = 0;
    FWGUI_API virtual void destroyContainer() = 0;

};
} // namespace container
} // namespace fwGui

#endif /*_FWGUI_FWMENUITEM_HPP_*/


