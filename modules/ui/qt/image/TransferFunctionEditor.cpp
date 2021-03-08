/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/ui/qt/image/TransferFunctionEditor.hpp"

#include <core/base.hpp>
#include <core/runtime/EConfigurationElement.hpp>
#include <core/runtime/operations.hpp>

#include <data/Composite.hpp>
#include <data/helper/Composite.hpp>
#include <data/TransferFunction.hpp>

#include <io/base/service/ioTypes.hpp>
#include <io/base/service/IReader.hpp>
#include <io/base/service/IWriter.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>

#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>

#include <filesystem>

namespace sight::module::ui::qt::image
{

static const service::IService::KeyType s_TF_POOL_INOUT    = "tfPool";
static const service::IService::KeyType s_TF_OUTPUT        = "tf";
static const service::IService::KeyType s_CURRENT_TF_INPUT = "currentTF";

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

TransferFunctionEditor::TransferFunctionEditor()
{
    FW_DEPRECATED_MSG(
        "`module::ui::qt::image::TransferFunctionEditor` is deprecated, please use `module::ui::qt::image::STransferFunction` instead.",
        "22.0");
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
            const auto path = core::runtime::getModuleResourceFilePath(itCfg->second.get_value<std::string>());
            m_paths.push_back(path);
        }

        const std::string value = config.get("<xmlattr>.useDefaultPath", "yes");
        SIGHT_FATAL_IF("'useDefaultPath' value must be 'yes' or 'no'", value != "yes" && value != "no");

