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

#include "uiTF/SMultipleTF.hpp"

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwIO/IReader.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/op/Add.hpp>

#include <QBoxLayout>

namespace uiTF
{

static const std::string s_USE_DEFAULT_PATH_CONFIG  = "useDefaultPath";
static const std::string s_PATH_CONFIG              = "path";
static const std::string s_TF_PER_PATH_CONFIG       = "tfPerPath";
static const std::string s_DELETE_ICON_CONFIG       = "deleteIcon";
static const std::string s_NEW_ICON_CONFIG          = "newIcon";
static const std::string s_COPY_ICON_CONFIG         = "copyIcon";
static const std::string s_REINITIALIZE_ICON_CONFIG = "reinitializeIcon";
static const std::string s_RENAME_ICON_CONFIG       = "renameIcon";
static const std::string s_ICON_WIDTH_CONFIG        = "iconWidth";
static const std::string s_ICON_HEIGHT_CONFIG       = "iconHeight";

static const ::fwServices::IService::KeyType s_CURRENT_TF_POOL_INPUT = "currentTFPool";

static const ::fwServices::IService::KeyType s_TF_POOLS_INOUT = "tfPools";

static const ::fwServices::IService::KeyType s_TF_POOL_OUTPUT = "tfPool";

//------------------------------------------------------------------------------

SMultipleTF::SMultipleTF()
{
    const std::filesystem::path modulePath = ::fwRuntime::getModuleResourcePath(std::string("uiTF"));

    m_deleteIcon       = modulePath / "delete.png";
    m_newIcon          = modulePath / "new.png";
    m_copyIcon         = modulePath / "new.png";
    m_renameIcon       = modulePath / "rename.png";
    m_reinitializeIcon = modulePath / "reinitialize.png";
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
        for(auto itCfg = pathCfg.first; itCfg != pathCfg.second; ++itCfg)
        {
            const auto path = ::fwRuntime::getModuleResourceFilePath(itCfg->second.get_value<std::string>());
            m_paths.push_back(path);
        }

        const auto configAttr = config->get_child_optional("<xmlattr>");

        if(configAttr)
        {
            useDefaultPath = configAttr->get<bool>(s_USE_DEFAULT_PATH_CONFIG, useDefaultPath);
            m_tfPerPath    = configAttr->get<bool>(s_TF_PER_PATH_CONFIG, m_tfPerPath);

            m_deleteIcon =
                ::fwRuntime::getModuleResourceFilePath(configAttr->get(s_DELETE_ICON_CONFIG, m_deleteIcon));
            m_newIcon  = ::fwRuntime::getModuleResourceFilePath(configAttr->get(s_NEW_ICON_CONFIG, m_newIcon));
            m_copyIcon =
                ::fwRuntime::getModuleResourceFilePath(configAttr->get(s_COPY_ICON_CONFIG, m_copyIcon));
            m_reinitializeIcon =
                ::fwRuntime::getModuleResourceFilePath(configAttr->get(s_REINITIALIZE_ICON_CONFIG, m_reinitializeIcon));
            m_renameIcon = ::fwRuntime::getModuleResourceFilePath(configAttr->get(s_RENAME_ICON_CONFIG, m_renameIcon));

            m_iconWidth  = configAttr->get< unsigned int >(s_ICON_WIDTH_CONFIG, m_iconWidth);
            m_iconHeight = configAttr->get< unsigned int >(s_ICON_HEIGHT_CONFIG, m_iconHeight);
        }
    }

