/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_CONTAINER_WXMENUITEMCONTAINER_HPP_
#define _FWGUIWX_CONTAINER_WXMENUITEMCONTAINER_HPP_

#include <wx/menu.h>
#include <wx/toolbar.h>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/container/fwMenuItem.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{
namespace container
{

/**
 * @brief   Defines the wxWidgets menuItem bar container for IHM.
 * @class   WxMenuItemContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API WxMenuItemContainer : public ::fwGui::container::fwMenuItem
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (WxMenuItemContainer)(::fwGui::container::fwMenuItem), (()), new WxMenuItemContainer );

    /**
     * @brief   Constructor.
     */
    FWGUIWX_API WxMenuItemContainer() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIWX_API virtual ~WxMenuItemContainer() throw() ;


    FWGUIWX_API virtual void clean();
    FWGUIWX_API virtual void destroyContainer();

    FWGUIWX_API virtual void setWxMenuItem(wxMenuItem* menuItem);
    FWGUIWX_API virtual wxMenuItem* getWxMenuItem();

    FWGUIWX_API virtual void setWxToolItem(wxToolBarToolBase* menuItem);
    FWGUIWX_API virtual wxToolBarToolBase* getWxToolItem();

private :

    wxMenuItem* m_menuItem;
    wxToolBarToolBase* m_toolBarToolBase;
};

} // namespace container
} // namespace fwGuiWx

#endif /*_FWGUIWX_CONTAINER_WXMENUITEMCONTAINER_HPP_*/


