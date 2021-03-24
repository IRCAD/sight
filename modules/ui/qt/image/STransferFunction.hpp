/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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
 * @brief Editor to select a transfer function.
 *
 * The available transfer function are stored in a composite (transferFunctionPool), if it contains at most one
 * transfert function, this service will read transfer function from the given pathes.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::qt::image::STransferFunction">
       <in key="currentTF" uid="..." optional="true" />
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
 * - \b currentTF [sight::data::TransferFunction](optional): current transfer function used to change editor selection.
 * It
 *      should be the same TF as the output.
 *      If it is not set, the default GreyLevel will be selected at start and the editor will not listen the change of
 *      TF in another service. Don't forget to set 'optional="true"' when you use this input, otherwise the service will
 *
 *      not start if a TF is not previously defined.
 * @subsection In-Out In-Out
 * - \b tfPool [sight::data::Composite]: composite containing the transfer function.
 *
 * @subsection Output Output
 * - \b tf [sight::data::TransferFunction]: selected transfer function.
 *
 * @subsection Configuration Configuration
 * - \b useDefaultPath (optional, default="true"): if true, load tf files from uiTF module.
 * - \b path (optional): path to a directory containing tf files.
 * - \b deleteIcon (optional): path of the delete button icon.
 * - \b newIcon (optional): path of the new button icon.
 * - \b reinitializeIcon (optional): path of the reinitialize button icon.
 * - \b renameIcon (optional): path of the rename button icon.
 * - \b importIcon (optional): path of the import button icon.
 * - \b exportIcon (optional): path of the export button icon.
 * - \b iconWidth (optional, default="16"): icon width.
 * - \b iconHeight (optional, default="16"): icon height.
 */
class MODULE_UI_QT_CLASS_API STransferFunction final : public QObject,
                                                       public sight::ui::base::IEditor
{

Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(STransferFunction, sight::ui::base::IEditor)

    /// Does nothing.
    MODULE_UI_QT_API STransferFunction();

    /// Does nothing.
    MODULE_UI_QT_API ~STransferFunction() noexcept override;

protected:

    /// Configures the transfer function editor.
    MODULE_UI_QT_API void configuring() override;

    /// Starts the service, creates container, place in buttons, comboBox, layout, and connect them.
    MODULE_UI_QT_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_SLOT.
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_SLOT.
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_SLOT.
     */
    MODULE_UI_QT_API KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    MODULE_UI_QT_API void updating() override;

    /**
     * @brief Selects the current transfer function.
     * @param _key key of the swapped data.
     */
    MODULE_UI_QT_API void swapping(const KeyType& _key) override;

    /// Stops the service, disconnect buttons and combo box, delete them and clean the container.
    MODULE_UI_QT_API void stopping() override;

private:

    /**
     * @brief Initializes the transfer functions.
     *
     * Add their names to the ComboBox. If the composite does not contain any TF (or only the default grey level TF,
     * the service creates a few from the ressources of the module.
     */
    void initTransferFunctions();

    /**
     * @brief Checks if the composite contain the specified TF.
     * @param _name the name used to search the TF.
     * @return True if the TF named _sName is found.
     */
    bool hasTransferFunctionName(const std::string& _name, const data::Composite::csptr _poolTF = nullptr) const;

    /**
     * @brief Create a string that represents a TF name not already present in the composite.
     *
     * For example, if blabla is already used, it will return blabla_1.
     *
     * @param _basename the name of the TF to create.
     * @return The new name of the TF.
     */
    std::string createTransferFunctionName( const std::string& _basename) const;

    /// Updates the output transferFunction with the selected TF in the ComboBox.
    void updateTransferFunction();

    /// Updates the TF preset from the TF pool.
    void updateTransferFunctionPreset();

private Q_SLOTS:

    /// Deletes the current seleted TF.
    void deleteTF();

    /// Creates a new TF.
    void newTF();

    /// Resets the current selected TF.
    void reinitializeTFPool();

    /// Renames the current selected TF.
    void renameTF();

    /// Imports a TF.
    void importTF();

    /// Exports the current selected TF.
    void exportTF();

    /// Changes the current selected TF.
    void presetChoice(int index);

private:

    /// Contains the list of all TF preset.
    QComboBox* m_transferFunctionPreset { nullptr };

    /// Contains the delete TF button.
    QPushButton* m_deleteButton { nullptr };

    /// Contains the new TF button.
    QPushButton* m_newButton { nullptr };

    /// Contains the reset TF button.
    QPushButton* m_reinitializeButton { nullptr };

    /// Contains the rename TF button.
    QPushButton* m_renameButton { nullptr };

    /// Contains the import TF button.
    QPushButton* m_importButton { nullptr };

    /// Contains the export TF button.
    QPushButton* m_exportButton { nullptr };

    /// Contains the current selected TF.
    data::TransferFunction::sptr m_selectedTF { nullptr };

    /// Stores path were looking for TF presets.
    typedef std::vector< std::filesystem::path > PathContainerType;
    PathContainerType m_paths;

    /// Defines the path of the delete button icon.
    std::filesystem::path m_deleteIcon;

    /// Defines the path of the new button icon.
    std::filesystem::path m_newIcon;

    /// Defines the path of the reinitialize button icon.
    std::filesystem::path m_reinitializeIcon;

    /// Defines the path of the rename button icon.
    std::filesystem::path m_renameIcon;

    /// Defines the path of the import button icon.
    std::filesystem::path m_importIcon;

    /// Defines the path of the export button icon.
    std::filesystem::path m_exportIcon;

    /// Defines icons width.
    unsigned int m_iconWidth { 16 };

    /// Defines icons height.
    unsigned int m_iconHeight { 16 };

};

} // namespace sight::module::ui::qt::image.
