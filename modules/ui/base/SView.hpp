/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/tools/Failed.hpp>

#include <ui/base/view/IView.hpp>

namespace sight::module::ui::base
{

/**
 * @brief Defines the generic layout for application.
 * @see sight::ui::base::IGuiContainer for the service configuration
 */
class MODULE_UI_BASE_CLASS_API SView : public sight::ui::base::view::IView
{
public:

    SIGHT_DECLARE_SERVICE(SView, sight::ui::base::view::IView);

    /// Constructor. Do nothing.
    MODULE_UI_BASE_API SView() noexcept = default;

    /// Destructor. Do nothing.
    MODULE_UI_BASE_API ~SView() noexcept override = default;

protected:

    /**
     * @name Service methods ( override from service::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     * @see sight::ui::base::IGuiContainer::initialize()
     */
    MODULE_UI_BASE_API void configuring() override;

    /**
     * @brief Register a view with defined id.
     * @see sight::ui::base::IGuiContainer::create()
     */
    MODULE_UI_BASE_API void starting() override;

    /**
     * @brief This method remove the view in the frame.
     * @see sight::ui::base::IGuiContainer::destroy()
     */
    MODULE_UI_BASE_API void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    MODULE_UI_BASE_API void updating() override;

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    MODULE_UI_BASE_API void info(std::ostream& _sstream) override;
    ///@}
};

} // namespace sight::module::ui::base
