/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SImage.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderVTK/vtk/fwVtkWindowLevelLookupTable.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/helper/TransferFunction.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkImageBlend.h>
#include <vtkImageCheckerboard.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImage);

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_OPACITY_SLOT = "updateImageOpacity";

const ::fwServices::IService::KeyType SImage::s_IMAGE_INOUT        = "image";
const ::fwServices::IService::KeyType SImage::s_TF_SELECTION_INOUT = "tfSelection";

//------------------------------------------------------------------------------

SImage::SImage() noexcept :
    m_imageRegister(nullptr),
    m_imagePortId(-1),
    m_imageOpacity(0.),
    m_allowAlphaInTF(false),
    m_lut(vtkSmartPointer<fwVtkWindowLevelLookupTable>::New()),
    m_map2colors(vtkSmartPointer<vtkImageMapToColors>::New()),
    m_imageData(vtkSmartPointer<vtkImageData>::New())
{
    this->installTFSlots(this);
    newSlot(s_UPDATE_IMAGE_OPACITY_SLOT, &SImage::updateImageOpacity, this);
}

//------------------------------------------------------------------------------

SImage::~SImage() noexcept
{
}

//------------------------------------------------------------------------------

void SImage::starting()
{
    this->initialize();

    ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite >(s_TF_SELECTION_INOUT);
    this->setTransferFunctionSelection(tfSelection);

    this->updating();
    this->installTFConnections();
}

//------------------------------------------------------------------------------

void SImage::stopping()
{
    this->removeTFConnections();
    this->destroyPipeline();
}

//------------------------------------------------------------------------------

void SImage::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        this->updateImage(image);
        this->buildPipeline();
        this->updateImageTransferFunction(image);
        this->updateWindowing();
        this->updateImageOpacity();
    }
    else
    {
        this->updateTransferFunction(image);
    }
    this->requestRender();
}

//------------------------------------------------------------------------------

void SImage::updatingTFPoints()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    this->updateImageTransferFunction(image);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SImage::updatingTFWindowing(double window, double level)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    this->setWindow(window);
    this->setLevel(level);
    this->updateWindowing();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SImage::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setVtkImageRegisterId( config.get<std::string>("vtkimageregister", ""));

    m_imageOpacity = config.get<double>("opacity", 0.);

    const std::string tfalpha = config.get<std::string>("tfalpha", "no");
    SLM_ASSERT("'tfalpha' value must be 'yes' or 'no', actual: " + tfalpha, tfalpha == "yes" || tfalpha == "no");
    this->setAllowAlphaInTF(tfalpha == "yes");

    this->setSelectedTFKey(config.get<std::string>("selectedTFKey", ""));
}

//------------------------------------------------------------------------------

