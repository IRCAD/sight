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

#include "modules/ui/qt/image/transfer_function.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <data/composite.hpp>
#include <data/transfer_function.hpp>

#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <QBoxLayout>

namespace sight::module::ui::qt::image
{

static const std::string s_USE_DEFAULT_PATH_CONFIG  = "useDefaultPath";
static const std::string s_PATH_CONFIG              = "path";
static const std::string s_DELETE_ICON_CONFIG       = "deleteIcon";
static const std::string s_NEW_ICON_CONFIG          = "newIcon";
static const std::string s_COPY_ICON_CONFIG         = "copyIcon";
static const std::string s_REINITIALIZE_ICON_CONFIG = "reinitializeIcon";
static const std::string s_RENAME_ICON_CONFIG       = "renameIcon";
static const std::string s_IMPORT_ICON_CONFIG       = "importIcon";
static const std::string s_EXPORT_ICON_CONFIG       = "exportIcon";
static const std::string s_ICON_WIDTH_CONFIG        = "iconWidth";
static const std::string s_ICON_HEIGHT_CONFIG       = "iconHeight";

const core::com::slots::key_t UPDATE_DEFAULT_PRESET_SLOT = "updateDefaultPreset";
const core::com::slots::key_t UPDATE_PRESETS_SLOT        = "updatePresets";

//------------------------------------------------------------------------------

transfer_function::transfer_function()
{
    new_slot(UPDATE_DEFAULT_PRESET_SLOT, &transfer_function::updateDefaultPreset, this);
    new_slot(UPDATE_PRESETS_SLOT, [this](){transfer_function::initializePresets();});

    const std::filesystem::path module_path = core::runtime::get_module_resource_path(
        std::string(
            "sight::module::ui::qt"
        )
    );

    m_deleteIcon       = module_path / "delete.png";
    m_newIcon          = module_path / "new.png";
    m_copyIcon         = module_path / "copy.png";
    m_renameIcon       = module_path / "rename.png";
    m_reinitializeIcon = module_path / "reinitialize.png";
    m_importIcon       = module_path / "import.png";
    m_exportIcon       = module_path / "export.png";
}

//------------------------------------------------------------------------------

void transfer_function::configuring()
{
    this->initialize();

    const config_t tree = this->get_config();
    const auto config   = tree.get_child_optional("config");

    bool use_default_path = true;
    if(config)
    {
        const auto path_cfg = config->equal_range(s_PATH_CONFIG);
        for(auto it_cfg = path_cfg.first ; it_cfg != path_cfg.second ; ++it_cfg)
        {
            const auto additional_tf_path = it_cfg->second.get_value<std::string>();
            if(!additional_tf_path.empty())
            {
                const auto path = core::runtime::get_module_resource_file_path(additional_tf_path);
                m_paths.push_back(path);
            }
        }

        const auto config_attr = config->get_child_optional("<xmlattr>");

        if(config_attr)
        {
            use_default_path = config_attr->get<bool>(s_USE_DEFAULT_PATH_CONFIG, use_default_path);

            const auto delete_icon_cfg = config_attr->get_optional<std::string>(s_DELETE_ICON_CONFIG);
            if(delete_icon_cfg)
            {
                m_deleteIcon = core::runtime::get_module_resource_file_path(delete_icon_cfg.value());
            }

            const auto new_icon_cfg = config_attr->get_optional<std::string>(s_NEW_ICON_CONFIG);
            if(new_icon_cfg)
            {
                m_newIcon = core::runtime::get_module_resource_file_path(new_icon_cfg.value());
            }

            const auto copy_icon_cfg = config_attr->get_optional<std::string>(s_COPY_ICON_CONFIG);
            if(copy_icon_cfg)
            {
                m_copyIcon = core::runtime::get_module_resource_file_path(copy_icon_cfg.value());
            }

            const auto reinitialize_icon_cfg = config_attr->get_optional<std::string>(s_REINITIALIZE_ICON_CONFIG);
            if(reinitialize_icon_cfg)
            {
                m_reinitializeIcon = core::runtime::get_module_resource_file_path(reinitialize_icon_cfg.value());
            }

            const auto rename_icon_cfg = config_attr->get_optional<std::string>(s_RENAME_ICON_CONFIG);
            if(rename_icon_cfg)
            {
                m_renameIcon = core::runtime::get_module_resource_file_path(rename_icon_cfg.value());
            }

            const auto import_icon_cfg = config_attr->get_optional<std::string>(s_IMPORT_ICON_CONFIG);
            if(import_icon_cfg)
            {
                m_importIcon = core::runtime::get_module_resource_file_path(import_icon_cfg.value());
            }

            const auto export_icon_cfg = config_attr->get_optional<std::string>(s_EXPORT_ICON_CONFIG);
            if(export_icon_cfg)
            {
                m_exportIcon = core::runtime::get_module_resource_file_path(export_icon_cfg.value());
            }

            m_iconWidth  = config_attr->get<unsigned int>(s_ICON_WIDTH_CONFIG, m_iconWidth);
            m_iconHeight = config_attr->get<unsigned int>(s_ICON_HEIGHT_CONFIG, m_iconHeight);
        }
    }

    if(use_default_path)
    {
        const auto path_root = core::runtime::get_module_resource_file_path("sight::module::ui::qt", "tf");
        m_paths.push_back(path_root);
    }
}

//------------------------------------------------------------------------------

void transfer_function::starting()
{
    this->create();

    // Get the Qt container
    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    // Buttons creation
    m_presetComboBox = new QComboBox();

    m_deleteButton = new QPushButton(QIcon(m_deleteIcon.string().c_str()), "");
    m_deleteButton->setToolTip(QString("Delete"));

    m_newButton = new QPushButton(QIcon(m_newIcon.string().c_str()), "");
    m_newButton->setToolTip(QString("New"));

    m_copyButton = new QPushButton(QIcon(m_copyIcon.string().c_str()), "");
    m_copyButton->setToolTip(QString("Copy"));

    m_reinitializeButton = new QPushButton(QIcon(m_reinitializeIcon.string().c_str()), "");
    m_reinitializeButton->setToolTip(QString("Reinitialize"));

    m_renameButton = new QPushButton(QIcon(m_renameIcon.string().c_str()), "");
    m_renameButton->setToolTip(QString("Rename"));

    m_importButton = new QPushButton(QIcon(m_importIcon.string().c_str()), "");
    m_importButton->setToolTip(QString("Import"));

    m_exportButton = new QPushButton(QIcon(m_exportIcon.string().c_str()), "");
    m_exportButton->setToolTip(QString("Export"));

    if(m_iconWidth > 0 && m_iconHeight > 0)
    {
        int icon_width  = int(m_iconWidth);
        int icon_height = int(m_iconHeight);
        m_deleteButton->setIconSize(QSize(icon_width, icon_height));
        m_newButton->setIconSize(QSize(icon_width, icon_height));
        m_copyButton->setIconSize(QSize(icon_width, icon_height));
        m_reinitializeButton->setIconSize(QSize(icon_width, icon_height));
        m_renameButton->setIconSize(QSize(icon_width, icon_height));
        m_importButton->setIconSize(QSize(icon_width, icon_height));
        m_exportButton->setIconSize(QSize(icon_width, icon_height));
    }

    // Layout management
    auto* const layout = new QBoxLayout(QBoxLayout::LeftToRight);

    layout->addWidget(m_presetComboBox);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_newButton);
    layout->addWidget(m_copyButton);
    layout->addWidget(m_reinitializeButton);
    layout->addWidget(m_renameButton);
    layout->addWidget(m_importButton);
    layout->addWidget(m_exportButton);

