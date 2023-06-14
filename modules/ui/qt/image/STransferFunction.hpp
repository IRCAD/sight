/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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
#include <data/Image.hpp>
#include <data/TransferFunction.hpp>

#include <ui/base/IEditor.hpp>

#include <QComboBox>
#include <QObject>
#include <QPushButton>

#include <filesystem>

namespace sight::module::ui::qt::image
{

/**
 * @brief Editor to select a transfer function from a list of presets.
 *
 * The default presets are dedicated to CT and MRI images. Other locations can be specified to add different presets.
 * The editor also generates a default transfer function called "GreyLevel". If a reference image is specified,
 * this function will be generated accordingly to the image type.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::qt::image::STransferFunction">
       <in key="image" uid="..." />
       <inout key="presets" uid="..." />
       <inout key="tf" uid="..." />
       <config useDefaultPath="true" >
           <path>....</path>
           <path>....</path>
           <path>....</path>
       </config>
   </service>
   @endcode
 *
 *
 * @subsection Input Input
 * - \b image [sight::data::Image](optional): reference image that can be used to generate the default transfer
 * function.
 *
 * @subsection In-Out In-Out
 * - \b current [sight::data::TransferFunction]: current transfer function used to change editor
 * selection. It should be the same as the output.
 * - \b presets [sight::data::Composite](optional): map of sight::data::TransferFunction that should be used as
 * presets, instead of loading it from the specified path(s).
 *
 * @subsection Configuration Configuration
 * - \b useDefaultPath (optional, default="true"): if true, load tf files from uiTF module.
 * - \b path (optional): path to a directory containing tf files.
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
class MODULE_UI_QT_CLASS_API STransferFunction final : public QObject,
                                                       public sight::ui::base::IEditor
{
Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(module::ui::qt::image::STransferFunction, sight::ui::base::IEditor);

    /// Creates the editor.
    MODULE_UI_QT_API STransferFunction();

    /// Destroyes the editor.
    MODULE_UI_QT_API ~STransferFunction() noexcept override = default;

private:

    /// Configures the editor.
    MODULE_UI_QT_API void configuring() override;

    /// Creates container and the UI.
    MODULE_UI_QT_API void starting() override;

    /// Does nothing.
    MODULE_UI_QT_API void updating() override;

    /// Destroys the UI.
    MODULE_UI_QT_API void stopping() override;

    /**
     * @brief
     * Connect Image::s_MODIFIED_SIG to this::IService::slots::s_UPDATE
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_BUFFER_SLOT
     */
    MODULE_UI_QT_API KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Checks if the composite contains the specified key.
     * @param _presets list of transfer functions.
     * @param _name the name used to search.
     * @return True if the preset named _name is found.
     */
    static bool hasPresetName(const sight::data::Composite& _presets, const std::string& _name);

    /**
     * @brief Create a string that represents a TF preset name not already present in a preset list.
     *
     * For example, if CT-GreyLevel is already used, it will return CT-GreyLevel_1.
     *
     * @param _presets list of transfer functions.
     * @param _basename the name of the TF preset to create.
     * @return The new name of the TF preset.
     */
    std::string createPresetName(const sight::data::Composite& _presets, const std::string& _basename) const;

    /**
     * @brief Initializes the composite.
     *
     * Add their names to m_presetComboBox. If the composite does not contain any TF (or only the default grey level TF,
     * the service creates a few from the resources of the module.
     */
    void initializePresets(const std::string& _currentPresetName = sight::data::TransferFunction::s_DEFAULT_TF_NAME);

    /// Sets the current TF preset to the output of this service.
    void setCurrentPreset();

    /// Update the default transfer function according to the image
    void updateDefaultPreset();

private Q_SLOTS:

    /// Changes the current selected TF preset.
    void presetChoice(int _index);

    /// Deletes the current selected TF preset.
    void deletePreset();

    /// Creates a new TF preset with a GreyLevel TF.
    void createPreset();

    /// Copies the current TF preset.
    void copyPreset();

    /// Resets the composite.
    void reinitializePresets();

    /// Renames the current selected TF preset.
    void renamePreset();

    /// Imports a TF preset.
    void importPreset();

    /// Exports the current selected TF preset.
    void exportPreset();

private:

    /// Stores path were looking for TF presets.
    std::vector<std::filesystem::path> m_paths;

    /// Contains the list of all TF preset.
    QComboBox* m_presetComboBox {nullptr};

    /// Contains the delete TF preset button.
    QPushButton* m_deleteButton {nullptr};

    /// Contains the new TF preset button.
    QPushButton* m_newButton {nullptr};

    /// Contains the copy TF preset button.
    QPushButton* m_copyButton {nullptr};

    /// Contains the reset TF preset button.
    QPushButton* m_reinitializeButton {nullptr};

    /// Contains the rename TF preset button.
    QPushButton* m_renameButton {nullptr};

    /// Contains the import TF button.
    QPushButton* m_importButton {nullptr};

    /// Contains the export TF button.
    QPushButton* m_exportButton {nullptr};

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
    unsigned int m_iconWidth {16};

    /// Defines icons height.
    unsigned int m_iconHeight {16};

    /// Working copy of the TF presets, can be internal or use the optional "presets" input
    data::Composite::sptr m_tfPresets;

    static constexpr std::string_view s_CURRENT_INPUT = "tf";
    static constexpr std::string_view s_IMAGE_INPUT   = "image";
    static constexpr std::string_view s_PRESETS_INOUT = "presets";
    data::ptr<data::TransferFunction, data::Access::inout> m_currentTF {this, s_CURRENT_INPUT, true};
    data::ptr<data::Composite, data::Access::inout> m_optPresets {this, s_PRESETS_INOUT, true, true};
    data::ptr<data::Image, data::Access::in> m_image {this, s_IMAGE_INPUT, true, true};
};

} // namespace sight::module::ui::qt::image.
