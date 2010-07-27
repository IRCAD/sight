/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_LAYOUTMANAGER_MENULAYOUTMANAGER_HPP_
#define _FWGUIQT_LAYOUTMANAGER_MENULAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/fwMenu.hpp>
#include <fwGui/layoutManager/IMenuLayoutManager.hpp>

#include "fwGuiQt/container/QtMenuContainer.hpp"
#include "fwGuiQt/config.hpp"

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
class FWGUIQT_CLASS_API MenuLayoutManager : public ::fwGui::layoutManager::IMenuLayoutManager
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MenuLayoutManager)(::fwGui::layoutManager::IMenuLayoutManager) ,(()), new MenuLayoutManager);

    /// Constructor. Do nothing.
    FWGUIQT_API MenuLayoutManager();

    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~MenuLayoutManager();

    /**
     * @brief Instantiate actions with parent menu.
     * @pre LayoutManager must be initialized before.
     * @pre parent menu must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::fwMenu::sptr parent );

    /**
     * @brief Destroy local menus.
     * @pre services using this actions must be stopped before.
     */
    FWGUIQT_API virtual void destroyLayout();


    /**
     * @brief Set the action visibility.
     */
    FWGUIQT_API virtual void actionIsVisible(::fwGui::fwMenuItem::sptr menuItem, bool isVisible);

    /**
     * @brief Set the action enable or not.
     */
    FWGUIQT_API virtual void actionIsEnabled(::fwGui::fwMenuItem::sptr menuItem, bool isEnabled);

    /**
     * @brief Set the action checked or not.
     */
    FWGUIQT_API virtual void actionIsChecked(::fwGui::fwMenuItem::sptr, bool isChecked);

protected:
    ::fwGuiQt::container::QtMenuContainer::sptr m_parent;

};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUIQT_LAYOUTMANAGER_MENULAYOUTMANAGER_HPP_*/


