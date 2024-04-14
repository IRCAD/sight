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

#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <ui/__/action.hpp>

#include <filesystem>

namespace sight::module::ui::series
{

/**
 * @brief   This service is used to merge selected series with a series_set.
 *          All the series in the vector are pushed in the destination series_set.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::series::push_selection">
            <in key="selectedSeries" uid="..." />
            <inout key="seriesSet" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [sight::data::vector]: Vector containing the selected series to merge.
 * @subsection In-Out In-Out:
 * - \b seriesSet [sight::data::series_set]: series_set where to put the series.
 */
class push_selection : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(push_selection, sight::ui::action);

    /// Constructor
    push_selection() noexcept;

    /// Destructor
    ~push_selection() noexcept override;

protected:

    /// Do nothing.
    void configuring() override;

    /// Override
    void starting() override;

    /// Override
    void stopping() override;

    /// Override
    void updating() override;

    /// Override
    void info(std::ostream& _sstream) override;

private:

    data::ptr<data::series_set, sight::data::access::inout> m_series_set {this, "seriesSet"};
    data::ptr<data::vector, sight::data::access::in> m_selected_series {this, "selectedSeries"};
};

} // namespace sight::module::ui::series
