/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/ui/base/config.hpp"

#include <service/IService.hpp>

#include <ui/base/IAction.hpp>

#include <boost/logic/tribool.hpp>

#include <vector>

namespace sight::module::ui::base
{

/**
 * @brief This action is used to modify the layout configuration.
 */
class MODULE_UI_BASE_CLASS_API SModifyLayout : public ::sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SModifyLayout, ::sight::ui::base::IAction);
    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_BASE_API SModifyLayout() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_BASE_API virtual ~SModifyLayout() noexcept;

protected:

    enum ActionType
    {
        MOVE,
        SHOW,
        HIDE,
        SHOW_OR_HIDE,
        ENABLE,
        DISABLE,
        DO_NOTHING
    };

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    MODULE_UI_BASE_API void info(std::ostream& _sstream) override;

    /**
     * @brief This method starts-updates or stops the specified services
     */
    MODULE_UI_BASE_API void updating() override;

    /**
     * @brief This method is used to configure the service parameters: specifies which views to show/hide/mode.
     *
     * @code{.xml}
       <config>
         <move uid="srv_uid" wid="new_view_wid" />
         <show wid="view_to_show" />
         <hide wid="view_to_hide />
         <show_or_hide wid="view_to_show_or_hide />
         <enable uid="action_to_enabled" />
         <disable uid="action_to_disabled" />
       </config>
       @endcode
     * - \b move configures the services to move in the new view given by wid
     * - \b show configures the element to show
     * - \b hide configures the element to hide
     * - \b show_or_hide configures the element to show or hide.
     *      If action is activated, element is shown else it is hidden.
     *
     * \<show\>, \<hide\> and \<show_or_hide\> tags can have 2 type of attribute :
     * - wid to show  or hide windows container (view)
     * - sid  to show or hide a gui container service (IEditor, IView, IRender...)
     *
     * The action in the menu/toolbar must be checkable (style="check").
     */
    MODULE_UI_BASE_API void configuring() override;

    MODULE_UI_BASE_API void starting() override;

    MODULE_UI_BASE_API void stopping() override;

private:

    /// < fwID, WIDContainer>
    typedef std::vector<std::pair<std::string, std::string> > MoveSrvVectType;
    /// map representing fwID's services and new associated wid parent container
    MoveSrvVectType m_moveSrv;

    /// < wid, showState>
    typedef std::vector<std::pair<std::string, ::boost::logic::tribool> > ShowSrvVectType;
    /// map representing wid container and show state
    ShowSrvVectType m_showSrvWid;

    /// map representing sid container and show state
    ShowSrvVectType m_showSrvSid;

    /// < fwID, enableState>
    typedef std::vector<std::pair<std::string, bool> > EnableSrvVectType;
    /// map representing fwID's services and enable state
    EnableSrvVectType m_enableSrv;
};

} // namespace sight::module::ui::base
