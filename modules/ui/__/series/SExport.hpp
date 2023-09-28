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

#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <ui/__/exporter.hpp>

namespace sight::module::ui::series
{

/**
 * @brief   This action allows to export the series specified in config to the SeriesSet.
 * The export is performed when updating the service.
 *
 * @note This action is not executable if the series already exists in the SeriesSet.
 *
 * @section Slots Slots
 * - \b checkAddedObjects(data::SeriesSet::container_type): make the action executable if the added series matches
 * the series we want to export.
 * - \b checkRemovedObjects(data::SeriesSet::container_type): make the action inexecutable if the added series matches
 * the series we want to export.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::series::SExport" autoConnect="true">
            <inout key="data" uid="..." />
            <inout key="container" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b data [sight::data::Series]: Source series to export.
 * - \b container [sight::data::SeriesSet]: Target series database where the series should be exported.
 */
class MODULE_UI_CLASS_API SExport : public sight::ui::exporter<data::SeriesSet>
{
public:

    SIGHT_DECLARE_SERVICE(SExport, sight::ui::exporter<data::SeriesSet>);

    /// Constructor
    constexpr SExport() = default;

    /// Destructor
    inline ~SExport() noexcept override = default;

protected:

    /// Adds the series specified by m_seriesId in the SeriesSet.
    void updating() override;

    void info(std::ostream& _sstream) override;
};

} // namespace sight::module::ui::series
