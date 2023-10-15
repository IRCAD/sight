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

    const std::filesystem::path modulePath = core::runtime::get_module_resource_path(
        std::string(
            "sight::module::ui::qt"
        )
    );

    m_deleteIcon       = modulePath / "delete.png";
    m_newIcon          = modulePath / "new.png";
    m_copyIcon         = modulePath / "copy.png";
    m_renameIcon       = modulePath / "rename.png";
    m_reinitializeIcon = modulePath / "reinitialize.png";
    m_importIcon       = modulePath / "import.png";
    m_exportIcon       = modulePath / "export.png";
}

//------------------------------------------------------------------------------

void transfer_function::configuring()
{
    this->initialize();

    const config_t tree = this->get_config();
    const auto config   = tree.get_child_optional("config");

    bool useDefaultPath = true;
    if(config)
    {
        const auto pathCfg = config->equal_range(s_PATH_CONFIG);
        for(auto itCfg = pathCfg.first ; itCfg != pathCfg.second ; ++itCfg)
        {
            const auto additionalTfPath = itCfg->second.get_value<std::string>();
            if(!additionalTfPath.empty())
            {
                const auto path = core::runtime::get_module_resource_file_path(additionalTfPath);
                m_paths.push_back(path);
            }
        }

        const auto configAttr = config->get_child_optional("<xmlattr>");

        if(configAttr)
        {
            useDefaultPath = configAttr->get<bool>(s_USE_DEFAULT_PATH_CONFIG, useDefaultPath);

            const auto deleteIconCfg = configAttr->get_optional<std::string>(s_DELETE_ICON_CONFIG);
            if(deleteIconCfg)
            {
                m_deleteIcon = core::runtime::get_module_resource_file_path(deleteIconCfg.value());
            }

            const auto newIconCfg = configAttr->get_optional<std::string>(s_NEW_ICON_CONFIG);
            if(newIconCfg)
            {
                m_newIcon = core::runtime::get_module_resource_file_path(newIconCfg.value());
            }

            const auto copyIconCfg = configAttr->get_optional<std::string>(s_COPY_ICON_CONFIG);
            if(copyIconCfg)
            {
                m_copyIcon = core::runtime::get_module_resource_file_path(copyIconCfg.value());
            }

            const auto reinitializeIconCfg = configAttr->get_optional<std::string>(s_REINITIALIZE_ICON_CONFIG);
            if(reinitializeIconCfg)
            {
                m_reinitializeIcon = core::runtime::get_module_resource_file_path(reinitializeIconCfg.value());
            }

            const auto renameIconCfg = configAttr->get_optional<std::string>(s_RENAME_ICON_CONFIG);
            if(renameIconCfg)
            {
                m_renameIcon = core::runtime::get_module_resource_file_path(renameIconCfg.value());
            }

            const auto importIconCfg = configAttr->get_optional<std::string>(s_IMPORT_ICON_CONFIG);
            if(importIconCfg)
            {
                m_importIcon = core::runtime::get_module_resource_file_path(importIconCfg.value());
            }

            const auto exportIconCfg = configAttr->get_optional<std::string>(s_EXPORT_ICON_CONFIG);
            if(exportIconCfg)
            {
                m_exportIcon = core::runtime::get_module_resource_file_path(exportIconCfg.value());
            }

            m_iconWidth  = configAttr->get<unsigned int>(s_ICON_WIDTH_CONFIG, m_iconWidth);
            m_iconHeight = configAttr->get<unsigned int>(s_ICON_HEIGHT_CONFIG, m_iconHeight);
        }
    }

    if(useDefaultPath)
    {
        const auto pathRoot = core::runtime::get_module_resource_file_path("sight::module::ui::qt", "tf");
        m_paths.push_back(pathRoot);
    }
}

//------------------------------------------------------------------------------

