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

#include <core/base.hpp>

#include <ui/base/IToolBarSrv.hpp>

namespace sight::modules::ui::base
{
namespace aspect
{

/**
 * @brief   Defines the default toolbar for standard application
 */
class MODULE_UI_BASE_CLASS_API SDefaultToolBar : public ::sight::ui::base::IToolBarSrv
{

public:

    fwCoreServiceMacro(SDefaultToolBar, ::sight::ui::base::IToolBarSrv)

    /// Constructor. Do nothing.
    MODULE_UI_BASE_API SDefaultToolBar() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_BASE_API virtual ~SDefaultToolBar() noexcept;

protected:

    /** @name Service methods ( override from service::IService )
     * @{
     */

    /**
     * @brief This method create a new ToolBar in frame.
     */

    /**
     * @brief Create the toolBar
     * @see sight::ui::base::IToolBarSrv::create()
     */
    MODULE_UI_BASE_API virtual void starting() override;

    /**
     * @brief This method remove the tools situated in the ToolBar.
     * @see sight::ui::base::IToolBarSrv::destroy()
     */
    MODULE_UI_BASE_API virtual void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    MODULE_UI_BASE_API virtual void updating() override;

    /**
     * @brief Configuring method allows to create a toolBar with several actions.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="toolbar2" type="::ui::base::IToolBarSrv" impl="::sight::modules::ui::base::aspect::SDefaultToolBar"
     * autoConnect="no"
     *>
           <gui>
               <layout>
                   <menuItem name="My item 2" style="radio" icon="TutoGui-0.1/icons/system.png"/>
                   <menuItem name="My item 3" style="radio" icon="TutoGui-0.1/icons/system.png"/>
                   <separator />
                   <menuItem name="My item A" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
                   <menuItem name="My item B" style="radio" icon="TutoGui-0.1/icons/monkey.png"/>
                   <separator />
                   <menu name="My menu" />
                   <separator />
                   <editor />
               </layout>
           </gui>
           <registry>
               <menuItem sid="item2" />
               <menuItem sid="item3" />
               <menuItem sid="item4" />
               <menuItem sid="item5" />
               <menu sid="menu" />
               <editor sid="editor" />
           </registry>
       </service>
       @endcode
     *   - \<gui\> \</gui\> : (mandatory) describe the interface of the service.
     *   - \<registry\> \</registry\> : (mandatory) describe the service management.
     *     - \<menuItem\> represents IActionSrv
     *     - \<menu\> represents IMenuSrv
     *     - \<editor\> represents container service (IEditor, IView, ...)
     *
     *   @warning
     *   - The number of item in the gui section must be EQUAL to the registry section.
     *   - The order of the item in each section (gui and registry) must be the same.\n
     *  For example: the item named "My item 2" will be connected with the service which have the sid = "item2".
     *   - A toolbar can't have the same service connected on two different buttons.
     *
     *  @see sight::ui::base::IToolBarSrv::initialize(),
     * sight::ui::base::layoutManager::IToolBarLayoutManager::initialize()
     */
    MODULE_UI_BASE_API virtual void configuring() override;

    /**
     * @brief This method is used to give information about the service. Do nothing.
     */
    MODULE_UI_BASE_API virtual void info(std::ostream& _sstream ) override;
    ///@}

};

}

}
