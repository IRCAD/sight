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

#include "module_gui/config.hpp"

#include <gui/IFrameSrv.hpp>

#include <core/tools/Failed.hpp>

namespace sight::modules::gui
{
namespace frame
{

/**
 * @brief Defines the default frame for standard application.
 */
class MODULE_GUI_CLASS_API SDefaultFrame : public ::sight::gui::IFrameSrv
{

public:

    fwCoreServiceMacro(SDefaultFrame, ::sight::gui::IFrameSrv)

    /// Constructor. Do nothing.
    MODULE_GUI_API SDefaultFrame() noexcept;

    /// Destructor. Do nothing.
    MODULE_GUI_API virtual ~SDefaultFrame() noexcept;

protected:

    /** @name Service methods ( override from services::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     * @see gui::IFrameSrv::initialize()
     */
    MODULE_GUI_API virtual void configuring() override;

    /**
     * @brief Register a view with defined id.
     * @see gui::IFrameSrv::create()
     */
    MODULE_GUI_API virtual void starting() override;

    /**
     * @brief This method remove the view in the frame.
     * @see gui::IFrameSrv::destroy()
     */
    MODULE_GUI_API virtual void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    MODULE_GUI_API virtual void updating() override;

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    MODULE_GUI_API virtual void info(std::ostream& _sstream ) override;
    ///@}

};

} // namespace frame
} // namespace sight::modules::gui
