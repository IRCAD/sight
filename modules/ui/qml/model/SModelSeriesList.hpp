/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/ui/qml/config.hpp"
#include "modules/ui/qml/model/OrganListModel.hpp"

#include <ui/qml/IQmlEditor.hpp>

#include <QObject>

namespace sight::module::ui::qml::model
{

/**
 * @brief   SModelSeriesList service allows to manage the ModelSeries organs.
 *
 * @section Signals Signals
 * - \b reconstructionSelected(data::Object::sptr): this signal emits the selected reconstruction
 * - \b emptiedSelection(): this signal is emitted when no reconstruction is selected
 *
 * @section QSlots Qt Slots
 * - \b onOrganSelected(int): called when an organ is selected, the index represents the index of the selected
 *      reconstruction into the modelSeries
 * - \b onShowReconstructions(int): called to show/hide all the reconstructions
 * - \b onOrganVisibilityChanged(int, bool): called to show/hide a reconstruction
 * - \b onCheckAllBoxes(bool): called when the organ's check box are checked/unchecked and thus shown/hidden
 *
 * @section QML Qml Configuration
 *
 * @code{.qml}
    SModelSeriesList {
        id: modelSeriesList

        // @disable-check M16
        listModel: organListModel

        // @disable-check M16
        onStarted: {
            modelList.enabled = true
        }
    }
   @endcode
 *
 * @section Objects Required objects
 *
 * @subsection In-Out In-Out
 * - \b modelSeries [sight::data::ModelSeries]: model series containing the organs to list
 */
class MODULE_UI_QML_CLASS_API SModelSeriesList : public sight::ui::qml::IQmlEditor
{
Q_OBJECT
Q_PROPERTY(OrganListModel * listModel MEMBER m_listModel)

public:

    SIGHT_DECLARE_SERVICE(SModelSeriesList, ::sight::ui::qml::IQmlEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QML_API SModelSeriesList() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QML_API virtual ~SModelSeriesList() noexcept;

protected:

    /// Call the IQmlEditor::starting method.
    void starting() override;

    /// Call the IQmlEditor::stopping method.
    void stopping() override;

    /// Update the list model with the modelSeries items
    void updating() override;

    /// Do nothing
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ModelSeries::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG to this::s_UPDATE_SLOT
     */
    KeyConnectionsMap getAutoConnections() const override;

    static const core::com::Signals::SignalKeyType s_RECONSTRUCTION_SELECTED_SIG;
    typedef core::com::Signal<void (data::Object::sptr)> ReconstructionSelectedSignalType;

    static const core::com::Signals::SignalKeyType s_EMPTIED_SELECTION_SIG;
    typedef core::com::Signal<void ()> EmptiedSelectionSignalType;

protected Q_SLOTS:

    /// Called when an organ is selected, the index represents the index of the selected reconstruction into the
    // modelSeries
    void onOrganSelected(int index);

    /// Called when the reconstruction should be shown/hidden
    void onShowReconstructions(int state);

    /// Called when the visibility of an organ should change
    void onOrganVisibilityChanged(int index, bool visible);

    /// Called when all the reconstruction are checked/unchecked, and
    void onCheckAllBoxes(bool checked);

private:

    /// Signal emitted when a reconstruction is selected
    ReconstructionSelectedSignalType::sptr m_sigReconstructionSelected;

    /// Signal emitted when we clean the list
    EmptiedSelectionSignalType::sptr m_sigEmptiedSelection;

    OrganListModel* m_listModel;

    /// Model series data
    const service::key_t s_MODEL_SERIES_INOUT;
    data::ptr<data::ModelSeries, data::Access::inout> m_modelSeries {this, s_MODEL_SERIES_INOUT, true};
};

} // uiMedDataQml
