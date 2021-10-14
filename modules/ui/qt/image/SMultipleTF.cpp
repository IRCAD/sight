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

#include "modules/ui/qt/image/SMultipleTF.hpp"

#include <core/runtime/EConfigurationElement.hpp>
#include <core/runtime/operations.hpp>

#include <data/helper/Composite.hpp>
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
static const std::string s_TF_PER_PATH_CONFIG       = "tfPerPath";
static const std::string s_DELETE_ICON_CONFIG       = "deleteIcon";
static const std::string s_NEW_ICON_CONFIG          = "newIcon";
static const std::string s_COPY_ICON_CONFIG         = "copyIcon";
static const std::string s_REINITIALIZE_ICON_CONFIG = "reinitializeIcon";
static const std::string s_RENAME_ICON_CONFIG       = "renameIcon";
static const std::string s_IMPORT_ICON_CONFIG       = "importIcon";
static const std::string s_EXPORT_ICON_CONFIG       = "exportIcon";
static const std::string s_ICON_WIDTH_CONFIG        = "iconWidth";
static const std::string s_ICON_HEIGHT_CONFIG       = "iconHeight";

static const std::string s_CONTEXT_TF = "TF_POOL";
static const std::string s_VERSION_TF = "V1";

//------------------------------------------------------------------------------

SMultipleTF::SMultipleTF()
{
    const std::filesystem::path modulePath = core::runtime::getModuleResourcePath(std::string("sight::module::ui::qt"));

    m_deleteIcon       = modulePath / "delete.png";
    m_newIcon          = modulePath / "new.png";
    m_copyIcon         = modulePath / "new.png";
    m_renameIcon       = modulePath / "rename.png";
    m_reinitializeIcon = modulePath / "reinitialize.png";
    m_importIcon       = modulePath / "import.png";
    m_exportIcon       = modulePath / "export.png";
}

//------------------------------------------------------------------------------

SMultipleTF::~SMultipleTF() noexcept
{
}

//------------------------------------------------------------------------------

