/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "uiTF/config.hpp"

#include <fwData/Composite.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QObject>

#include <filesystem>

class QComboBox;
class QPushButton;
class QIcon;

namespace uiTF
{

/**
 * @brief Editor to select a transfer function.
 *
 * The available transfer function are stored in a composite (transferFunctionPool), if it contains at most one
 * transfert function, this service will read transfer function from the given pathes.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiTF::STransferFunction">
       <in key="currentTF" uid="..." optional="yes" />
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
 * - \b currentTF [::fwData::TransferFunction](optional): current transfer function used to change editor selection. It
 *      should be the same TF as the output.
 *      If it is not set, the default GreyLevel will be selected at start and the editor will not listen the change of
 *      TF in another service. Don't forget to set 'optional="yes"' when you use this input, otherwise the service will
 *      not start if a TF is not previously defined.
 * @subsection In-Out In-Out
 * - \b tfPool [::fwData::Composite]: composite containing the transfer function.
 * @subsection Output Output
 * - \b tf [::fwData::TransferFunction]: selected transfer function.
 * @subsection Configuration Configuration
 * - \b useDefaultPath (optional, default="true"): if true, load tf files from uiTF bundle.
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
class UITF_CLASS_API STransferFunction final : public QObject,
                                               public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceMacro(STransferFunction, ::fwGui::editor::IEditor)

    /// Does nothing.
    UITF_API STransferFunction();

    /// Does nothing.
    UITF_API virtual ~STransferFunction() noexcept;

private:

    /// Configures the transfer function editor.
    virtual void configuring() override;

    /// Starts the service, creates container, place in buttons, comboBox, layout, and connect them.
    virtual void starting() override;

    /// Does nothing.
    virtual void updating() override;

    /// Stops the service, disconnect buttons and combo box, delete them and clean the container.
    virtual void stopping() override;

    /**
     * @brief Selects the current transfer function.
     * @param _key key of the swapped data.
     */
    void swapping(const KeyType& _key) override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_SLOT.
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_SLOT.
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_SLOT.
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Initializes the transfer functions.
     *
     * Add their names to the ComboBox. If the composite does not contain any TF (or only the default grey level TF,
     * the service creates a few from the ressources of the Bundle.
     */
    void initTransferFunctions();

    /**
     * @brief Checks if the composite contain the specified TF.
     * @param _name the name used to search the TF.
     * @return True if the TF named _sName is found.
     */
    bool hasTransferFunctionName(const std::string& _name) const;

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
    QComboBox* m_pTransferFunctionPreset;

    /// Contains the delete TF button.
    QPushButton* m_deleteButton;

    /// Contains the new TF button.
    QPushButton* m_newButton;

    /// Contains the reset TF button.
    QPushButton* m_reinitializeButton;

    /// Contains the rename TF button.
    QPushButton* m_renameButton;

    /// Contains the import TF button.
    QPushButton* m_importButton;

    /// Contains the export TF button.
    QPushButton* m_exportButton;

    /// Contains the current selected TF.
    ::fwData::TransferFunction::sptr m_selectedTF;

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

}
