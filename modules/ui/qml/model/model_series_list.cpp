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
#include <data/helper/Field.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

namespace sight::module::ui::qml::model
{

const core::com::signals::key_t model_series_list::RECONSTRUCTION_SELECTED_SIG = "reconstructionSelected";
const core::com::signals::key_t model_series_list::EMPTIED_SELECTION_SIG       = "emptiedSelection";

model_series_list::model_series_list() noexcept
{
    m_sigReconstructionSelected = new_signal<ReconstructionSelectedSignalType>(RECONSTRUCTION_SELECTED_SIG);
    m_sigEmptiedSelection       = new_signal<EmptiedSelectionSignalType>(EMPTIED_SELECTION_SIG);
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
    m_listModel->updateModelSeries(nullptr);

    sight::ui::qml::editor::stopping();
}

//------------------------------------------------------------------------------

void model_series_list::configuring()
{
}

//------------------------------------------------------------------------------

void model_series_list::updating()
{
    auto modelSeries = m_modelSeries.lock();
    SIGHT_ASSERT("inout 'modelSeries' is missing", modelSeries);

    SIGHT_ASSERT("list model is not defined.", m_listModel);
    m_listModel->updateModelSeries(modelSeries.get_shared());
}

//------------------------------------------------------------------------------

void model_series_list::onOrganSelected(int index)
{
    if(index >= 0)
    {
        auto modelSeries = m_modelSeries.lock();
        SIGHT_ASSERT("'" << s_MODEL_SERIES_INOUT << "' must be defined as 'inout'", modelSeries);

        const auto& recs = modelSeries->getReconstructionDB();

        const auto& selectedRec = recs.at(static_cast<std::size_t>(index));
        m_sigReconstructionSelected->async_emit(selectedRec);
    }
    else
    {
        m_sigEmptiedSelection->async_emit();
    }
}

//------------------------------------------------------------------------------

void model_series_list::onShowReconstructions(int state)
{
    auto modelSeries = m_modelSeries.lock();
    SIGHT_ASSERT("'" << s_MODEL_SERIES_INOUT << "' must be defined as 'inout'", modelSeries);
    data::helper::Field helper(modelSeries.get_shared());
    helper.addOrSwap("ShowReconstructions", std::make_shared<data::boolean>(state == Qt::Unchecked));
    helper.notify();
}

//------------------------------------------------------------------------------

void model_series_list::onOrganVisibilityChanged(int index, bool visible)
{
    if(index >= 0)
    {
        auto modelSeries = m_modelSeries.lock();
        SIGHT_ASSERT("'" << s_MODEL_SERIES_INOUT << "' must be defined as 'inout'", modelSeries);

        const auto& recs        = modelSeries->getReconstructionDB();
        const auto& selectedRec = recs.at(static_cast<std::size_t>(index));

        if(selectedRec->getIsVisible() != visible)
        {
            selectedRec->setIsVisible(visible);

            data::reconstruction::VisibilityModifiedSignalType::sptr sig;
            sig = selectedRec->signal<data::reconstruction::VisibilityModifiedSignalType>(
                data::reconstruction::VISIBILITY_MODIFIED_SIG
            );
            sig->async_emit(visible);
        }
    }
}

//------------------------------------------------------------------------------

void model_series_list::onCheckAllBoxes(bool checked)
{
    auto modelSeries = m_modelSeries.lock();
    SIGHT_ASSERT("'" << s_MODEL_SERIES_INOUT << "' must be defined as 'inout'", modelSeries);

    for(const auto& rec : modelSeries->getReconstructionDB())
    {
        if(rec->getIsVisible() != checked)
        {
            rec->setIsVisible(checked);

            data::reconstruction::VisibilityModifiedSignalType::sptr sig;
            sig = rec->signal<data::reconstruction::VisibilityModifiedSignalType>(
                data::reconstruction::VISIBILITY_MODIFIED_SIG
            );
            sig->async_emit(checked);
        }
    }

    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t model_series_list::auto_connections() const
{
    return {
        {s_MODEL_SERIES_INOUT, data::model_series::MODIFIED_SIG, service::slots::UPDATE},
        {s_MODEL_SERIES_INOUT, data::model_series::RECONSTRUCTIONS_ADDED_SIG, service::slots::UPDATE},
        {s_MODEL_SERIES_INOUT, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::model