void SMultipleTF::configuring()
{
    this->initialize();

    const ConfigType tree = this->getConfigTree();
    const auto config     = tree.get_child_optional("config");

    bool useDefaultPath = true;
    if(config)
    {
        const auto pathCfg = config->equal_range(s_PATH_CONFIG);
        for(auto itCfg = pathCfg.first ; itCfg != pathCfg.second ; ++itCfg)
        {
            const auto path = core::runtime::getModuleResourceFilePath(itCfg->second.get_value<std::string>());
            m_paths.push_back(path);
        }

        const auto configAttr = config->get_child_optional("<xmlattr>");

        if(configAttr)
        {
            useDefaultPath = configAttr->get<bool>(s_USE_DEFAULT_PATH_CONFIG, useDefaultPath);
            m_tfPerPath    = configAttr->get<bool>(s_TF_PER_PATH_CONFIG, m_tfPerPath);

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

void SMultipleTF::starting()
{
    this->create();

    // Get the Qt container
    const auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    // Buttons creation
    m_tfPoolsPreset = new QComboBox();

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
        m_deleteButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_newButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_copyButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_reinitializeButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_renameButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_importButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_exportButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
    }

    // Layout management
    QBoxLayout* const layout = new QBoxLayout(QBoxLayout::LeftToRight);

    layout->addWidget(m_tfPoolsPreset);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_newButton);
    layout->addWidget(m_copyButton);
    layout->addWidget(m_reinitializeButton);
    layout->addWidget(m_renameButton);
    layout->addWidget(m_importButton);
    layout->addWidget(m_exportButton);

    qtContainer->setLayout(layout);

    // Manage connection with the editor.
    QObject::connect(m_tfPoolsPreset, qOverload<int>(&QComboBox::activated), this, &SMultipleTF::presetChoice);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &SMultipleTF::deletePool);
    QObject::connect(m_newButton, &QPushButton::clicked, this, &SMultipleTF::newPool);
    QObject::connect(m_copyButton, &QPushButton::clicked, this, &SMultipleTF::copyPool);
    QObject::connect(m_reinitializeButton, &QPushButton::clicked, this, &SMultipleTF::reinitializePools);
    QObject::connect(m_renameButton, &QPushButton::clicked, this, &SMultipleTF::renamePool);
    QObject::connect(m_importButton, &QPushButton::clicked, this, &SMultipleTF::importPool);
    QObject::connect(m_exportButton, &QPushButton::clicked, this, &SMultipleTF::exportPool);

    // Initializes the TF pool from paths.
    this->initializePools();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SMultipleTF::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_TF_POOLS, data::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_TF_POOLS, data::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_TF_POOLS, data::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SMultipleTF::updating()
{
}

//------------------------------------------------------------------------------

void SMultipleTF::swapping(std::string_view _key)
{
    // Avoid swapping if it's the same TF.
    if(_key == s_CURRENT_TF_POOL)
    {
        const auto tfPool = m_currentTfPool.lock();

        if(tfPool && tfPool.get_shared() != m_selectedTFPool.lock())
        {
            this->updatePoolsPreset();
        }
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

bool SMultipleTF::hasPoolName(const std::string& _name, data::Composite::csptr _tfPools) const
{
    return _tfPools->find(_name) != _tfPools->end();
}

//------------------------------------------------------------------------------

std::string SMultipleTF::createPoolName(const std::string& _basename, data::Composite::csptr _tfPools) const
{
    bool hasTransferFunctionName = true;
    std::string newName          = _basename;
    int cpt                      = 1;
    while(hasTransferFunctionName)
    {
        std::stringstream tmpStr;
        tmpStr << _basename << "_" << cpt;
        newName                 = tmpStr.str();
        hasTransferFunctionName = this->hasPoolName(newName, _tfPools);
        cpt++;
    }

    return newName;
}

//------------------------------------------------------------------------------

void SMultipleTF::initializePools()
{
    {
        // Gets TF pools.
        const auto tfPools = m_tfPools.lock();

        const data::Composite::sptr sTFPools = tfPools.get_shared();
        data::helper::Composite compositeHelper(sTFPools);

        // Add the default TF if it not exists.
        const std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;
        if(!this->hasPoolName(defaultTFName, sTFPools))
        {
            const data::TransferFunction::sptr defaultTf = data::TransferFunction::createDefaultTF();
            const data::Composite::sptr defaultComposite = data::Composite::New();
            defaultComposite->getContainer()[defaultTFName] = defaultTf;
            compositeHelper.add(defaultTFName, defaultComposite);
        }

        // Test if transfer function composite has few TF
        if(tfPools->size() <= 1)
        {
            // Creates the TF atoms reader.
            const auto tf       = data::TransferFunction::New();
            const auto tfReader = service::add<io::base::service::IReader>("sight::module::io::session::SReader");
            tfReader->setInOut(tf, io::base::service::s_DATA_KEY);

            // Parse all paths contained in m_path and read basic TF.
            for(std::filesystem::path dirPath : m_paths)
            {
                data::Composite::sptr composite = data::Composite::New();

                SIGHT_ASSERT("Invalid directory path '" + dirPath.string() + "'", std::filesystem::exists(dirPath));
                for(std::filesystem::directory_iterator it(dirPath) ;
                    it != std::filesystem::directory_iterator() ;
                    ++it)
                {
                    if(!std::filesystem::is_directory(*it)
                       && it->path().extension().string() == ".tf")
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

                        if(!tf->getName().empty())
                        {
                            const data::TransferFunction::sptr newTF =
                                data::Object::copy<data::TransferFunction>(tf);
                            if(this->hasPoolName(newTF->getName(), sTFPools))
                            {
                                newTF->setName(this->createPoolName(newTF->getName(), sTFPools));
                            }

                            composite->getContainer()[newTF->getName()] = newTF;

                            // If it's requested to have one TF per file, add the composite and create a new one.
                            if(!m_tfPerPath)
                            {
                                if(composite->size() > 0)
                                {
                                    compositeHelper.add(newTF->getName(), composite);
                                    composite = data::Composite::New();
                                }
                            }
                        }
                    }
                }

                // If it's requested to have one TF per path, add the composite and create a new one.
                if(m_tfPerPath)
                {
                    if(composite->size() > 0)
                    {
                        compositeHelper.add(dirPath.filename().u8string(), composite);
                        composite = data::Composite::New();
                    }
                }
            }

            // Delete the reader.
            service::remove(tfReader);

            // Creates the multiple TF atoms reader.
            const auto tfPool      = data::Composite::New();
            const auto mulTFReader = service::add<io::base::service::IReader>("sight::module::io::session::SReader");
            mulTFReader->setInOut(tfPool, io::base::service::s_DATA_KEY);

            // Parse all path contained in m_path and read multiple TF.
            for(std::filesystem::path dirPath : m_paths)
            {
                SIGHT_ASSERT("Invalid directory path '" + dirPath.string() + "'", std::filesystem::exists(dirPath));
                for(std::filesystem::directory_iterator it(dirPath) ;
                    it != std::filesystem::directory_iterator() ;
                    ++it)
                {
                    if(!std::filesystem::is_directory(*it)
                       && it->path().extension().string() == ".tfp")
                    {
                        const std::filesystem::path file = *it;

                        service::IService::ConfigType config;
                        config.put("dialog.<xmlattr>.extension", ".tfp");
                        config.put("dialog.<xmlattr>.description", "Transfer Function Pool");
                        config.put("file", file.string());
                        config.put("archive.<xmlattr>.format", "filesystem");

                        mulTFReader->setConfiguration(config);
                        mulTFReader->configure();
                        mulTFReader->start();
                        mulTFReader->update();
                        mulTFReader->stop();

                        // Check the loaded composite.
                        if(tfPool->size() >= 1)
                        {
                            const data::Composite::sptr newTFPool =
                                data::Object::copy<data::Composite>(tfPool);

                            std::string poolName(file.stem().string());
                            if(this->hasPoolName(poolName, sTFPools))
                            {
                                poolName = this->createPoolName(poolName, sTFPools);
                            }

                            compositeHelper.add(poolName, newTFPool);
                        }
                    }
                }
            }

            // Delete the reader.
            service::remove(mulTFReader);
        }

        // Sends signals.
        compositeHelper.notify();
    }

    // Update all presets in the editor.
    this->updatePoolsPreset();
}

//------------------------------------------------------------------------------

void SMultipleTF::updatePoolsPreset()
{
    // Store the current composite.
    std::string currentPoolName = data::TransferFunction::s_DEFAULT_TF_NAME;

    {
        // Gets TF pools.
        const auto tfPools = m_tfPools.lock();

        // Iterate over each composite to add them to the presets selector.
        for(data::Composite::value_type elt : *tfPools)
        {
            m_tfPoolsPreset->addItem(elt.first.c_str());
        }

        // If the current TF pool exists, find it.
        const auto currentTfPool = m_currentTfPool.lock();

        if(currentTfPool)
        {
            for(data::Composite::value_type elt : *tfPools)
            {
                if(elt.second == currentTfPool.get_shared())
                {
                    currentPoolName = elt.first;
                    break;
                }
            }
        }
    }

    const int index = m_tfPoolsPreset->findText(QString::fromStdString(currentPoolName));

    // Set the current composite
    this->presetChoice(index);
}

//------------------------------------------------------------------------------

void SMultipleTF::presetChoice(int _index)
{
    m_tfPoolsPreset->setCurrentIndex(_index);
    // Set the output to the current preset.
    this->setCurrentPool();

    const std::string tfName = m_tfPoolsPreset->currentText().toStdString();
    const bool isEnabled     = (tfName != data::TransferFunction::s_DEFAULT_TF_NAME);

    m_renameButton->setEnabled(isEnabled);
    m_deleteButton->setEnabled(isEnabled);
}

//------------------------------------------------------------------------------

void SMultipleTF::setCurrentPool()
{
    const std::string newSelectedTFPoolKey = m_tfPoolsPreset->currentText().toStdString();

    const auto tfPools = m_tfPools.lock();

    const data::Object::sptr newSelectedObject    = (*tfPools)[newSelectedTFPoolKey];
    const data::Composite::sptr newSelectedTFPool = data::Composite::dynamicCast(newSelectedObject);
    SIGHT_ASSERT("inout '" << s_TF_POOLS << "' must contain only Composite.", newSelectedTFPool);

    if(newSelectedTFPool && newSelectedTFPool != m_selectedTFPool.lock())
    {
        m_selectedTFPool = newSelectedTFPool;
        this->setOutput(s_NEW_SELECTED_POOL, newSelectedTFPool);
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::deletePool()
{
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Deleting confirmation");
    messageBox.setMessage("Are you sure you want to delete this TF pool ?");
    messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::CANCEL);
    sight::ui::base::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if(answerCopy != sight::ui::base::dialog::IMessageDialog::CANCEL)
    {
        {
            // Gets TF pools.
            const auto tfPools = m_tfPools.lock();

            // Remove the current TF pool from the Composite.
            const std::string selectedTFPoolKey = m_tfPoolsPreset->currentText().toStdString();
            data::helper::Composite compositeHelper(tfPools.get_shared());

            compositeHelper.remove(selectedTFPoolKey);

            // Sends signals.
            compositeHelper.notify();

            m_tfPoolsPreset->removeItem(m_tfPoolsPreset->findText(QString::fromStdString(selectedTFPoolKey)));
        }

        // Find the new current composite.
        const std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;
        const int index                 = m_tfPoolsPreset->findText(QString::fromStdString(defaultTFName));

        // Set the current composite
        this->presetChoice(index);
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::newPool()
{
    const std::string str = m_tfPoolsPreset->currentText().toStdString();
    std::string newName(str);

    sight::ui::base::dialog::InputDialog inputDialog;
    inputDialog.setTitle("New TF pool");
    inputDialog.setMessage("TF pool name :");
    inputDialog.setInput(newName);
    newName = inputDialog.getInput();

    if(!newName.empty())
    {
        {
            // Gets TF pools.
            const auto tfPools = m_tfPools.lock();

            const data::Composite::sptr sTFPools = tfPools.get_shared();
            if(!this->hasPoolName(newName, sTFPools))
            {
                // Create the new composite.
                const data::TransferFunction::sptr defaultTf = data::TransferFunction::createDefaultTF();
                const data::Composite::sptr defaultComposite = data::Composite::New();
                defaultComposite->getContainer()[newName] = defaultTf;

                // Add a new composite.
                data::helper::Composite compositeHelper(sTFPools);
                {
                    compositeHelper.add(newName, defaultComposite);
                }
                compositeHelper.notify();

                // Creates presets.
                m_tfPoolsPreset->clear();
                for(data::Composite::value_type elt : *tfPools)
                {
                    m_tfPoolsPreset->addItem(elt.first.c_str());
                }
            }
        }

        // Set the current composite.
        this->presetChoice(m_tfPoolsPreset->findText(QString::fromStdString(newName.c_str())));
    }
    else
    {
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage("This TF pool name already exists so you can not overwrite it.");
        messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
        messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::copyPool()
{
    const std::string str = m_tfPoolsPreset->currentText().toStdString();
    std::string newName(str);

    sight::ui::base::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Copy TF pool");
    inputDialog.setMessage("TF pool name :");
    inputDialog.setInput(newName);
    newName = inputDialog.getInput();

    if(!newName.empty())
    {
        {
            // Gets TF pools.
            const auto tfPools = m_tfPools.lock();

            const data::Composite::sptr sTFPools = tfPools.get_shared();
            if(!this->hasPoolName(newName, sTFPools))
            {
                data::Object::sptr object                 = (*tfPools)[str];
                const data::Composite::sptr currentTFPool = data::Composite::dynamicCast(object);
                SIGHT_ASSERT("inout '" << s_TF_POOLS << "' must contain only Composite.", currentTFPool);

                // Copy the composite.
                data::helper::Composite compositeHelper(sTFPools);
                {
                    compositeHelper.add(newName, data::Object::copy(currentTFPool));
                }
                compositeHelper.notify();

                // Creates presets.
                m_tfPoolsPreset->clear();
                for(data::Composite::value_type elt : *tfPools)
                {
                    m_tfPoolsPreset->addItem(elt.first.c_str());
                }
            }
        }

        // Set the current composite.
        this->presetChoice(m_tfPoolsPreset->findText(QString::fromStdString(newName.c_str())));
    }
    else
    {
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage("This TF pool name already exists so you can not overwrite it.");
        messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
        messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::reinitializePools()
{
    // Clears previous settings.
    m_tfPoolsPreset->clear();

    {
        // Get the composite.
        const auto tfPools = m_tfPools.lock();

        data::helper::Composite compositeHelper(tfPools.get_shared());

        // Clear it.
        compositeHelper.clear();

        // Sends signals.
        compositeHelper.notify();
    }

    // Initialize pools.
    this->initializePools();
}

//------------------------------------------------------------------------------

void SMultipleTF::renamePool()
{
    const std::string str = m_tfPoolsPreset->currentText().toStdString();
    std::string newName(str);

    sight::ui::base::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Renaming TF pool");
    inputDialog.setMessage("TF pool name :");
    inputDialog.setInput(newName);
    newName = inputDialog.getInput();

    if(!newName.empty() && newName != str)
    {
        {
            // Gets TF pools.
            const auto tfPools = m_tfPools.lock();

            const data::Composite::sptr sTFPools = tfPools.get_shared();
            if(!this->hasPoolName(newName, sTFPools))
            {
                data::Object::sptr object = (*tfPools)[str];

                // Rename the composite.
                data::helper::Composite compositeHelper(sTFPools);
                compositeHelper.remove(str);
                compositeHelper.add(newName, object);
                compositeHelper.notify();

                // Creates presets.
                m_tfPoolsPreset->clear();
                for(data::Composite::value_type elt : *tfPools)
                {
                    m_tfPoolsPreset->addItem(elt.first.c_str());
                }
            }
            else
            {
                sight::ui::base::dialog::MessageDialog messageBox;
                messageBox.setTitle("Warning");
                messageBox.setMessage("This TF pool name already exists so you can not overwrite it.");
                messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
                messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
                messageBox.show();
                return;
            }
        }

        // Set the current composite.
        this->presetChoice(m_tfPoolsPreset->findText(QString::fromStdString(newName.c_str())));
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::importPool()
{
    const data::Composite::sptr tfPool = data::Composite::New();

    const auto reader = service::add<io::base::service::IReader>("sight::module::io::session::SReader");

    reader->setInOut(tfPool, io::base::service::s_DATA_KEY);

    service::IService::ConfigType config;
    config.put("dialog.<xmlattr>.extension", ".tfp");
    config.put("dialog.<xmlattr>.description", "Transfer Function Pool");
    config.put("archive.<xmlattr>.format", "filesystem");

    reader->configure(config);
    reader->start();
    reader->openLocationDialog();
    reader->update().wait();

    // Check the loaded composite.
    if(tfPool->size() >= 1)
    {
        int index = 0;
        {
            std::string poolName(reader->getFile().filename().stem().string());

            // Get the composite.
            const auto tfPools = m_tfPools.lock();

            const data::Composite::sptr sTFPools = tfPools.get_shared();
            if(this->hasPoolName(poolName, sTFPools))
            {
                poolName = this->createPoolName(poolName, sTFPools);
            }

            data::helper::Composite compositeHelper(sTFPools);
            compositeHelper.add(poolName, tfPool);
            compositeHelper.notify();

            m_tfPoolsPreset->addItem(QString(poolName.c_str()));
            index = static_cast<int>((*tfPools).size() - 1);
        }

        this->presetChoice(index);
    }

    reader->stop().wait();
    service::remove(reader);
}

//------------------------------------------------------------------------------

void SMultipleTF::exportPool()
{
    const auto writer        = service::add<io::base::service::IWriter>("sight::module::io::session::SWriter");
    const auto currentTfPool = m_currentTfPool.lock();
    writer->setInput(currentTfPool.get_shared(), io::base::service::s_DATA_KEY);

    service::IService::ConfigType config;
    config.add("dialog.<xmlattr>.extension", ".tfp");
    config.add("dialog.<xmlattr>.description", "Transfer Function Pool");
    config.put("archive.<xmlattr>.format", "filesystem");

    writer->configure(config);
    writer->start();
    writer->openLocationDialog();
    writer->update().wait();
    writer->stop().wait();
    service::remove(writer);
}

//------------------------------------------------------------------------------

} // end namespace
