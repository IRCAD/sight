///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_CONTAINER_WXMENUCONTAINER_HPP_
#define _FWGUIWX_CONTAINER_WXMENUCONTAINER_HPP_

#include <wx/menu.h>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/fwMenu.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{
namespace container
{

/**
 * @brief   Defines the wxWidgets menu bar container for IHM.
 * @class   WxMenuContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API WxMenuContainer : public ::fwGui::fwMenu
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (WxMenuContainer)(::fwGui::fwMenu), (()), new WxMenuContainer );

    /**
     * @brief   Constructor.
     */
    FWGUIWX_API WxMenuContainer() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIWX_API virtual ~WxMenuContainer() throw() ;


    FWGUIWX_API virtual void clean();
    FWGUIWX_API virtual void destroyContainer();

    FWGUIWX_API virtual void setWxMenu(wxMenu* menu);
    FWGUIWX_API virtual wxMenu* getWxMenu();

private :

    wxMenu* m_menu;
};

} // namespace container
} // namespace fwGuiWx

#endif /*_FWGUIWX_CONTAINER_WXMENUCONTAINER_HPP_*/


