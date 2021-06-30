/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "SModelSeriesList.hpp"

#include <core/com/Signal.hxx>

#include <data/Boolean.hpp>
#include <data/helper/Field.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

namespace sight::module::ui::qml::model
{

const core::com::Signals::SignalKeyType SModelSeriesList::s_RECONSTRUCTION_SELECTED_SIG = "reconstructionSelected";
const core::com::Signals::SignalKeyType SModelSeriesList::s_EMPTIED_SELECTION_SIG       = "emptiedSelection";

const service::IService::KeyType s_MODEL_SERIES_INOUT = "modelSeries";

SModelSeriesList::SModelSeriesList() noexcept :
    m_listModel(nullptr)
{
    m_sigReconstructionSelected = newSignal<ReconstructionSelectedSignalType>(s_RECONSTRUCTION_SELECTED_SIG);
    m_sigEmptiedSelection       = newSignal<EmptiedSelectionSignalType>(s_EMPTIED_SELECTION_SIG);
}

//------------------------------------------------------------------------------

SModelSeriesList::~SModelSeriesList() noexcept
{
}

//------------------------------------------------------------------------------

void SModelSeriesList::starting()
{
    sight::ui::qml::IQmlEditor::starting();
}

//------------------------------------------------------------------------------

void SModelSeriesList::stopping()
{
    m_listModel->updateModelSeries(nullptr);

    sight::ui::qml::IQmlEditor::stopping();
}

//------------------------------------------------------------------------------

void SModelSeriesList::configuring()
{
}

//------------------------------------------------------------------------------

void SModelSeriesList::updating()
{
    auto modelSeries = m_modelSeries.lock();
    SIGHT_ASSERT("inout 'modelSeries' is missing", modelSeries);

    SIGHT_ASSERT("list model is not defined.", m_listModel);
    m_listModel->updateModelSeries(modelSeries.get_shared());
}

//------------------------------------------------------------------------------

void SModelSeriesList::onOrganSelected(int index)
{
    if(index >= 0)
    {
        auto modelSeries = m_modelSeries.lock();
        SIGHT_ASSERT("'" + s_MODEL_SERIES_INOUT + "' must be defined as 'inout'", modelSeries);

        const auto& recs = modelSeries->getReconstructionDB();

        const auto& selectedRec = recs.at(static_cast<size_t>(index));
        m_sigReconstructionSelected->asyncEmit(selectedRec);
    }
    else
    {
        m_sigEmptiedSelection->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onShowReconstructions(int state)
{
    auto modelSeries = m_modelSeries.lock();
    SIGHT_ASSERT("'" + s_MODEL_SERIES_INOUT + "' must be defined as 'inout'", modelSeries);
    data::helper::Field helper(modelSeries.get_shared());
    helper.addOrSwap("ShowReconstructions", data::Boolean::New(state == Qt::Unchecked));
    helper.notify();
}

//------------------------------------------------------------------------------

void SModelSeriesList::onOrganVisibilityChanged(int index, bool visible)
{
    if(index >= 0)
    {
        auto modelSeries = m_modelSeries.lock();
        SIGHT_ASSERT("'" + s_MODEL_SERIES_INOUT + "' must be defined as 'inout'", modelSeries);

        const auto& recs        = modelSeries->getReconstructionDB();
        const auto& selectedRec = recs.at(static_cast<size_t>(index));

        if(selectedRec->getIsVisible() != visible)
        {
            selectedRec->setIsVisible(visible);

            data::Reconstruction::VisibilityModifiedSignalType::sptr sig;
            sig = selectedRec->signal<data::Reconstruction::VisibilityModifiedSignalType>(
                data::Reconstruction::s_VISIBILITY_MODIFIED_SIG
            );
            sig->asyncEmit(visible);
        }
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCheckAllBoxes(bool checked)
{
    auto modelSeries = m_modelSeries.lock();
    SIGHT_ASSERT("'" + s_MODEL_SERIES_INOUT + "' must be defined as 'inout'", modelSeries);

    for(const auto& rec : modelSeries->getReconstructionDB())
    {
        if(rec->getIsVisible() != checked)
        {
            rec->setIsVisible(checked);

            data::Reconstruction::VisibilityModifiedSignalType::sptr sig;
            sig = rec->signal<data::Reconstruction::VisibilityModifiedSignalType>(
                data::Reconstruction::s_VISIBILITY_MODIFIED_SIG
            );
            sig->asyncEmit(checked);
        }
    }

    this->updating();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SModelSeriesList::getAutoConnections() const
{
    KeyConnectionsMap connections;

    // FIXME hack to support deprecated getObject() with any key
    auto modelSeries = m_modelSeries.lock();
    if(modelSeries)
    {
        connections.push(s_MODEL_SERIES_INOUT, data::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push(s_MODEL_SERIES_INOUT, data::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_UPDATE_SLOT);
        connections.push(s_MODEL_SERIES_INOUT, data::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);
    }

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::model
