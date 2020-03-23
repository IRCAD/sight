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

#include "uiTF/STransferFunction.hpp"

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwIO/ioTypes.hpp>
#include <fwIO/IReader.hpp>
#include <fwIO/IWriter.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>

#include <filesystem>

namespace uiTF
{

static const std::string s_USE_DEFAULT_PATH_CONFIG = "useDefaultPath";
static const std::string s_PATH_CONFIG             = "path";

static const ::fwServices::IService::KeyType s_CURRENT_TF_INPUT = "currentTF";
static const ::fwServices::IService::KeyType s_TF_POOL_INOUT    = "tfPool";

static const ::fwServices::IService::KeyType s_TF_OUTPUT = "tf";

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiTF::STransferFunction)

//------------------------------------------------------------------------------

STransferFunction::STransferFunction()
{
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
    const auto config     = tree.get_child_optional("config.<xmlattr>");

    bool useDefaultPath = true;
    if(config)
    {
        const auto pathCfg = config->equal_range(s_PATH_CONFIG);
        for(auto itCfg = pathCfg.first; itCfg != pathCfg.second; ++itCfg)
        {
            const auto path = ::fwRuntime::getBundleResourceFilePath(itCfg->second.get_value<std::string>());
            m_paths.push_back(path);
        }

        useDefaultPath = config->get<bool>(s_USE_DEFAULT_PATH_CONFIG, useDefaultPath);
    }

    if(useDefaultPath)
    {
        const auto pathRoot = ::fwRuntime::getBundleResourceFilePath("uiTF", "tf");
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
    m_pTransferFunctionPreset = new QComboBox();

    const std::filesystem::path bundlePath = ::fwRuntime::getBundleResourcePath(std::string("uiTF"));

    const auto deletePath = bundlePath / "delete.png";
    m_deleteButton = new QPushButton(QIcon(deletePath.string().c_str()), "");
    m_deleteButton->setToolTip(QString("Delete"));

    const auto newPath = bundlePath / "new.png";
    m_newButton = new QPushButton(QIcon(newPath.string().c_str()), "");
    m_newButton->setToolTip(QString("New"));

    const auto reinitializePath = bundlePath / "reinitialize.png";
    m_reinitializeButton = new QPushButton(QIcon(reinitializePath.string().c_str()), "");
    m_reinitializeButton->setToolTip(QString("Reinitialize"));

    const auto renamePath = bundlePath / "rename.png";
    m_renameButton = new QPushButton(QIcon(renamePath.string().c_str()), "");
    m_renameButton->setToolTip(QString("Rename"));

    const auto importPath = bundlePath / "import.png";
    m_importButton = new QPushButton(QIcon(importPath.string().c_str()), "");
    m_importButton->setToolTip(QString("Import"));

    const auto exportPath = bundlePath / "export.png";
    m_exportButton = new QPushButton(QIcon(exportPath.string().c_str()), "");
    m_exportButton->setToolTip(QString("Export"));

    // Layout management
    QBoxLayout* const layout = new QBoxLayout(QBoxLayout::LeftToRight);

    layout->addWidget(m_pTransferFunctionPreset);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_newButton);
    layout->addWidget(m_reinitializeButton);
    layout->addWidget(m_renameButton);
    layout->addWidget(m_importButton);
    layout->addWidget(m_exportButton);

    qtContainer->setLayout(layout);

    // Qt signals management ( connection to buttons )
    QObject::connect(m_pTransferFunctionPreset, SIGNAL(activated(int)), this, SLOT(presetChoice(int)));
    QObject::connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteTF()));
    QObject::connect(m_newButton, SIGNAL(clicked()), this, SLOT(newTF()));
    QObject::connect(m_reinitializeButton, SIGNAL(clicked()), this, SLOT(reinitializeTFPool()));
    QObject::connect(m_renameButton, SIGNAL(clicked()), this, SLOT(renameTF()));
    QObject::connect(m_importButton, SIGNAL(clicked()), this, SLOT(importTF()));
    QObject::connect(m_exportButton, SIGNAL(clicked()), this, SLOT(exportTF()));

    // preset initialization
    this->initTransferFunctions();
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
    if(_key == s_CURRENT_TF_INPUT
       && this->getInput< ::fwData::TransferFunction >(_key)
       && this->getInput< ::fwData::TransferFunction >(_key) != m_selectedTF)
    {
        this->updateTransferFunctionPreset();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::stopping()
{
    // Qt signals management ( disconnection )
    QObject::disconnect(m_pTransferFunctionPreset, SIGNAL(activated(int)), this, SLOT(presetChoice(int)));
    QObject::disconnect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteTF()));
    QObject::disconnect(m_newButton, SIGNAL(clicked()), this, SLOT(newTF()));
    QObject::disconnect(m_reinitializeButton, SIGNAL(clicked()), this, SLOT(reinitializeTFPool()));
    QObject::disconnect(m_renameButton, SIGNAL(clicked()), this, SLOT(renameTF()));
    QObject::disconnect(m_importButton, SIGNAL(clicked()), this, SLOT(importTF()));
    QObject::disconnect(m_exportButton, SIGNAL(clicked()), this, SLOT(exportTF()));

    this->destroy();
}

