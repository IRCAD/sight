/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_LAYOUTMANAGER_ITOOLBARLAYOUTMANAGER_HPP_
#define _FWGUI_LAYOUTMANAGER_ITOOLBARLAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwToolBar.hpp"
#include "fwGui/fwMenuItem.hpp"
#include "fwGui/IMenuItemCallback.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the toolBar layout manager for IHM.
 * @class   IToolBarLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IToolBarLayoutManager : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IToolBarLayoutManager)(::fwCore::BaseObject) )

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;
    typedef std::vector< ::fwGui::IMenuItemCallback::sptr > CallbacksType;

    class ActionInfo
         {
         public :

            ActionInfo() :
                m_name(""),
                m_icon(""),
                m_isSeparator(false),
                m_isCheckable (false),
                m_isRadio(false),
                m_isChecked (false),
                m_isEnabled(true)
            {}

            std::string m_name;
            std::string m_icon;
            bool        m_isCheckable;
            bool        m_isRadio;
            bool        m_isChecked;
            bool        m_isEnabled;
            bool        m_isSeparator;
         };

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IToolBarLayoutManager();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IToolBarLayoutManager();

    /**
     * @brief Returns the vector of fwMenuItem managed by this layout.
     */
    FWGUI_API virtual std::vector< ::fwGui::fwMenuItem::sptr > getMenuItems();

    /**
     * @brief Configuring method allows to create a toolBar with several actions. The created toolbar is a horizontal one without border.
     * Each icon has a size of 32x32 by default.
     * Here a sample of the DefaultToolBar service declaration with two actions:
     * @verbatim
        <service uid="toolBar" type="::gui::aspect::IToolBar" implementation="::gui::aspect::DefaultToolBar" autoComChannel="no">
            <toolBitmapSize height="40" width="40" />
            <action uid="action_new" name="New file" icon="Bundles/MyApplication/icons/newFile.png"/>
            <separator/>
            <action uid="action_3Dview" style="radio" name="3D view" icon="Bundles/MyApplication/icons//icon-3D.png" />
            <action uid="action_2Dview" style="radio" name="2D view" icon="Bundles/MyApplication/icons/icon-2D.png" />
            <separator/>
            <action uid="action_hideInformation"  style="check" name="Show information window" icon="Bundles/MyApplication/icons/View-INFO.png" />
        </service>
       @endverbatim
     * - <toolBitmapSize height="40" width="40" /> : has to change the size of the icon used in the toolbar.
     * - <separator /> : allows to put a separation in the tool bar.
     * - Action Attributes:
     *       - icon is the icon file to use.
     *       - name is the text that will be put inside the tool tip.
     *       - state can be {check}. This attribute is available only with the style radio and check.
     *       - style is the style of the button. The available choice is {radio|check}. If style isn't specified normal state(without 'state')is used.
     */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    /**
     * @brief Instantiate actions with parent toolBar.
     */
    FWGUI_API virtual void createLayout( ::fwGui::fwToolBar::sptr parent ) = 0;

    /**
     * @brief Destroy local actions.
     * @pre services using this actions must be stopped before.
     */
    FWGUI_API virtual void destroyLayout() = 0;


    /**
     * @brief Set the action visibility.
     */
    FWGUI_API virtual void menuItemSetVisible(::fwGui::fwMenuItem::sptr, bool isVisible) = 0;

    /**
     * @brief Set the action enable or not.
     */
    FWGUI_API virtual void menuItemSetEnabled(::fwGui::fwMenuItem::sptr, bool isEnabled) = 0;

    /**
     * @brief Set the action checked or not.
     */
    FWGUI_API virtual void menuItemSetChecked(::fwGui::fwMenuItem::sptr, bool isChecked) = 0;

    /**
     * @brief Sets callbacks associate with toolBar items.
     */
    FWGUI_API virtual void setCallbacks(CallbacksType callbacks) {m_callbacks = callbacks;};

protected:

    /**
     * @brief Helper to destroy local actions.
     */
    FWGUI_API virtual void destroyActions();

    /// All actions managed by this layout.
    std::vector< ::fwGui::fwMenuItem::sptr > m_menuItems;

    /// Save action informations from configuration.
    std::vector< ActionInfo > m_actionInfo;

    /// Callbacks associate with toolBar items
    CallbacksType m_callbacks;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_ITOOLBARLAYOUTMANAGER_HPP_*/


