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

#include <data/series.hpp>
#include <data/series_set.hpp>

#include <ui/__/exporter.hpp>

namespace sight::module::ui::series
{

/**
 * @brief   This action allows to export the series specified in config to the series_set.
 * The export is performed when updating the service.
 *
 * @note This action is not executable if the series already exists in the series_set.
 *
 * @section Slots Slots
 * - \b checkAddedObjects(data::series_set::container_type): make the action executable if the added series matches
 * the series we want to export.
 * - \b checkRemovedObjects(data::series_set::container_type): make the action inexecutable if the added series matches
 * the series we want to export.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::series::exporter" auto_connect="true">
            <inout key="data" uid="..." />
            <inout key="container" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b data [sight::data::series]: Source series to export.
 * - \b container [sight::data::series_set]: Target series database where the series should be exported.
 */
class MODULE_UI_CLASS_API exporter : public sight::ui::exporter<data::series_set>
{
public:

    SIGHT_DECLARE_SERVICE(exporter, sight::ui::exporter<data::series_set>);

    /// Constructor
    constexpr exporter() = default;

    /// Destructor
    inline ~exporter() noexcept override = default;

protected:

    /// Adds the series specified by m_seriesId in the series_set.
    void updating() override;

    void info(std::ostream& _sstream) override;
};

} // namespace sight::module::ui::series
