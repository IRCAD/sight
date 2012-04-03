/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/IEditionService.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/TransferFunctionMsg.hpp>

#include <fwServices/macros.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>
#include <fwData/TransferFunction.hpp>

#include <vtkIO/vtk.hpp>
#include <vtkIO/helper/TransfertFunction.hpp>

#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>

#include <fwRenderVTK/vtk/fwVtkWindowLevelLookupTable.hpp>

#include "visuVTKAdaptor/Image.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Image, ::fwData::Image ) ;

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

Image::Image() throw()
{
    SLM_TRACE_FUNC();
    m_lut        = fwVtkWindowLevelLookupTable::New();
    m_map2colors = vtkImageMapToColors::New();
    m_imageData  = vtkImageData::New();

    m_imageRegister = NULL;

    m_imagePortId = -1;
    m_allowAlphaInTF = false;

    // Manage events
    this->installTFSelectionEventHandler(this);
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER                     );
    addNewHandledEvent( ::fwComEd::ImageMsg::MODIFIED                   );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE                  );
    addNewHandledEvent( ::fwComEd::ImageMsg::TRANSPARENCY               );
    addNewHandledEvent( ::fwComEd::ImageMsg::VISIBILITY                 );
    addNewHandledEvent( ::fwComEd::TransferFunctionMsg::MODIFIED_POINTS );
    addNewHandledEvent( ::fwComEd::TransferFunctionMsg::WINDOWING       );
}

//------------------------------------------------------------------------------

Image::~Image() throw()
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

void Image::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
    this->installTFObserver( this->getSptr() );
}

//------------------------------------------------------------------------------

void Image::doStop() throw(fwTools::Failed)
{
    this->removeTFObserver();
    this->destroyPipeline();
}

//------------------------------------------------------------------------------

void Image::doSwap() throw(fwTools::Failed)
{
    this->removeTFObserver();
    this->doUpdate();
    this->installTFObserver( this->getSptr() );
}

//------------------------------------------------------------------------------

void Image::doUpdate() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        this->updateImage(image);
        this->buildPipeline();
        this->updateImageTransferFunction(image);
        this->updateWindowing(image);
        this->updateImageOpacity();
    }
    else
    {
        this->updateTransferFunction(image, this->getSptr());
    }
}

//------------------------------------------------------------------------------

void Image::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        if ( msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( msg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
        {
            this->doUpdate();

            // Hack to force imageSlice update until it is not able to detect a new image
            ::fwComEd::ImageMsg::NewSptr msg;
            msg->setSliceIndex(m_axialIndex, m_frontalIndex, m_sagittalIndex);
            ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
        }

        if ( msg->hasEvent( ::fwComEd::ImageMsg::MODIFIED ) )
        {
            m_imageData->Modified();

            this->setVtkPipelineModified();
        }

        if (this->upadteTFObserver(msg, this->getSptr()) || msg->hasEvent( ::fwComEd::TransferFunctionMsg::MODIFIED_POINTS ) )
        {
            this->updateImageTransferFunction(image);
        }

        if ( msg->hasEvent( ::fwComEd::TransferFunctionMsg::WINDOWING ) )
        {
            ::fwComEd::TransferFunctionMsg::csptr tfmsg = ::fwComEd::TransferFunctionMsg::dynamicConstCast(msg);
            this->setWindow(tfmsg->getWindow());
            this->setLevel(tfmsg->getLevel());
            updateWindowing(image);
        }

        if ( msg->hasEvent( ::fwComEd::ImageMsg::TRANSPARENCY ) || msg->hasEvent( ::fwComEd::ImageMsg::VISIBILITY ) )
        {
            this->updateImageOpacity();
        }
    }
    else
    {
        this->destroyPipeline();
    }
}

//------------------------------------------------------------------------------

void Image::configuring() throw(fwTools::Failed)
{
    assert(m_configuration->getName() == "config");
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
    ::vtkIO::toVTKImage(image,m_imageData);

    this->updateImageInfos(image);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::updateWindowing( ::fwData::Image::sptr image )
{
    m_lut->SetWindow(this->getWindow());
    m_lut->SetLevel(this->getLevel());
    m_lut->Modified();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::updateImageTransferFunction( ::fwData::Image::sptr image )
{
    this->updateTransferFunction(image, this->getSptr());
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    ::vtkIO::helper::TransfertFunction::toVtkLookupTable( tf, m_lut, m_allowAlphaInTF, 256 );

    m_lut->SetClamping( !tf->getIsClamped() );

    this->setWindow(tf->getWindow());
    this->setLevel(tf->getLevel());

    this->updateWindowing(image);

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::updateImageOpacity()
{
    if (m_imagePortId>= 0)
    {
        ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();
        if(img->getField( "TRANSPARENCY" ) )
        {
            ::fwData::Integer::sptr transparency = img->getField< ::fwData::Integer >( "TRANSPARENCY" );
            m_imageOpacity = (100 - (*transparency) ) / 100.0 ;
        }
        if(img->getField( "VISIBILITY" ) )
        {
            ::fwData::Boolean::sptr visible = img->getField< ::fwData::Boolean >( "VISIBILITY" );
            m_imageOpacity = (*visible)?m_imageOpacity:0.0;
        }
        vtkImageBlend *imageBlend = vtkImageBlend::SafeDownCast(m_imageRegister);
        imageBlend->SetOpacity(m_imagePortId, m_imageOpacity);
        OSLM_TRACE( "vtkImageBlend " << this->m_imageRegisterId << " opacity :" << m_imagePortId << "," << m_imageOpacity );
        this->setVtkPipelineModified();
    }
}

//------------------------------------------------------------------------------

void Image::buildPipeline( )
{
    SLM_TRACE_FUNC();
    m_map2colors->SetInput(m_imageData);
    m_map2colors->SetLookupTable(m_lut);
    m_map2colors->SetOutputFormatToRGBA();

    if (!m_imageRegisterId.empty())
    {
        m_imageRegister = this->getVtkObject(m_imageRegisterId);
    }

    vtkImageAlgorithm *algorithm  = vtkImageAlgorithm::SafeDownCast(m_imageRegister);
    vtkImageData      *imageData  = vtkImageData::SafeDownCast(m_imageRegister);
    vtkImageBlend     *imageBlend = vtkImageBlend::SafeDownCast(m_imageRegister);

    SLM_ASSERT("Invalid vtk image register", algorithm||imageData||imageBlend );
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
    else if (algorithm)
    {
        SLM_TRACE("Register is a vtkImageAlgorithm");
        algorithm->SetInputConnection(m_map2colors->GetOutputPort());
    }
    else if (imageData)
    {
        SLM_TRACE("Register is a vtkImageData");
        m_map2colors->SetOutput(imageData);
    }

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::destroyPipeline( )
{
    vtkImageAlgorithm *algorithm  = vtkImageAlgorithm::SafeDownCast(m_imageRegister);
    vtkImageData      *imageData  = vtkImageData::SafeDownCast(m_imageRegister);
    vtkImageBlend     *imageBlend = vtkImageBlend::SafeDownCast(m_imageRegister);

    if (imageBlend)
    {
        if (m_imagePortId >= 0)
        {
            //Warning : only the removal of the last input connection in the image blend is safe.
            imageBlend->RemoveInputConnection(0, m_map2colors->GetOutputPort());
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


} //namespace visuVTKAdaptor
