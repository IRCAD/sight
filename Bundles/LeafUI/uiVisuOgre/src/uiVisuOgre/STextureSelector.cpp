/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/STextureSelector.hpp"

#include <fwCom/Signal.hxx>
#include <fwComEd/helper/Array.hpp>

#include <fwData/Array.hpp>
#include <fwData/Material.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Image.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/ImageReader.hpp>
#include <fwVtkIO/VtiImageReader.hpp>

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QFileDialog>

#include <boost/algorithm/string.hpp>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuOgre::STextureSelector, ::fwData::Reconstruction);

//------------------------------------------------------------------------------

STextureSelector::STextureSelector() throw()
{

}

//------------------------------------------------------------------------------

STextureSelector::~STextureSelector() throw()
{
}

//------------------------------------------------------------------------------

void STextureSelector::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    m_loadButton = new QPushButton(QString("Load texture"), container);
    m_loadButton->setToolTip(QString("Selected organ's texture"));
    m_loadButton->setMinimumSize (m_loadButton->sizeHint());

    m_deleteButton = new QPushButton(QString("Remove texture"), container);
    m_deleteButton->setToolTip(QString("Remove organ's texture"));
    m_deleteButton->setMinimumSize (m_deleteButton->sizeHint());

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_loadButton, 0 );
    layout->addWidget( m_deleteButton, 0 );

    container->setLayout( layout );
    container->setEnabled(true);

    QObject::connect(m_loadButton, SIGNAL(clicked ()), this, SLOT(onLoadButton( )));
    QObject::connect(m_deleteButton, SIGNAL(clicked ()), this, SLOT(onDeleteButton( )));

    this->updating();
}

//------------------------------------------------------------------------------

void STextureSelector::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_loadButton, SIGNAL(clicked ()), this, SLOT(onLoadButton( )));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void STextureSelector::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->initialize();
}

//------------------------------------------------------------------------------

void STextureSelector::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void STextureSelector::onLoadButton()
{
    std::string textureName = QFileDialog::getOpenFileName(0,
                                                           QString("Open texture"), "../../data/",
                                                           QString("VTK texture (*.vti)")).toStdString();

    if(textureName.empty())
    {
        return;
    }

    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();
    SLM_ASSERT("No associated Reconstruction", reconstruction);

    ::boost::filesystem::path imgFile (textureName);

    ::fwData::Material::sptr material = reconstruction->getMaterial();
    ::fwData::Image::sptr image       = material->getDiffuseTexture();
    // Test if the material is already textured
    bool existingTexture = (image != nullptr);

    // Use a reader of fwVtkIO library to read an image
    ::fwDataIO::reader::IObjectReader::sptr myReader;

    // Create a progress bar and attach it to reader
    //::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Image ");
    std::string ext = ::boost::filesystem::extension(imgFile);
    ::boost::algorithm::to_lower(ext);

    if(ext == ".vti")
    {
        ::fwVtkIO::VtiImageReader::sptr vtiReader = ::fwVtkIO::VtiImageReader::New();
        vtiReader->setFile(imgFile);
        myReader = vtiReader;
    }
    else
    {
        OSLM_ERROR("Only .vti is supported");
    }

    // We have to instanciate a new image if necessary
    if(!existingTexture)
    {
        image = ::fwData::Image::New();
        material->setDiffuseTexture(image);
    }

    // Set the image (already created, but empty) that will be modified
    myReader->setObject(image);

    try
    {
        // Launch reading process
        myReader->read();

        // If we didn't have to create a new texture, we can notify the associated image
        if(existingTexture)
        {
            auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            sig->emit();
        }
        else
        {
            auto sig = material->signal< ::fwData::Material::AddedTextureSignalType >(
                ::fwData::Material::s_ADDED_TEXTURE_SIG);
            sig->emit(image);
        }
    }
    catch (const std::exception & e)
    {
        OSLM_ERROR("Failed to load texture : "<<e.what());
    }
    catch( ... )
    {
        OSLM_ERROR("Failed to load texture");
    }
}

//------------------------------------------------------------------------------

void STextureSelector::onDeleteButton()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();
    ::fwData::Material::sptr material             = reconstruction->getMaterial();
    ::fwData::Image::sptr image                   = material->getDiffuseTexture();

    if(image)
    {
        material->setDiffuseTexture(::fwData::Image::sptr());
        auto sig = material->signal< ::fwData::Material::RemovedTextureSignalType >(
            ::fwData::Material::s_REMOVED_TEXTURE_SIG);
        sig->emit(image);

        image->getDataArray()->clear();
        auto imgSig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        imgSig->emit();
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