        useDefaultPath = (value == "yes");
    }
    if (useDefaultPath)
    {
        const auto pathRoot = core::runtime::getModuleResourceFilePath("sight::module::ui::qt", "tf");
        m_paths.push_back(pathRoot);
    }
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::starting()
{
    this->create();

    // Get the Qt container
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    // Buttons creation
    m_pTransferFunctionPreset = new QComboBox();

    std::filesystem::path modulePath = core::runtime::getModuleResourcePath(std::string("sight::module::ui::qt"));

    const auto deletePath = modulePath / "delete.png";
    m_deleteButton = new QPushButton(QIcon(deletePath.string().c_str()), "");
    m_deleteButton->setToolTip(QString("Delete"));

    const auto newPath = modulePath / "new.png";
    m_newButton = new QPushButton(QIcon(newPath.string().c_str()), "");
    m_newButton->setToolTip(QString("New"));

    const auto reinitializePath = modulePath / "reinitialize.png";
    m_reinitializeButton = new QPushButton(QIcon(reinitializePath.string().c_str()), "");
    m_reinitializeButton->setToolTip(QString("Reinitialize"));

    const auto renamePath = modulePath / "rename.png";
    m_renameButton = new QPushButton(QIcon(renamePath.string().c_str()), "");
    m_renameButton->setToolTip(QString("Rename"));

    const auto importPath = modulePath / "import.png";
    m_importButton = new QPushButton(QIcon(importPath.string().c_str()), "");
    m_importButton->setToolTip(QString("Import"));

    const auto exportPath = modulePath / "export.png";
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
    bool isEnabled     = (tfName != data::TransferFunction::s_DEFAULT_TF_NAME);

    m_renameButton->setEnabled(isEnabled);
    m_deleteButton->setEnabled(isEnabled);
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::deleteTF()
{
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Deleting confirmation");
    messageBox.setMessage("Are you sure you want to delete this transfer function?");
    messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::YES);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::CANCEL);
    sight::ui::base::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if (answerCopy != sight::ui::base::dialog::IMessageDialog::CANCEL)
    {
        data::Composite::sptr poolTF = this->getInOut< data::Composite >(s_TF_POOL_INOUT);
        SIGHT_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

        if( poolTF->size() > 1 )
        {
            int indexSelectedTF       = m_pTransferFunctionPreset->currentIndex();
            std::string selectedTFKey = m_pTransferFunctionPreset->currentText().toStdString();

            data::helper::Composite compositeHelper(poolTF);
            SIGHT_ASSERT("TF '"+ selectedTFKey +"' missing in pool", this->hasTransferFunctionName(selectedTFKey));
            compositeHelper.remove(selectedTFKey);

            {
                auto sig = poolTF->signal< data::Composite::RemovedObjectsSignalType >(
                    data::Composite::s_REMOVED_OBJECTS_SIG);
                core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                compositeHelper.notify();
            }

            m_pTransferFunctionPreset->removeItem(indexSelectedTF);
            std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;

            int index = m_pTransferFunctionPreset->findText(QString::fromStdString(defaultTFName));
            index = std::max(index, 0);
            this->presetChoice(index);
        }
        else
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                "You can not remove this transfer function because the program requires at least one.",
                sight::ui::base::dialog::IMessageDialog::WARNING );
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

    sight::ui::base::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Creating transfer function");
    inputDialog.setMessage("Transfer function name:");
    inputDialog.setInput( newName );
    newName = inputDialog.getInput();

    if (!newName.empty())
    {
        if(!this->hasTransferFunctionName(newName))
        {
            data::TransferFunction::sptr pNewTransferFunction;

            pNewTransferFunction = data::Object::copy(m_selectedTF);
            pNewTransferFunction->setName(newName);

            data::Composite::sptr poolTF = this->getInOut< data::Composite >(s_TF_POOL_INOUT);
            SIGHT_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

            data::helper::Composite compositeHelper(poolTF);
            compositeHelper.add(newName, pNewTransferFunction);

            m_pTransferFunctionPreset->addItem(QString(newName.c_str()));
            m_pTransferFunctionPreset->setCurrentIndex(m_pTransferFunctionPreset->count()-1);
            this->updateTransferFunction();

            compositeHelper.notify();
        }
        else
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                "This transfer function name already exists so you can not overwrite it.",
                sight::ui::base::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::reinitializeTFPool()
{
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Reinitializing confirmation");
    messageBox.setMessage("Are you sure you want to reinitialize all transfer functions?");
    messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::YES);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::CANCEL);
    sight::ui::base::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if (answerCopy != sight::ui::base::dialog::IMessageDialog::CANCEL)
    {
        data::Composite::sptr poolTF = this->getInOut< data::Composite >(s_TF_POOL_INOUT);
        SIGHT_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

        data::helper::Composite compositeHelper(poolTF);
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

    sight::ui::base::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Creating transfer function");
    inputDialog.setMessage("Transfer function name:");
    inputDialog.setInput( newName );
    newName = inputDialog.getInput();

    if (!newName.empty() && newName != str)
    {
        if( !this->hasTransferFunctionName(newName) )
        {
            data::Composite::sptr poolTF = this->getInOut< data::Composite >(s_TF_POOL_INOUT);
            SIGHT_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

            data::TransferFunction::sptr pTF;
            pTF = data::TransferFunction::dynamicCast((*poolTF)[str]);
            pTF->setName(newName);

            data::helper::Composite compositeHelper(poolTF);
            compositeHelper.remove(str);
            compositeHelper.add(newName, pTF);
            compositeHelper.notify();

            m_pTransferFunctionPreset->setItemText(m_pTransferFunctionPreset->currentIndex(), QString(newName.c_str()));
            m_pTransferFunctionPreset->setCurrentIndex(m_pTransferFunctionPreset->findText(QString(newName.c_str())));

            this->updateTransferFunction();
        }
        else
        {
            sight::ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage("This transfer function name already exists so you can not overwrite it.");
            messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
            messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
            messageBox.show();
        }
    }
    if ( newName.empty() )
    {
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage("You have to give a name to your transfer function.");
        messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
        messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::importTF()
{
    data::Composite::sptr poolTF = this->getInOut< data::Composite >(s_TF_POOL_INOUT);
    SIGHT_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

    data::helper::Composite compositeHelper(poolTF);

    data::TransferFunction::sptr tf         = data::TransferFunction::New();
    io::base::service::IReader::sptr reader = service::add< io::base::service::IReader >(
        "::sight::module::io::atoms::SReader");

    reader->registerInOut(tf, io::base::service::s_DATA_KEY);

    reader->start();
    reader->openLocationDialog();
    reader->update().wait();
    reader->stop().wait();
    service::OSR::unregisterService(reader);

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
    io::base::service::IWriter::sptr writer = service::add< io::base::service::IWriter >(
        "::sight::module::io::atoms::SWriter");

    writer->registerInput(m_selectedTF, io::base::service::s_DATA_KEY);

    writer->start();
    writer->openLocationDialog();
    writer->update().wait();
    writer->stop().wait();
    service::OSR::unregisterService(writer);
}

//------------------------------------------------------------------------------

void TransferFunctionEditor::initTransferFunctions()
{
    // Get transfer function composite (pool TF)
    data::Composite::sptr poolTF = this->getInOut< data::Composite >(s_TF_POOL_INOUT);
    SIGHT_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

    data::helper::Composite compositeHelper(poolTF);

    const std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;
    if(!this->hasTransferFunctionName(defaultTFName))
    {
        data::TransferFunction::sptr defaultTf = data::TransferFunction::createDefaultTF();
        compositeHelper.add(defaultTFName, defaultTf);
    }

    // Test if transfer function composite has few TF
    if( poolTF->size() <= 1 )
    {
        // Parse all TF contained in uiTF module's resources
        std::vector< std::filesystem::path > paths;
        for(std::filesystem::path dirPath :  m_paths)
        {
            SIGHT_ASSERT("Invalid directory path '"+dirPath.string()+"'", std::filesystem::exists(dirPath));
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

        data::TransferFunction::sptr tf         = data::TransferFunction::New();
        io::base::service::IReader::sptr reader = service::add< io::base::service::IReader >(
            "::sight::module::io::atoms::SReader");
        reader->registerInOut(tf, io::base::service::s_DATA_KEY);

        core::runtime::EConfigurationElement::sptr srvCfg  = core::runtime::EConfigurationElement::New("service");
        core::runtime::EConfigurationElement::sptr fileCfg = core::runtime::EConfigurationElement::New("file");
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
                data::TransferFunction::sptr newTF = data::Object::copy< data::TransferFunction >(tf);
                if( this->hasTransferFunctionName( newTF->getName() ) )
                {
                    newTF->setName( this->createTransferFunctionName( newTF->getName() ) );
                }

                compositeHelper.add(newTF->getName(), newTF);
            }
            tf->initTF();
        }
        service::OSR::unregisterService(reader);
    }
    compositeHelper.notify();

    this->updateTransferFunctionPreset();
}

//---------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::updateTransferFunctionPreset()
{
    data::Composite::sptr poolTF = this->getInOut< data::Composite >(s_TF_POOL_INOUT);
    SIGHT_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

    const std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;
    // Manage TF preset
    m_pTransferFunctionPreset->clear();
    for(data::Composite::value_type elt :  *poolTF)
    {
        m_pTransferFunctionPreset->addItem( elt.first.c_str() );
    }

    std::string currentTFName        = defaultTFName;
    data::TransferFunction::csptr tf = this->getInput< data::TransferFunction >(s_CURRENT_TF_INPUT);

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
    data::Composite::sptr poolTF = this->getInOut< data::Composite >(s_TF_POOL_INOUT);
    SIGHT_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);
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
    data::Composite::sptr poolTF = this->getInOut< data::Composite >(s_TF_POOL_INOUT);
    SIGHT_ASSERT("inout '" + s_TF_POOL_INOUT + "' is not defined.", poolTF);

    std::string newSelectedTFKey = m_pTransferFunctionPreset->currentText().toStdString();
    SIGHT_DEBUG("Transfer function selected : " +  newSelectedTFKey);

    SIGHT_ASSERT("TF '"+ newSelectedTFKey +"' missing in pool", this->hasTransferFunctionName(newSelectedTFKey));

    data::Object::sptr newSelectedTF = (*poolTF)[newSelectedTFKey];

    if(newSelectedTF && m_selectedTF != newSelectedTF)
    {
        this->setOutput(s_TF_OUTPUT, newSelectedTF);
        m_selectedTF = data::TransferFunction::dynamicCast(newSelectedTF);
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap TransferFunctionEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_TF_POOL_INOUT, data::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push( s_TF_POOL_INOUT, data::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push( s_TF_POOL_INOUT, data::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // end namespace
