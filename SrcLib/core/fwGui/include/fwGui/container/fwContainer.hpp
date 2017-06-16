/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_CONTAINER_FWCONTAINER_HPP__
#define __FWGUI_CONTAINER_FWCONTAINER_HPP__

#include "fwGui/config.hpp"
#include "fwGui/GuiBaseObject.hpp"

namespace fwGui
{
namespace container
{
/**
 * @brief   Defines the generic container for IHM.
 *
 */
class FWGUI_CLASS_API fwContainer : public ::fwGui::GuiBaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (fwContainer)(::fwGui::GuiBaseObject) );

    FWGUI_API virtual void clean()            = 0;
    FWGUI_API virtual void destroyContainer() = 0;
    FWGUI_API virtual bool isShownOnScreen()  = 0;

    FWGUI_API virtual void setVisible(bool isVisible) = 0;
    FWGUI_API virtual void setEnabled(bool isEnabled) = 0;

};

} // namespace container
} // namespace fwGui

#endif /*__FWGUI_CONTAINER_FWCONTAINER_HPP__*/

