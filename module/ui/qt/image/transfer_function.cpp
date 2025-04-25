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

#include "module/ui/qt/image/transfer_function.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <data/map.hpp>
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

static const std::string USE_DEFAULT_PATH_CONFIG  = "useDefaultPath";
static const std::string PATH_CONFIG              = "path";
static const std::string DELETE_ICON_CONFIG       = "deleteIcon";
static const std::string NEW_ICON_CONFIG          = "newIcon";
static const std::string COPY_ICON_CONFIG         = "copyIcon";
static const std::string REINITIALIZE_ICON_CONFIG = "reinitializeIcon";
static const std::string RENAME_ICON_CONFIG       = "renameIcon";
static const std::string IMPORT_ICON_CONFIG       = "importIcon";
static const std::string EXPORT_ICON_CONFIG       = "exportIcon";
static const std::string ICON_WIDTH_CONFIG        = "iconWidth";
static const std::string ICON_HEIGHT_CONFIG       = "iconHeight";

const core::com::slots::key_t UPDATE_DEFAULT_PRESET_SLOT = "updateDefaultPreset";
const core::com::slots::key_t UPDATE_PRESETS_SLOT        = "updatePresets";

//------------------------------------------------------------------------------

transfer_function::transfer_function()
{
    new_slot(UPDATE_DEFAULT_PRESET_SLOT, &transfer_function::update_default_preset, this);
    new_slot(UPDATE_PRESETS_SLOT, [this](){transfer_function::initialize_presets();});

    const std::filesystem::path module_path = core::runtime::get_module_resource_path(
        std::string(
            "sight::module::ui::qt"
        )
    );

    m_delete_icon       = module_path / "delete.svg";
    m_new_icon          = module_path / "new.svg";
    m_copy_icon         = module_path / "copy.svg";
    m_rename_icon       = module_path / "text.svg";
    m_reinitialize_icon = module_path / "reset.svg";
    m_import_icon       = module_path / "import.svg";
    m_export_icon       = module_path / "export.svg";
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
        const auto path_cfg = config->equal_range(PATH_CONFIG);
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
            use_default_path = config_attr->get<bool>(USE_DEFAULT_PATH_CONFIG, use_default_path);

            const auto delete_icon_cfg = config_attr->get_optional<std::string>(DELETE_ICON_CONFIG);
            if(delete_icon_cfg)
            {
                m_delete_icon = core::runtime::get_module_resource_file_path(delete_icon_cfg.value());
            }

            const auto new_icon_cfg = config_attr->get_optional<std::string>(NEW_ICON_CONFIG);
            if(new_icon_cfg)
            {
                m_new_icon = core::runtime::get_module_resource_file_path(new_icon_cfg.value());
            }

            const auto copy_icon_cfg = config_attr->get_optional<std::string>(COPY_ICON_CONFIG);
            if(copy_icon_cfg)
            {
                m_copy_icon = core::runtime::get_module_resource_file_path(copy_icon_cfg.value());
            }

            const auto reinitialize_icon_cfg = config_attr->get_optional<std::string>(REINITIALIZE_ICON_CONFIG);
            if(reinitialize_icon_cfg)
            {
                m_reinitialize_icon = core::runtime::get_module_resource_file_path(reinitialize_icon_cfg.value());
            }

            const auto rename_icon_cfg = config_attr->get_optional<std::string>(RENAME_ICON_CONFIG);
            if(rename_icon_cfg)
            {
                m_rename_icon = core::runtime::get_module_resource_file_path(rename_icon_cfg.value());
            }

            const auto import_icon_cfg = config_attr->get_optional<std::string>(IMPORT_ICON_CONFIG);
            if(import_icon_cfg)
            {
                m_import_icon = core::runtime::get_module_resource_file_path(import_icon_cfg.value());
            }

            const auto export_icon_cfg = config_attr->get_optional<std::string>(EXPORT_ICON_CONFIG);
            if(export_icon_cfg)
            {
                m_export_icon = core::runtime::get_module_resource_file_path(export_icon_cfg.value());
            }

            m_icon_width  = config_attr->get<unsigned int>(ICON_WIDTH_CONFIG, m_icon_width);
            m_icon_height = config_attr->get<unsigned int>(ICON_HEIGHT_CONFIG, m_icon_height);
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
    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    // Layout management
    auto* const layout = new QBoxLayout(QBoxLayout::LeftToRight);

    // Buttons creation
    m_preset_combo_box = new QComboBox();
    layout->addWidget(m_preset_combo_box);

    // Manage connection with the editor.
    QObject::connect(
        m_preset_combo_box,
        qOverload<int>(&QComboBox::activated),
        this,
        &transfer_function::preset_choice
    );

    if(*m_editable)
    {
        m_delete_button = new QPushButton(QIcon(m_delete_icon.string().c_str()), "");
        m_delete_button->setToolTip(QString("Delete"));

        m_new_button = new QPushButton(QIcon(m_new_icon.string().c_str()), "");
        m_new_button->setToolTip(QString("New"));

        m_copy_button = new QPushButton(QIcon(m_copy_icon.string().c_str()), "");
        m_copy_button->setToolTip(QString("Copy"));

        m_reinitialize_button = new QPushButton(QIcon(m_reinitialize_icon.string().c_str()), "");
        m_reinitialize_button->setToolTip(QString("Reinitialize"));

        m_rename_button = new QPushButton(QIcon(m_rename_icon.string().c_str()), "");
        m_rename_button->setToolTip(QString("Rename"));

        m_import_button = new QPushButton(QIcon(m_import_icon.string().c_str()), "");
        m_import_button->setToolTip(QString("Import"));

        m_export_button = new QPushButton(QIcon(m_export_icon.string().c_str()), "");
        m_export_button->setToolTip(QString("Export"));

        if(m_icon_width > 0 && m_icon_height > 0)
        {
            int icon_width  = int(m_icon_width);
            int icon_height = int(m_icon_height);
            m_delete_button->setIconSize(QSize(icon_width, icon_height));
            m_new_button->setIconSize(QSize(icon_width, icon_height));
            m_copy_button->setIconSize(QSize(icon_width, icon_height));
            m_reinitialize_button->setIconSize(QSize(icon_width, icon_height));
            m_rename_button->setIconSize(QSize(icon_width, icon_height));
            m_import_button->setIconSize(QSize(icon_width, icon_height));
            m_export_button->setIconSize(QSize(icon_width, icon_height));
        }

        layout->addWidget(m_delete_button);
        layout->addWidget(m_new_button);
        layout->addWidget(m_copy_button);
        layout->addWidget(m_reinitialize_button);
        layout->addWidget(m_rename_button);
        layout->addWidget(m_import_button);
        layout->addWidget(m_export_button);

        QObject::connect(m_delete_button, &QPushButton::clicked, this, &transfer_function::delete_preset);
        QObject::connect(m_new_button, &QPushButton::clicked, this, &transfer_function::create_preset);
        QObject::connect(m_copy_button, &QPushButton::clicked, this, &transfer_function::copy_preset);
        QObject::connect(m_reinitialize_button, &QPushButton::clicked, this, &transfer_function::reinitialize_presets);
        QObject::connect(m_rename_button, &QPushButton::clicked, this, &transfer_function::rename_preset);
        QObject::connect(m_import_button, &QPushButton::clicked, this, &transfer_function::import_preset);
        QObject::connect(m_export_button, &QPushButton::clicked, this, &transfer_function::export_preset);
    }

    qt_container->set_layout(layout);

    // Initializes the TF preset from paths.
    this->initialize_presets();

    this->update_default_preset();
}

