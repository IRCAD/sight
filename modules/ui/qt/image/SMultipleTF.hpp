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

#include <core/macros.hpp>

#include <data/Composite.hpp>

#include <QComboBox>
#include <QObject>
#include <QPushButton>

#include <ui/base/IEditor.hpp>

#include <filesystem>

namespace sight::modules::ui::qt::image
{

/**
 * @brief Editor to select a transfer function pool.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::sight::modules::ui::qt::image::SMultipleTF">
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
 * - \b currentTFPool [sight::data::Composite](optional): current transfer function pool used to change editor
 * selection.
 *      It should be the same pool as the output.
 *      Don't forget to set 'optional="yes"' when you use this input, otherwise the service will not start if a
 *      pool is not previously defined.
 *
 * @subsection In-Out In-Out
 * - \b tfPools [sight::data::Composite]: composite containing transfer function pool.
 *
 * @subsection Output Output
 * - \b tfPool [sight::data::Composite]: selected transfer function pool.
 *
 * @subsection Configuration Configuration
 * - \b useDefaultPath (optional, default="true"): if true, load tf files from uiTF module.
 * - \b path (optional): path to a directory containing tf files.
 * - \b tfPerPath (optional, default="false"): if true, merge all TF contained in each path to a new composite
 *                                             (only works for basic TF, multiple TF are read as is).
 * - \b deleteIcon (optional): path of the delete button icon.
 * - \b newIcon (optional): path of the new button icon.
 * - \b copyIcon (optional): path of the copy button icon.
 * - \b reinitializeIcon (optional): path of the reinitialize button icon.
 * - \b renameIcon (optional): path of the rename button icon.
 * - \b importIcon (optional): path of the import button icon.
 * - \b exportIcon (optional): path of the export button icon.
 * - \b iconWidth (optional, default="16"): icon width.
 * - \b iconHeight (optional, default="16"): icon height.
 */
class MODULE_UI_QT_CLASS_API SMultipleTF final :
    public QObject,
    public sight::ui::base::IEditor
{

Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(modules::ui::qt::image::SMultipleTF, sight::ui::base::IEditor)

    /// Creates the editor.
    MODULE_UI_QT_API SMultipleTF();

    /// Destroyes the editor.
    MODULE_UI_QT_API ~SMultipleTF() noexcept override;

protected:

    /// Configures the editor.
    MODULE_UI_QT_API void configuring() override;

    /// Creates container and the UI.
    MODULE_UI_QT_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Composite::s_ADDED_OBJECTS_SIG to modules::ui::qt::image::SMultipleTF::s_UPDATE_SLOT.
     * Connect data::Composite::s_CHANGED_OBJECTS_SIG to modules::ui::qt::image::SMultipleTF::s_UPDATE_SLOT.
     * Connect data::Composite::s_REMOVED_OBJECTS_SIG to modules::ui::qt::image::SMultipleTF::s_UPDATE_SLOT.
     */
    MODULE_UI_QT_API KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    MODULE_UI_QT_API void updating() override;

    /**
     * @brief Selects the current transfer function pool.
     * @param _key key of the swapped data.
     */
    MODULE_UI_QT_API void swapping(const KeyType& _key) override;

    /// Destroyes the UI.
    MODULE_UI_QT_API void stopping() override;

private:

    /**
     * @brief Checks if the composite contains the specified key.
     * @param _name the name used to search.
     * @param _tfPools the inout of this service, this parameter avoid dead lock.
     * @return True if the pool named _name is found.
     */
    bool hasPoolName(const std::string& _name, data::Composite::csptr _tfPools) const;

    /**
     * @brief Create a string that represents a TF pool name not already present in the composite.
     *
     * For example, if CT-GreyLevel is already used, it will return CT-GreyLevel_1.
     *
     * @param _basename the name of the TF pool to create.
     * @param _tfPools the inout of this service, this parameter avoid dead lock.
     * @return The new name of the TF pool.
     */
    std::string createPoolName(const std::string& _basename, data::Composite::csptr _tfPools) const;

    /**
     * @brief Initializes the composite.
     *
     * Add their names to m_tfPoolsPreset. If the composite does not contain any TF (or only the default grey level TF,
     * the service creates a few from the ressources of the module.
     *
     * @see updatePoolsPreset()
     */
    void initializePools();

    /// Updates the TF pool preset from the composite.
    void updatePoolsPreset();

    /// Sets the current TF pool to the output of this service.
    void setCurrentPool();

private Q_SLOTS:

    /// Changes the current selected TF pool.
    void presetChoice(int _index);

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

    /// Imports a TF pool.
    void importPool();

    /// Exports the current selected TF pool.
    void exportPool();

private:

    /// If true, all TF contains in each path will be merged in a TF pool, else, one TF pool will be create for each
    /// files.
    bool m_tfPerPath { false };

    /// Stores path were looking for TF presets.
    std::vector< std::filesystem::path > m_paths;

    /// Contains the list of all TF preset.
    QComboBox* m_tfPoolsPreset { nullptr };

    /// Contains the delete TF pool button.
    QPushButton* m_deleteButton { nullptr };

    /// Contains the new TF pool button.
    QPushButton* m_newButton { nullptr };

    /// Contains the copy TF pool button.
    QPushButton* m_copyButton { nullptr };

    /// Contains the reset TF pool button.
    QPushButton* m_reinitializeButton { nullptr };

    /// Contains the rename TF pool button.
    QPushButton* m_renameButton { nullptr };

    /// Contains the import TF button.
    QPushButton* m_importButton { nullptr };

    /// Contains the export TF button.
    QPushButton* m_exportButton { nullptr };

    /// Contains the current selected TF pool.
    data::Composite::sptr m_currentTFPool { nullptr };

    /// Defines the path of the delete button icon.
    std::filesystem::path m_deleteIcon;

    /// Defines the path of the new button icon.
    std::filesystem::path m_newIcon;

    /// Defines the path of the copy button icon.
    std::filesystem::path m_copyIcon;

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

} // namespace sight::modules::ui::qt::image.
