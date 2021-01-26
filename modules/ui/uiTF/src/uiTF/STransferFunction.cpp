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

#include "uiTF/STransferFunction.hpp"

#include <core/base.hpp>
#include <core/runtime/EConfigurationElement.hpp>
#include <core/runtime/operations.hpp>

#include <data/Composite.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectReadToWriteLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/TransferFunction.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwIO/ioTypes.hpp>
#include <fwIO/IReader.hpp>
#include <fwIO/IWriter.hpp>

#include <services/macros.hpp>
#include <services/op/Add.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>

#include <filesystem>

namespace uiTF
{

static const std::string s_USE_DEFAULT_PATH_CONFIG  = "useDefaultPath";
static const std::string s_PATH_CONFIG              = "path";
static const std::string s_DELETE_ICON_CONFIG       = "deleteIcon";
static const std::string s_NEW_ICON_CONFIG          = "newIcon";
static const std::string s_REINITIALIZE_ICON_CONFIG = "reinitializeIcon";
static const std::string s_RENAME_ICON_CONFIG       = "renameIcon";
static const std::string s_IMPORT_ICON_CONFIG       = "importIcon";
static const std::string s_EXPORT_ICON_CONFIG       = "exportIcon";
static const std::string s_ICON_WIDTH_CONFIG        = "iconWidth";
static const std::string s_ICON_HEIGHT_CONFIG       = "iconHeight";

static const services::IService::KeyType s_CURRENT_TF_INPUT = "currentTF";
static const services::IService::KeyType s_TF_POOL_INOUT    = "tfPool";

static const services::IService::KeyType s_TF_OUTPUT = "tf";

static const std::string s_CONTEXT_TF = "TF";
static const std::string s_VERSION_TF = "V1";

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiTF::STransferFunction)

//------------------------------------------------------------------------------

STransferFunction::STransferFunction()
{
    const std::filesystem::path modulePath = core::runtime::getModuleResourcePath(std::string("uiTF"));

    m_deleteIcon       = modulePath / "delete.png";
    m_newIcon          = modulePath / "new.png";
    m_reinitializeIcon = modulePath / "reinitialize.png";
    m_renameIcon       = modulePath / "rename.png";
    m_importIcon       = modulePath / "import.png";
    m_exportIcon       = modulePath / "export.png";
}

//------------------------------------------------------------------------------

STransferFunction::~STransferFunction() noexcept
{
}

//------------------------------------------------------------------------------

void STransferFunction::configuring()
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
            const auto path = core::runtime::getModuleResourceFilePath(itCfg->second.get_value<std::string>());
            m_paths.push_back(path);
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

            m_iconWidth  = configAttr->get< unsigned int >(s_ICON_WIDTH_CONFIG, m_iconWidth);
            m_iconHeight = configAttr->get< unsigned int >(s_ICON_HEIGHT_CONFIG, m_iconHeight);
        }
    }

    if(useDefaultPath)
    {
        const auto pathRoot = core::runtime::getModuleResourceFilePath("uiTF", "tf");
        m_paths.push_back(pathRoot);
    }
}

//------------------------------------------------------------------------------

void STransferFunction::starting()
{
    this->create();

    // Get the Qt container
    const ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    // Buttons creation
    m_transferFunctionPreset = new QComboBox();

    m_deleteButton = new QPushButton(QIcon(m_deleteIcon.string().c_str()), "");
    m_deleteButton->setToolTip(QString("Delete"));

    m_newButton = new QPushButton(QIcon(m_newIcon.string().c_str()), "");
    m_newButton->setToolTip(QString("New"));

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
        m_reinitializeButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_renameButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_importButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        m_exportButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
    }

    // Layout management
    QBoxLayout* const layout = new QBoxLayout(QBoxLayout::LeftToRight);

    layout->addWidget(m_transferFunctionPreset);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_newButton);
    layout->addWidget(m_reinitializeButton);
    layout->addWidget(m_renameButton);
    layout->addWidget(m_importButton);
    layout->addWidget(m_exportButton);

    qtContainer->setLayout(layout);

    // Qt signals management ( connection to buttons )
    QObject::connect(m_transferFunctionPreset, qOverload<int>(&QComboBox::activated),
                     this, &STransferFunction::presetChoice);
    QObject::connect(m_deleteButton, &QPushButton::clicked, this, &STransferFunction::deleteTF);
    QObject::connect(m_newButton, &QPushButton::clicked, this, &STransferFunction::newTF);
    QObject::connect(m_reinitializeButton, &QPushButton::clicked, this, &STransferFunction::reinitializeTFPool);
    QObject::connect(m_renameButton, &QPushButton::clicked, this, &STransferFunction::renameTF);
    QObject::connect(m_importButton, &QPushButton::clicked, this, &STransferFunction::importTF);
    QObject::connect(m_exportButton, &QPushButton::clicked, this, &STransferFunction::exportTF);

    // preset initialization
    this->initTransferFunctions();
}

