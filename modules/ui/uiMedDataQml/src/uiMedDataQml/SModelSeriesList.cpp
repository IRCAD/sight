/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
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

#include "uiMedDataQml/SModelSeriesList.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/helper/Field.hpp>

#include <fwMedData/ModelSeries.hpp>

namespace uiMedDataQml
{

const ::fwCom::Signals::SignalKeyType SModelSeriesList::s_RECONSTRUCTION_SELECTED_SIG = "reconstructionSelected";
const ::fwCom::Signals::SignalKeyType SModelSeriesList::s_EMPTIED_SELECTION_SIG       = "emptiedSelection";

const ::fwServices::IService::KeyType s_MODEL_SERIES_INOUT = "modelSeries";

SModelSeriesList::SModelSeriesList() noexcept :
    m_listModel(nullptr)
{
    m_sigReconstructionSelected = newSignal< ReconstructionSelectedSignalType >( s_RECONSTRUCTION_SELECTED_SIG );
    m_sigEmptiedSelection       = newSignal< EmptiedSelectionSignalType >( s_EMPTIED_SELECTION_SIG );

    this->registerObject(s_MODEL_SERIES_INOUT, AccessType::INOUT, true);
}

//------------------------------------------------------------------------------

SModelSeriesList::~SModelSeriesList() noexcept
{

}

//------------------------------------------------------------------------------

void SModelSeriesList::starting()
{
    ::fwQml::IQmlEditor::starting();
}

//------------------------------------------------------------------------------

void SModelSeriesList::stopping()
{
    m_listModel->updateModelSeries(nullptr);

    ::fwQml::IQmlEditor::stopping();
}

//------------------------------------------------------------------------------

void SModelSeriesList::configuring()
{

}

//------------------------------------------------------------------------------

void SModelSeriesList::updating()
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getInOut< ::fwMedData::ModelSeries >(s_MODEL_SERIES_INOUT);
    SLM_ASSERT("inout 'modelSeries' is missing", modelSeries);

    SLM_ASSERT("list model is not defined.", m_listModel);
    m_listModel->updateModelSeries(modelSeries);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onOrganSelected(int index)
{
    if (index >= 0)
    {
        ::fwMedData::ModelSeries::sptr modelSeries = this->getInOut< ::fwMedData::ModelSeries >(s_MODEL_SERIES_INOUT);
        SLM_ASSERT("'" + s_MODEL_SERIES_INOUT+ "' must be defined as 'inout'", modelSeries);

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

void SModelSeriesList::onShowReconstructions(int state )
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getInOut< ::fwMedData::ModelSeries >(s_MODEL_SERIES_INOUT);
    SLM_ASSERT("'" + s_MODEL_SERIES_INOUT+ "' must be defined as 'inout'", modelSeries);
    ::fwDataTools::helper::Field helper( modelSeries );
    helper.addOrSwap("ShowReconstructions", ::fwData::Boolean::New(state == Qt::Unchecked));
    helper.notify();
}

//------------------------------------------------------------------------------

void SModelSeriesList::onOrganVisibilityChanged(int index, bool visible)
{
    if (index >= 0)
    {
        ::fwMedData::ModelSeries::sptr modelSeries = this->getInOut< ::fwMedData::ModelSeries >(s_MODEL_SERIES_INOUT);
        SLM_ASSERT("'" + s_MODEL_SERIES_INOUT+ "' must be defined as 'inout'", modelSeries);

        const auto& recs        = modelSeries->getReconstructionDB();
        const auto& selectedRec = recs.at(static_cast<size_t>(index));

        if (selectedRec->getIsVisible() != visible)
        {
            selectedRec->setIsVisible(visible);

            ::fwData::Reconstruction::VisibilityModifiedSignalType::sptr sig;
            sig = selectedRec->signal< ::fwData::Reconstruction::VisibilityModifiedSignalType >(
                ::fwData::Reconstruction::s_VISIBILITY_MODIFIED_SIG);
            sig->asyncEmit(visible);
        }
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCheckAllBoxes( bool checked )
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getInOut< ::fwMedData::ModelSeries >(s_MODEL_SERIES_INOUT);
    SLM_ASSERT("'" + s_MODEL_SERIES_INOUT+ "' must be defined as 'inout'", modelSeries);

    for (const auto& rec : modelSeries->getReconstructionDB())
    {
        if (rec->getIsVisible() != checked)
        {
            rec->setIsVisible(checked);

            ::fwData::Reconstruction::VisibilityModifiedSignalType::sptr sig;
            sig = rec->signal< ::fwData::Reconstruction::VisibilityModifiedSignalType >(
                ::fwData::Reconstruction::s_VISIBILITY_MODIFIED_SIG);
            sig->asyncEmit(checked);
        }
    }
    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SModelSeriesList::getAutoConnections() const
{
    KeyConnectionsMap connections;

    // FIXME hack to support deprecated getObject() with any key
    if (this->getInOut< ::fwMedData::ModelSeries >(s_MODEL_SERIES_INOUT))
    {
        connections.push(s_MODEL_SERIES_INOUT, ::fwMedData::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push(s_MODEL_SERIES_INOUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_UPDATE_SLOT);
        connections.push(s_MODEL_SERIES_INOUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);
    }

    return connections;
}

//------------------------------------------------------------------------------

} // namespace uiMedDataQml
