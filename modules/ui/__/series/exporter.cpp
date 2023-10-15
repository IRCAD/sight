/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "exporter.hpp"

#include <core/tools/os.hpp>

namespace sight::module::ui::series
{

//------------------------------------------------------------------------------

void exporter::updating()
{
    auto data = m_data.lock();

    const auto& [description, ok] = sight::ui::dialog::input::showInputDialog(
        "Export series",
        "Enter the series description",
        data->getSeriesDescription()
    );

    if(ok && !description.empty())
    {
        data->setSeriesDescription(description);

        std::string physicians = data->getPerformingPhysicianName();
        if(physicians.empty())
        {
            physicians = core::tools::os::get_env("USERNAME", core::tools::os::get_env("LOGNAME", "Unknown"));
        }

        data->setPerformingPhysicianName(physicians);

        {
            auto container            = m_container.lock();
            const auto scoped_emitter = container->scoped_emit();
            container->push_back(data.get_shared());
        }

        this->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

void exporter::info(std::ostream& _sstream)
{
    // Update message
    _sstream << std::string("export");
}

} // namespace sight::module::ui::series
