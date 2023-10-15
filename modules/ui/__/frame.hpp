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

#include "modules/ui/__/config.hpp"

#include <core/tools/failed.hpp>

#include <ui/__/frame.hpp>

namespace sight::module::ui
{

/**
 * @brief Defines the default frame for standard application.
 */
class MODULE_UI_CLASS_API frame : public sight::ui::frame
{
public:

    SIGHT_DECLARE_SERVICE(frame, sight::ui::frame);

    /// Constructor. Do nothing.
    MODULE_UI_API frame() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_API ~frame() noexcept override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     * @see sight::ui::frame::initialize()
     */
    MODULE_UI_API void configuring() override;

    /**
     * @brief Register a view with defined id.
     * @see sight::ui::frame::create()
     */
    MODULE_UI_API void starting() override;

    /**
     * @brief This method remove the view in the frame.
     * @see sight::ui::frame::destroy()
     */
    MODULE_UI_API void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    MODULE_UI_API void updating() override;

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    MODULE_UI_API void info(std::ostream& _sstream) override;
    ///@}
};

} // namespace sight::module::ui
