/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "model_series_list.hpp"

#include <core/com/signal.hxx>

#include <data/boolean.hpp>
#include <data/helper/field.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

namespace sight::module::ui::qml::model
{

const core::com::signals::key_t model_series_list::RECONSTRUCTION_SELECTED_SIG = "reconstruction_selected";
const core::com::signals::key_t model_series_list::EMPTIED_SELECTION_SIG       = "emptied_selection";

model_series_list::model_series_list() noexcept
{
    m_sig_reconstruction_selected = new_signal<reconstruction_selected_signal_t>(RECONSTRUCTION_SELECTED_SIG);
    m_sig_emptied_selection       = new_signal<emptied_selection_signal_t>(EMPTIED_SELECTION_SIG);
}

//------------------------------------------------------------------------------

model_series_list::~model_series_list() noexcept =
    default;

//------------------------------------------------------------------------------

void model_series_list::starting()
{
    sight::ui::qml::editor::starting();
}

//------------------------------------------------------------------------------

void model_series_list::stopping()
{
    m_list_model->update_model_series(nullptr);

    sight::ui::qml::editor::stopping();
}

//------------------------------------------------------------------------------

void model_series_list::configuring()
{
}

//------------------------------------------------------------------------------

void model_series_list::updating()
{
    auto model_series = m_model_series.lock();
    SIGHT_ASSERT("inout 'modelSeries' is missing", model_series);

    SIGHT_ASSERT("list model is not defined.", m_list_model);
    m_list_model->update_model_series(model_series.get_shared());
}

//------------------------------------------------------------------------------

void model_series_list::on_organ_selected(int _index)
{
    if(_index >= 0)
    {
        auto model_series = m_model_series.lock();
        SIGHT_ASSERT("'" << MODEL_SERIES_INOUT << "' must be defined as 'inout'", model_series);

        const auto& recs = model_series->get_reconstruction_db();

        const auto& selected_rec = recs.at(static_cast<std::size_t>(_index));
        m_sig_reconstruction_selected->async_emit(selected_rec);
    }
    else
    {
        m_sig_emptied_selection->async_emit();
    }
}

//------------------------------------------------------------------------------

void model_series_list::on_show_reconstructions(int _state)
{
    auto model_series = m_model_series.lock();
    SIGHT_ASSERT("'" << MODEL_SERIES_INOUT << "' must be defined as 'inout'", model_series);
    data::helper::field helper(model_series.get_shared());
    helper.add_or_swap("ShowReconstructions", std::make_shared<data::boolean>(_state == Qt::Unchecked));
    helper.notify();
}

//------------------------------------------------------------------------------

void model_series_list::on_organ_visibility_changed(int _index, bool _visible)
{
    if(_index >= 0)
    {
        auto model_series = m_model_series.lock();
        SIGHT_ASSERT("'" << MODEL_SERIES_INOUT << "' must be defined as 'inout'", model_series);

        const auto& recs         = model_series->get_reconstruction_db();
        const auto& selected_rec = recs.at(static_cast<std::size_t>(_index));

        if(selected_rec->get_is_visible() != _visible)
        {
            selected_rec->set_is_visible(_visible);

            data::reconstruction::visibility_modified_signal_t::sptr sig;
            sig = selected_rec->signal<data::reconstruction::visibility_modified_signal_t>(
                data::reconstruction::VISIBILITY_MODIFIED_SIG
            );
            sig->async_emit(_visible);
        }
    }
}

//------------------------------------------------------------------------------

void model_series_list::on_check_all_boxes(bool _checked)
{
    auto model_series = m_model_series.lock();
    SIGHT_ASSERT("'" << MODEL_SERIES_INOUT << "' must be defined as 'inout'", model_series);

    for(const auto& rec : model_series->get_reconstruction_db())
    {
        if(rec->get_is_visible() != _checked)
        {
            rec->set_is_visible(_checked);

            data::reconstruction::visibility_modified_signal_t::sptr sig;
            sig = rec->signal<data::reconstruction::visibility_modified_signal_t>(
                data::reconstruction::VISIBILITY_MODIFIED_SIG
            );
            sig->async_emit(_checked);
        }
    }

    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t model_series_list::auto_connections() const
{
    return {
        {MODEL_SERIES_INOUT, data::model_series::MODIFIED_SIG, service::slots::UPDATE},
        {MODEL_SERIES_INOUT, data::model_series::RECONSTRUCTIONS_ADDED_SIG, service::slots::UPDATE},
        {MODEL_SERIES_INOUT, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::model
