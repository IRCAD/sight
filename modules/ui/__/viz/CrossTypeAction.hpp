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

#include <ui/__/action.hpp>

#include <map>
#include <string>

namespace sight::module::ui::viz
{

/**
 * @brief   This action allows change the cross type
 */
class MODULE_UI_CLASS_API CrossTypeAction : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(CrossTypeAction, sight::ui::action);

    /// Constructor. Do nothing.
    MODULE_UI_API CrossTypeAction() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_API ~CrossTypeAction() noexcept override = default;

protected:

    /// Starts action
    void starting() override;

    /// Stops action
    void stopping() override;

    /// Notifies message to change cross type
    void updating() override;

    /**
     * @brief Configure action.
     * @code{.xml}
       <service type="ui::action" impl="::uiData::CrossTypeAction" autoConnect="false">
        <crossType>hide</crossType>
       </service>
       @endcode
     * - \b crossType : must be full, half or hide
     */
    void configuring() override;

    void info(std::ostream& _sstream) override;

private:

    static std::map<std::string, float> m_scaleConversion;
    std::string m_crossType;

    typedef core::com::signal<void (double)> CrossTypeModifiedSignalType;
    CrossTypeModifiedSignalType::sptr m_sigCrossTypeModified; ///< Signal emitted when cross type is modified
};

} // namespace sight::module::ui::viz