void transfer_function::starting()
{
    this->create();

    // Get the Qt container
    const auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

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
        int iconWidth  = int(m_iconWidth);
        int iconHeight = int(m_iconHeight);
        m_deleteButton->setIconSize(QSize(iconWidth, iconHeight));
        m_newButton->setIconSize(QSize(iconWidth, iconHeight));
        m_copyButton->setIconSize(QSize(iconWidth, iconHeight));
        m_reinitializeButton->setIconSize(QSize(iconWidth, iconHeight));
        m_renameButton->setIconSize(QSize(iconWidth, iconHeight));
        m_importButton->setIconSize(QSize(iconWidth, iconHeight));
        m_exportButton->setIconSize(QSize(iconWidth, iconHeight));
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

    qtContainer->setLayout(layout);

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
    std::string selectedTFKey = m_presetComboBox->currentText().toStdString();
    const std::string tfName  = [this]{const auto currentTf = m_currentTF.lock(); return currentTf->name();}();

    if(selectedTFKey != tfName)
    {
        const int index = m_presetComboBox->findText(QString::fromStdString(tfName));
        if(index < 0)
        {
            SIGHT_ERROR("Could not find transfer function '" + tfName + "'");
            return;
        }

        this->presetChoice(index);
        // Update the selected key now that we have found the one we were looking for
        selectedTFKey = m_presetComboBox->currentText().toStdString();
    }

    // Now updates the TF
    const auto optPresets           = m_optPresets.lock();
    sight::data::composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
    const auto newSelectedTF        = std::dynamic_pointer_cast<data::transfer_function>(presets[selectedTFKey]);
    {
        auto tfLock          = data::mt::locked_ptr(newSelectedTF);
        const auto currentTf = m_currentTF.lock();
        newSelectedTF->deep_copy(currentTf.get_shared());
        auto sig = newSelectedTF->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
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

bool transfer_function::hasPresetName(const sight::data::composite& presets, const std::string& _name)
{
    return presets.find(_name) != presets.end();
}

//------------------------------------------------------------------------------

std::string transfer_function::createPresetName(
    const sight::data::composite& presets,
    const std::string& _basename
) const
{
    bool hasTransferFunctionName = true;
    std::string newName          = _basename;
    int cpt                      = 1;
    while(hasTransferFunctionName)
    {
        std::stringstream tmpStr;
        tmpStr << _basename << "_" << cpt;
        newName                 = tmpStr.str();
        hasTransferFunctionName = this->hasPresetName(presets, newName);
        cpt++;
    }

    return newName;
}

//------------------------------------------------------------------------------

void transfer_function::initializePresets(const std::string& _currentPresetName)
{
    m_tfPresets = std::make_shared<sight::data::composite>();
    std::string currentPresetName = _currentPresetName;

    {
        const auto optPresets           = m_optPresets.lock();
        sight::data::composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
        if(optPresets != nullptr)
        {
            // If we specify the presets, use the internal map to save initial state
            m_tfPresets->deep_copy(optPresets.get_shared());

            if(presets.find(currentPresetName) == presets.end())
            {
                currentPresetName = presets.begin()->first;
            }
        }
        else
        {
            // Add the default TF if it not exists.
            const std::string& defaultTFName = data::transfer_function::s_DEFAULT_TF_NAME;

            if(!this->hasPresetName(presets, defaultTFName))
            {
                const auto image = m_image.lock();
                if(image)
                {
                    const data::transfer_function::sptr defaultTf =
                        data::transfer_function::createDefaultTF(image->getType());

                    const auto scoped_emitter = presets.scoped_emit();
                    scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
                    presets[defaultTFName] = defaultTf;
                }
            }

            // Test if transfer function composite has few TF
            if(presets.size() <= 1)
            {
                // Creates the TF reader.
                const auto tf       = std::make_shared<data::transfer_function>();
                const auto tfReader = sight::service::add<io::service::reader>(
                    "sight::module::io::session::reader"
                );
                tfReader->set_inout(tf, io::service::s_DATA_KEY);

                // Parse all paths contained in m_path and read basic TF.
                for(const std::filesystem::path& dirPath : m_paths)
                {
                    SIGHT_ASSERT("Invalid directory path '" + dirPath.string() + "'", std::filesystem::exists(dirPath));
                    for(std::filesystem::directory_iterator it(dirPath) ;
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

                            tfReader->set_config(config);
                            tfReader->configure();
                            tfReader->start();
                            tfReader->update();
                            tfReader->stop();

                            if(!tf->name().empty())
                            {
                                const data::transfer_function::sptr newTF =
                                    data::object::copy<data::transfer_function>(tf);
                                if(this->hasPresetName(presets, newTF->name()))
                                {
                                    newTF->setName(this->createPresetName(presets, newTF->name()));
                                }

                                presets[newTF->name()] = newTF;
                            }
                        }
                    }
                }

                // Delete the reader.
                sight::service::remove(tfReader);
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
            const auto currentTfPreset = m_currentTF.lock();

            if(currentTfPreset)
            {
                for(const auto& elt : presets)
                {
                    if(std::dynamic_pointer_cast<sight::data::transfer_function>(elt.second)->name()
                       == currentTfPreset->name())
                    {
                        currentPresetName = elt.first;
                        break;
                    }
                }
            }
        }
    }

    int index = m_presetComboBox->findText(QString::fromStdString(currentPresetName));
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

    const std::string tfName = m_presetComboBox->currentText().toStdString();
    const bool isEnabled     = (tfName != data::transfer_function::s_DEFAULT_TF_NAME);

    m_renameButton->setEnabled(isEnabled);
    m_deleteButton->setEnabled(isEnabled);
}

//------------------------------------------------------------------------------

void transfer_function::setCurrentPreset()
{
    const std::string newSelectedTFKey = m_presetComboBox->currentText().toStdString();

    const auto optPresets           = m_optPresets.lock();
    sight::data::composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
    const auto newSelectedTF        = std::dynamic_pointer_cast<data::transfer_function>(presets[newSelectedTFKey]);

    const auto currentTf = m_currentTF.lock();
    if(newSelectedTF && newSelectedTF->name() != currentTf->name())
    {
        currentTf->deep_copy(newSelectedTF);
        auto sig = currentTf->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void transfer_function::updateDefaultPreset()
{
    const auto optPresets = m_optPresets.lock();
    const auto image      = m_image.lock();
    if(image != nullptr && optPresets == nullptr)
    {
        const data::transfer_function::sptr defaultTf = data::transfer_function::createDefaultTF(image->getType());
        const std::string defaultTFName               = data::transfer_function::s_DEFAULT_TF_NAME;

        (*m_tfPresets)[defaultTFName] = defaultTf;

        const auto currentTf = m_currentTF.lock();
        currentTf->deep_copy(defaultTf);
        auto sig = currentTf->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void transfer_function::deletePreset()
{
    if(m_presetComboBox->count() < 2)
    {
        sight::ui::dialog::message messageBox;
        messageBox.setTitle("Delete error");
        messageBox.setMessage("Transfer function cannot be deleted, at least one transfer function is required.");
        messageBox.setIcon(sight::ui::dialog::message::WARNING);
        messageBox.addButton(sight::ui::dialog::message::OK);
        messageBox.show();
        return;
    }

    sight::ui::dialog::message messageBox;
    messageBox.setTitle("Delete confirmation");
    messageBox.setMessage("Are you sure you want to delete this TF preset ?");
    messageBox.setIcon(sight::ui::dialog::message::QUESTION);
    messageBox.addButton(sight::ui::dialog::message::OK);
    messageBox.addButton(sight::ui::dialog::message::CANCEL);
    sight::ui::dialog::message::Buttons answerCopy = messageBox.show();

    if(answerCopy != sight::ui::dialog::message::CANCEL)
    {
        const int index = std::max(m_presetComboBox->currentIndex() - 1, 0);
        {
            // Remove the current TF preset from the Composite.
            const std::string selectedTFPresetKey = m_presetComboBox->currentText().toStdString();
            const auto optPresets                 = m_optPresets.lock();
            sight::data::composite& presets       = (optPresets != nullptr) ? *optPresets : *m_tfPresets;

            const auto scoped_emitter = presets.scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));

            presets.erase(selectedTFPresetKey);

            m_presetComboBox->removeItem(m_presetComboBox->findText(QString::fromStdString(selectedTFPresetKey)));
        }

        // Set the current composite
        this->presetChoice(index);
    }
}

//------------------------------------------------------------------------------

void transfer_function::createPreset()
{
    const auto& oldName = m_presetComboBox->currentText().toStdString();
    sight::ui::dialog::input input;
    input.setTitle("Create new transfer function");
    input.setMessage("Enter transfer function name :");
    input.set_input(oldName);
    const auto& [newName, ok] = input.getInput();

    if(!ok)
    {
        return;
    }

    if(!newName.empty())
    {
        {
            const auto optPresets           = m_optPresets.lock();
            sight::data::composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;

            // Gets TF presets.
            if(!this->hasPresetName(presets, newName))
            {
                // Create the new composite.
                const auto image = m_image.lock();
                auto defaultTf   = image
                                   ? data::transfer_function::createDefaultTF(image->getType())
                                   : data::transfer_function::createDefaultTF();

                defaultTf->setName(newName);

                const auto scoped_emitter = presets.scoped_emit();
                scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
                presets[newName] = defaultTf;

                // Recreates presets.
                m_presetComboBox->clear();
                for(const auto& elt : presets)
                {
                    m_presetComboBox->addItem(elt.first.c_str());
                }
            }
            else
            {
                sight::ui::dialog::message messageBox;
                messageBox.setTitle("Warning");
                messageBox.setMessage("This TF preset name already exists so you can not overwrite it.");
                messageBox.setIcon(sight::ui::dialog::message::WARNING);
                messageBox.addButton(sight::ui::dialog::message::OK);
                messageBox.show();
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
    const auto& oldName = m_presetComboBox->currentText().toStdString();

    sight::ui::dialog::input input;
    input.setTitle("Copy transfer function");
    input.setMessage("Enter new transfer function name:");
    input.set_input(oldName);
    const auto& [newName, ok] = input.getInput();

    if(!ok)
    {
        return;
    }

    if(!newName.empty())
    {
        {
            const auto optPresets           = m_optPresets.lock();
            sight::data::composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
            // Gets TF presets.
            if(this->hasPresetName(presets, newName))
            {
                sight::ui::dialog::message messageBox;
                messageBox.setTitle("Error");
                messageBox.setMessage("This preset name already exists, please choose another one.");
                messageBox.setIcon(sight::ui::dialog::message::WARNING);
                messageBox.addButton(sight::ui::dialog::message::OK);
                messageBox.show();
                return;
            }

            const auto currentTF = std::dynamic_pointer_cast<data::transfer_function>(presets[oldName]);
            SIGHT_ASSERT("Can not find current TF.", currentTF);

            data::transfer_function::sptr tf = data::object::copy(currentTF);

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
        sight::ui::dialog::message messageBox;
        messageBox.setTitle("Error");
        messageBox.setMessage("No preset name specified.");
        messageBox.setIcon(sight::ui::dialog::message::WARNING);
        messageBox.addButton(sight::ui::dialog::message::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void transfer_function::reinitializePresets()
{
    // Clears previous settings.
    m_presetComboBox->clear();

    {
        const auto optPresets = m_optPresets.lock();
        if(optPresets != nullptr)
        {
            // If we specify the presets, restore the initial state from the internal map
            const auto scoped_emitter = optPresets->scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
            optPresets->deep_copy(m_tfPresets);
        }
    }
    m_tfPresets->clear();

    // Initialize presets.
    this->initializePresets();
}

//------------------------------------------------------------------------------

void transfer_function::renamePreset()
{
    const auto& oldName = m_presetComboBox->currentText().toStdString();

    sight::ui::dialog::input input;
    input.setTitle("Rename transfer function preset");
    input.setMessage("Enter new name:");
    input.set_input(oldName);
    const auto& [newName, ok] = input.getInput();

    if(!ok)
    {
        return;
    }

    if(!newName.empty() && newName != oldName)
    {
        {
            const auto optPresets           = m_optPresets.lock();
            sight::data::composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
            // Gets TF presets.
            if(!this->hasPresetName(presets, newName))
            {
                auto tf = std::dynamic_pointer_cast<data::transfer_function>(presets[oldName]);

                // Rename the composite.
                presets.erase(oldName);
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
                sight::ui::dialog::message messageBox;
                messageBox.setTitle("Warning");
                messageBox.setMessage("This TF preset name already exists so you can not overwrite it.");
                messageBox.setIcon(sight::ui::dialog::message::WARNING);
                messageBox.addButton(sight::ui::dialog::message::OK);
                messageBox.show();
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
    const auto newTF = std::make_shared<data::transfer_function>();

    const auto reader = sight::service::add<io::service::reader>("sight::module::io::session::reader");

    reader->set_inout(newTF, io::service::s_DATA_KEY);

    service::config_t config;
    config.put("dialog.<xmlattr>.extension", ".tf");
    config.put("dialog.<xmlattr>.description", "Transfer Function Preset");
    config.put("archive.<xmlattr>.format", "filesystem");

    reader->configure(config);
    reader->start();
    reader->openLocationDialog();
    reader->update().wait();

    // Check the loaded composite.
    if(!newTF->empty())
    {
        int index = 0;
        {
            std::string presetName(newTF->name());

            const auto optPresets           = m_optPresets.lock();
            sight::data::composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
            if(this->hasPresetName(presets, presetName))
            {
                presetName = this->createPresetName(presets, presetName);
            }

            const auto scoped_emitter = presets.scoped_emit();
            scoped_emitter->block(slot(UPDATE_PRESETS_SLOT));
            presets[presetName] = newTF;
            newTF->setName(presetName);

            m_presetComboBox->addItem(QString(presetName.c_str()));
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
        const auto currentTf = m_currentTF.const_lock();
        writer->set_input(currentTf.get_shared(), io::service::s_DATA_KEY);
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
