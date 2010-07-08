///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_CONTAINER_WXMENUBARCONTAINER_HPP_
#define _FWGUIWX_CONTAINER_WXMENUBARCONTAINER_HPP_

#include <wx/menu.h>

#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>

#include <fwGui/fwMenuBar.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{
namespace container
{

/**
 * @brief   Defines the wxWidgets menu bar container for IHM.
 * @class   WxMenuBarContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API WxMenuBarContainer : public ::fwGui::fwMenuBar
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (WxMenuBarContainer)(::fwGui::fwMenuBar), (()), new WxMenuBarContainer );

    /**
     * @brief   Constructor.
     */
    FWGUIWX_API WxMenuBarContainer() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUIWX_API virtual ~WxMenuBarContainer() throw() ;


    FWGUIWX_API virtual void clean();
    FWGUIWX_API virtual void destroyContainer();

    FWGUIWX_API virtual void setWxMenuBar(wxMenuBar* menuBar);
    FWGUIWX_API virtual wxMenuBar* getWxMenuBar();

private :

    wxMenuBar* m_menuBar;
};

} // namespace container
} // namespace fwGuiWx

#endif /*_FWGUIWX_CONTAINER_WXMENUBARCONTAINER_HPP_*/


