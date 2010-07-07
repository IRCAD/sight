/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_LAYOUTMANAGER_IMENULAYOUTMANAGER_HPP_
#define _FWGUI_LAYOUTMANAGER_IMENULAYOUTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwMenu.hpp"
#include "fwGui/fwMenuItem.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the menu layout manager for IHM.
 * @class   IMenuLayoutManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IMenuLayoutManager : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IMenuLayoutManager)(::fwCore::BaseObject) )

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::string RegistryKeyType;

    typedef enum {
        DEFAULT,
        QUIT,
        ABOUT,
        HELP,
        NEW
    }actionType;

    class ActionInfo
         {
         public :

            ActionInfo() :
                m_isCheckable (false),
                m_isRadio(false),
                m_isChecked (false),
                m_isEnabled(true),
                m_type(DEFAULT)
            {}

            std::string m_name;
            std::string m_shortcut;
            bool        m_isCheckable;
            bool        m_isRadio;
            bool        m_isChecked;
            bool        m_isEnabled;
            actionType  m_type;
         };

    const static RegistryKeyType REGISTRAR_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IMenuLayoutManager();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IMenuLayoutManager();

    /**
     * @brief Returns the vector of fwMenuItem managed by this layout.
     */
    FWGUI_API virtual std::vector< ::fwGui::fwMenuItem::sptr > getMenuItems();

    /**
     * @brief Configure the actions before creation.
     */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    /**
     * @brief Instantiate actions with parent menu.
     */
    FWGUI_API virtual void createLayout( ::fwGui::fwMenu::sptr parent ) = 0;

    /**
     * @brief Destroy local actions.
     * @pre services using this actions must be stopped before.
     */
    FWGUI_API virtual void destroyLayout() = 0;


    /**
     * @brief Set the action visibility.
     */
    FWGUI_API virtual std::vector< ::fwGui::fwMenuItem::sptr > actionIsVisible(::fwGui::fwMenuItem::sptr, bool isVisible) = 0;

    /**
     * @brief Set the action enable or not.
     */
    FWGUI_API virtual std::vector< ::fwGui::fwMenuItem::sptr > actionIsEnabled(::fwGui::fwMenuItem::sptr, bool isEnabled) = 0;


protected:

    /**
     * @brief Helper to destroy local actions.
     */
    FWGUI_API virtual void destroyActions();

    /// All actions managed by this layout.
    std::vector< ::fwGui::fwMenuItem::sptr > m_menuItems;

    /// Save action informations from configuration.
    std::vector< ActionInfo > m_actionInfo;

};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_IMENULAYOUTMANAGER_HPP_*/


