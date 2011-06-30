/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/bind.hpp>

#include <fwXML/Serializer.hpp>
#include <fwXML/policy/NeverSplitPolicy.hpp>
#include <fwXML/policy/UniquePathPolicy.hpp>

#include <QWidget>
#include <QBoxLayout>
#include <QComboBox>
#include <QString>
#include <QPushButton>
#include <QIcon>
#include <QDir>
#include <QFileDialog>

//#include <vtkLookupTableManager.h>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwServices/IEditionService.hpp>
#include <fwServices/macros.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiTF/TransferFunctionEditor.hpp"

namespace uiTF
{

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::editor::IEditor, ::uiTF::TransferFunctionEditor, ::fwData::Image );

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

TransferFunctionEditor::TransferFunctionEditor() throw()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

TransferFunctionEditor::~TransferFunctionEditor() throw()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create();

    // Get the Qt container
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());
    m_container = qtContainer->getQtContainer();
    SLM_ASSERT("Sorry, the qt container is not valid", m_container);

    // Buttons creation
    m_pTransferFunctionPreset = new QComboBox(m_container);

    ::boost::filesystem::path deletePath ("Bundles/uiTF_" + std::string(UITF_VER) + "/delete.png");
    m_deleteButton = new QPushButton(QIcon(deletePath.string().c_str()), "", m_container);
    m_deleteButton->setToolTip(QString("Delete"));

    ::boost::filesystem::path newPath ("Bundles/uiTF_" + std::string(UITF_VER) + "/new.png");
    m_newButton = new QPushButton(QIcon(newPath.string().c_str()), "", m_container);
    m_newButton->setToolTip(QString("New"));

    ::boost::filesystem::path reinitializePath ("Bundles/uiTF_" + std::string(UITF_VER) + "/reinitialize.png");
    m_reinitializeButton = new QPushButton(QIcon(reinitializePath.string().c_str()), "", m_container);
    m_reinitializeButton->setToolTip(QString("Reinitialize"));

    ::boost::filesystem::path renamePath ("Bundles/uiTF_" + std::string(UITF_VER) + "/rename.png");
    m_renameButton = new QPushButton(QIcon(renamePath.string().c_str()), "", m_container);
    m_renameButton->setToolTip(QString("Rename"));

    ::boost::filesystem::path importPath ("Bundles/uiTF_" + std::string(UITF_VER) + "/import.png");
    m_importButton = new QPushButton(QIcon(importPath.string().c_str()), "", m_container);
    m_importButton->setToolTip(QString("Import"));

    ::boost::filesystem::path exportPath ("Bundles/uiTF_" + std::string(UITF_VER) + "/export.png");
    m_exportButton = new QPushButton(QIcon(exportPath.string().c_str()), "", m_container);
    m_exportButton->setToolTip(QString("Export"));

    // Layout management
    QBoxLayout* layout =  new QBoxLayout(QBoxLayout::LeftToRight, m_container);

    layout->addWidget(m_pTransferFunctionPreset);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_newButton);
    layout->addWidget(m_reinitializeButton);
    layout->addWidget(m_renameButton);
    layout->addWidget(m_importButton);
    layout->addWidget(m_exportButton);

    m_container->setLayout(layout);

    // Qt signals management ( connection to buttons )
    QObject::connect(m_pTransferFunctionPreset, SIGNAL(   activated(int)), this, SLOT(presetChoice(int)));
    QObject::connect(m_deleteButton, SIGNAL(   pressed()), this, SLOT(deleteTF()));
    QObject::connect(m_newButton, SIGNAL(   pressed()), this, SLOT(newTF()));
    QObject::connect(m_reinitializeButton, SIGNAL(   pressed()), this, SLOT(reinitializeTF()));
    QObject::connect(m_renameButton, SIGNAL(   pressed()), this, SLOT(renameTF()));
    QObject::connect(m_importButton, SIGNAL(   pressed()), this, SLOT(importTF()));
    QObject::connect(m_exportButton, SIGNAL(   pressed()), this, SLOT(exportTF()));

    // preset initialization
    initTransferFunctions();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::updating(::fwServices::ObjectMsg::csptr _msg) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Qt signals management ( disconnection )
    QObject::disconnect(m_pTransferFunctionPreset, SIGNAL(   activated(int)), this, SLOT((int)));
    QObject::disconnect(m_deleteButton, SIGNAL(   pressed()), this, SLOT(deleteTF()));
    QObject::disconnect(m_newButton, SIGNAL(   pressed()), this, SLOT(newTF()));
    QObject::disconnect(m_reinitializeButton, SIGNAL(   pressed()), this, SLOT(reinitializeTF()));
    QObject::disconnect(m_renameButton, SIGNAL(   pressed()), this, SLOT(renameTF()));
    QObject::disconnect(m_importButton, SIGNAL(   pressed()), this, SLOT(importTF()));
    QObject::disconnect(m_exportButton, SIGNAL(   pressed()), this, SLOT(exportTF()));

    // Buttons destruction
    m_pTransferFunctionPreset->deleteLater();
    m_deleteButton->deleteLater();
    m_newButton->deleteLater();
    m_reinitializeButton->deleteLater();
    m_renameButton->deleteLater();
    m_importButton->deleteLater();
    m_exportButton->deleteLater();

    // Container cleaning
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());
    qtContainer->clean();

    this->destroy();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::presetChoice(int index)
{
    SLM_TRACE_FUNC();
    m_pTransferFunctionPreset->setCurrentIndex(index);
    this->updateImageWithSeletedTransferFunction();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::deleteTF()
{
    SLM_TRACE_FUNC();

    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Deleting confirmation");
    messageBox.setMessage("Are you sure you want to delete this transfer function?");
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    ::fwGui::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if (answerCopy != ::fwGui::dialog::IMessageDialog::CANCEL)
    {
        ::fwData::Composite::sptr tfc = m_tranferFunctionComposite.lock();

        if( tfc->size() > 1 )
        {
            int indexSelectedTF = m_pTransferFunctionPreset->currentIndex();
            std::string selectedTF = m_selectedTranferFunctionId.lock()->value();
            tfc->erase(selectedTF);
            m_pTransferFunctionPreset->removeItem(indexSelectedTF);
            std::string defaultTFName = ::fwData::TransfertFunction::defaultTransfertFunctionName;

            if ( tfc->find(defaultTFName) != tfc->end() )
            {
                m_pTransferFunctionPreset->setCurrentIndex(m_pTransferFunctionPreset->findText(QString(defaultTFName.c_str())));
            }
            else
            {
                m_pTransferFunctionPreset->setCurrentIndex(0);
            }

            this->updateImageWithSeletedTransferFunction();
        }
        else
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage("You can not remove this transfer function because the program requires at least one.");
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::newTF()
{
    SLM_TRACE_FUNC();

    std::string name (m_selectedTranferFunctionId.lock()->value());
    std::string newName = name;
    if( hasTransferFunctionName(newName) )
    {
        newName = createTransferFunctionName(newName);
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
            ::fwData::TransfertFunction::NewSptr pNewTransferFunction ;
            ::fwData::Object::sptr obj = (*m_tranferFunctionComposite.lock())[name];

            pNewTransferFunction->deepCopy(obj);
            pNewTransferFunction->setCRefName(newName);
            (*m_tranferFunctionComposite.lock())[newName] = pNewTransferFunction;
            m_pTransferFunctionPreset->addItem(QString(newName.c_str()));
            m_pTransferFunctionPreset->setCurrentIndex(m_pTransferFunctionPreset->count()-1);

            this->updateImageWithSeletedTransferFunction();
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
    else
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage("You have to give a name to your transfer function.");
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::reinitializeTF()
{
    SLM_TRACE_FUNC();

    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Reinitializing confirmation");
    messageBox.setMessage("Are you sure you want to reinitialize all transfer functions?");
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    ::fwGui::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

    if (answerCopy != ::fwGui::dialog::IMessageDialog::CANCEL)
    {
        (*m_tranferFunctionComposite.lock()).clear();
        this->initTransferFunctions();

        this->updateImageWithSeletedTransferFunction();
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::renameTF()
{
    SLM_TRACE_FUNC();
    if ( m_selectedTranferFunctionId.lock()->value().find("STD") != std::string::npos )
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage("STD transfer function can not be renamed.");
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
        return;
    }

    std::string newName (m_selectedTranferFunctionId.lock()->value());

    fwGui::dialog::InputDialog inputDialog;
    inputDialog.setTitle("Creating transfer function");
    inputDialog.setMessage("Transfer function name:");
    inputDialog.setInput( newName );
    newName = inputDialog.getInput();

    if (    ! newName.empty() &&
            newName != m_selectedTranferFunctionId.lock()->value())
    {
        if( ! hasTransferFunctionName(newName) )
        {
            std::string str = m_pTransferFunctionPreset->currentText().toStdString();
            ::fwData::TransfertFunction::sptr pTF = ::fwData::TransfertFunction::dynamicCast((*m_tranferFunctionComposite.lock())[str]);
            pTF->setCRefName(newName);

            (*m_tranferFunctionComposite.lock()).erase(str);
            (*m_tranferFunctionComposite.lock())[newName] = pTF;
            m_selectedTranferFunctionId.lock()->value() = newName;
            m_pTransferFunctionPreset->setItemText(m_pTransferFunctionPreset->currentIndex(), QString(newName.c_str()));
            m_pTransferFunctionPreset->setCurrentIndex(m_pTransferFunctionPreset->findText(QString(newName.c_str())));

            this->updateImageWithSeletedTransferFunction();
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

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::importTF()
{
    SLM_TRACE_FUNC();

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Select one or more transfer functions to import");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::LocationDialog::MULTI_FILES);
    dialogFile.addFilter("XML","*.xml");
    ::fwData::location::MultiFiles::sptr  files;
    files = ::fwData::location::MultiFiles::dynamicCast(dialogFile.show());

    bool someTfHasBeenImported = false;

    if ( files )
    {
        fwXML::Serializer serializer;
        ::boost::shared_ptr< ::fwXML::NeverSplitPolicy > spolicy (new fwXML::NeverSplitPolicy);
        serializer.setSplitPolicy(spolicy);
        ::fwData::TransfertFunction::sptr  pTf;

        bool tfIsImported;


        BOOST_FOREACH( ::boost::filesystem::path tfPath, files->getPaths() )
        {

            tfIsImported = true;
            ::fwTools::Object::sptr  pObject;

            try
            {
                pObject = serializer.deSerialize(tfPath);
            }
            catch (...)
            {
                SLM_DEBUG("This XML file is not a data object");
                tfIsImported = false;
            }

            if ( tfIsImported )
            {
                pTf = ::fwData::TransfertFunction::dynamicCast(pObject);
                if (pTf == 0)
                {
                    SLM_DEBUG("This XML file is not a transfer function");
                    tfIsImported = false;
                }
            }

            if ( tfIsImported )
            {
                // If we have one TF imported and if it is the first, clear composite before add TF
                if ( ! someTfHasBeenImported )
                {
                    someTfHasBeenImported = true;
                    (*m_tranferFunctionComposite.lock()).clear();
                    m_pTransferFunctionPreset->clear();
                }

                if( hasTransferFunctionName( pTf->getName() ) )
                {
                    pTf->setCRefName( createTransferFunctionName( pTf->getName() ) );
                }

                ::fwData::TransfertFunction::NewSptr  pNewTf;
                pNewTf->deepCopy(pTf);
                (*m_tranferFunctionComposite.lock())[pTf->getName()] = pNewTf;
                m_pTransferFunctionPreset->addItem(QString(pTf->getName().c_str()));
            }
            else
            {
                std::stringstream xmlFile;
                xmlFile <<  "Sorry, the xml file \"" <<  tfPath.string() <<  "\" does not contain a transfer function. It has not been imported.";
                ::fwGui::dialog::MessageDialog messageBox;
                messageBox.setTitle("Warning");
                messageBox.setMessage(xmlFile.str().c_str());
                messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
                messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
                messageBox.show();
            }
        }
    }

    if( someTfHasBeenImported )
    {
        m_pTransferFunctionPreset->setCurrentIndex((*m_tranferFunctionComposite.lock()).size()-1);
        this->updateImageWithSeletedTransferFunction();
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::exportTF()
{
    SLM_TRACE_FUNC();

    if (m_selectedTranferFunctionId.lock()->value().find("STD")!=std::string::npos)
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage("STD transfer function can not be exported.");
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
        return;
    }

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Export transfer function");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::LocationDialog::SINGLE_FILE);
    dialogFile.addFilter("XML","*.xml");
    ::fwData::location::SingleFile::sptr  fileName;
    fileName = ::fwData::location::SingleFile::dynamicCast(dialogFile.show());

    if ( fileName )
    {
        try
        {
            ::boost::filesystem::path tfPath = fileName->getPath();
            ::boost::filesystem::path tfPathRoot = tfPath.parent_path();

            bool exportFile = true;
            if ( ::boost::filesystem::exists(tfPath) )
            {
                ::fwGui::dialog::MessageDialog messageBox;
                messageBox.setTitle("Overwriting confirmation");
                messageBox.setMessage("Are you sure you want to overwrite this transfer function?");
                messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
                messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
                messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
                ::fwGui::dialog::IMessageDialog::Buttons answerCopy = messageBox.show();

                if (answerCopy == ::fwGui::dialog::IMessageDialog::YES)
                {
                    ::boost::filesystem::remove(tfPath);
                }
                else
                {
                    exportFile = false;
                }
            }

            if ( exportFile )
            {
                fwXML::Serializer serializer;
                serializer.rootFolder() = tfPathRoot.string();
                ::boost::shared_ptr< ::fwXML::UniquePathPolicy > pPathPolicy (new ::fwXML::UniquePathPolicy(tfPath.leaf()));
                serializer.setPathPolicy(pPathPolicy);
                ::boost::shared_ptr< ::fwXML::NeverSplitPolicy > pSplitPolicy (new ::fwXML::NeverSplitPolicy());
                serializer.setSplitPolicy(pSplitPolicy);

                serializer.serialize( (*m_tranferFunctionComposite.lock())[m_selectedTranferFunctionId.lock()->value()] );

                if ( ! ::boost::filesystem::exists(tfPath) )
                {
                    OSLM_ERROR("Input File \"" <<  tfPath.string() <<  "\" does not exist\n");
                }

            }

        }
        catch(std::exception& e)
        {
            OSLM_ERROR("Error: " <<  e.what());
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::initTransferFunctions()
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Sorry, this image is not a medical image", this->getObject()->getFieldSize( ::fwComEd::Dictionary::m_transfertFunctionCompositeId ) );

    // Get transfert function composite and selected TF
    ::fwData::Composite::sptr tfc = this->getObject()->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );
    m_tranferFunctionComposite = tfc;
    ::fwData::String::sptr selectedTfId = this->getObject()->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_transfertFunctionId );
    m_selectedTranferFunctionId = selectedTfId;
    OSLM_TRACE("m_selectedTranferFunctionId : " <<  selectedTfId->value());


    // Test if transfert function composite has few TF, if no then create default TF
    std::string defaultTFName = ::fwData::TransfertFunction::defaultTransfertFunctionName;
    if (    tfc->size() == 0 ||
            tfc->find(defaultTFName) != tfc->end() && tfc->size() == 1 )
    {
        if ( tfc->size() == 0 )
        {
            (*tfc)[defaultTFName] = ::fwData::TransfertFunction::createDefaultTransfertFunction(  this->getObject< ::fwData::Image >() );
            selectedTfId->value() = defaultTFName;
        }

        // Parse all TF contained in uiTF Bundle's ressources
        std::vector< ::boost::filesystem::path > paths;
        ::boost::filesystem::path pathRoot ("Bundles/uiTF_" + std::string(UITF_VER) + "/tf");
        for(    ::boost::filesystem::directory_iterator it(pathRoot);
                it != ::boost::filesystem::directory_iterator();
                ++it )
        {
            if(     ! ::boost::filesystem::is_directory(*it) &&
                    ::boost::filesystem::extension(*it) == ".xml" )
            {
                paths.push_back(*it);
                OSLM_DEBUG( "New TF xml parsed in bundle :" << it->string() );
            }
        }

        // Load TF parsed from xml file
        ::fwXML::Serializer serializer;
        ::boost::shared_ptr< ::fwXML::NeverSplitPolicy > spolicy (new ::fwXML::NeverSplitPolicy);
        serializer.setSplitPolicy(spolicy);
        ::fwData::TransfertFunction::sptr pTf;

        BOOST_FOREACH( ::boost::filesystem::path file, paths )
        {
            ::fwTools::Object::sptr pObject = serializer.deSerialize( file, false , false /*NO schema verification*/ );
            pTf =  ::fwData::TransfertFunction::dynamicCast(pObject);
            SLM_ASSERT( "Sorry, loaded object is not a TF object.", pTf );

            if( hasTransferFunctionName( pTf->getName() ) )
            {
                pTf->setCRefName( createTransferFunctionName( pTf->getName() ) );
            }
            ::fwData::TransfertFunction::NewSptr pNewTf;
            pNewTf->deepCopy(pTf);
            (*tfc)[ pTf->getName() ] = pNewTf;
        }

//        // Load TF from VTK
//        ::fwData::Integer::sptr minField = this->getObject()->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
//        ::fwData::Integer::sptr maxField = this->getObject()->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );
//
//        int width  = maxField->value() - minField->value();
//        SLM_ASSERT("Sorry, width is null." ,width != 0);
//
//        std::vector<std::string> lutNames = vtkLookupTableManager::GetAvailableLookupTables();
//        for(    unsigned int i=0;
//                i<lutNames.size();
//                i++)
//        {
//            ::fwData::TransfertFunction::sptr  pTf = ::fwData::TransfertFunction::New();
//            pTf->setCRefName( lutNames[i].c_str());
//            ::boost::shared_ptr< ::fwData::Color > color;
//
//            vtkLookupTable* lut = vtkLookupTableManager::GetLookupTable(i);
//            unsigned int nbValues = lut->GetNumberOfTableValues ();
//            for(    unsigned int index=0;
//                    index < nbValues;
//                    index+=8)
//            {
//                int step = minField->value() + (width/nbValues) * index;
//                vtkFloatingPointType rgba[ 4 ];
//                lut->GetTableValue ( index, rgba );
//                color = pTf->getColor( step );
//                color->getRefRGBA()[0] = rgba[0];
//                color->getRefRGBA()[1] = rgba[1];
//                color->getRefRGBA()[2] = rgba[2];
//                color->getRefRGBA()[3] = (float) index /(float) nbValues;
//            }
//
//            (*tfc)[pTf->getName()] = pTf;
//        }

        // If no TF in the uiTF Bundle's ressources, create a Basic TF
        if( paths.size() == 0 )
        {
            ::fwData::TransfertFunction::sptr  pTf = ::fwData::TransfertFunction::New();
            pTf->setCRefName( "Basic TF" );
            ::fwData::Color::sptr color;

            color = pTf->getColor( 0 );
            color->getRefRGBA()[0] = 0;
            color->getRefRGBA()[1] = 0;
            color->getRefRGBA()[2] = 0;
            color->getRefRGBA()[3] = 0;

            color = pTf->getColor( 100 );
            color->getRefRGBA()[0] = 1;
            color->getRefRGBA()[1] = 1;
            color->getRefRGBA()[2] = 1;
            color->getRefRGBA()[3] = 1;

            (*tfc)[pTf->getName()] = pTf;
        }
    }


    // Manage TF preset
    m_pTransferFunctionPreset->clear();

    for (   ::fwData::Composite::Container::iterator iterTF = tfc->begin();
            iterTF != tfc->end();
            ++iterTF )
    {
        m_pTransferFunctionPreset->addItem( iterTF->first.c_str() );
    }

    SLM_ASSERT("Sorry, selected TF does not exist in tfc", tfc->find( selectedTfId->value() ) !=  tfc->end() );
    int index = m_pTransferFunctionPreset->findText( QString( selectedTfId->value().c_str() ) );
    m_pTransferFunctionPreset->setCurrentIndex(index);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::notification()
{
    SLM_TRACE_FUNC();

    ::fwComEd::fieldHelper::MedicalImageHelpers::updateMinMaxFromTF( this->getObject< ::fwData::Image >() );

    ::fwComEd::ImageMsg::NewSptr imageModifiedMsg;
    imageModifiedMsg->addEvent(::fwComEd::ImageMsg::TRANSFERTFUNCTION) ;
    ::fwServices::IEditionService::notify( this->getSptr(),  this->getObject< ::fwData::Image >(), imageModifiedMsg );
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

bool TransferFunctionEditor::hasTransferFunctionName(const std::string & _sName)
{
    SLM_TRACE_FUNC();
    return m_tranferFunctionComposite.lock()->find(_sName) != m_tranferFunctionComposite.lock()->end();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

std::string TransferFunctionEditor::createTransferFunctionName(const std::string & _sBasename)
{
    SLM_TRACE_FUNC();

    bool bHasTransferFunctionName = true;
    std::string newName = _sBasename;
    int cpt = 1;
    while (bHasTransferFunctionName)
    {
        std::stringstream tmpStr;
        tmpStr <<  _sBasename <<  "_" <<  cpt;
        newName = tmpStr.str();
        bHasTransferFunctionName = hasTransferFunctionName(newName);
        cpt++;
    }

    return  newName;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void TransferFunctionEditor::updateImageWithSeletedTransferFunction()
{
    SLM_TRACE_FUNC();

    std::string str = m_pTransferFunctionPreset->currentText().toStdString();
    OSLM_DEBUG("Transfer function selected : " <<  str);
    m_selectedTranferFunctionId.lock()->value() = str;

    this->notification();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

} // end namespace
