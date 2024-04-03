/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/tools/failed.hpp>

#include <ui/__/frame.hpp>

namespace sight::module::ui
{

/**
 * @brief Defines the default frame for standard application.
 */
class frame : public sight::ui::frame
{
public:

    SIGHT_DECLARE_SERVICE(frame, sight::ui::frame);

    /// Constructor. Do nothing.
    frame() noexcept;

    /// Destructor. Do nothing.
    ~frame() noexcept override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     * @see sight::ui::frame::initialize()
     */
    void configuring() override;

    /**
     * @brief Register a view with defined id.
     * @see sight::ui::frame::create()
     */
    void starting() override;

    /**
     * @brief This method remove the view in the frame.
     * @see sight::ui::frame::destroy()
     */
    void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    void updating() override;

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    void info(std::ostream& _sstream) override;
    ///@}
};

} // namespace sight::module::ui