void SImage::updateImage( ::fwData::Image::sptr image  )
{
    ::fwVtkIO::toVTKImage(image, m_imageData);

    this->updateImageInfos(image);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImage::updateWindowing()
{
    m_lut->SetWindow(this->getWindow());
    m_lut->SetLevel(this->getLevel());
    m_lut->Modified();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImage::updateImageTransferFunction( ::fwData::Image::sptr image )
{
    this->updateTransferFunction(image);
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    ::fwVtkIO::helper::TransferFunction::toVtkLookupTable( tf, m_lut, m_allowAlphaInTF, 256 );

    m_lut->SetClamping( !tf->getIsClamped() );

    this->updateWindowing();

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImage::updateImageOpacity()
{
    if (m_imagePortId >= 0)
    {
        ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();
        if(img->getField( "TRANSPARENCY" ) )
        {
            ::fwData::Integer::sptr transparency = img->getField< ::fwData::Integer >( "TRANSPARENCY" );
            m_imageOpacity                       = (100 - (*transparency) ) / 100.0;
        }
        if(img->getField( "VISIBILITY" ) )
        {
            ::fwData::Boolean::sptr visible = img->getField< ::fwData::Boolean >( "VISIBILITY" );
            m_imageOpacity                  = (*visible) ? m_imageOpacity : 0.0;
        }

        vtkImageBlend* imageBlend = vtkImageBlend::SafeDownCast(m_imageRegister);
        if(nullptr != imageBlend )
        {
            imageBlend->SetOpacity(m_imagePortId, m_imageOpacity);
            OSLM_TRACE(
                "vtkImageBlend " << this->m_imageRegisterId << " opacity :" << m_imagePortId << "," << m_imageOpacity );
        }

        this->setVtkPipelineModified();
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SImage::buildPipeline( )
{
    m_map2colors->SetInputData(m_imageData);
    m_map2colors->SetLookupTable(m_lut);
    m_map2colors->SetOutputFormatToRGBA();

    if (!m_imageRegisterId.empty())
    {
        m_imageRegister = this->getVtkObject(m_imageRegisterId);
    }

    vtkImageAlgorithm* algorithm       = vtkImageAlgorithm::SafeDownCast(m_imageRegister);
    vtkImageData* imageData            = vtkImageData::SafeDownCast(m_imageRegister);
    vtkImageBlend* imageBlend          = vtkImageBlend::SafeDownCast(m_imageRegister);
    vtkImageCheckerboard* imageChecker = vtkImageCheckerboard::SafeDownCast(m_imageRegister);

    SLM_ASSERT("Invalid vtk image register", algorithm||imageData||imageBlend||imageChecker );
    if (imageBlend)
    {
        SLM_TRACE("Register is a vtkImageBlend");
        if (m_imagePortId < 0)
        {
            m_imagePortId = imageBlend->GetNumberOfInputConnections(0);
            imageBlend->AddInputConnection(m_map2colors->GetOutputPort());
            OSLM_TRACE(this->getID() << ": Added image " << m_imagePortId << " on vtkImageBlend");
        }
    }
    else if (imageChecker)
    {
        SLM_TRACE("Register is a vtkImageCheckerboard");
        if (m_imagePortId < 0)
        {
            m_imagePortId = imageChecker->GetNumberOfInputConnections(0);
            imageChecker->SetInputConnection(m_imagePortId, m_map2colors->GetOutputPort());
            OSLM_TRACE(this->getID() << ": Added image " << m_imagePortId << " on vtkImageCheckerboard");
        }
    }
    else if (algorithm)
    {
        SLM_TRACE("Register is a vtkImageAlgorithm");
        algorithm->SetInputConnection(m_map2colors->GetOutputPort());
    }
    else if (imageData)
    {
        SLM_TRACE("Register is a vtkImageData");
        m_map2colors->SetOutput(imageData);
        m_map2colors->Update();
    }

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImage::destroyPipeline( )
{
    vtkImageAlgorithm* algorithm       = vtkImageAlgorithm::SafeDownCast(m_imageRegister);
    vtkImageData* imageData            = vtkImageData::SafeDownCast(m_imageRegister);
    vtkImageBlend* imageBlend          = vtkImageBlend::SafeDownCast(m_imageRegister);
    vtkImageCheckerboard* imageChecker = vtkImageCheckerboard::SafeDownCast(m_imageRegister);

    if (imageBlend)
    {
        if (m_imagePortId >= 0)
        {
            //Warning : only the removal of the last input connection in the image blend is safe.
            imageBlend->RemoveInputConnection(0, m_map2colors->GetOutputPort());
            m_imagePortId = -1;
        }
    }
    else if(imageChecker)
    {
        if (m_imagePortId >= 0)
        {
            //Warning : only the removal of the last input connection in the image blend is safe.
            imageChecker->RemoveInputConnection(0, m_map2colors->GetOutputPort());
            m_imagePortId = -1;
        }
    }
    else if (algorithm)
    {
        algorithm->RemoveInputConnection(0, m_map2colors->GetOutputPort());
    }
    else if (imageData)
    {
        m_map2colors->SetOutput(0);
    }

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImage::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
