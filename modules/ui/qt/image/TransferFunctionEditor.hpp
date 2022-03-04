/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include "modules/ui/qt/config.hpp"

#include <data/Composite.hpp>
#include <data/TransferFunction.hpp>

#include <ui/base/IEditor.hpp>

#include <QObject>

#include <filesystem>

class QComboBox;
class QPushButton;
class QIcon;

namespace sight::module::ui::qt::image
{

/**
 * @deprecated this service will be removed in sight 22.0, please use STransferFunction instead.
 * @brief Editor to select a transfer function.
 *
 * The available transfer function are stored in a composite (transferFunctionPool), if it contains at most one
 * transfert function, this service will read transfer function from the given pathes.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::qt::image::TransferFunctionEditor">
       <in key="currentTF" uid="..." />
       <inout key="tfPool" uid="..." />
       <out key="tf" uid="..." />
       <config useDefaultPath="true">
           <path>....</path>
           <path>....</path>
           <path>....</path>
       </config>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b currentTF [sight::data::TransferFunction](optional) : current transfer function used to change editor selection.
 * It
 *      should be the same TF as the output.
 *      If it is not set, the default GreyLevel will be selected at start and the editor will not listen the change of
 *      TF in another service.
 * @subsection In-Out In-Out
 * - \b tfPool [sight::data::Composite]: composite containing the transfer function.
 * @subsection Output Output
 * - \b tf [sight::data::TransferFunction]: selected transfer function.
 * @subsection Configuration Configuration
 * - \b useDefaultPath (optional)(default = true): if true, load tf files from uiTF module.
 * - \b path (optional): path to a directory containing tf files.
 */
class MODULE_UI_QT_CLASS_API TransferFunctionEditor : public QObject,
                                                      public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(TransferFunctionEditor, sight::ui::base::IEditor);

    /// Basic constructor, do nothing.
    MODULE_UI_QT_API TransferFunctionEditor();

    /// Basic destructor, do nothing.
    MODULE_UI_QT_API virtual ~TransferFunctionEditor() noexcept;

protected:

    /// Configure the transfer function editor.
    MODULE_UI_QT_API void configuring() override;

    /// Start the TransferFunctionEditor, create Container, place in Buttons, ComboBox, Layout, and connect them.
    MODULE_UI_QT_API void starting() override;

    /// Update the TransferFunctionEditor, do nothing.
    MODULE_UI_QT_API void updating() override;

    /// Stop the TransferFunctionEditor, disconnect Buttons and Combo Box, delete them and clean the container.
    MODULE_UI_QT_API void stopping() override;

    /// Selects the current transfer function
    MODULE_UI_QT_API void swapping(std::string_view key) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_SLOT
     */
    MODULE_UI_QT_API KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Initialize the transfer functions.
     *
     * Add their names to the ComboBox. If the composite does not contain any TF (or only the default grey level TF,
     * the service creates a few from the ressources of the module.
     */
    MODULE_UI_QT_API void initTransferFunctions();

    /// Check if the image contain the specified TF.
    MODULE_UI_QT_API bool hasTransferFunctionName(const std::string& _sName) const;

    /// Create a string that represents a TF name not already present in the composite. For example, if "test" is
    /// already used, it will return "test_1".
    MODULE_UI_QT_API std::string createTransferFunctionName(const std::string& _sBasename) const;

    /// Update the output transferFunction with the selected TF in the ComboBox.
    MODULE_UI_QT_API void updateTransferFunction();

    /// Update the TF preset from the TF pool
    MODULE_UI_QT_API void updateTransferFunctionPreset();

private Q_SLOTS:

    void deleteTF();
    void newTF();
    void reinitializeTFPool();
    void renameTF();
    void importTF();
    void exportTF();
    void presetChoice(int index);

private:

    typedef std::vector<std::filesystem::path> PathContainerType;

    QComboBox* m_pTransferFunctionPreset;
    QPushButton* m_deleteButton;
    QPushButton* m_newButton;
    QPushButton* m_reinitializeButton;
    QPushButton* m_renameButton;
    QPushButton* m_importButton;
    QPushButton* m_exportButton;

    data::TransferFunction::sptr m_selectedTF;

    /// Paths of the tf files to load.
    PathContainerType m_paths;

    static constexpr std::string_view s_CURRENT_TF      = "currentTF";
    static constexpr std::string_view s_TF_POOL         = "tfPool";
    static constexpr std::string_view s_NEW_SELECTED_TF = "tf";

    data::ptr<data::TransferFunction, data::Access::in> m_currentTf {this, s_CURRENT_TF, false, true};
    data::ptr<data::Composite, data::Access::inout> m_tfPool {this, s_TF_POOL};
    data::ptr<data::TransferFunction, data::Access::out> m_newSelectedTf {this, s_NEW_SELECTED_TF};
};

} // namespace sight::module
