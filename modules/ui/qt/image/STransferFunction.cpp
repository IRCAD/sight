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

#include "modules/ui/qt/image/STransferFunction.hpp"

#include <core/com/Slots.hxx>
#include <core/runtime/path.hpp>

#include <data/Composite.hpp>
#include <data/TransferFunction.hpp>

#include <io/base/service/IReader.hpp>
#include <io/base/service/IWriter.hpp>

#include <service/base.hpp>

#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

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

static const std::string s_CONTEXT_TF = "TF_PRESET";
static const std::string s_VERSION_TF = "V1";

const core::com::Slots::SlotKeyType s_UPDATE_DEFAULT_PRESET_SLOT = "updateDefaultPreset";

//------------------------------------------------------------------------------

STransferFunction::STransferFunction()
{
    newSlot(s_UPDATE_DEFAULT_PRESET_SLOT, &STransferFunction::updateDefaultPreset, this);

    const std::filesystem::path modulePath = core::runtime::getModuleResourcePath(std::string("sight::module::ui::qt"));

    m_deleteIcon       = modulePath / "delete.png";
    m_newIcon          = modulePath / "new.png";
    m_copyIcon         = modulePath / "copy.png";
    m_renameIcon       = modulePath / "rename.png";
    m_reinitializeIcon = modulePath / "reinitialize.png";
    m_importIcon       = modulePath / "import.png";
    m_exportIcon       = modulePath / "export.png";
}

//------------------------------------------------------------------------------

void STransferFunction::configuring()
{
    this->initialize();

    const ConfigType tree = this->getConfiguration();
    const auto config     = tree.get_child_optional("config");

    bool useDefaultPath = true;
    if(config)
    {
        const auto pathCfg = config->equal_range(s_PATH_CONFIG);
        for(auto itCfg = pathCfg.first ; itCfg != pathCfg.second ; ++itCfg)
        {
            const auto additionalTfPath = itCfg->second.get_value<std::string>();
            if(!additionalTfPath.empty())
            {
                const auto path = core::runtime::getModuleResourceFilePath(additionalTfPath);
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
                m_deleteIcon = core::runtime::getModuleResourceFilePath(deleteIconCfg.value());
            }

            const auto newIconCfg = configAttr->get_optional<std::string>(s_NEW_ICON_CONFIG);
            if(newIconCfg)
            {
                m_newIcon = core::runtime::getModuleResourceFilePath(newIconCfg.value());
            }

            const auto copyIconCfg = configAttr->get_optional<std::string>(s_COPY_ICON_CONFIG);
            if(copyIconCfg)
            {
                m_copyIcon = core::runtime::getModuleResourceFilePath(copyIconCfg.value());
            }

            const auto reinitializeIconCfg = configAttr->get_optional<std::string>(s_REINITIALIZE_ICON_CONFIG);
            if(reinitializeIconCfg)
            {
                m_reinitializeIcon = core::runtime::getModuleResourceFilePath(reinitializeIconCfg.value());
            }

            const auto renameIconCfg = configAttr->get_optional<std::string>(s_RENAME_ICON_CONFIG);
            if(renameIconCfg)
            {
                m_renameIcon = core::runtime::getModuleResourceFilePath(renameIconCfg.value());
            }

            const auto importIconCfg = configAttr->get_optional<std::string>(s_IMPORT_ICON_CONFIG);
            if(importIconCfg)
            {
                m_importIcon = core::runtime::getModuleResourceFilePath(importIconCfg.value());
            }

            const auto exportIconCfg = configAttr->get_optional<std::string>(s_EXPORT_ICON_CONFIG);
            if(exportIconCfg)
            {
                m_exportIcon = core::runtime::getModuleResourceFilePath(exportIconCfg.value());
            }

            m_iconWidth  = configAttr->get<unsigned int>(s_ICON_WIDTH_CONFIG, m_iconWidth);
            m_iconHeight = configAttr->get<unsigned int>(s_ICON_HEIGHT_CONFIG, m_iconHeight);
        }
    }

    if(useDefaultPath)
    {
        const auto pathRoot = core::runtime::getModuleResourceFilePath("sight::module::ui::qt", "tf");
        m_paths.push_back(pathRoot);
    }
}

//------------------------------------------------------------------------------

