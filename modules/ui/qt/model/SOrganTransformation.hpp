/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/Matrix4.hpp>
#include <data/Reconstruction.hpp>

#include <QObject>
#include <QPointer>

#include <ui/base/IEditor.hpp>

#include <map>
#include <string>

class QListWidget;
class QPushButton;
class QListWidgetItem;
class QComboBox;
class QCheckBox;

namespace sight::module::ui::qt
{
namespace model
{

/**
 * @brief Display the organs list and allow an interactive selection to set the corresponding meshes in a composite
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::sight::module::ui::qt::model::SOrganTransformation">
       <inout key="modelSeries" uid="..." />
       <inout key="composite" uid="..." />
   </service>
   @endcode
 * @subsection InOut InOut
 * - \b modelSeries [sight::data::ModelSeries]: modelSeries to modify.
 * - \b composite [sight::data::Composite]: composite.
 */
class MODULE_UI_QT_CLASS_API SOrganTransformation : public QObject,
                                                    public sight::ui::base::IEditor
{
Q_OBJECT
public:

    fwCoreServiceMacro(SOrganTransformation, sight::ui::base::IEditor)

    /// constructor
    MODULE_UI_QT_API SOrganTransformation() noexcept;
    /// destructor
    MODULE_UI_QT_API virtual ~SOrganTransformation() noexcept;

protected:

    MODULE_UI_QT_API virtual void configuring() override;
    MODULE_UI_QT_API virtual void starting() override;
    MODULE_UI_QT_API virtual void stopping() override;
    MODULE_UI_QT_API virtual void swapping() override;
    MODULE_UI_QT_API virtual void updating() override;
    MODULE_UI_QT_API virtual void info( ::std::ostream& stream ) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ModelSeries::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_SLOT
     */
    MODULE_UI_QT_API virtual KeyConnectionsMap getAutoConnections() const override;

private Q_SLOTS:
    void onReconstructionCheck(QListWidgetItem* currentItem);
    void onResetClick();
    void onSaveClick();
    void onLoadClick();

    /// Slot to check all the organs of the list
    void onSelectAllChanged(int state);

private:

    void refresh();
    void notitfyTransformationMatrix(data::Matrix4::sptr aTransMat);

    /// Create the transformation in mesh field. This field is used in the adaptors to transform the mesh
    void addMeshTransform();

    // ReconstructionMapType
    typedef ::std::map< ::std::string, data::Reconstruction::sptr > ReconstructionMapType;
    typedef ::std::map< ::std::string, data::Matrix4::sptr> InnerMatMappingType;
    typedef ::std::map< ::std::string, InnerMatMappingType> SaveMappingType;

    ReconstructionMapType m_reconstructionMap;
    QPointer< QPushButton > m_saveButton;
    QPointer< QPushButton > m_loadButton;
    QPointer< QPushButton > m_resetButton;
    QPointer< QListWidget > m_reconstructionListBox;
    QPointer< QComboBox > m_saveSelectionComboBox;
    QPointer< QCheckBox > m_selectAllCheckBox;

    //variables for the functionalities of saving & loading
    SaveMappingType m_saveListing;
    unsigned int m_saveCount;
};

} // namespace model
} // namespace sight::module::ui::qt
