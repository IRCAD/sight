/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_LAYOUTMANAGER_TOOLBARLAYOUTMANAGER_HPP_
#define _FWGUIQT_LAYOUTMANAGER_TOOLBARLAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/fwToolBar.hpp>
#include <fwGui/layoutManager/IToolBarLayoutManager.hpp>

#include "fwGuiQt/container/QtToolBarContainer.hpp"
#include "fwGuiQt/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the toolBar layout manager for IHM.
 * @class   ToolBarLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API ToolBarLayoutManager : public ::fwGui::layoutManager::IToolBarLayoutManager
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ToolBarLayoutManager)(::fwGui::layoutManager::IToolBarLayoutManager) ,(()), new ToolBarLayoutManager);

    /// Constructor. Do nothing.
    FWGUIQT_API ToolBarLayoutManager();

    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~ToolBarLayoutManager();

    /**
     * @brief Instantiate actions with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    FWGUIQT_API virtual void createLayout( ::fwGui::fwToolBar::sptr parent );

    /**
     * @brief Destroy local toolBars.
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
    ::fwGuiQt::container::QtToolBarContainer::sptr m_parent;

};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUIQT_LAYOUTMANAGER_TOOLBARLAYOUTMANAGER_HPP_*/