void STransferFunction::starting()
{
    this->create();

    // Get the Qt container
    const auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

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
    QObject::connect(m_presetComboBox, qOverload<int>(&QComboBox::activated), this, &STransferFunction::presetChoice);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &STransferFunction::deletePreset);
    QObject::connect(m_newButton, &QPushButton::clicked, this, &STransferFunction::createPreset);
    QObject::connect(m_copyButton, &QPushButton::clicked, this, &STransferFunction::copyPreset);
    QObject::connect(m_reinitializeButton, &QPushButton::clicked, this, &STransferFunction::reinitializePresets);
    QObject::connect(m_renameButton, &QPushButton::clicked, this, &STransferFunction::renamePreset);
    QObject::connect(m_importButton, &QPushButton::clicked, this, &STransferFunction::importPreset);
    QObject::connect(m_exportButton, &QPushButton::clicked, this, &STransferFunction::exportPreset);

    // Initializes the TF preset from paths.
    this->initializePresets();

    this->updateDefaultPreset();
}

//------------------------------------------------------------------------------

void STransferFunction::updating()
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
    sight::data::Composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
    const auto newSelectedTF        = std::dynamic_pointer_cast<data::TransferFunction>(presets[selectedTFKey]);
    {
        auto tfLock          = data::mt::locked_ptr(newSelectedTF);
        const auto currentTf = m_currentTF.lock();
        newSelectedTF->deepCopy(currentTf.get_shared());
        auto sig = newSelectedTF->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap STransferFunction::getAutoConnections() const
{
    return {
        {s_IMAGE_INPUT, data::Image::s_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_IMAGE_INPUT, data::Image::s_BUFFER_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_CURRENT_INPUT, data::TransferFunction::s_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_CURRENT_INPUT, data::TransferFunction::s_POINTS_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_CURRENT_INPUT, data::TransferFunction::s_WINDOWING_MODIFIED_SIG, IService::slots::s_UPDATE}
    };
}

//------------------------------------------------------------------------------

bool STransferFunction::hasPresetName(const sight::data::Composite& presets, const std::string& _name)
{
    return presets.find(_name) != presets.end();
}

//------------------------------------------------------------------------------

std::string STransferFunction::createPresetName(
    const sight::data::Composite& presets,
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

void STransferFunction::initializePresets()
{
    m_tfPresets = sight::data::Composite::New();
    std::string currentPresetName = data::TransferFunction::s_DEFAULT_TF_NAME;

    {
        const auto optPresets           = m_optPresets.lock();
        sight::data::Composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
        if(optPresets != nullptr)
        {
            // If we specify the presets, use the internal map to save initial state
            m_tfPresets->deepCopy(optPresets.get_shared());

            currentPresetName = presets.begin()->first;
        }
        else
        {
            // Add the default TF if it not exists.
            const std::string& defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;

            if(!this->hasPresetName(presets, defaultTFName))
            {
                const auto image = m_image.lock();
                if(image)
                {
                    const data::TransferFunction::sptr defaultTf =
                        data::TransferFunction::createDefaultTF(image->getType());

                    const auto scoped_emitter = presets.scoped_emit();
                    presets[defaultTFName] = defaultTf;
                }
            }

            // Test if transfer function composite has few TF
            if(presets.size() <= 1)
            {
                // Creates the TF reader.
                const auto tf       = data::TransferFunction::New();
                const auto tfReader = service::add<io::base::service::IReader>("sight::module::io::session::SReader");
                tfReader->setInOut(tf, io::base::service::s_DATA_KEY);

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
                            service::IService::ConfigType config;
                            config.put("file", file.string());
                            config.put("archive.<xmlattr>.format", "filesystem");

                            tfReader->setConfiguration(config);
                            tfReader->configure();
                            tfReader->start();
                            tfReader->update();
                            tfReader->stop();

                            if(!tf->name().empty())
                            {
                                const data::TransferFunction::sptr newTF =
                                    data::Object::copy<data::TransferFunction>(tf);
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
                service::remove(tfReader);
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
                    if(elt.second == currentTfPreset.get_shared())
                    {
                        currentPresetName = elt.first;
                        break;
                    }
                }
            }
        }
    }

    const int index = m_presetComboBox->findText(QString::fromStdString(currentPresetName));

    // Set the current composite
    this->presetChoice(index);
}

//------------------------------------------------------------------------------

void STransferFunction::presetChoice(int _index)
{
    m_presetComboBox->setCurrentIndex(_index);
    // Set the output to the current preset.
    this->setCurrentPreset();

    const std::string tfName = m_presetComboBox->currentText().toStdString();
    const bool isEnabled     = (tfName != data::TransferFunction::s_DEFAULT_TF_NAME);

    m_renameButton->setEnabled(isEnabled);
    m_deleteButton->setEnabled(isEnabled);
}

//------------------------------------------------------------------------------

void STransferFunction::setCurrentPreset()
{
    const std::string newSelectedTFKey = m_presetComboBox->currentText().toStdString();

    const auto optPresets           = m_optPresets.lock();
    sight::data::Composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
    const auto newSelectedTF        = std::dynamic_pointer_cast<data::TransferFunction>(presets[newSelectedTFKey]);

    const auto currentTf = m_currentTF.lock();
    if(newSelectedTF && newSelectedTF->name() != currentTf->name())
    {
        currentTf->deepCopy(newSelectedTF);
        auto sig = currentTf->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        core::com::Connection::Blocker block(sig->getConnection(slot(IService::slots::s_UPDATE)));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::updateDefaultPreset()
{
    const auto optPresets = m_optPresets.lock();
    const auto image      = m_image.lock();
    if(image != nullptr && optPresets == nullptr)
    {
        const data::TransferFunction::sptr defaultTf = data::TransferFunction::createDefaultTF(image->getType());
        const std::string defaultTFName              = data::TransferFunction::s_DEFAULT_TF_NAME;

        (*m_tfPresets)[defaultTFName] = defaultTf;

        const auto currentTf = m_currentTF.lock();
        currentTf->deepCopy(defaultTf);
        auto sig = currentTf->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::deletePreset()
{
    if(m_presetComboBox->count() < 2)
    {
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Delete error");
        messageBox.setMessage("Transfer function cannot be deleted, at least one transfer function is required.");
        messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
        messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
        return;
    }

    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Delete confirmation");
    messageBox.setMessage("Are you sure you want to delete this TF preset ?");
    messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::CANCEL);
    sight::ui::base::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if(answerCopy != sight::ui::base::dialog::IMessageDialog::CANCEL)
    {
        const int index = std::max(m_presetComboBox->currentIndex() - 1, 0);
        {
            // Remove the current TF preset from the Composite.
            const std::string selectedTFPresetKey = m_presetComboBox->currentText().toStdString();
            const auto optPresets                 = m_optPresets.lock();
            sight::data::Composite& presets       = (optPresets != nullptr) ? *optPresets : *m_tfPresets;

            const auto scoped_emitter = presets.scoped_emit();

            presets.erase(selectedTFPresetKey);

            m_presetComboBox->removeItem(m_presetComboBox->findText(QString::fromStdString(selectedTFPresetKey)));
        }

        // Set the current composite
        this->presetChoice(index);
    }
}

//------------------------------------------------------------------------------

void STransferFunction::createPreset()
{
    const auto& oldName = m_presetComboBox->currentText().toStdString();
    sight::ui::base::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Create new transfer function");
    inputDialog.setMessage("Enter transfer function name :");
    inputDialog.setInput(oldName);
    const auto& [newName, ok] = inputDialog.getInput();

    if(!ok)
    {
        return;
    }

    if(!newName.empty())
    {
        {
            const auto optPresets           = m_optPresets.lock();
            sight::data::Composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;

            // Gets TF presets.
            if(!this->hasPresetName(presets, newName))
            {
                // Create the new composite.
                const auto image = m_image.lock();
                auto defaultTf   = image
                                   ? data::TransferFunction::createDefaultTF(image->getType())
                                   : data::TransferFunction::createDefaultTF();

                defaultTf->setName(newName);

                const auto scoped_emitter = presets.scoped_emit();
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
                sight::ui::base::dialog::MessageDialog messageBox;
                messageBox.setTitle("Warning");
                messageBox.setMessage("This TF preset name already exists so you can not overwrite it.");
                messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
                messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
                messageBox.show();
                return;
            }
        }

        // Set the current composite.
        this->presetChoice(m_presetComboBox->findText(QString::fromStdString(newName)));
    }
}

//------------------------------------------------------------------------------

void STransferFunction::copyPreset()
{
    const auto& oldName = m_presetComboBox->currentText().toStdString();

    sight::ui::base::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Copy transfer function");
    inputDialog.setMessage("Enter new transfer function name:");
    inputDialog.setInput(oldName);
    const auto& [newName, ok] = inputDialog.getInput();

    if(!ok)
    {
        return;
    }

    if(!newName.empty())
    {
        {
            const auto optPresets           = m_optPresets.lock();
            sight::data::Composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
            // Gets TF presets.
            if(this->hasPresetName(presets, newName))
            {
                sight::ui::base::dialog::MessageDialog messageBox;
                messageBox.setTitle("Error");
                messageBox.setMessage("This preset name already exists, please choose another one.");
                messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
                messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
                messageBox.show();
                return;
            }

            const auto currentTF = std::dynamic_pointer_cast<data::TransferFunction>(presets[oldName]);
            SIGHT_ASSERT("Can not find current TF.", currentTF);

            data::TransferFunction::sptr tf = data::Object::copy(currentTF);

            tf->setName(newName);

            const auto scoped_emitter = presets.scoped_emit();
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
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Error");
        messageBox.setMessage("No preset name specified.");
        messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
        messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::reinitializePresets()
{
    // Clears previous settings.
    m_presetComboBox->clear();

    {
        const auto optPresets = m_optPresets.lock();
        if(optPresets != nullptr)
        {
            // If we specify the presets, restore the initial state from the internal map
            const auto scoped_emitter = optPresets->scoped_emit();
            optPresets->deepCopy(m_tfPresets);
        }
    }
    m_tfPresets->clear();

    // Initialize presets.
    this->initializePresets();
}

//------------------------------------------------------------------------------

void STransferFunction::renamePreset()
{
    const auto& oldName = m_presetComboBox->currentText().toStdString();

    sight::ui::base::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Rename transfer function preset");
    inputDialog.setMessage("Enter new name:");
    inputDialog.setInput(oldName);
    const auto& [newName, ok] = inputDialog.getInput();

    if(!ok)
    {
        return;
    }

    if(!newName.empty() && newName != oldName)
    {
        {
            const auto optPresets           = m_optPresets.lock();
            sight::data::Composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
            // Gets TF presets.
            if(!this->hasPresetName(presets, newName))
            {
                auto tf = std::dynamic_pointer_cast<data::TransferFunction>(presets[oldName]);

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
                sight::ui::base::dialog::MessageDialog messageBox;
                messageBox.setTitle("Warning");
                messageBox.setMessage("This TF preset name already exists so you can not overwrite it.");
                messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
                messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
                messageBox.show();
                return;
            }
        }

        // Set the current composite.
        this->presetChoice(m_presetComboBox->findText(QString::fromStdString(newName)));
    }
}

//------------------------------------------------------------------------------

void STransferFunction::importPreset()
{
    const auto newTF = data::TransferFunction::New();

    const auto reader = service::add<io::base::service::IReader>("sight::module::io::session::SReader");

    reader->setInOut(newTF, io::base::service::s_DATA_KEY);

    service::IService::ConfigType config;
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
            sight::data::Composite& presets = (optPresets != nullptr) ? *optPresets : *m_tfPresets;
            if(this->hasPresetName(presets, presetName))
            {
                presetName = this->createPresetName(presets, presetName);
            }

            const auto scoped_emitter = presets.scoped_emit();
            presets[presetName] = newTF;
            newTF->setName(presetName);

            m_presetComboBox->addItem(QString(presetName.c_str()));
            index = static_cast<int>(presets.size() - 1);
        }

        this->presetChoice(index);
    }

    reader->stop().wait();
    service::remove(reader);
}

//------------------------------------------------------------------------------

void STransferFunction::exportPreset()
{
    const auto writer = service::add<io::base::service::IWriter>("sight::module::io::session::SWriter");
    {
        const auto currentTf = m_currentTF.const_lock();
        writer->setInput(currentTf.get_shared(), io::base::service::s_DATA_KEY);
    }

    service::IService::ConfigType config;
    config.add("dialog.<xmlattr>.extension", ".tf");
    config.add("dialog.<xmlattr>.description", "Transfer Function Preset");
    config.put("archive.<xmlattr>.format", "filesystem");

    writer->configure(config);
    writer->start();
    writer->openLocationDialog();
    writer->update().wait();
    writer->stop().wait();
    service::remove(writer);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
