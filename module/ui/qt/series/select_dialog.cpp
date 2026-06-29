/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include "select_dialog.hpp"

namespace sight::module::ui::qt::series
{

//------------------------------------------------------------------------------

select_dialog::select_dialog()
{
    new_signal<signals::selected_t>(signals::IMAGE_SELECTED);
    new_signal<signals::selected_t>(signals::MODEL_SELECTED);
}

//------------------------------------------------------------------------------

void select_dialog::configuring()
{
}

//------------------------------------------------------------------------------

void select_dialog::starting()
{
}

//-----------------------------------------------------------------------------

void select_dialog::updating()
{
    const auto series_set = m_series_set.lock();

    SIGHT_THROW_IF("Missing input database series", !series_set);
    SIGHT_THROW_IF("The series set is empty, nothing can be extracted.", series_set->empty());

    // TODO: Prompt later to select the element to extract, now just take the first element
    SIGHT_INFO(
        "[select_dialog] Extracting the first element of the seriesBD, future development will prompt"
        " the user to pick a series"
    );

    auto first_element = series_set->front();

    if(auto model_series = std::dynamic_pointer_cast<sight::data::model_series>(first_element); model_series)
    {
        m_model_series = model_series;
        this->async_emit(signals::MODEL_SELECTED);
    }
    else if(auto image_series = std::dynamic_pointer_cast<sight::data::image_series>(first_element); image_series)
    {
        m_image = image_series;
        this->async_emit(signals::IMAGE_SELECTED);
    }
}

//-----------------------------------------------------------------------------

void select_dialog::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::series
