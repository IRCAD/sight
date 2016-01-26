/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_CONTAINER_FWMENUITEM_HPP__
#define __FWGUI_CONTAINER_FWMENUITEM_HPP__

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace container
{
/**
 * @brief   Defines the menu item for IHM.
 * @class   fwMenuItem
 *
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API fwMenuItem : public ::fwGui::GuiBaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro ( (fwMenuItem)(::fwGui::GuiBaseObject) );

    FWGUI_API virtual void clean()            = 0;
    FWGUI_API virtual void destroyContainer() = 0;

};
} // namespace container
} // namespace fwGui

#endif /*__FWGUI_CONTAINER_FWMENUITEM_HPP__*/


