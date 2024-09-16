/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include <data/image.hpp>
#include <data/map.hpp>
#include <data/transfer_function.hpp>

#include <ui/__/editor.hpp>

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
   <service type="sight::module::ui::qt::image::transfer_function">
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
 * - \b image [sight::data::image](optional): reference image that can be used to generate the default transfer
 * function.
 *
 * @subsection In-Out In-Out
 * - \b current [sight::data::transfer_function]: current transfer function used to change editor
 * selection. It should be the same as the output.
 * - \b presets [sight::data::map](optional): map of sight::data::transfer_function that should be used as
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
class transfer_function final : public QObject,
                                public sight::ui::editor
{
Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(module::ui::qt::image::transfer_function, sight::ui::editor);

    /// Creates the editor.
    transfer_function();

    /// Destroyes the editor.
    ~transfer_function() noexcept override = default;

private:

    /// Configures the editor.
    void configuring() override;

    /// Creates container and the UI.
    void starting() override;

    /// Does nothing.
    void updating() override;

    /// Destroys the UI.
    void stopping() override;

    /**
     * @brief
     * Connect image::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect image::BUFFER_MODIFIED_SIG to this::UPDATE_BUFFER_SLOT
     */
    connections_t auto_connections() const override;

    /**
     * @brief Checks if the map contains the specified key.
     * @param _presets list of transfer functions.
     * @param _name the name used to search.
     * @return True if the preset named _name is found.
     */
    static bool has_preset_name(const sight::data::map& _presets, const std::string& _name);

    /**
     * @brief Create a string that represents a TF preset name not already present in a preset list.
     *
     * For example, if CT-GreyLevel is already used, it will return CT-GreyLevel_1.
     *
     * @param _presets list of transfer functions.
     * @param _basename the name of the TF preset to create.
     * @return The new name of the TF preset.
     */
    static std::string create_preset_name(const sight::data::map& _presets, const std::string& _basename);

    /**
     * @brief Initializes the map.
     *
     * Add their names to m_presetComboBox. If the map does not contain any TF (or only the default grey level TF,
     * the service creates a few from the resources of the module.
     */
    void initialize_presets(
        const std::string& _current_preset_name = sight::data::transfer_function::DEFAULT_TF_NAME
    );

    /// Sets the current TF preset to the output of this service.
    void set_current_preset();

    /// Update the default transfer function according to the image
    void update_default_preset();

private Q_SLOTS:

    /// Changes the current selected TF preset.
    void preset_choice(int _index);

    /// Deletes the current selected TF preset.
    void delete_preset();

    /// Creates a new TF preset with a GreyLevel TF.
    void create_preset();

    /// Copies the current TF preset.
    void copy_preset();

    /// Resets the map.
    void reinitialize_presets();

    /// Renames the current selected TF preset.
    void rename_preset();

    /// Imports a TF preset.
    void import_preset();

    /// Exports the current selected TF preset.
    void export_preset();

private:

    /// Stores path were looking for TF presets.
    std::vector<std::filesystem::path> m_paths;

    /// Contains the list of all TF preset.
    QComboBox* m_preset_combo_box {nullptr};

    /// Contains the delete TF preset button.
    QPushButton* m_delete_button {nullptr};

    /// Contains the new TF preset button.
    QPushButton* m_new_button {nullptr};

    /// Contains the copy TF preset button.
    QPushButton* m_copy_button {nullptr};

    /// Contains the reset TF preset button.
    QPushButton* m_reinitialize_button {nullptr};

    /// Contains the rename TF preset button.
    QPushButton* m_rename_button {nullptr};

    /// Contains the import TF button.
    QPushButton* m_import_button {nullptr};

    /// Contains the export TF button.
    QPushButton* m_export_button {nullptr};

    /// Defines the path of the delete button icon.
    std::filesystem::path m_delete_icon;

    /// Defines the path of the new button icon.
    std::filesystem::path m_new_icon;

    /// Defines the path of the copy button icon.
    std::filesystem::path m_copy_icon;

    /// Defines the path of the reinitialize button icon.
    std::filesystem::path m_reinitialize_icon;

    /// Defines the path of the rename button icon.
    std::filesystem::path m_rename_icon;

    /// Defines the path of the import button icon.
    std::filesystem::path m_import_icon;

    /// Defines the path of the export button icon.
    std::filesystem::path m_export_icon;

    /// Defines icons width.
    unsigned int m_icon_width {16};

    /// Defines icons height.
    unsigned int m_icon_height {16};

    /// Working copy of the TF presets, can be internal or use the optional "presets" input
    data::map::sptr m_tf_presets;

    static constexpr std::string_view CURRENT_INPUT = "tf";
    static constexpr std::string_view IMAGE_INPUT   = "image";
    static constexpr std::string_view PRESETS_INOUT = "presets";
    data::ptr<data::transfer_function, data::access::inout> m_current_tf {this, CURRENT_INPUT};
    data::ptr<data::map, data::access::inout> m_opt_presets {this, PRESETS_INOUT, true};
    data::ptr<data::image, data::access::in> m_image {this, IMAGE_INPUT, true};
};

} // namespace sight::module::ui::qt::image.
