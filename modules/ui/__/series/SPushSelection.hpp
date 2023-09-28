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

#include <data/SeriesSet.hpp>
#include <data/Vector.hpp>

#include <ui/__/action.hpp>

#include <filesystem>

namespace sight::module::ui::series
{

/**
 * @brief   This service is used to merge selected series with a SeriesSet.
 *          All the series in the vector are pushed in the destination SeriesSet.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::series::SPushSelection">
            <in key="selectedSeries" uid="..." />
            <inout key="seriesSet" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [sight::data::Vector]: Vector containing the selected series to merge.
 * @subsection In-Out In-Out:
 * - \b seriesSet [sight::data::SeriesSet]: SeriesSet where to put the series.
 */
class MODULE_UI_CLASS_API SPushSelection : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(SPushSelection, sight::ui::action);

    /// Constructor
    MODULE_UI_API SPushSelection() noexcept;

    /// Destructor
    MODULE_UI_API ~SPushSelection() noexcept override;

protected:

    /// Do nothing.
    MODULE_UI_API void configuring() override;

    /// Override
    MODULE_UI_API void starting() override;

    /// Override
    MODULE_UI_API void stopping() override;

    /// Override
    MODULE_UI_API void updating() override;

    /// Override
    MODULE_UI_API void info(std::ostream& _sstream) override;

private:

    data::ptr<data::SeriesSet, sight::data::Access::inout> m_series_set {this, "seriesSet"};
    data::ptr<data::Vector, sight::data::Access::in> m_selectedSeries {this, "selectedSeries"};
};

} // namespace sight::module::ui::series