//------------------------------------------------------------------------------

void transfer_function::updating()
{
    // Select first the TF in the presets if has changed
    std::string selected_tf_key = m_preset_combo_box->currentText().toStdString();
    const std::string tf_name   = [this]{const auto current_tf = m_current_tf.lock(); return current_tf->name();}();

    if(selected_tf_key != tf_name)
    {
        const int index = m_preset_combo_box->findText(QString::fromStdString(tf_name));
        if(index < 0)
        {
            SIGHT_ERROR("Could not find transfer function '" + tf_name + "'");
            return;
        }

        this->preset_choice(index);
        // Update the selected key now that we have found the one we were looking for
        selected_tf_key = m_preset_combo_box->currentText().toStdString();
    }

    // Now updates the TF
    const auto opt_presets     = m_opt_presets.lock();
    sight::data::map& presets  = (opt_presets != nullptr) ? *opt_presets : *m_tf_presets;
    const auto new_selected_tf = std::dynamic_pointer_cast<data::transfer_function>(presets[selected_tf_key]);
    {
        auto tf_lock          = data::mt::locked_ptr(new_selected_tf);
        const auto current_tf = m_current_tf.lock();
        new_selected_tf->deep_copy(current_tf.get_shared());
        new_selected_tf->async_emit(data::object::MODIFIED_SIG);
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
        {IMAGE_INPUT, data::image::MODIFIED_SIG, UPDATE_DEFAULT_PRESET_SLOT},
        {IMAGE_INPUT, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {CURRENT_INPUT, data::transfer_function::MODIFIED_SIG, service::slots::UPDATE},
        {CURRENT_INPUT, data::transfer_function::POINTS_MODIFIED_SIG, service::slots::UPDATE},
        {CURRENT_INPUT, data::transfer_function::WINDOWING_MODIFIED_SIG, service::slots::UPDATE},
        {PRESETS_INOUT, data::map::MODIFIED_SIG, UPDATE_PRESETS_SLOT},
        {PRESETS_INOUT, data::map::ADDED_OBJECTS_SIG, UPDATE_PRESETS_SLOT},
        {PRESETS_INOUT, data::map::CHANGED_OBJECTS_SIG, UPDATE_PRESETS_SLOT},
        {PRESETS_INOUT, data::map::REMOVED_OBJECTS_SIG, UPDATE_PRESETS_SLOT}
    };
}

//------------------------------------------------------------------------------

bool transfer_function::has_preset_name(const sight::data::map& _presets, const std::string& _name)
{
    return _presets.find(_name) != _presets.end();
}

//------------------------------------------------------------------------------

std::string transfer_function::create_preset_name(
    const sight::data::map& _presets,
    const std::string& _basename
)
{
    bool has_transfer_function_name = true;
    std::string new_name            = _basename;
    int cpt                         = 1;
    while(has_transfer_function_name)
    {
        std::stringstream tmp_str;
        tmp_str << _basename << "_" << cpt;
        new_name                   = tmp_str.str();
        has_transfer_function_name = sight::module::ui::qt::image::transfer_function::has_preset_name(
            _presets,
            new_name
        );
        cpt++;
    }

    return new_name;
}

//------------------------------------------------------------------------------

void transfer_function::initialize_presets(const std::string& _current_preset_name)
{
    m_tf_presets = std::make_shared<sight::data::map>();
    std::string current_preset_name = _current_preset_name;

    {
        const auto opt_presets    = m_opt_presets.lock();
        sight::data::map& presets = (opt_presets != nullptr) ? *opt_presets : *m_tf_presets;
        if(opt_presets != nullptr)
        {
            // If we specify the presets, use the internal map to save initial state
            m_tf_presets->deep_copy(opt_presets.get_shared());

            if(presets.find(current_preset_name) == presets.end())
            {
                current_preset_name = presets.begin()->first;
            }
        }
        else
        {
            // Add the default TF if it not exists.
            const std::string& default_tf_name = data::transfer_function::DEFAULT_TF_NAME;

            if(!sight::module::ui::qt::image::transfer_function::has_preset_name(presets, default_tf_name))
            {
                const auto image = m_image.lock();
                if(image)
                {
                    const data::transfer_function::sptr default_tf =
                        data::transfer_function::create_default_tf(image->type());

                    const auto scoped_emitter = presets.scoped_emit();
                    scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
                    presets[default_tf_name] = default_tf;
                }
            }

            // Test if transfer function map has few TF
            if(presets.size() <= 1)
            {
                // Creates the TF reader.
                const auto tf        = std::make_shared<data::transfer_function>();
                const auto tf_reader = sight::service::add<io::service::reader>(
                    "sight::module::io::session::reader"
                );
                tf_reader->set_inout(tf, io::service::DATA_KEY);

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

                            // Add a new map for each TF path.
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
                                if(sight::module::ui::qt::image::transfer_function::has_preset_name(
                                       presets,
                                       new_tf->name()
                                ))
                                {
                                    new_tf->set_name(
                                        sight::module::ui::qt::image::transfer_function::create_preset_name(
                                            presets,
                                            new_tf->name()
                                        )
                                    );
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
        m_preset_combo_box->clear();

        {
            // Gets TF presets.
            // Iterate over each map to add them to the presets selector.
            for(const auto& elt : presets)
            {
                m_preset_combo_box->addItem(elt.first.c_str());
            }

            // If the current TF preset exists, find it.
            const auto current_tf_preset = m_current_tf.lock();

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

    int index = m_preset_combo_box->findText(QString::fromStdString(current_preset_name));
    if(index == 1)
    {
        // Fallback if the previously selected TF no longer exists
        index = m_preset_combo_box->findText(QString::fromStdString(data::transfer_function::DEFAULT_TF_NAME));
    }

    // Set the current map
    this->preset_choice(index);
}

//------------------------------------------------------------------------------

void transfer_function::preset_choice(int _index)
{
    m_preset_combo_box->setCurrentIndex(_index);
    // Set the output to the current preset.
    this->set_current_preset();

    const std::string tf_name = m_preset_combo_box->currentText().toStdString();

    if(*m_editable)
    {
        const bool is_enabled = (tf_name != data::transfer_function::DEFAULT_TF_NAME);

        m_rename_button->setEnabled(is_enabled);
        m_delete_button->setEnabled(is_enabled);
    }
}

//------------------------------------------------------------------------------

void transfer_function::set_current_preset()
{
    const std::string new_selected_tf_key = m_preset_combo_box->currentText().toStdString();

    const auto opt_presets     = m_opt_presets.lock();
    sight::data::map& presets  = (opt_presets != nullptr) ? *opt_presets : *m_tf_presets;
    const auto new_selected_tf = std::dynamic_pointer_cast<data::transfer_function>(presets[new_selected_tf_key]);

    const auto current_tf = m_current_tf.lock();
    if(new_selected_tf && new_selected_tf->name() != current_tf->name())
    {
        current_tf->deep_copy(new_selected_tf);
        current_tf->async_emit(this, data::object::MODIFIED_SIG);
    }
}

//------------------------------------------------------------------------------

void transfer_function::update_default_preset()
{
    const auto opt_presets = m_opt_presets.lock();
    const auto image       = m_image.lock();
    if(image != nullptr && opt_presets == nullptr)
    {
        const data::transfer_function::sptr default_tf = data::transfer_function::create_default_tf(image->type());
        const std::string default_tf_name              = data::transfer_function::DEFAULT_TF_NAME;

        (*m_tf_presets)[default_tf_name] = default_tf;

        const auto current_tf = m_current_tf.lock();
        current_tf->deep_copy(default_tf);
        current_tf->async_emit(data::object::MODIFIED_SIG);
    }
}

//------------------------------------------------------------------------------

void transfer_function::delete_preset()
{
    if(m_preset_combo_box->count() < 2)
    {
        sight::ui::dialog::message message_box;
        message_box.set_title("Delete error");
        message_box.set_message("Transfer function cannot be deleted, at least one transfer function is required.");
        message_box.set_icon(sight::ui::dialog::message::warning);
        message_box.add_button(sight::ui::dialog::message::ok);
        message_box.show();
        return;
    }

    sight::ui::dialog::message message_box;
    message_box.set_title("Delete confirmation");
    message_box.set_message("Are you sure you want to delete this TF preset ?");
    message_box.set_icon(sight::ui::dialog::message::question);
    message_box.add_button(sight::ui::dialog::message::ok);
    message_box.add_button(sight::ui::dialog::message::cancel);
    sight::ui::dialog::message::buttons answer_copy = message_box.show();

    if(answer_copy != sight::ui::dialog::message::cancel)
    {
        const int index = std::max(m_preset_combo_box->currentIndex() - 1, 0);
        {
            // Remove the current TF preset from the Map.
            const std::string selected_tf_preset_key = m_preset_combo_box->currentText().toStdString();
            const auto opt_presets                   = m_opt_presets.lock();
            sight::data::map& presets                = (opt_presets != nullptr) ? *opt_presets : *m_tf_presets;

            const auto scoped_emitter = presets.scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));

            presets.erase(selected_tf_preset_key);

            m_preset_combo_box->removeItem(
                m_preset_combo_box->findText(
                    QString::fromStdString(
                        selected_tf_preset_key
                    )
                )
            );
        }

        // Set the current map
        this->preset_choice(index);
    }
}

//------------------------------------------------------------------------------

void transfer_function::create_preset()
{
    const auto& old_name = m_preset_combo_box->currentText().toStdString();
    sight::ui::dialog::input input;
    input.set_title("Create new transfer function");
    input.set_message("Enter transfer function name :");
    input.set_input(old_name);
    const auto& [newName, ok] = input.get_input();

    if(!ok)
    {
        return;
    }

    if(!newName.empty())
    {
        {
            const auto opt_presets    = m_opt_presets.lock();
            sight::data::map& presets = (opt_presets != nullptr) ? *opt_presets : *m_tf_presets;

            // Gets TF presets.
            if(!sight::module::ui::qt::image::transfer_function::has_preset_name(presets, newName))
            {
                // Create the new map.
                const auto image = m_image.lock();
                auto default_tf  = image
                                   ? data::transfer_function::create_default_tf(image->type())
                                   : data::transfer_function::create_default_tf();

                default_tf->set_name(newName);

                const auto scoped_emitter = presets.scoped_emit();
                scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
                presets[newName] = default_tf;

                // Recreates presets.
                m_preset_combo_box->clear();
                for(const auto& elt : presets)
                {
                    m_preset_combo_box->addItem(elt.first.c_str());
                }
            }
            else
            {
                sight::ui::dialog::message message_box;
                message_box.set_title("Warning");
                message_box.set_message("This TF preset name already exists so you can not overwrite it.");
                message_box.set_icon(sight::ui::dialog::message::warning);
                message_box.add_button(sight::ui::dialog::message::ok);
                message_box.show();
                return;
            }
        }

        // Set the current map.
        this->preset_choice(m_preset_combo_box->findText(QString::fromStdString(newName)));
    }
}

//------------------------------------------------------------------------------

void transfer_function::copy_preset()
{
    const auto& old_name = m_preset_combo_box->currentText().toStdString();

    sight::ui::dialog::input input;
    input.set_title("Copy transfer function");
    input.set_message("Enter new transfer function name:");
    input.set_input(old_name);
    const auto& [newName, ok] = input.get_input();

    if(!ok)
    {
        return;
    }

    if(!newName.empty())
    {
        {
            const auto opt_presets    = m_opt_presets.lock();
            sight::data::map& presets = (opt_presets != nullptr) ? *opt_presets : *m_tf_presets;
            // Gets TF presets.
            if(sight::module::ui::qt::image::transfer_function::has_preset_name(presets, newName))
            {
                sight::ui::dialog::message message_box;
                message_box.set_title("Error");
                message_box.set_message("This preset name already exists, please choose another one.");
                message_box.set_icon(sight::ui::dialog::message::warning);
                message_box.add_button(sight::ui::dialog::message::ok);
                message_box.show();
                return;
            }

            const auto current_tf = std::dynamic_pointer_cast<data::transfer_function>(presets[old_name]);
            SIGHT_ASSERT("Can not find current TF.", current_tf);

            data::transfer_function::sptr tf = data::object::copy(current_tf);

            tf->set_name(newName);

            const auto scoped_emitter = presets.scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
            presets[newName] = tf;

            // Recreates presets.
            m_preset_combo_box->clear();
            for(const auto& elt : presets)
            {
                m_preset_combo_box->addItem(elt.first.c_str());
            }
        }
        // Set the current map.
        this->preset_choice(m_preset_combo_box->findText(QString::fromStdString(newName)));
    }
    else
    {
        sight::ui::dialog::message message_box;
        message_box.set_title("Error");
        message_box.set_message("No preset name specified.");
        message_box.set_icon(sight::ui::dialog::message::warning);
        message_box.add_button(sight::ui::dialog::message::ok);
        message_box.show();
    }
}

//------------------------------------------------------------------------------

void transfer_function::reinitialize_presets()
{
    // Clears previous settings.
    m_preset_combo_box->clear();

    {
        const auto opt_presets = m_opt_presets.lock();
        if(opt_presets != nullptr)
        {
            // If we specify the presets, restore the initial state from the internal map
            const auto scoped_emitter = opt_presets->scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
            opt_presets->deep_copy(m_tf_presets);
        }
    }
    m_tf_presets->clear();

    // Initialize presets.
    this->initialize_presets();
}

//------------------------------------------------------------------------------

void transfer_function::rename_preset()
{
    const auto& old_name = m_preset_combo_box->currentText().toStdString();

    sight::ui::dialog::input input;
    input.set_title("Rename transfer function preset");
    input.set_message("Enter new name:");
    input.set_input(old_name);
    const auto& [newName, ok] = input.get_input();

    if(!ok)
    {
        return;
    }

    if(!newName.empty() && newName != old_name)
    {
        {
            const auto opt_presets    = m_opt_presets.lock();
            sight::data::map& presets = (opt_presets != nullptr) ? *opt_presets : *m_tf_presets;
            // Gets TF presets.
            if(!sight::module::ui::qt::image::transfer_function::has_preset_name(presets, newName))
            {
                auto tf = std::dynamic_pointer_cast<data::transfer_function>(presets[old_name]);

                // Rename the map.
                presets.erase(old_name);
                presets[newName] = tf;

                // Creates presets.
                m_preset_combo_box->clear();
                for(const auto& elt : presets)
                {
                    m_preset_combo_box->addItem(elt.first.c_str());
                }

                tf->set_name(newName);
            }
            else
            {
                sight::ui::dialog::message message_box;
                message_box.set_title("Warning");
                message_box.set_message("This TF preset name already exists so you can not overwrite it.");
                message_box.set_icon(sight::ui::dialog::message::warning);
                message_box.add_button(sight::ui::dialog::message::ok);
                message_box.show();
                return;
            }
        }

        // Set the current map.
        this->preset_choice(m_preset_combo_box->findText(QString::fromStdString(newName)));
    }
}

//------------------------------------------------------------------------------

void transfer_function::import_preset()
{
    const auto new_tf = std::make_shared<data::transfer_function>();

    const auto reader = sight::service::add<io::service::reader>("sight::module::io::session::reader");

    reader->set_inout(new_tf, io::service::DATA_KEY);

    service::config_t config;
    config.put("dialog.<xmlattr>.extension", ".tf");
    config.put("dialog.<xmlattr>.description", "Transfer Function Preset");
    config.put("archive.<xmlattr>.format", "filesystem");

    reader->configure(config);
    reader->start();
    reader->open_location_dialog();
    reader->update().wait();

    // Check the loaded map.
    if(!new_tf->empty())
    {
        int index = 0;
        {
            std::string preset_name(new_tf->name());

            const auto opt_presets    = m_opt_presets.lock();
            sight::data::map& presets = (opt_presets != nullptr) ? *opt_presets : *m_tf_presets;
            if(sight::module::ui::qt::image::transfer_function::has_preset_name(presets, preset_name))
            {
                preset_name = sight::module::ui::qt::image::transfer_function::create_preset_name(presets, preset_name);
            }

            const auto scoped_emitter = presets.scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
            presets[preset_name] = new_tf;
            new_tf->set_name(preset_name);

            m_preset_combo_box->addItem(QString(preset_name.c_str()));
            index = static_cast<int>(presets.size() - 1);
        }

        this->preset_choice(index);
    }

    reader->stop().wait();
    sight::service::remove(reader);
}

//------------------------------------------------------------------------------

void transfer_function::export_preset()
{
    const auto writer = sight::service::add<io::service::writer>("sight::module::io::session::writer");
    {
        const auto current_tf = m_current_tf.const_lock();
        writer->set_input(current_tf.get_shared(), io::service::DATA_KEY);
    }

    service::config_t config;
    config.add("dialog.<xmlattr>.extension", ".tf");
    config.add("dialog.<xmlattr>.description", "Transfer Function Preset");
    config.put("archive.<xmlattr>.format", "filesystem");

    writer->configure(config);
    writer->start();
    writer->open_location_dialog();
    writer->update().wait();
    writer->stop().wait();
    sight::service::remove(writer);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
