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

#include "modules/ui/qt/config.hpp"

#include <data/composite.hpp>
#include <data/matrix4.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>

#include <map>
#include <string>

class QListWidget;
class QPushButton;
class QListWidgetItem;
class QComboBox;
class QCheckBox;

namespace sight::module::ui::qt::model
{

/**
 * @brief Display the organs list and allow an interactive selection to set the corresponding meshes in a composite
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::qt::model::organ_transformation">
       <inout key="modelSeries" uid="..." />
       <inout key="composite" uid="..." />
   </service>
   @endcode
 * @subsection InOut InOut
 * - \b modelSeries [sight::data::model_series]: modelSeries to modify.
 * - \b composite [sight::data::composite]: composite.
 */
class MODULE_UI_QT_CLASS_API organ_transformation : public QObject,
                                                    public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(organ_transformation, sight::ui::editor);

    /// constructor
    MODULE_UI_QT_API organ_transformation() noexcept;
    /// destructor
    MODULE_UI_QT_API ~organ_transformation() noexcept override;

protected:

    MODULE_UI_QT_API void configuring() override;
    MODULE_UI_QT_API void starting() override;
    MODULE_UI_QT_API void stopping() override;
    MODULE_UI_QT_API void updating() override;
    MODULE_UI_QT_API void info(std::ostream& stream) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ModelSeries::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect ModelSeries::RECONSTRUCTIONS_ADDED_SIG to this::service::slots::UPDATE
     * Connect ModelSeries::RECONSTRUCTIONS_REMOVED_SIG to this::service::slots::UPDATE
     * Connect Composite::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect Composite::ADDED_OBJECTS_SIG to this::service::slots::UPDATE
     * Connect Composite::REMOVED_OBJECTS_SIG to this::service::slots::UPDATE
     * Connect Composite::CHANGED_OBJECTS_SIG to this::service::slots::UPDATE
     */
    MODULE_UI_QT_API connections_t auto_connections() const override;

private Q_SLOTS:

    void onReconstructionCheck(QListWidgetItem* currentItem);
    void onResetClick();
    void onSaveClick();
    void onLoadClick();

    /// Slot to check all the organs of the list
    void onSelectAllChanged(int state);

private:

    void refresh();
    static void notifyTransformationMatrix(data::matrix4::sptr aTransMat);

    /// Create the transformation in mesh field. This field is used in the adaptors to transform the mesh
    void addMeshTransform();

    // ReconstructionMapType
    typedef std::map<std::string, data::reconstruction::sptr> ReconstructionMapType;
    typedef std::map<std::string, data::matrix4::sptr> InnerMatMappingType;
    typedef std::map<std::string, InnerMatMappingType> SaveMappingType;

    ReconstructionMapType m_reconstructionMap;
    QPointer<QPushButton> m_saveButton;
    QPointer<QPushButton> m_loadButton;
    QPointer<QPushButton> m_resetButton;
    QPointer<QListWidget> m_reconstructionListBox;
    QPointer<QComboBox> m_saveSelectionComboBox;
    QPointer<QCheckBox> m_selectAllCheckBox;

    //variables for the functionalities of saving & loading
    SaveMappingType m_saveListing;
    unsigned int m_saveCount {0};

    static constexpr std::string_view s_MODEL_SERIES = "modelSeries";
    static constexpr std::string_view s_COMPOSITE    = "composite";
    data::ptr<data::model_series, data::Access::inout> m_modelSeries {this, "modelSeries", true};
    data::ptr<data::composite, data::Access::inout> m_composite {this, "composite", true};
};

} // namespace sight::module::ui::qt::model
