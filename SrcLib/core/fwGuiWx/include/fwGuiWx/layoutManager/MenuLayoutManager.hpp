/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_LAYOUTMANAGER_MENULAYOUTMANAGER_HPP_
#define _FWGUIWX_LAYOUTMANAGER_MENULAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/fwMenu.hpp>
#include <fwGui/layoutManager/IMenuLayoutManager.hpp>

#include "fwGuiWx/container/WxMenuContainer.hpp"
#include "fwGuiWx/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the menu layout manager for IHM.
 * @class   MenuLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API MenuLayoutManager : public ::fwGui::layoutManager::IMenuLayoutManager
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MenuLayoutManager)(::fwGui::layoutManager::IMenuLayoutManager) ,(()), new MenuLayoutManager);

    /// Constructor. Do nothing.
    FWGUIWX_API MenuLayoutManager();

    /// Destructor. Do nothing.
    FWGUIWX_API virtual ~MenuLayoutManager();

    /**
     * @brief Instantiate actions with parent menu.
     * @pre LayoutManager must be initialized before.
     * @pre parent menu must be instanced.
     */
    FWGUIWX_API virtual void createLayout( ::fwGui::fwMenu::sptr parent );

    /**
     * @brief Destroy local menus.
     * @pre services using this actions must be stopped before.
     */
    FWGUIWX_API virtual void destroyLayout();


    /**
     * @brief Set the action visibility.
     */
    FWGUIWX_API virtual void menuItemSetVisible(::fwGui::fwMenuItem::sptr menuItem, bool isVisible);

    /**
     * @brief Set the action enable or not.
     */
    FWGUIWX_API virtual void menuItemSetEnabled(::fwGui::fwMenuItem::sptr menuItem, bool isEnabled);

    /**
     * @brief Set the action checked or not.
     */
    FWGUIWX_API virtual void menuItemSetChecked(::fwGui::fwMenuItem::sptr, bool isChecked);

protected:

    ::fwGuiWx::container::WxMenuContainer::sptr m_parent;

    /// Vector of separators
    std::vector< ::fwGui::fwMenuItem::sptr > m_separators;

    static const std::map< ::fwGui::layoutManager::IMenuLayoutManager::ActionType, int> SPECIAL_ACTION_TO_WXID;
};


} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUIWX_LAYOUTMANAGER_MENULAYOUTMANAGER_HPP_*/


