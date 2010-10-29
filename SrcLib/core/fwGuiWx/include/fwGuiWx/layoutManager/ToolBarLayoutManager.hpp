/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_LAYOUTMANAGER_TOOLBARLAYOUTMANAGER_HPP_
#define _FWGUIWX_LAYOUTMANAGER_TOOLBARLAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/container/fwToolBar.hpp>
#include <fwGui/layoutManager/IToolBarLayoutManager.hpp>

#include "fwGuiWx/container/WxToolBarContainer.hpp"
#include "fwGuiWx/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the tool bar layout manager for IHM.
 * @class   ToolBarLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API ToolBarLayoutManager : public ::fwGui::layoutManager::IToolBarLayoutManager
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ToolBarLayoutManager)(::fwGui::layoutManager::IToolBarLayoutManager) ,(()), new ToolBarLayoutManager);

    /// Constructor. Do nothing.
    FWGUIWX_API ToolBarLayoutManager();

    /// Destructor. Do nothing.
    FWGUIWX_API virtual ~ToolBarLayoutManager();

    /**
     * @brief Instantiate actions with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    FWGUIWX_API virtual void createLayout( ::fwGui::container::fwToolBar::sptr parent );

    /**
     * @brief Destroy local toolbar.
     * @pre services using this actions must be stopped before.
     */
    FWGUIWX_API virtual void destroyLayout();


    /**
     * @brief Set the action visibility.
     */
    FWGUIWX_API virtual void menuItemSetVisible(::fwGui::container::fwMenuItem::sptr menuItem, bool isVisible);

    /**
     * @brief Set the action enable or not.
     */
    FWGUIWX_API virtual void menuItemSetEnabled(::fwGui::container::fwMenuItem::sptr menuItem, bool isEnabled);

    /**
     * @brief Set the action checked or not.
     */
    FWGUIWX_API virtual void menuItemSetChecked(::fwGui::container::fwMenuItem::sptr, bool isChecked);

protected:

    ::fwGuiWx::container::WxToolBarContainer::sptr m_parent;

};


} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUIWX_LAYOUTMANAGER_TOOLBARLAYOUTMANAGER_HPP_*/


