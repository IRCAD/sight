/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/STextureSelector.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Array.hpp>
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwIO/ioTypes.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <uiIO/editor/SIOSelector.hpp>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiVisuOgre::STextureSelector, ::fwData::Reconstruction);

static const std::string s_RECONSTRUCTION_INOUT = "reconstruction";

//------------------------------------------------------------------------------

STextureSelector::STextureSelector() noexcept
{

}

//------------------------------------------------------------------------------

STextureSelector::~STextureSelector() noexcept
{
}

//------------------------------------------------------------------------------

void STextureSelector::starting()
{
    SLM_TRACE_FUNC();

    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    m_loadButton = new QPushButton(QString("Load texture"));
    m_loadButton->setToolTip(QString("Selected organ's texture"));
    m_loadButton->setMinimumSize(m_loadButton->sizeHint());

    m_deleteButton = new QPushButton(QString("Remove texture"));
    m_deleteButton->setToolTip(QString("Remove organ's texture"));
    m_deleteButton->setMinimumSize(m_deleteButton->sizeHint());

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_loadButton, 0 );
    layout->addWidget( m_deleteButton, 0 );

    qtContainer->setLayout( layout );
    qtContainer->setEnabled(true);

    QObject::connect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadButton()));
    QObject::connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButton()));

    this->updating();
}

//------------------------------------------------------------------------------

void STextureSelector::stopping()
{
    SLM_TRACE_FUNC();

    this->destroy();
}

//------------------------------------------------------------------------------

void STextureSelector::configuring()
{
    SLM_TRACE_FUNC();

    this->initialize();
}

//------------------------------------------------------------------------------

void STextureSelector::updating()
{
}

//------------------------------------------------------------------------------

void STextureSelector::onLoadButton()
{
    auto reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("No associated Reconstruction", reconstruction);

    ::fwData::Material::sptr material = reconstruction->getMaterial();
    ::fwData::Image::sptr image       = material->getDiffuseTexture();

    bool existingTexture = (image != nullptr);

    // We have to instantiate a new image if necessary
    if(!existingTexture)
    {
        image = ::fwData::Image::New();
        material->setDiffuseTexture(image);
    }

    auto srv = ::fwServices::add< ::fwGui::editor::IDialogEditor >("::uiIO::editor::SIOSelector");
    srv->registerInOut(image, ::fwIO::s_DATA_KEY);
    auto ioSelectorSrv = ::uiIO::editor::SIOSelector::dynamicCast(srv);
    if (ioSelectorSrv != nullptr)
    {
        ioSelectorSrv->setIOMode(::uiIO::editor::SIOSelector::READER_MODE);
        ioSelectorSrv->configure();
        ioSelectorSrv->start();
        ioSelectorSrv->update();
        ioSelectorSrv->stop();
        ::fwServices::OSR::unregisterService( ioSelectorSrv );

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
}

//------------------------------------------------------------------------------

void STextureSelector::onDeleteButton()
{
    auto reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("No associated Reconstruction", reconstruction);

    ::fwData::Material::sptr material = reconstruction->getMaterial();
    ::fwData::Image::sptr image       = material->getDiffuseTexture();

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
