/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "uiTF/config.hpp"

#include <fwData/Composite.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <filesystem>
#include <QObject>

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
   <service type="::uiTF::TransferFunctionEditor">
       <in key="currentTF" uid="..." optional="yes" />
       <inout key="tfPool" uid="..." />
       <out key="tf" uid="..." />
       <config useDefaultPath="yes">
           <path>....</path>
           <path>....</path>
           <path>....</path>
       </config>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b currentTF [::fwData::TransferFunction](optional) : current transfer function used to change editor selection. It
 *      should be the same TF as the output.
 *      If it is not set, the default GreyLevel will be selected at start and the editor will not listen the change of
 *      TF in another service. Don't forget to set 'optional="yes"' when you use this input, otherwise the service will
 *      not start if a TF is not previously defined.
 * @subsection In-Out In-Out
 * - \b tfPool [::fwData::Composite]: composite containing the transfer function.
 * @subsection Output Output
 * - \b tf [::fwData::TransferFunction]: selected transfer function.
 * @subsection Configuration Configuration
 * - \b useDefaultPath (optional)(default = yes): if true, load tf files from uiTF bundle.
 * - \b path (optional): path to a directory containing tf files.
 */
class UITF_CLASS_API TransferFunctionEditor : public QObject,
                                              public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceMacro(TransferFunctionEditor, ::fwGui::editor::IEditor);

    /// Basic constructor, do nothing.
    UITF_API TransferFunctionEditor();

    /// Basic destructor, do nothing.
    UITF_API virtual ~TransferFunctionEditor() noexcept;

protected:

    /// Configure the transfer function editor.
    UITF_API virtual void configuring() override;

    /// Start the TransferFunctionEditor, create Container, place in Buttons, ComboBox, Layout, and connect them.
    UITF_API virtual void starting() override;

    /// Update the TransferFunctionEditor, do nothing.
    UITF_API virtual void updating() override;

    /// Stop the TransferFunctionEditor, disconnect Buttons and Combo Box, delete them and clean the container.
    UITF_API virtual void stopping() override;

    /// Selects the current transfer function
    UITF_API void swapping(const KeyType& key) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_SLOT
     */
    UITF_API virtual KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Initialize the transfer functions.
     *
     * Add their names to the ComboBox. If the composite does not contain any TF (or only the default grey level TF,
     * the service creates a few from the ressources of the Bundle.
     */
    UITF_API void initTransferFunctions();

    /// Check if the image contain the specified TF.
    UITF_API bool hasTransferFunctionName(const std::string& _sName) const;

    /// Create a string that represents a TF name not already present in the composite. For example, if blabla is
    /// already used, it will return blabla_1.
    UITF_API std::string createTransferFunctionName( const std::string& _sBasename ) const;

    /// Update the output transferFunction with the selected TF in the ComboBox.
    UITF_API void updateTransferFunction();

    /// Update the TF preset from the TF pool
    UITF_API void updateTransferFunctionPreset();

private Q_SLOTS:

    void deleteTF();
    void newTF();
    void reinitializeTFPool();
    void renameTF();
    void importTF();
    void exportTF();
    void presetChoice(int index);

private:

    typedef std::vector< std::filesystem::path > PathContainerType;

    QComboBox* m_pTransferFunctionPreset;
    QPushButton* m_deleteButton;
    QPushButton* m_newButton;
    QPushButton* m_reinitializeButton;
    QPushButton* m_renameButton;
    QPushButton* m_importButton;
    QPushButton* m_exportButton;

    ::fwData::TransferFunction::sptr m_selectedTF;

    /// Paths of the tf files to load.
    PathContainerType m_paths;
};

}
