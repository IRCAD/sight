/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "uiTF/TransferFunctionEditor.hpp"

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
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

#include <filesystem>
#include <QBoxLayout>
#include <QComboBox>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace uiTF
{

static const ::fwServices::IService::KeyType s_TF_POOL_INOUT    = "tfPool";
static const ::fwServices::IService::KeyType s_TF_OUTPUT        = "tf";
static const ::fwServices::IService::KeyType s_CURRENT_TF_INPUT = "currentTF";

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiTF::TransferFunctionEditor);

//------------------------------------------------------------------------------

TransferFunctionEditor::TransferFunctionEditor()
{
}

//------------------------------------------------------------------------------

TransferFunctionEditor::~TransferFunctionEditor() noexcept
{
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::configuring()
{
    this->initialize();

    const ConfigType srvConfig = this->getConfigTree();

    bool useDefaultPath = true;
    if (srvConfig.count("config"))
    {
        const ConfigType config = srvConfig.get_child("config");

        const auto pathCfg = config.equal_range("path");
        for(auto itCfg = pathCfg.first; itCfg != pathCfg.second; ++itCfg)
        {
            const auto path = ::fwRuntime::getBundleResourceFilePath(itCfg->second.get_value<std::string>());
            m_paths.push_back(path);
        }

        const std::string value = config.get("<xmlattr>.useDefaultPath", "yes");
        SLM_FATAL_IF("'useDefaultPath' value must be 'yes' or 'no'", value != "yes" && value != "no");

        useDefaultPath = (value == "yes");
    }
    if (useDefaultPath)
    {
        const auto pathRoot = ::fwRuntime::getBundleResourceFilePath("uiTF", "tf");
        m_paths.push_back(pathRoot);
    }
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::starting()
{
    this->create();

    // Get the Qt container
    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    // Buttons creation
    m_pTransferFunctionPreset = new QComboBox();

    std::filesystem::path bundlePath = ::fwRuntime::getBundleResourcePath(std::string("uiTF"));

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
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);

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

void TransferFunctionEditor::updating()
{
    this->updateTransferFunctionPreset();
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::stopping()
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

void TransferFunctionEditor::swapping(const KeyType& key)
{
    if (key == s_CURRENT_TF_INPUT)
    {
        this->updateTransferFunctionPreset();
    }
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::presetChoice(int index)
{
    m_pTransferFunctionPreset->setCurrentIndex(index);
    this->updateTransferFunction();

    std::string tfName = m_pTransferFunctionPreset->currentText().toStdString();
    bool isEnabled     = (tfName != ::fwData::TransferFunction::s_DEFAULT_TF_NAME);

    m_renameButton->setEnabled(isEnabled);
    m_deleteButton->setEnabled(isEnabled);
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::deleteTF()
{
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Deleting confirmation");
    messageBox.setMessage("Are you sure you want to delete this transfer function?");
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    ::fwGui::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if (answerCopy != ::fwGui::dialog::IMessageDialog::CANCEL)
    {
        ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

        if( poolTF->size() > 1 )
        {
            int indexSelectedTF       = m_pTransferFunctionPreset->currentIndex();
            std::string selectedTFKey = m_pTransferFunctionPreset->currentText().toStdString();

            ::fwDataTools::helper::Composite compositeHelper(poolTF);
            SLM_ASSERT("TF '"+ selectedTFKey +"' missing in pool", this->hasTransferFunctionName(selectedTFKey));
            compositeHelper.remove(selectedTFKey);

            {
                auto sig = poolTF->signal< ::fwData::Composite::RemovedObjectsSignalType >(
                    ::fwData::Composite::s_REMOVED_OBJECTS_SIG);
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                compositeHelper.notify();
            }

            m_pTransferFunctionPreset->removeItem(indexSelectedTF);
            std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;

            int index = m_pTransferFunctionPreset->findText(QString::fromStdString(defaultTFName));
            index = std::max(index, 0);
            this->presetChoice(index);
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning",
                "You can not remove this transfer function because the program requires at least one.",
                ::fwGui::dialog::IMessageDialog::WARNING );
        }
    }
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::newTF()
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

    if (!newName.empty())
    {
        if(!this->hasTransferFunctionName(newName))
        {
            ::fwData::TransferFunction::sptr pNewTransferFunction;

            pNewTransferFunction = ::fwData::Object::copy(m_selectedTF);
            pNewTransferFunction->setName(newName);

            ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
            SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

            ::fwDataTools::helper::Composite compositeHelper(poolTF);
            compositeHelper.add(newName, pNewTransferFunction);

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

void TransferFunctionEditor::reinitializeTFPool()
{
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Reinitializing confirmation");
    messageBox.setMessage("Are you sure you want to reinitialize all transfer functions?");
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    ::fwGui::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if (answerCopy != ::fwGui::dialog::IMessageDialog::CANCEL)
    {
        ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

        ::fwDataTools::helper::Composite compositeHelper(poolTF);
        compositeHelper.clear();
        compositeHelper.notify();

        this->initTransferFunctions();

        this->updateTransferFunction();
    }
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::renameTF()
{
    std::string str = m_pTransferFunctionPreset->currentText().toStdString();
    std::string newName(str);

    fwGui::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Creating transfer function");
    inputDialog.setMessage("Transfer function name:");
    inputDialog.setInput( newName );
    newName = inputDialog.getInput();

    if (!newName.empty() && newName != str)
    {
        if( !this->hasTransferFunctionName(newName) )
        {
            ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
            SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

            ::fwData::TransferFunction::sptr pTF;
            pTF = ::fwData::TransferFunction::dynamicCast((*poolTF)[str]);
            pTF->setName(newName);

            ::fwDataTools::helper::Composite compositeHelper(poolTF);
            compositeHelper.remove(str);
            compositeHelper.add(newName, pTF);
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
    if ( newName.empty() )
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

void TransferFunctionEditor::importTF()
{
    ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

    ::fwDataTools::helper::Composite compositeHelper(poolTF);

    ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::New();
    ::fwIO::IReader::sptr reader        = ::fwServices::add< ::fwIO::IReader >("::ioAtoms::SReader");

    reader->registerInOut(tf, ::fwIO::s_DATA_KEY);

    reader->start();
    reader->configureWithIHM();
    reader->update().wait();
    reader->stop().wait();
    ::fwServices::OSR::unregisterService(reader);

    if (!tf->getName().empty())
    {
        if( this->hasTransferFunctionName( tf->getName() ) )
        {
            tf->setName( this->createTransferFunctionName( tf->getName() ) );
        }

        compositeHelper.add(tf->getName(), tf);
        m_pTransferFunctionPreset->addItem(QString(tf->getName().c_str()));
        this->presetChoice(static_cast<int>((*poolTF).size()-1));

        compositeHelper.notify();
    }
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::exportTF()
{
    ::fwIO::IWriter::sptr writer = ::fwServices::add< ::fwIO::IWriter >("::ioAtoms::SWriter");

    writer->registerInput(m_selectedTF, ::fwIO::s_DATA_KEY);

    writer->start();
    writer->configureWithIHM();
    writer->update().wait();
    writer->stop().wait();
    ::fwServices::OSR::unregisterService(writer);
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::initTransferFunctions()
{
    // Get transfer function composite (pool TF)
    ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

    ::fwDataTools::helper::Composite compositeHelper(poolTF);

    const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
    if(!this->hasTransferFunctionName(defaultTFName))
    {
        ::fwData::TransferFunction::sptr defaultTf = ::fwData::TransferFunction::createDefaultTF();
        compositeHelper.add(defaultTFName, defaultTf);
    }

    // Test if transfer function composite has few TF
    if( poolTF->size() <= 1 )
    {
        // Parse all TF contained in uiTF Bundle's resources
        std::vector< std::filesystem::path > paths;
        for(std::filesystem::path dirPath :  m_paths)
        {
            SLM_ASSERT("Invalid directory path '"+dirPath.string()+"'", std::filesystem::exists(dirPath));
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

        ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::New();
        ::fwIO::IReader::sptr reader        = ::fwServices::add< ::fwIO::IReader >("::ioAtoms::SReader");
        reader->registerInOut(tf, ::fwIO::s_DATA_KEY);

        ::fwRuntime::EConfigurationElement::sptr srvCfg  = ::fwRuntime::EConfigurationElement::New("service");
        ::fwRuntime::EConfigurationElement::sptr fileCfg = ::fwRuntime::EConfigurationElement::New("file");
        srvCfg->addConfigurationElement(fileCfg);

        for( std::filesystem::path file :  paths )
        {
            fileCfg->setValue(file.string());
            reader->setConfiguration(srvCfg);
            reader->configure();
            reader->start();
            reader->update();
            reader->stop();

            if (!tf->getName().empty())
            {
                ::fwData::TransferFunction::sptr newTF = ::fwData::Object::copy< ::fwData::TransferFunction >(tf);
                if( this->hasTransferFunctionName( newTF->getName() ) )
                {
                    newTF->setName( this->createTransferFunctionName( newTF->getName() ) );
                }

                compositeHelper.add(newTF->getName(), newTF);
            }
            tf->initTF();
        }
        ::fwServices::OSR::unregisterService(reader);
    }
    compositeHelper.notify();

    this->updateTransferFunctionPreset();
}

//---------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::updateTransferFunctionPreset()
{
    ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

    const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
    // Manage TF preset
    m_pTransferFunctionPreset->clear();
    for(::fwData::Composite::value_type elt :  *poolTF)
    {
        m_pTransferFunctionPreset->addItem( elt.first.c_str() );
    }

    std::string currentTFName = defaultTFName;
    ::fwData::TransferFunction::csptr tf = this->getInput< ::fwData::TransferFunction >(s_CURRENT_TF_INPUT);

    if (tf)
    {
        currentTFName = tf->getName();
    }
    else if (m_selectedTF)
    {
        currentTFName = m_selectedTF->getName();
    }

    int index = m_pTransferFunctionPreset->findText( QString::fromStdString(currentTFName) );
    index = std::max(index, 0);

    this->presetChoice(index);
}

//------------------------------------------------------------------------------

bool TransferFunctionEditor::hasTransferFunctionName(const std::string& _sName) const
{
    ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);
    return poolTF->find(_sName) != poolTF->end();
}

//------------------------------------------------------------------------------

std::string TransferFunctionEditor::createTransferFunctionName(const std::string& _sBasename) const
{
    bool bHasTransferFunctionName = true;
    std::string newName           = _sBasename;
    int cpt                       = 1;
    while (bHasTransferFunctionName)
    {
        std::stringstream tmpStr;
        tmpStr <<  _sBasename <<  "_" <<  cpt;
        newName                  = tmpStr.str();
        bHasTransferFunctionName = this->hasTransferFunctionName(newName);
        cpt++;
    }

    return newName;
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::updateTransferFunction()
{
    ::fwData::Composite::sptr poolTF = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

    std::string newSelectedTFKey = m_pTransferFunctionPreset->currentText().toStdString();
    SLM_DEBUG("Transfer function selected : " +  newSelectedTFKey);

    SLM_ASSERT("TF '"+ newSelectedTFKey +"' missing in pool", this->hasTransferFunctionName(newSelectedTFKey));

    ::fwData::Object::sptr newSelectedTF = (*poolTF)[newSelectedTFKey];

    if(newSelectedTF && m_selectedTF != newSelectedTF)
    {
        this->setOutput(s_TF_OUTPUT, newSelectedTF);
        m_selectedTF = ::fwData::TransferFunction::dynamicCast(newSelectedTF);
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap TransferFunctionEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_TF_POOL_INOUT, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push( s_TF_POOL_INOUT, ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push( s_TF_POOL_INOUT, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // end namespace