//------------------------------------------------------------------------------

void STransferFunction::presetChoice(int index)
{
    m_pTransferFunctionPreset->setCurrentIndex(index);
    this->updateTransferFunction();

    const std::string tfName = m_pTransferFunctionPreset->currentText().toStdString();
    const bool isEnabled     = (tfName != ::fwData::TransferFunction::s_DEFAULT_TF_NAME);

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
        const ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", poolTF);
        ::fwData::mt::ObjectReadToWriteLock poolTFLock(poolTF);

        if( poolTF->size() > 1 )
        {
            const int indexSelectedTF       = m_pTransferFunctionPreset->currentIndex();
            const std::string selectedTFKey = m_pTransferFunctionPreset->currentText().toStdString();

            ::fwDataTools::helper::Composite compositeHelper(poolTF);
            SLM_ASSERT("TF '"+ selectedTFKey +"' missing in pool", this->hasTransferFunctionName(selectedTFKey));
            poolTFLock.upgrade();
            compositeHelper.remove(selectedTFKey);

            {
                const auto sig = poolTF->signal< ::fwData::Composite::RemovedObjectsSignalType >(
                    ::fwData::Composite::s_REMOVED_OBJECTS_SIG);
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));

                poolTFLock.downgrade();
                poolTFLock.unlock();

                compositeHelper.notify();
            }

            m_pTransferFunctionPreset->removeItem(indexSelectedTF);
            const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;

            int index = m_pTransferFunctionPreset->findText(QString::fromStdString(defaultTFName));
            index = std::max(index, 0);
            this->presetChoice(index);
        }
        else
        {
            poolTFLock.unlock();
            ::fwGui::dialog::MessageDialog::showMessageDialog(
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
            ::fwData::TransferFunction::sptr pNewTransferFunction;

            pNewTransferFunction = ::fwData::Object::copy(m_selectedTF);
            pNewTransferFunction->setName(newName);

            const ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
            SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", poolTF);

            ::fwDataTools::helper::Composite compositeHelper(poolTF);
            {
                const ::fwData::mt::ObjectWriteLock poolTFLock(poolTF);
                compositeHelper.add(newName, pNewTransferFunction);
            }

            m_pTransferFunctionPreset->addItem(QString(newName.c_str()));
            m_pTransferFunctionPreset->setCurrentIndex(m_pTransferFunctionPreset->count()-1);
            this->updateTransferFunction();

            compositeHelper.notify();
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
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
        ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", poolTF);

        ::fwDataTools::helper::Composite compositeHelper(poolTF);
        {
            ::fwData::mt::ObjectWriteLock poolTFLock(poolTF);
            compositeHelper.clear();
        }
        compositeHelper.notify();

        this->initTransferFunctions();

        this->updateTransferFunction();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::renameTF()
{
    const std::string str = m_pTransferFunctionPreset->currentText().toStdString();
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
            const ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
            SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", poolTF);
            ::fwData::mt::ObjectWriteLock poolTFLock(poolTF);

            ::fwData::TransferFunction::sptr pTF = ::fwData::TransferFunction::dynamicCast((*poolTF)[str]);
            {
                const ::fwData::mt::ObjectWriteLock TFLock(pTF);
                pTF->setName(newName);
            }

            ::fwDataTools::helper::Composite compositeHelper(poolTF);
            compositeHelper.remove(str);
            compositeHelper.add(newName, pTF);
            poolTFLock.unlock();
            compositeHelper.notify();

            m_pTransferFunctionPreset->setItemText(m_pTransferFunctionPreset->currentIndex(), QString(newName.c_str()));
            m_pTransferFunctionPreset->setCurrentIndex(m_pTransferFunctionPreset->findText(QString(newName.c_str())));

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
    if( newName.empty() )
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage("You have to give a name to your transfer function.");
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::importTF()
{
    const ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", poolTF);

    ::fwDataTools::helper::Composite compositeHelper(poolTF);

    const ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::New();
    const ::fwIO::IReader::sptr reader        = ::fwServices::add< ::fwIO::IReader >("::ioAtoms::SReader");

    reader->registerInOut(tf, ::fwIO::s_DATA_KEY);

    reader->start();
    reader->configureWithIHM();
    reader->update().wait();
    reader->stop().wait();
    ::fwServices::OSR::unregisterService(reader);

    if(!tf->getName().empty())
    {
        if(this->hasTransferFunctionName( tf->getName() ) )
        {
            tf->setName( this->createTransferFunctionName( tf->getName() ) );
        }

        {
            ::fwData::mt::ObjectWriteLock poolTFLock(poolTF);
            compositeHelper.add(tf->getName(), tf);
        }

        m_pTransferFunctionPreset->addItem(QString(tf->getName().c_str()));
        this->presetChoice(static_cast<int>((*poolTF).size()-1));

        compositeHelper.notify();
    }
}

//------------------------------------------------------------------------------

void STransferFunction::exportTF()
{
    const ::fwIO::IWriter::sptr writer = ::fwServices::add< ::fwIO::IWriter >("::ioAtoms::SWriter");

    writer->registerInput(m_selectedTF, ::fwIO::s_DATA_KEY);

    writer->start();
    writer->configureWithIHM();
    writer->update().wait();
    writer->stop().wait();
    ::fwServices::OSR::unregisterService(writer);
}

//------------------------------------------------------------------------------

void STransferFunction::initTransferFunctions()
{
    // Get transfer function composite (pool TF)
    const ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", poolTF);

    ::fwDataTools::helper::Composite compositeHelper(poolTF);

    const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
    if(!this->hasTransferFunctionName(defaultTFName))
    {
        ::fwData::TransferFunction::sptr defaultTf = ::fwData::TransferFunction::createDefaultTF();
        const ::fwData::mt::ObjectWriteLock poolTFLock(poolTF);
        compositeHelper.add(defaultTFName, defaultTf);
    }

    // Test if transfer function composite has few TF
    ::fwData::mt::ObjectReadToWriteLock poolTFLock(poolTF);
    if(poolTF->size() <= 1)
    {
        poolTFLock.unlock();
        // Parse all TF contained in uiTF Bundle's resources
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

        const ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::New();
        const ::fwIO::IReader::sptr reader        = ::fwServices::add< ::fwIO::IReader >("::ioAtoms::SReader");
        reader->registerInOut(tf, ::fwIO::s_DATA_KEY);

        const ::fwRuntime::EConfigurationElement::sptr srvCfg  = ::fwRuntime::EConfigurationElement::New("service");
        const ::fwRuntime::EConfigurationElement::sptr fileCfg = ::fwRuntime::EConfigurationElement::New("file");
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
                const ::fwData::TransferFunction::sptr newTF = ::fwData::Object::copy< ::fwData::TransferFunction >(tf);
                if( this->hasTransferFunctionName( newTF->getName() ) )
                {
                    newTF->setName( this->createTransferFunctionName( newTF->getName() ) );
                }

                poolTFLock.lock();
                poolTFLock.upgrade();
                compositeHelper.add(newTF->getName(), newTF);
                poolTFLock.downgrade();
                poolTFLock.unlock();
            }
            tf->initTF();
        }
        ::fwServices::OSR::unregisterService(reader);
    }
    else
    {
        poolTFLock.unlock();
    }

    compositeHelper.notify();

    this->updateTransferFunctionPreset();
}

//---------------------------------------------------------------------------------------------------------------------

void STransferFunction::updateTransferFunctionPreset()
{
    const ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", poolTF);

    const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
    // Manage TF preset
    m_pTransferFunctionPreset->clear();
    ::fwData::mt::ObjectReadLock poolTFLock(poolTF);
    for(::fwData::Composite::value_type elt :  *poolTF)
    {
        m_pTransferFunctionPreset->addItem( elt.first.c_str() );
    }
    poolTFLock.unlock();

    std::string currentTFName                  = defaultTFName;
    const ::fwData::TransferFunction::csptr tf = this->getInput< ::fwData::TransferFunction >(s_CURRENT_TF_INPUT);

    if(tf)
    {
        currentTFName = tf->getName();
    }
    else if(m_selectedTF)
    {
        currentTFName = m_selectedTF->getName();
    }

    int index = m_pTransferFunctionPreset->findText( QString::fromStdString(currentTFName) );
    index = std::max(index, 0);

    this->presetChoice(index);
}

//------------------------------------------------------------------------------

bool STransferFunction::hasTransferFunctionName(const std::string& _name) const
{
    const ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", poolTF);
    const ::fwData::mt::ObjectReadLock poolTFLock(poolTF);
    return poolTF->find(_name) != poolTF->end();
}

//------------------------------------------------------------------------------

std::string STransferFunction::createTransferFunctionName(const std::string& _basename) const
{
    bool bHasTransferFunctionName = true;
    std::string newName           = _basename;
    int cpt                       = 1;
    while(bHasTransferFunctionName)
    {
        std::stringstream tmpStr;
        tmpStr <<  _basename <<  "_" <<  cpt;
        newName                  = tmpStr.str();
        bHasTransferFunctionName = this->hasTransferFunctionName(newName);
        cpt++;
    }

    return newName;
}

//------------------------------------------------------------------------------

void STransferFunction::updateTransferFunction()
{
    ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", poolTF);

    const std::string newSelectedTFKey = m_pTransferFunctionPreset->currentText().toStdString();
    SLM_DEBUG("Transfer function selected : " +  newSelectedTFKey);

    SLM_ASSERT("TF '"+ newSelectedTFKey +"' missing in pool", this->hasTransferFunctionName(newSelectedTFKey));

    const ::fwData::mt::ObjectReadLock poolTFLock(poolTF);
    const ::fwData::Object::sptr newSelectedTF = (*poolTF)[newSelectedTFKey];

    if(newSelectedTF && m_selectedTF != newSelectedTF)
    {
        this->setOutput(s_TF_OUTPUT, newSelectedTF);
        m_selectedTF = ::fwData::TransferFunction::dynamicCast(newSelectedTF);
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap STransferFunction::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_TF_POOL_INOUT, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push( s_TF_POOL_INOUT, ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push( s_TF_POOL_INOUT, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // end namespace