    qt_container->setLayout(layout);

    // Manage connection with the editor.
    QObject::connect(m_presetComboBox, qOverload<int>(&QComboBox::activated), this, &transfer_function::presetChoice);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &transfer_function::deletePreset);
    QObject::connect(m_newButton, &QPushButton::clicked, this, &transfer_function::createPreset);
    QObject::connect(m_copyButton, &QPushButton::clicked, this, &transfer_function::copyPreset);
    QObject::connect(m_reinitializeButton, &QPushButton::clicked, this, &transfer_function::reinitializePresets);
    QObject::connect(m_renameButton, &QPushButton::clicked, this, &transfer_function::renamePreset);
    QObject::connect(m_importButton, &QPushButton::clicked, this, &transfer_function::importPreset);
    QObject::connect(m_exportButton, &QPushButton::clicked, this, &transfer_function::exportPreset);

    // Initializes the TF preset from paths.
    this->initializePresets();

    this->updateDefaultPreset();
}

//------------------------------------------------------------------------------

void transfer_function::updating()
{
    // Select first the TF in the presets if has changed
    std::string selected_tf_key = m_presetComboBox->currentText().toStdString();
    const std::string tf_name   = [this]{const auto current_tf = m_currentTF.lock(); return current_tf->name();}();

    if(selected_tf_key != tf_name)
    {
        const int index = m_presetComboBox->findText(QString::fromStdString(tf_name));
        if(index < 0)
        {
            SIGHT_ERROR("Could not find transfer function '" + tf_name + "'");
            return;
        }

        this->presetChoice(index);
        // Update the selected key now that we have found the one we were looking for
        selected_tf_key = m_presetComboBox->currentText().toStdString();
    }

    // Now updates the TF
    const auto opt_presets          = m_optPresets.lock();
    sight::data::composite& presets = (opt_presets != nullptr) ? *opt_presets : *m_tfPresets;
    const auto new_selected_tf      = std::dynamic_pointer_cast<data::transfer_function>(presets[selected_tf_key]);
    {
        auto tf_lock          = data::mt::locked_ptr(new_selected_tf);
        const auto current_tf = m_currentTF.lock();
        new_selected_tf->deep_copy(current_tf.get_shared());
        auto sig = new_selected_tf->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void transfer_function::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

service::connections_t transfer_function::auto_connections() const
{
    return {
        {s_IMAGE_INPUT, data::image::MODIFIED_SIG, UPDATE_DEFAULT_PRESET_SLOT},
        {s_IMAGE_INPUT, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {s_CURRENT_INPUT, data::transfer_function::MODIFIED_SIG, service::slots::UPDATE},
        {s_CURRENT_INPUT, data::transfer_function::POINTS_MODIFIED_SIG, service::slots::UPDATE},
        {s_CURRENT_INPUT, data::transfer_function::WINDOWING_MODIFIED_SIG, service::slots::UPDATE},
        {s_PRESETS_INOUT, data::composite::MODIFIED_SIG, UPDATE_PRESETS_SLOT},
        {s_PRESETS_INOUT, data::composite::ADDED_OBJECTS_SIG, UPDATE_PRESETS_SLOT},
        {s_PRESETS_INOUT, data::composite::CHANGED_OBJECTS_SIG, UPDATE_PRESETS_SLOT},
        {s_PRESETS_INOUT, data::composite::REMOVED_OBJECTS_SIG, UPDATE_PRESETS_SLOT}
    };
}

//------------------------------------------------------------------------------

bool transfer_function::hasPresetName(const sight::data::composite& _presets, const std::string& _name)
{
    return _presets.find(_name) != _presets.end();
}

//------------------------------------------------------------------------------

std::string transfer_function::createPresetName(
    const sight::data::composite& _presets,
    const std::string& _basename
) const
{
    bool has_transfer_function_name = true;
    std::string new_name            = _basename;
    int cpt                         = 1;
    while(has_transfer_function_name)
    {
        std::stringstream tmp_str;
        tmp_str << _basename << "_" << cpt;
        new_name                   = tmp_str.str();
        has_transfer_function_name = this->hasPresetName(_presets, new_name);
        cpt++;
    }

    return new_name;
}

//------------------------------------------------------------------------------

void transfer_function::initializePresets(const std::string& _current_preset_name)
{
    m_tfPresets = std::make_shared<sight::data::composite>();
    std::string current_preset_name = _current_preset_name;

    {
        const auto opt_presets          = m_optPresets.lock();
        sight::data::composite& presets = (opt_presets != nullptr) ? *opt_presets : *m_tfPresets;
        if(opt_presets != nullptr)
        {
            // If we specify the presets, use the internal map to save initial state
            m_tfPresets->deep_copy(opt_presets.get_shared());

            if(presets.find(current_preset_name) == presets.end())
            {
                current_preset_name = presets.begin()->first;
            }
        }
        else
        {
            // Add the default TF if it not exists.
            const std::string& default_tf_name = data::transfer_function::s_DEFAULT_TF_NAME;

            if(!this->hasPresetName(presets, default_tf_name))
            {
                const auto image = m_image.lock();
                if(image)
                {
                    const data::transfer_function::sptr default_tf =
                        data::transfer_function::createDefaultTF(image->getType());

                    const auto scoped_emitter = presets.scoped_emit();
                    scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
                    presets[default_tf_name] = default_tf;
                }
            }

            // Test if transfer function composite has few TF
            if(presets.size() <= 1)
            {
                // Creates the TF reader.
                const auto tf        = std::make_shared<data::transfer_function>();
                const auto tf_reader = sight::service::add<io::service::reader>(
                    "sight::module::io::session::reader"
                );
                tf_reader->set_inout(tf, io::service::s_DATA_KEY);

                // Parse all paths contained in m_path and read basic TF.
                for(const std::filesystem::path& dir_path : m_paths)
                {
                    SIGHT_ASSERT(
                        "Invalid directory path '" + dir_path.string() + "'",
                        std::filesystem::exists(dir_path)
                    );
                    for(std::filesystem::directory_iterator it(dir_path) ;
                        it != std::filesystem::directory_iterator() ;
                        ++it)
                    {
                        if(!std::filesystem::is_directory(*it) && it->path().extension().string() == ".tf")
                        {
                            const std::filesystem::path file = *it;

                            // Add a new composite for each TF path.
                            service::config_t config;
                            config.put("file", file.string());
                            config.put("archive.<xmlattr>.format", "filesystem");

                            tf_reader->set_config(config);
                            tf_reader->configure();
                            tf_reader->start();
                            tf_reader->update();
                            tf_reader->stop();

                            if(!tf->name().empty())
                            {
                                const data::transfer_function::sptr new_tf =
                                    data::object::copy<data::transfer_function>(tf);
                                if(this->hasPresetName(presets, new_tf->name()))
                                {
                                    new_tf->setName(this->createPresetName(presets, new_tf->name()));
                                }

                                presets[new_tf->name()] = new_tf;
                            }
                        }
                    }
                }

                // Delete the reader.
                sight::service::remove(tf_reader);
            }
        }

        // Update all presets in the editor.
        m_presetComboBox->clear();

        {
            // Gets TF presets.
            // Iterate over each composite to add them to the presets selector.
            for(const auto& elt : presets)
            {
                m_presetComboBox->addItem(elt.first.c_str());
            }

            // If the current TF preset exists, find it.
            const auto current_tf_preset = m_currentTF.lock();

            if(current_tf_preset)
            {
                for(const auto& elt : presets)
                {
                    if(std::dynamic_pointer_cast<sight::data::transfer_function>(elt.second)->name()
                       == current_tf_preset->name())
                    {
                        current_preset_name = elt.first;
                        break;
                    }
                }
            }
        }
    }

    int index = m_presetComboBox->findText(QString::fromStdString(current_preset_name));
    if(index == 1)
    {
        // Fallback if the previously selected TF no longer exists
        index = m_presetComboBox->findText(QString::fromStdString(data::transfer_function::s_DEFAULT_TF_NAME));
    }

    // Set the current composite
    this->presetChoice(index);
}

//------------------------------------------------------------------------------

void transfer_function::presetChoice(int _index)
{
    m_presetComboBox->setCurrentIndex(_index);
    // Set the output to the current preset.
    this->setCurrentPreset();

    const std::string tf_name = m_presetComboBox->currentText().toStdString();
    const bool is_enabled     = (tf_name != data::transfer_function::s_DEFAULT_TF_NAME);

    m_renameButton->setEnabled(is_enabled);
    m_deleteButton->setEnabled(is_enabled);
}

//------------------------------------------------------------------------------

void transfer_function::setCurrentPreset()
{
    const std::string new_selected_tf_key = m_presetComboBox->currentText().toStdString();

    const auto opt_presets          = m_optPresets.lock();
    sight::data::composite& presets = (opt_presets != nullptr) ? *opt_presets : *m_tfPresets;
    const auto new_selected_tf      = std::dynamic_pointer_cast<data::transfer_function>(presets[new_selected_tf_key]);

    const auto current_tf = m_currentTF.lock();
    if(new_selected_tf && new_selected_tf->name() != current_tf->name())
    {
        current_tf->deep_copy(new_selected_tf);
        auto sig = current_tf->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void transfer_function::updateDefaultPreset()
{
    const auto opt_presets = m_optPresets.lock();
    const auto image       = m_image.lock();
    if(image != nullptr && opt_presets == nullptr)
    {
        const data::transfer_function::sptr default_tf = data::transfer_function::createDefaultTF(image->getType());
        const std::string default_tf_name              = data::transfer_function::s_DEFAULT_TF_NAME;

        (*m_tfPresets)[default_tf_name] = default_tf;

        const auto current_tf = m_currentTF.lock();
        current_tf->deep_copy(default_tf);
        auto sig = current_tf->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void transfer_function::deletePreset()
{
    if(m_presetComboBox->count() < 2)
    {
        sight::ui::dialog::message message_box;
        message_box.setTitle("Delete error");
        message_box.setMessage("Transfer function cannot be deleted, at least one transfer function is required.");
        message_box.setIcon(sight::ui::dialog::message::WARNING);
        message_box.addButton(sight::ui::dialog::message::OK);
        message_box.show();
        return;
    }

    sight::ui::dialog::message message_box;
    message_box.setTitle("Delete confirmation");
    message_box.setMessage("Are you sure you want to delete this TF preset ?");
    message_box.setIcon(sight::ui::dialog::message::QUESTION);
    message_box.addButton(sight::ui::dialog::message::OK);
    message_box.addButton(sight::ui::dialog::message::CANCEL);
    sight::ui::dialog::message::Buttons answer_copy = message_box.show();

    if(answer_copy != sight::ui::dialog::message::CANCEL)
    {
        const int index = std::max(m_presetComboBox->currentIndex() - 1, 0);
        {
            // Remove the current TF preset from the Composite.
            const std::string selected_tf_preset_key = m_presetComboBox->currentText().toStdString();
            const auto opt_presets                   = m_optPresets.lock();
            sight::data::composite& presets          = (opt_presets != nullptr) ? *opt_presets : *m_tfPresets;

            const auto scoped_emitter = presets.scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));

            presets.erase(selected_tf_preset_key);

            m_presetComboBox->removeItem(m_presetComboBox->findText(QString::fromStdString(selected_tf_preset_key)));
        }

        // Set the current composite
        this->presetChoice(index);
    }
}

//------------------------------------------------------------------------------

void transfer_function::createPreset()
{
    const auto& old_name = m_presetComboBox->currentText().toStdString();
    sight::ui::dialog::input input;
    input.setTitle("Create new transfer function");
    input.setMessage("Enter transfer function name :");
    input.set_input(old_name);
    const auto& [newName, ok] = input.getInput();

    if(!ok)
    {
        return;
    }

    if(!newName.empty())
    {
        {
            const auto opt_presets          = m_optPresets.lock();
            sight::data::composite& presets = (opt_presets != nullptr) ? *opt_presets : *m_tfPresets;

            // Gets TF presets.
            if(!this->hasPresetName(presets, newName))
            {
                // Create the new composite.
                const auto image = m_image.lock();
                auto default_tf  = image
                                   ? data::transfer_function::createDefaultTF(image->getType())
                                   : data::transfer_function::createDefaultTF();

                default_tf->setName(newName);

                const auto scoped_emitter = presets.scoped_emit();
                scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
                presets[newName] = default_tf;

                // Recreates presets.
                m_presetComboBox->clear();
                for(const auto& elt : presets)
                {
                    m_presetComboBox->addItem(elt.first.c_str());
                }
            }
            else
            {
                sight::ui::dialog::message message_box;
                message_box.setTitle("Warning");
                message_box.setMessage("This TF preset name already exists so you can not overwrite it.");
                message_box.setIcon(sight::ui::dialog::message::WARNING);
                message_box.addButton(sight::ui::dialog::message::OK);
                message_box.show();
                return;
            }
        }

        // Set the current composite.
        this->presetChoice(m_presetComboBox->findText(QString::fromStdString(newName)));
    }
}

//------------------------------------------------------------------------------

void transfer_function::copyPreset()
{
    const auto& old_name = m_presetComboBox->currentText().toStdString();

    sight::ui::dialog::input input;
    input.setTitle("Copy transfer function");
    input.setMessage("Enter new transfer function name:");
    input.set_input(old_name);
    const auto& [newName, ok] = input.getInput();

    if(!ok)
    {
        return;
    }

    if(!newName.empty())
    {
        {
            const auto opt_presets          = m_optPresets.lock();
            sight::data::composite& presets = (opt_presets != nullptr) ? *opt_presets : *m_tfPresets;
            // Gets TF presets.
            if(this->hasPresetName(presets, newName))
            {
                sight::ui::dialog::message message_box;
                message_box.setTitle("Error");
                message_box.setMessage("This preset name already exists, please choose another one.");
                message_box.setIcon(sight::ui::dialog::message::WARNING);
                message_box.addButton(sight::ui::dialog::message::OK);
                message_box.show();
                return;
            }

            const auto current_tf = std::dynamic_pointer_cast<data::transfer_function>(presets[old_name]);
            SIGHT_ASSERT("Can not find current TF.", current_tf);

            data::transfer_function::sptr tf = data::object::copy(current_tf);

            tf->setName(newName);

            const auto scoped_emitter = presets.scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
            presets[newName] = tf;

            // Recreates presets.
            m_presetComboBox->clear();
            for(const auto& elt : presets)
            {
                m_presetComboBox->addItem(elt.first.c_str());
            }
        }
        // Set the current composite.
        this->presetChoice(m_presetComboBox->findText(QString::fromStdString(newName)));
    }
    else
    {
        sight::ui::dialog::message message_box;
        message_box.setTitle("Error");
        message_box.setMessage("No preset name specified.");
        message_box.setIcon(sight::ui::dialog::message::WARNING);
        message_box.addButton(sight::ui::dialog::message::OK);
        message_box.show();
    }
}

//------------------------------------------------------------------------------

void transfer_function::reinitializePresets()
{
    // Clears previous settings.
    m_presetComboBox->clear();

    {
        const auto opt_presets = m_optPresets.lock();
        if(opt_presets != nullptr)
        {
            // If we specify the presets, restore the initial state from the internal map
            const auto scoped_emitter = opt_presets->scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
            opt_presets->deep_copy(m_tfPresets);
        }
    }
    m_tfPresets->clear();

    // Initialize presets.
    this->initializePresets();
}

//------------------------------------------------------------------------------

void transfer_function::renamePreset()
{
    const auto& old_name = m_presetComboBox->currentText().toStdString();

    sight::ui::dialog::input input;
    input.setTitle("Rename transfer function preset");
    input.setMessage("Enter new name:");
    input.set_input(old_name);
    const auto& [newName, ok] = input.getInput();

    if(!ok)
    {
        return;
    }

    if(!newName.empty() && newName != old_name)
    {
        {
            const auto opt_presets          = m_optPresets.lock();
            sight::data::composite& presets = (opt_presets != nullptr) ? *opt_presets : *m_tfPresets;
            // Gets TF presets.
            if(!this->hasPresetName(presets, newName))
            {
                auto tf = std::dynamic_pointer_cast<data::transfer_function>(presets[old_name]);

                // Rename the composite.
                presets.erase(old_name);
                presets[newName] = tf;

                // Creates presets.
                m_presetComboBox->clear();
                for(const auto& elt : presets)
                {
                    m_presetComboBox->addItem(elt.first.c_str());
                }

                tf->setName(newName);
            }
            else
            {
                sight::ui::dialog::message message_box;
                message_box.setTitle("Warning");
                message_box.setMessage("This TF preset name already exists so you can not overwrite it.");
                message_box.setIcon(sight::ui::dialog::message::WARNING);
                message_box.addButton(sight::ui::dialog::message::OK);
                message_box.show();
                return;
            }
        }

        // Set the current composite.
        this->presetChoice(m_presetComboBox->findText(QString::fromStdString(newName)));
    }
}

//------------------------------------------------------------------------------

void transfer_function::importPreset()
{
    const auto new_tf = std::make_shared<data::transfer_function>();

    const auto reader = sight::service::add<io::service::reader>("sight::module::io::session::reader");

    reader->set_inout(new_tf, io::service::s_DATA_KEY);

    service::config_t config;
    config.put("dialog.<xmlattr>.extension", ".tf");
    config.put("dialog.<xmlattr>.description", "Transfer Function Preset");
    config.put("archive.<xmlattr>.format", "filesystem");

    reader->configure(config);
    reader->start();
    reader->openLocationDialog();
    reader->update().wait();

    // Check the loaded composite.
    if(!new_tf->empty())
    {
        int index = 0;
        {
            std::string preset_name(new_tf->name());

            const auto opt_presets          = m_optPresets.lock();
            sight::data::composite& presets = (opt_presets != nullptr) ? *opt_presets : *m_tfPresets;
            if(this->hasPresetName(presets, preset_name))
            {
                preset_name = this->createPresetName(presets, preset_name);
            }

            const auto scoped_emitter = presets.scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
            presets[preset_name] = new_tf;
            new_tf->setName(preset_name);

            m_presetComboBox->addItem(QString(preset_name.c_str()));
            index = static_cast<int>(presets.size() - 1);
        }

        this->presetChoice(index);
    }

    reader->stop().wait();
    sight::service::remove(reader);
}

//------------------------------------------------------------------------------

void transfer_function::exportPreset()
{
    const auto writer = sight::service::add<io::service::writer>("sight::module::io::session::writer");
    {
        const auto current_tf = m_currentTF.const_lock();
        writer->set_input(current_tf.get_shared(), io::service::s_DATA_KEY);
    }

    service::config_t config;
    config.add("dialog.<xmlattr>.extension", ".tf");
    config.add("dialog.<xmlattr>.description", "Transfer Function Preset");
    config.put("archive.<xmlattr>.format", "filesystem");

    writer->configure(config);
    writer->start();
    writer->openLocationDialog();
    writer->update().wait();
    writer->stop().wait();
    sight::service::remove(writer);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
