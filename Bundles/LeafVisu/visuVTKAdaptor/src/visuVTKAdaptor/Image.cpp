/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Image.hpp"

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

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Image, ::fwData::Image );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_OPACITY_SLOT = "updateImageOpacity";

//------------------------------------------------------------------------------

Image::Image() noexcept :
    m_imageRegister(nullptr),
    m_imagePortId(-1),
    m_imageOpacity(0.),
    m_allowAlphaInTF(false),
    m_lut(fwVtkWindowLevelLookupTable::New()),
    m_map2colors(vtkImageMapToColors::New()),
    m_imageData(vtkImageData::New())
{
    this->installTFSlots(this);
    newSlot(s_UPDATE_IMAGE_OPACITY_SLOT, &Image::updateImageOpacity, this);
}

//------------------------------------------------------------------------------

Image::~Image() noexcept
{
    SLM_TRACE_FUNC();
    m_lut->Delete();
    m_lut = NULL;

    m_map2colors->Delete();
    m_map2colors = NULL;

    m_imageData->Delete();
    m_imageData = NULL;
}

//------------------------------------------------------------------------------

void Image::doStart()
{
    ::fwData::Composite::wptr tfSelection = this->getSafeInOut< ::fwData::Composite>(this->getTFSelectionFwID());
    this->setTransferFunctionSelection(tfSelection);

    this->doUpdate();
    this->installTFConnections();
}

//------------------------------------------------------------------------------

void Image::doStop()
{
    this->removeTFConnections();
    this->destroyPipeline();
}

//------------------------------------------------------------------------------

void Image::doSwap()
{
    this->removeTFConnections();
    this->doUpdate();
    this->installTFConnections();
}

//------------------------------------------------------------------------------

void Image::doUpdate()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
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
}

//------------------------------------------------------------------------------

void Image::updatingTFPoints()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageTransferFunction(image);
    this->requestRender();
}

//------------------------------------------------------------------------------

void Image::updatingTFWindowing(double window, double level)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->setWindow(window);
    this->setLevel(level);
    this->updateWindowing();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Image::doConfigure()
{
    SLM_ASSERT("Configuration must begin with <config>", m_configuration->getName() == "config");
    if(m_configuration->hasAttribute("vtkimageregister") )
    {
        this->setVtkImageRegisterId( m_configuration->getAttributeValue("vtkimageregister") );
    }
    if(m_configuration->hasAttribute("opacity") )
    {
        this->setImageOpacity(::boost::lexical_cast<double>(m_configuration->getAttributeValue("opacity")));
    }
    if(m_configuration->hasAttribute("tfalpha") )
    {
        this->setAllowAlphaInTF(m_configuration->getAttributeValue("tfalpha") == "yes");
    }

    this->parseTFConfig( m_configuration );
}

//------------------------------------------------------------------------------

void Image::updateImage( ::fwData::Image::sptr image  )
{
    ::fwVtkIO::toVTKImage(image, m_imageData);

    this->updateImageInfos(image);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::updateWindowing()
{
    m_lut->SetWindow(this->getWindow());
    m_lut->SetLevel(this->getLevel());
    m_lut->Modified();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::updateImageTransferFunction( ::fwData::Image::sptr image )
{
    this->updateTransferFunction(image);
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    ::fwVtkIO::helper::TransferFunction::toVtkLookupTable( tf, m_lut, m_allowAlphaInTF, 256 );

    m_lut->SetClamping( !tf->getIsClamped() );

    this->updateWindowing();

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::updateImageOpacity()
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

void Image::buildPipeline( )
{
    SLM_TRACE_FUNC();
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

void Image::destroyPipeline( )
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

::fwServices::IService::KeyConnectionsType Image::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair(::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT));
    connections.push_back( std::make_pair(::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT));
    connections.push_back( std::make_pair(::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT));
    connections.push_back( std::make_pair(::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT));

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
