/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <ui/__/action.hpp>

#include <map>
#include <string>

namespace sight::module::ui::viz
{

/**
 * @brief   This action allows change the cross type
 */
class cross_type_action : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(cross_type_action, sight::ui::action);

    struct signals
    {
        using cross_type_modified_t = core::com::signal<void (double)>;
        static inline const signal_key_t CROSS_TYPE_MODIFIED = "crossTypeModified";
    };

    /// Constructor. Do nothing.
    cross_type_action() noexcept;

    /// Destructor. Do nothing.
    ~cross_type_action() noexcept override = default;

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
       <service type="ui::action" impl="::uiData::cross_type_action" auto_connect="false">
        <crossType>hide</crossType>
       </service>
       @endcode
     * - \b crossType : must be full, half or hide
     */
    void configuring() override;

    void info(std::ostream& _sstream) override;

private:

    std::string m_cross_type;
};

} // namespace sight::module::ui::viz
