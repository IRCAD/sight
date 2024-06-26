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

#include <data/set.hpp>

#include <ui/__/exporter.hpp>

namespace sight::module::ui
{

/**
 * @brief   This action allows to export an object specified in config to a container.
 * The export is performed when updating the service.
 *
 * @note This action is not executable if the object already exists in the container.
 *
 * @section Slots Slots
 * - \b checkAddedObjects(data::set::container_t): make the action executable if the added objects matches
 * the object we want to export.
 * - \b checkRemovedObjects(data::set::container_t): make the action inexecutable if the added objects matches
 * the object we want to export.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::export_set" auto_connect="true">
            <inout key="data" uid="..." />
            <inout key="container" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b data [sight::data::object]: Source object to export.
 * - \b container [sight::data::set]: Target set where the object should be exported.
 */
class export_set : public sight::ui::exporter<data::set>
{
public:

    SIGHT_DECLARE_SERVICE(export_set, sight::ui::exporter<data::set>);

    /// Constructor
    constexpr export_set() = default;

    /// Destructor
    inline ~export_set() noexcept override = default;

protected:

    void info(std::ostream& _sstream) override;
};

} // namespace sight::module::ui
