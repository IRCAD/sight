/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/helpers.hpp>

#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwServices/macros.hpp>
#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/Color.hpp>
#include <fwData/String.hpp>

#include <vtkIO/vtk.hpp>

#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>

#include "visuVTKAdaptor/Image.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Image, ::fwData::Image ) ;

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

Image::Image() throw()
{
    SLM_TRACE_FUNC();
    m_lut        = vtkLookupTable::New();
    m_map2colors = vtkImageMapToColors::New();
    m_imageData  = vtkImageData::New();

    m_imageRegister = NULL;

    m_imagePortId = -1;
    m_allowAlphaInTF = false;
    m_useImageTF = true;

    // Manage events
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER            );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE         );
    addNewHandledEvent( ::fwComEd::ImageMsg::MODIFIED          );
    addNewHandledEvent( ::fwComEd::ImageMsg::TRANSFERTFUNCTION );
    addNewHandledEvent( ::fwComEd::ImageMsg::WINDOWING         );
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
    SLM_TRACE_FUNC();
    OSLM_TRACE("starting " << this->getName());

    this->doUpdate();
}

//------------------------------------------------------------------------------

void Image::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->destroyPipeline();
}

//------------------------------------------------------------------------------

void Image::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    doUpdate();
}

//------------------------------------------------------------------------------

void Image::doUpdate() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        updateImage(image);
        buildPipeline();
        updateTransfertFunction(image);
        updateWindowing(image);
        updateImageOpacity();
    }
}

//------------------------------------------------------------------------------

void Image::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {

        if ( msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( msg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
        {
//            this->destroyPipeline();
            doUpdate();

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

        if ( msg->hasEvent( ::fwComEd::ImageMsg::TRANSFERTFUNCTION ) )
        {
            updateTransfertFunction(image);
        }

        if ( msg->hasEvent( ::fwComEd::ImageMsg::WINDOWING ) )
        {
            ::fwComEd::ImageMsg::csptr imsg = ::fwComEd::ImageMsg::dynamicConstCast(msg);
            imsg->getWindowMinMax( m_windowMin, m_windowMax);
            updateWindowing(image);
        }

    }
}

//------------------------------------------------------------------------------

void Image::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

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
}

//------------------------------------------------------------------------------


void Image::updateImage( ::fwData::Image::sptr image  )
{
    SLM_TRACE_FUNC();
    ::vtkIO::toVTKImage(image,m_imageData);

    this->updateImageInfos(image);

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::updateWindowing( ::fwData::Image::sptr image )
{
    SLM_TRACE_FUNC();
    //std::pair<bool,bool> fieldsAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkMinMaxTF( image );
    // Temp test because theses cases are not manage ( need to notify if there are modifications of Min/Max/TF )
    //assert( ! fieldsAreModified.first && ! fieldsAreModified.second );

    m_lut->SetTableRange( m_windowMin->value(), m_windowMax->value() );
    m_lut->Modified();
    setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::updateTransfertFunction( ::fwData::Image::sptr image )
{
    SLM_TRACE_FUNC();
    ::fwData::Composite::sptr tfComposite = m_transfertFunctions;
    std::string tfName = m_transfertFunctionId->value();
    ::fwData::TransfertFunction::sptr pTransfertFunction = ::fwData::TransfertFunction::dynamicCast(tfComposite->getRefMap()[tfName]);
    if ( m_useImageTF )
    {
        ::vtkIO::convertTF2vtkTF( pTransfertFunction, m_lut, m_allowAlphaInTF );
    }
    else
    {
        ::vtkIO::convertTF2vtkTFBW( pTransfertFunction, m_lut);
    }
    setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image::updateImageOpacity()
{
    SLM_TRACE_FUNC();
    if (m_imagePortId>= 0)
    {
        vtkImageBlend *imageBlend = vtkImageBlend::SafeDownCast(m_imageRegister);
        imageBlend->SetOpacity(m_imagePortId, m_imageOpacity);
        OSLM_TRACE( "vtkImageBlend " << this->m_imageRegisterId << " opacity :" << m_imagePortId << "," << m_imageOpacity );
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

} //namespace visuVTKAdaptor