    if(useDefaultPath)
    {
        const auto pathRoot = ::fwRuntime::getModuleResourceFilePath("uiTF", "tf");
        m_paths.push_back(pathRoot);
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::starting()
{
    this->create();

    // Get the Qt container
    const ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

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

    if(m_iconWidth > 0 && m_iconHeight > 0)
    {
        m_deleteButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_newButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_copyButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_reinitializeButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_renameButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
    }

    // Layout management
    QBoxLayout* const layout = new QBoxLayout(QBoxLayout::LeftToRight);

    layout->addWidget(m_tfPoolsPreset);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_newButton);
    layout->addWidget(m_copyButton);
    layout->addWidget(m_reinitializeButton);
    layout->addWidget(m_renameButton);

    qtContainer->setLayout(layout);

    // Manage connection with the editor.
    QObject::connect(m_tfPoolsPreset, qOverload<int>(&QComboBox::activated), this, &SMultipleTF::presetChoice);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &SMultipleTF::deletePool);
    QObject::connect(m_newButton, &QPushButton::clicked, this, &SMultipleTF::newPool);
    QObject::connect(m_copyButton, &QPushButton::clicked, this, &SMultipleTF::copyPool);
    QObject::connect(m_reinitializeButton, &QPushButton::clicked, this, &SMultipleTF::reinitializePools);
    QObject::connect(m_renameButton, &QPushButton::clicked, this, &SMultipleTF::renamePool);

