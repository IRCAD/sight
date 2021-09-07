/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <core/macros.hpp>

#include <data/Image.hpp>

#include <ui/base/IAction.hpp>

namespace sight::module::ui::base::metrics
{

/**
 * @brief This action adds distances on an image field.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::ui::base::metrics::SAddDistance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::Image]: Image containing the distance field.
 */
class MODULE_UI_BASE_CLASS_API SAddDistance final : public ::sight::ui::base::IAction
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SAddDistance, sight::ui::base::IAction);

    /// Creates the service.
    MODULE_UI_BASE_API SAddDistance() noexcept;

    /// Destroys the service.
    MODULE_UI_BASE_API ~SAddDistance() noexcept override;

protected:

    /// Configures the action.
    MODULE_UI_BASE_API void configuring() override;

    /// Starts the action.
    MODULE_UI_BASE_API void starting() override;

    /// Adds a new distance to the image field.
    MODULE_UI_BASE_API void updating() override;

    /// Stops the action.
    MODULE_UI_BASE_API void stopping() override;

private:

    data::ptr<data::Image, data::Access::inout> m_image {this, "image"};
};

} // namespace sight::module::ui::base::metrics
