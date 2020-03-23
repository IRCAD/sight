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

#include <fwCore/macros.hpp>

#include <fwData/Composite.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QComboBox>
#include <QObject>
#include <QPushButton>

#include <filesystem>

namespace uiTF
{

/**
 * @brief Editor to select a transfer function pool.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiTF::SMultipleTF">
       <in key="currentTFPool" uid="..." optional="yes" />
       <inout key="tfPools" uid="..." />
       <out key="tfPool" uid="..." />
       <config useDefaultPath="true" tfPerPath="false" >
           <path>....</path>
           <path>....</path>
           <path>....</path>
       </config>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b currentTFPool [::fwData::Composite](optional): current transfer function pool used to change editor selection.
 *      It should be the same pool as the output.
 *      Don't forget to set 'optional="yes"' when you use this input, otherwise the service will not start if a
 *      pool is not previously defined.
 *
 * @subsection In-Out In-Out
 * - \b tfPools [::fwData::Composite]: composite containing transfer function pool.
 *
 * @subsection Output Output
 * - \b tfPool [::fwData::Composite]: selected transfer function pool.
 *
 * @subsection Configuration Configuration
 * - \b useDefaultPath (optional, default=true): if true, load tf files from uiTF bundle.
 * - \b path (optional): path to a directory containing tf files.
 * - \b tfPerPath (optional, default=false): if true, merge all TF contains in each path to a new composite.
 */
class UITF_CLASS_API SMultipleTF final :
    public QObject,
    public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceMacro(::uiTF::SMultipleTF, ::fwGui::editor::IEditor)

    /// Creates the editor.
    UITF_API SMultipleTF();

    /// Destroyes the editor.
    UITF_API virtual ~SMultipleTF() noexcept;

private:

    /// Configures the editor.
    virtual void configuring() override;

    /// Creates container and the UI.
    virtual void starting() override;

    /// Does nothing.
    virtual void updating() override;

    /// Destroyes the UI.
    virtual void stopping() override;

    /**
     * @brief Selects the current transfer function pool.
     * @param _key key of the swapped data.
     */
    virtual void swapping(const KeyType& _key) override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to ::uiTF::SMultipleTF::s_UPDATE_SLOT.
     * Connect Composite::s_CHANGED_OBJECTS_SIG to ::uiTF::SMultipleTF::s_UPDATE_SLOT.
     * Connect Composite::s_REMOVED_OBJECTS_SIG to ::uiTF::SMultipleTF::s_UPDATE_SLOT.
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Checks if the composite contains the specified key.
     * @param _name the name used to search.
     * @return True if the pool named _name is found.
     */
    bool hasPoolName(const std::string& _name) const;

    /**
     * @brief Create a string that represents a TF pool name not already present in the composite.
     *
     * For example, if blabla is already used, it will return blabla_1.
     *
     * @param _basename the name of the TF pool to create.
     * @return The new name of the TF pool.
     */
    std::string createPoolName(const std::string& _basename) const;

    /**
     * @brief Initializes the composite.
     *
     * Add their names to m_tfPoolsPreset. If the composite does not contain any TF (or only the default grey level TF,
     * the service creates a few from the ressources of the Bundle.
     *
     * @see updatePoolsPreset()
     */
    void initializePools();

    /// Updates the TF pool preset from the composite.
    void updatePoolsPreset();

    /// Changes the current selected TF pool.
    void presetChoice(int _index);

    /// Sets the current TF pool to the output of this service.
    void setCurrentPool();

    /// Deletes the current seleted TF pool.
    void deletePool();

    /// Creates a new TF pool with a GreyLevel TF.
    void newPool();

    /// Copies the current TF pool.
    void copyPool();

    /// Resets the composite.
    void reinitializePools();

    /// Renames the current selected TF pool.
    void renamePool();

    /// If true, all TF contains in each path will be merged in a TF pool, else, one TF pool will be create for each
    // files.
    bool m_tfPerPath { false };

    /// Stores path were looking for TF presets.
    std::vector< std::filesystem::path > m_paths;

    /// Contains the list of all TF preset.
    QComboBox* m_tfPoolsPreset;

    /// Contains the delete TF pool button.
    QPushButton* m_deleteButton;

    /// Contains the new TF pool button.
    QPushButton* m_newButton;

    /// Contains the copy TF pool button.
    QPushButton* m_copyButton;

    /// Contains the reset TF pool button.
    QPushButton* m_reinitializeButton;

    /// Contains the rename TF pool button.
    QPushButton* m_renameButton;

    /// Contains the current selected TF pool.
    ::fwData::Composite::sptr m_currentTFPool;

};

}