    // Initializes the TF pool from paths.
    this->initializePools();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMultipleTF::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_TF_POOLS_INOUT, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_TF_POOLS_INOUT, ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_TF_POOLS_INOUT, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SMultipleTF::updating()
{
}

//------------------------------------------------------------------------------

void SMultipleTF::swapping(const KeyType& _key)
{
    // Avoid swapping if it's the same TF.
    if(_key == s_CURRENT_TF_POOL_INPUT
       && this->getInput< ::fwData::Composite >(_key)
       && this->getInput< ::fwData::Composite >(_key) != m_currentTFPool)
    {
        this->updatePoolsPreset();
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

bool SMultipleTF::hasPoolName(const std::string& _name) const
{
    const ::fwData::Composite::csptr tfPools = this->getInOut< ::fwData::Composite >(s_TF_POOLS_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' does not exist.", tfPools);
    const ::fwData::mt::ObjectReadLock tfPoolsLock(tfPools);
    return tfPools->find(_name) != tfPools->end();
}

//------------------------------------------------------------------------------

std::string SMultipleTF::createPoolName(const std::string& _basename) const
{
    bool hasTransferFunctionName = true;
    std::string newName          = _basename;
    int cpt                      = 1;
    while(hasTransferFunctionName)
    {
        std::stringstream tmpStr;
        tmpStr <<  _basename <<  "_" <<  cpt;
        newName                 = tmpStr.str();
        hasTransferFunctionName = this->hasPoolName(newName);
        cpt++;
    }

    return newName;
}

//------------------------------------------------------------------------------

void SMultipleTF::initializePools()
{
    // Gets TF pools.
    const ::fwData::Composite::sptr tfPools = this->getInOut< ::fwData::Composite >(s_TF_POOLS_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' does not exist.", tfPools);

    ::fwDataTools::helper::Composite compositeHelper(tfPools);

    // Add the default TF if it not exists.
    const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
    if(!this->hasPoolName(defaultTFName))
    {
        const ::fwData::TransferFunction::sptr defaultTf = ::fwData::TransferFunction::createDefaultTF();
        const ::fwData::Composite::sptr defaultComposite = ::fwData::Composite::New();
        defaultComposite->getContainer()[defaultTFName] = defaultTf;
        const ::fwData::mt::ObjectWriteLock tfPoolsLock(tfPools);
        compositeHelper.add(defaultTFName, defaultComposite);
    }

    // Test if transfer function composite has few TF
    ::fwData::mt::ObjectReadToWriteLock tfPoolsLock(tfPools);
    if(tfPools->size() <= 1)
    {
        tfPoolsLock.unlock();

        // Creates the TF atoms reader.
        const ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::New();
        const ::fwIO::IReader::sptr reader        = ::fwServices::add< ::fwIO::IReader >("::ioAtoms::SReader");
        reader->registerInOut(tf, ::fwIO::s_DATA_KEY);

        const ::fwRuntime::EConfigurationElement::sptr srvCfg  = ::fwRuntime::EConfigurationElement::New("service");
        const ::fwRuntime::EConfigurationElement::sptr fileCfg = ::fwRuntime::EConfigurationElement::New("file");
        srvCfg->addConfigurationElement(fileCfg);

        // Parse all path contained in m_path.
        for(std::filesystem::path dirPath : m_paths)
        {
            ::fwData::Composite::sptr composite = ::fwData::Composite::New();

            SLM_ASSERT("Invalid directory path '" + dirPath.string() + "'", std::filesystem::exists(dirPath));
            for(std::filesystem::directory_iterator it(dirPath);
                it != std::filesystem::directory_iterator();
                ++it )
            {

                if(!std::filesystem::is_directory(*it) &&
                   it->path().extension().string() == ".json")
                {
                    const std::filesystem::path file = *it;

                    // Add a new composite for each TF path.
                    fileCfg->setValue(file.string());
                    reader->setConfiguration(srvCfg);
                    reader->configure();
                    reader->start();
                    reader->update();
                    reader->stop();

                    if(!tf->getName().empty())
                    {
                        const ::fwData::TransferFunction::sptr newTF
                            = ::fwData::Object::copy< ::fwData::TransferFunction >(tf);
                        if(this->hasPoolName(newTF->getName()))
                        {
                            newTF->setName(this->createPoolName(newTF->getName()));
                        }

                        composite->getContainer()[newTF->getName()] = newTF;

                        // If it's requested to have one TF per file, add the composite and create a new one.
                        if(!m_tfPerPath)
                        {
                            if(composite->size() > 0)
                            {
                                tfPoolsLock.lock();
                                tfPoolsLock.upgrade();
                                compositeHelper.add(newTF->getName(), composite);
                                tfPoolsLock.downgrade();
                                tfPoolsLock.unlock();
                                composite = ::fwData::Composite::New();
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
                    tfPoolsLock.lock();
                    tfPoolsLock.upgrade();
                    compositeHelper.add(dirPath.filename().u8string(), composite);
                    tfPoolsLock.downgrade();
                    tfPoolsLock.unlock();
                    composite = ::fwData::Composite::New();
                }
            }
        }

        // Delete the reader.
        ::fwServices::OSR::unregisterService(reader);
    }
    else
    {
        tfPoolsLock.unlock();
    }

    // Sends signals.
    compositeHelper.notify();

    // Update all presets in the editor.
    this->updatePoolsPreset();
}

//------------------------------------------------------------------------------

void SMultipleTF::updatePoolsPreset()
{
    // Gets TF pools.
    const ::fwData::Composite::csptr tfPools = this->getInOut< ::fwData::Composite >(s_TF_POOLS_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' does not exist.", tfPools);

    // Iterate over each composite to add them to the presets selector.
    ::fwData::mt::ObjectReadLock tfPoolsLock(tfPools);
    for(::fwData::Composite::value_type elt : *tfPools)
    {
        m_tfPoolsPreset->addItem(elt.first.c_str());
    }
    tfPoolsLock.unlock();

    // Find the current composite.
    std::string currentPoolName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;

    // If the current TF pool exists, find it.
    const ::fwData::Composite::csptr tfPool = this->getInput< ::fwData::Composite >(s_CURRENT_TF_POOL_INPUT);
    if(tfPool)
    {
        tfPoolsLock.lock();
        for(::fwData::Composite::value_type elt : *tfPools)
        {
            if(elt.second == tfPool)
            {
                currentPoolName = elt.first;
                break;
            }
        }
        tfPoolsLock.unlock();
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
    const bool isEnabled     = (tfName != ::fwData::TransferFunction::s_DEFAULT_TF_NAME);

    m_renameButton->setEnabled(isEnabled);
    m_deleteButton->setEnabled(isEnabled);
}

//------------------------------------------------------------------------------

void SMultipleTF::setCurrentPool()
{
    const std::string newSelectedTFPoolKey = m_tfPoolsPreset->currentText().toStdString();

    const ::fwData::Composite::sptr tfPools = this->getInOut< ::fwData::Composite >(s_TF_POOLS_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' does not exist.", tfPools);
    ::fwData::mt::ObjectReadLock tfPoolsLock(tfPools);

    const ::fwData::Object::sptr newSelectedObject    = (*tfPools)[newSelectedTFPoolKey];
    const ::fwData::Composite::sptr newSelectedTFPool = ::fwData::Composite::dynamicCast(newSelectedObject);
    SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' must contain only Composite.", newSelectedTFPool);

    if(newSelectedTFPool && newSelectedTFPool != m_currentTFPool)
    {
        m_currentTFPool = newSelectedTFPool;
        this->setOutput(s_TF_POOL_OUTPUT, newSelectedTFPool);
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::deletePool()
{
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Deleting confirmation");
    messageBox.setMessage("Are you sure you want to delete this TF pool ?");
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    ::fwGui::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if(answerCopy != ::fwGui::dialog::IMessageDialog::CANCEL)
    {
        const ::fwData::Composite::sptr tfPools = this->getInOut< ::fwData::Composite >(s_TF_POOLS_INOUT);
        SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' does not exist.", tfPools);
        ::fwData::mt::ObjectReadToWriteLock tfPoolsLock(tfPools);

        // Remove the current TF pool from the Composite.
        const std::string selectedTFPoolKey = m_tfPoolsPreset->currentText().toStdString();
        ::fwDataTools::helper::Composite compositeHelper(tfPools);

        tfPoolsLock.upgrade();
        compositeHelper.remove(selectedTFPoolKey);
        tfPoolsLock.downgrade();
        tfPoolsLock.unlock();

        // Sends signals.
        compositeHelper.notify();

        m_tfPoolsPreset->removeItem(m_tfPoolsPreset->findText(QString::fromStdString(selectedTFPoolKey)));

        // Find the new current composite.
        const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
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

    fwGui::dialog::InputDialog inputDialog;
    inputDialog.setTitle("New TF pool");
    inputDialog.setMessage("TF pool name :");
    inputDialog.setInput(newName);
    newName = inputDialog.getInput();

    if(!newName.empty())
    {
        if(!this->hasPoolName(newName))
        {
            // Get the composite.
            const ::fwData::Composite::sptr tfPools = this->getInOut< ::fwData::Composite >(s_TF_POOLS_INOUT);
            SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' does not exist.", tfPools);

            // Create the new composite.
            const ::fwData::TransferFunction::sptr defaultTf = ::fwData::TransferFunction::createDefaultTF();
            const ::fwData::Composite::sptr defaultComposite = ::fwData::Composite::New();
            defaultComposite->getContainer()[newName] = defaultTf;

            // Add a new composite.
            ::fwDataTools::helper::Composite compositeHelper(tfPools);
            {
                const ::fwData::mt::ObjectWriteLock tfPoolsLock(tfPools);
                compositeHelper.add(newName, defaultComposite);
            }
            compositeHelper.notify();

            // Creates presets.
            m_tfPoolsPreset->clear();
            {
                const ::fwData::mt::ObjectReadLock tfPoolsLock(tfPools);
                for(::fwData::Composite::value_type elt : *tfPools)
                {
                    m_tfPoolsPreset->addItem(elt.first.c_str());
                }
            }

            // Set the current composite.
            this->presetChoice(m_tfPoolsPreset->findText(QString::fromStdString(newName.c_str())));
        }
        else
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage("This TF pool name already exists so you can not overwrite it.");
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::copyPool()
{
    const std::string str = m_tfPoolsPreset->currentText().toStdString();
    std::string newName(str);

    fwGui::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Copy TF pool");
    inputDialog.setMessage("TF pool name :");
    inputDialog.setInput(newName);
    newName = inputDialog.getInput();

    if(!newName.empty())
    {
        if(!this->hasPoolName(newName))
        {
            // Get the composite.
            const ::fwData::Composite::sptr tfPools = this->getInOut< ::fwData::Composite >(s_TF_POOLS_INOUT);
            SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' does not exist.", tfPools);

            ::fwData::Object::sptr object = (*tfPools)[str];
            const ::fwData::Composite::sptr currentTFPool = ::fwData::Composite::dynamicCast(object);
            SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' must contain only Composite.", currentTFPool);

            // Copy the composite.
            ::fwDataTools::helper::Composite compositeHelper(tfPools);
            {
                const ::fwData::mt::ObjectWriteLock tfPoolsLock(tfPools);
                compositeHelper.add(newName, ::fwData::Object::copy(currentTFPool));
            }
            compositeHelper.notify();

            // Creates presets.
            m_tfPoolsPreset->clear();
            {
                const ::fwData::mt::ObjectReadLock tfPoolsLock(tfPools);
                for(::fwData::Composite::value_type elt : *tfPools)
                {
                    m_tfPoolsPreset->addItem(elt.first.c_str());
                }
            }

            // Set the current composite.
            this->presetChoice(m_tfPoolsPreset->findText(QString::fromStdString(newName.c_str())));
        }
        else
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage("This TF pool name already exists so you can not overwrite it.");
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
    }
}

//------------------------------------------------------------------------------

void SMultipleTF::reinitializePools()
{
    // Clears previous settings.
    m_tfPoolsPreset->clear();

    // Get the composite.
    const ::fwData::Composite::sptr tfPools = this->getInOut< ::fwData::Composite >(s_TF_POOLS_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' does not exist.", tfPools);

    ::fwDataTools::helper::Composite compositeHelper(tfPools);

    // Clear it.
    {
        const ::fwData::mt::ObjectWriteLock tfPoolsLock(tfPools);
        compositeHelper.clear();
    }

    // Sends signals.
    compositeHelper.notify();

    // Initialize pools.
    this->initializePools();
}

//------------------------------------------------------------------------------

void SMultipleTF::renamePool()
{
    const std::string str = m_tfPoolsPreset->currentText().toStdString();
    std::string newName(str);

    fwGui::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Renaming TF pool");
    inputDialog.setMessage("TF pool name :");
    inputDialog.setInput(newName);
    newName = inputDialog.getInput();

    if(!newName.empty() && newName != str)
    {
        if(!this->hasPoolName(newName))
        {
            // Get the composite.
            const ::fwData::Composite::sptr tfPools = this->getInOut< ::fwData::Composite >(s_TF_POOLS_INOUT);
            SLM_ASSERT("inout '" + s_TF_POOLS_INOUT + "' does not exist.", tfPools);

            ::fwData::Object::sptr object = (*tfPools)[str];

            // Rename the composite.
            ::fwDataTools::helper::Composite compositeHelper(tfPools);
            {
                const ::fwData::mt::ObjectWriteLock tfPoolsLock(tfPools);
                compositeHelper.remove(str);
                compositeHelper.add(newName, object);
            }
            compositeHelper.notify();

            // Creates presets.
            m_tfPoolsPreset->clear();
            {
                const ::fwData::mt::ObjectReadLock tfPoolsLock(tfPools);
                for(::fwData::Composite::value_type elt : *tfPools)
                {
                    m_tfPoolsPreset->addItem(elt.first.c_str());
                }
            }

            // Set the current composite.
            this->presetChoice(m_tfPoolsPreset->findText(QString::fromStdString(newName.c_str())));
        }
        else
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage("This TF pool name already exists so you can not overwrite it.");
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
    }
}

//------------------------------------------------------------------------------

} // end namespace