//------------------------------------------------------------------------------

::services::IService::KeyConnectionsMap STransferFunction::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_TF_POOL_INOUT, data::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push( s_TF_POOL_INOUT, data::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push( s_TF_POOL_INOUT, data::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void STransferFunction::updating()
{
    this->updateTransferFunctionPreset();
}

//------------------------------------------------------------------------------

void STransferFunction::swapping(const KeyType& _key)
{
    // Avoid swapping if it's the same TF.
    if(_key == s_CURRENT_TF_INPUT)
    {
        const auto tfW = this->getWeakInput< const data::TransferFunction >(_key);
        const auto tf  = tfW.lock();

        if(tf && tf.get_shared() != m_selectedTF)
        {
            this->updateTransferFunctionPreset();
        }
    }
}

//------------------------------------------------------------------------------

void STransferFunction::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void STransferFunction::presetChoice(int index)
{
    m_transferFunctionPreset->setCurrentIndex(index);
    this->updateTransferFunction();

    const std::string tfName = m_transferFunctionPreset->currentText().toStdString();
    const bool isEnabled     = (tfName != data::TransferFunction::s_DEFAULT_TF_NAME);

    m_renameButton->setEnabled(isEnabled);
    m_deleteButton->setEnabled(isEnabled);
}

//------------------------------------------------------------------------------

void STransferFunction::deleteTF()
{
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Deleting confirmation");
    messageBox.setMessage("Are you sure you want to delete this transfer function?");
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    ::fwGui::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if(answerCopy != ::fwGui::dialog::IMessageDialog::CANCEL)
    {
        const auto poolTF = this->getLockedInOut< data::Composite >(s_TF_POOL_INOUT);

        if(poolTF->size() > 1)
        {
            const int indexSelectedTF       = m_transferFunctionPreset->currentIndex();
            const std::string selectedTFKey = m_transferFunctionPreset->currentText().toStdString();

            ::fwDataTools::helper::Composite compositeHelper(poolTF.get_shared());
            SLM_ASSERT("TF '"+ selectedTFKey +"' missing in pool", this->hasTransferFunctionName(selectedTFKey));

            compositeHelper.remove(selectedTFKey);
            {
                const auto sig = poolTF->signal< data::Composite::RemovedObjectsSignalType >(
                    data::Composite::s_REMOVED_OBJECTS_SIG);
                core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                compositeHelper.notify();
            }

            m_transferFunctionPreset->removeItem(indexSelectedTF);
            const std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;

            int index = m_transferFunctionPreset->findText(QString::fromStdString(defaultTFName));
            index = std::max(index, 0);
            this->presetChoice(index);
        }
        else
        {
            ::fwGui::dialog::MessageDialog::show(
                "Warning",
                "You can not remove this transfer function because the program requires at least one.",
                ::fwGui::dialog::IMessageDialog::WARNING );
        }
    }
}

//------------------------------------------------------------------------------

void STransferFunction::newTF()
{
    std::string newName = m_selectedTF->getName();
    if( this->hasTransferFunctionName(newName) )
    {
        newName = this->createTransferFunctionName(newName);
    }

    fwGui::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Creating transfer function");
    inputDialog.setMessage("Transfer function name:");
    inputDialog.setInput( newName );
    newName = inputDialog.getInput();

    if(!newName.empty())
    {
        if(!this->hasTransferFunctionName(newName))
        {
            data::TransferFunction::sptr pNewTransferFunction;

            pNewTransferFunction = data::Object::copy(m_selectedTF);
            pNewTransferFunction->setName(newName);
            {
                const auto poolTF = this->getLockedInOut< data::Composite >(s_TF_POOL_INOUT);
                ::fwDataTools::helper::Composite compositeHelper(poolTF.get_shared());
                compositeHelper.add(newName, pNewTransferFunction);
                compositeHelper.notify();
            }
            m_transferFunctionPreset->addItem(QString(newName.c_str()));
            m_transferFunctionPreset->setCurrentIndex(m_transferFunctionPreset->count()-1);
            this->updateTransferFunction();
        }
        else
        {
            ::fwGui::dialog::MessageDialog::show(
                "Warning",
                "This transfer function name already exists so you can not overwrite it.",
                ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

void STransferFunction::reinitializeTFPool()
{
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Reinitializing confirmation");
    messageBox.setMessage("Are you sure you want to reinitialize all transfer functions?");
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    ::fwGui::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if(answerCopy != ::fwGui::dialog::IMessageDialog::CANCEL)
    {
        {
            const auto poolTF = this->getLockedInOut< data::Composite >(s_TF_POOL_INOUT);
            ::fwDataTools::helper::Composite compositeHelper(poolTF.get_shared());
            compositeHelper.clear();
            compositeHelper.notify();
        }

        this->initTransferFunctions();

        this->updateTransferFunction();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::renameTF()
{
    const std::string str = m_transferFunctionPreset->currentText().toStdString();
    std::string newName(str);

    fwGui::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Creating transfer function");
    inputDialog.setMessage("Transfer function name:");
    inputDialog.setInput( newName );
    newName = inputDialog.getInput();

    if(!newName.empty() && newName != str)
    {
        if(!this->hasTransferFunctionName(newName) )
        {
            {
                const auto poolTF = this->getLockedInOut< data::Composite >(s_TF_POOL_INOUT);

                const data::TransferFunction::sptr pTF = data::TransferFunction::dynamicCast((*poolTF)[str]);
                {
                    const data::mt::locked_ptr TFLock(pTF);
                    pTF->setName(newName);
                }

                ::fwDataTools::helper::Composite compositeHelper(poolTF.get_shared());
                compositeHelper.remove(str);
                compositeHelper.add(newName, pTF);
                compositeHelper.notify();
            }

            m_transferFunctionPreset->setItemText(m_transferFunctionPreset->currentIndex(), QString(newName.c_str()));
            m_transferFunctionPreset->setCurrentIndex(m_transferFunctionPreset->findText(QString(newName.c_str())));

            this->updateTransferFunction();
        }
        else
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage("This transfer function name already exists so you can not overwrite it.");
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
    }
}

//------------------------------------------------------------------------------

void STransferFunction::importTF()
{
    const data::TransferFunction::sptr tf = data::TransferFunction::New();
    const ::fwIO::IReader::sptr reader    = services::add< ::fwIO::IReader >("::ioAtoms::SReader");

    reader->registerInOut(tf, ::fwIO::s_DATA_KEY);

    services::IService::ConfigType config;
    config.add("archive.<xmlattr>.backend", "json");
    config.add("archive.extension", ".tf");

    reader->configure(config);
    reader->start();
    reader->openLocationDialog();
    reader->update().wait();
    reader->stop().wait();
    services::OSR::unregisterService(reader);

    if(!tf->getName().empty())
    {
        if(this->hasTransferFunctionName( tf->getName() ) )
        {
            tf->setName(this->createTransferFunctionName(tf->getName()));
        }

        const auto poolTF = this->getLockedInOut< data::Composite >(s_TF_POOL_INOUT);

        ::fwDataTools::helper::Composite compositeHelper(poolTF.get_shared());
        compositeHelper.add(tf->getName(), tf);

        m_transferFunctionPreset->addItem(QString(tf->getName().c_str()));
        this->presetChoice(static_cast<int>((*poolTF).size()-1));

        compositeHelper.notify();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::exportTF()
{
    const ::fwIO::IWriter::sptr writer = services::add< ::fwIO::IWriter >("::ioAtoms::SWriter");

    writer->registerInput(m_selectedTF, ::fwIO::s_DATA_KEY);

    services::IService::ConfigType config;
    config.add("patcher.<xmlattr>.context", s_CONTEXT_TF);
    config.add("patcher.<xmlattr>.version", s_VERSION_TF);
    config.add("archive.<xmlattr>.backend", "json");
    config.add("archive.extension", ".tf");
    config.add("extensions.extension", ".tf");
    config.add("extensions.extension.<xmlattr>.label", "Transfer Function");

    writer->configure(config);
    writer->start();
    writer->openLocationDialog();
    writer->update().wait();
    writer->stop().wait();
    services::OSR::unregisterService(writer);
}

//------------------------------------------------------------------------------

void STransferFunction::initTransferFunctions()
{
    {
        // Get transfer function composite (pool TF)
        const auto poolTF = this->getLockedInOut< data::Composite >(s_TF_POOL_INOUT);

        ::fwDataTools::helper::Composite compositeHelper(poolTF.get_shared());

        const std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;
        if(!this->hasTransferFunctionName(defaultTFName, poolTF.get_shared()))
        {
            data::TransferFunction::sptr defaultTf = data::TransferFunction::createDefaultTF();
            compositeHelper.add(defaultTFName, defaultTf);
        }

        // Test if transfer function composite has few TF
        if(poolTF->size() <= 1)
        {
            // Parse all TF contained in uiTF module's resources
            std::vector< std::filesystem::path > paths;
            for(std::filesystem::path dirPath :  m_paths)
            {
                SLM_ASSERT("Invalid directory path '" + dirPath.string() + "'", std::filesystem::exists(dirPath));
                for(std::filesystem::directory_iterator it(dirPath);
                    it != std::filesystem::directory_iterator();
                    ++it )
                {
                    if(!std::filesystem::is_directory(*it) &&
                       it->path().extension().string() == ".json")
                    {
                        paths.push_back(*it);
                    }
                }
            }

            const data::TransferFunction::sptr tf = data::TransferFunction::New();
            const ::fwIO::IReader::sptr reader    = services::add< ::fwIO::IReader >("::ioAtoms::SReader");
            reader->registerInOut(tf, ::fwIO::s_DATA_KEY);

            const core::runtime::EConfigurationElement::sptr srvCfg = core::runtime::EConfigurationElement::New(
                "service");
            const core::runtime::EConfigurationElement::sptr fileCfg =
                core::runtime::EConfigurationElement::New("file");
            srvCfg->addConfigurationElement(fileCfg);

            for( std::filesystem::path file :  paths )
            {
                fileCfg->setValue(file.string());
                reader->setConfiguration(srvCfg);
                reader->configure();
                reader->start();
                reader->update();
                reader->stop();

                if(!tf->getName().empty())
                {
                    const data::TransferFunction::sptr newTF =
                        data::Object::copy< data::TransferFunction >(tf);
                    if(this->hasTransferFunctionName(newTF->getName(), poolTF.get_shared()))
                    {
                        newTF->setName( this->createTransferFunctionName( newTF->getName() ) );
                    }

                    compositeHelper.add(newTF->getName(), newTF);
                }
                tf->initTF();
            }
            services::OSR::unregisterService(reader);
        }

        compositeHelper.notify();
    }

    this->updateTransferFunctionPreset();
}

//---------------------------------------------------------------------------------------------------------------------

void STransferFunction::updateTransferFunctionPreset()
{
    const std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;

    // Manage TF preset
    m_transferFunctionPreset->clear();
    {
        const auto poolTF = this->getLockedInOut< data::Composite >(s_TF_POOL_INOUT);
        for(data::Composite::value_type elt :  *poolTF)
        {
            m_transferFunctionPreset->addItem( elt.first.c_str() );
        }
    }

    std::string currentTFName = defaultTFName;
    const auto tfW            = this->getWeakInput< data::TransferFunction >(s_CURRENT_TF_INPUT);
    {
        const auto tf = tfW.lock();
        if(tf)
        {
            currentTFName = tf->getName();
        }
        else if(m_selectedTF)
        {
            currentTFName = m_selectedTF->getName();
        }
    }

    int index = m_transferFunctionPreset->findText( QString::fromStdString(currentTFName) );
    index = std::max(index, 0);

    this->presetChoice(index);
}

//------------------------------------------------------------------------------

bool STransferFunction::hasTransferFunctionName(const std::string& _name,
                                                const data::Composite::csptr _poolTF) const
{
    if(_poolTF)
    {
        return _poolTF->find(_name) != _poolTF->end();
    }
    else
    {
        const auto poolTF = this->getLockedInOut< data::Composite >(s_TF_POOL_INOUT);
        return poolTF->find(_name) != poolTF->end();
    }
}

//------------------------------------------------------------------------------

std::string STransferFunction::createTransferFunctionName(const std::string& _basename) const
{
    bool hasTransferFunctionName = true;
    std::string newName          = _basename;
    int cpt                      = 1;
    while(hasTransferFunctionName)
    {
        std::stringstream tmpStr;
        tmpStr <<  _basename <<  "_" <<  cpt;
        newName                 = tmpStr.str();
        hasTransferFunctionName = this->hasTransferFunctionName(newName);
        cpt++;
    }

    return newName;
}

//------------------------------------------------------------------------------

void STransferFunction::updateTransferFunction()
{
    const std::string newSelectedTFKey = m_transferFunctionPreset->currentText().toStdString();
    SLM_DEBUG("Transfer function selected : " +  newSelectedTFKey);

    SLM_ASSERT("TF '"+ newSelectedTFKey +"' missing in pool", this->hasTransferFunctionName(newSelectedTFKey));

    const auto poolTF = this->getLockedInOut< data::Composite >(s_TF_POOL_INOUT);

    const data::Object::sptr newSelectedTF = (*poolTF)[newSelectedTFKey];

    if(newSelectedTF && m_selectedTF != newSelectedTF)
    {
        this->setOutput(s_TF_OUTPUT, newSelectedTF);
        m_selectedTF = data::TransferFunction::dynamicCast(newSelectedTF);
    }
}

//------------------------------------------------------------------------------

} // namespace uiTF.
