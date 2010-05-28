/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/helpers.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwServices/Base.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/Color.hpp>
#include <fwData/String.hpp>

#include <vtkIO/vtk.hpp>

#include <vtkImageData.h>

#include "visuVTKAdaptor/Image.hpp"
#include "visuVTKAdaptor/ImageSlice.hpp"
#include "visuVTKAdaptor/NegatoOneSlice.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::NegatoOneSlice, ::fwData::Image ) ;

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

NegatoOneSlice::NegatoOneSlice() throw()
{
    SLM_TRACE_FUNC();
    m_allowAlphaInTF = false;
    m_interpolation  = true;
    m_manageImageSource = false;

    m_imageSource = NULL;

    // Manage events
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER            );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE         );
    addNewHandledEvent( ::fwComEd::ImageMsg::MODIFIED          );
}

//------------------------------------------------------------------------------

NegatoOneSlice::~NegatoOneSlice() throw()
{
    SLM_TRACE_FUNC();
    this->unregisterServices();
    this->cleanImageSource();
}

//------------------------------------------------------------------------------
vtkObject* NegatoOneSlice::getImageSource()
{
    if ( !m_imageSource )
    {
        OSLM_TRACE(this->getUUID() << ": Create ImageSource");
        if (!m_imageSourceId.empty())
        {
            m_imageSource = this->getVtkObject(m_imageSourceId);
        }
        else
        {
            m_imageSource = vtkImageData::New();
            m_manageImageSource = true;
        }
    }

    return m_imageSource;
}

//------------------------------------------------------------------------------

void NegatoOneSlice::cleanImageSource()
{
    if (m_manageImageSource && m_imageSource)
    {
        m_imageSource->Delete();
        m_imageSource = NULL;
    }
}

//------------------------------------------------------------------------------

::fwRenderVTK::IVtkAdaptorService::sptr NegatoOneSlice::getImageSliceAdaptor()
{
    ::fwRenderVTK::IVtkAdaptorService::sptr imageSliceAdaptor;

    if (m_imageSliceAdaptor.expired())
    {
        OSLM_TRACE(this->getUUID() << ": Create ImageSlice Adaptor Service");
        ::fwData::Image::sptr image;
        ::fwData::Composite::sptr sceneComposite;

        image          = this->getObject< ::fwData::Image >();
        sceneComposite = this->getRenderService()->getObject< ::fwData::Composite >();

        imageSliceAdaptor = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >(
                sceneComposite,
                "::visuVTKAdaptor::ImageSlice"
                );
        imageSliceAdaptor->setRenderService(this->getRenderService());
        imageSliceAdaptor->setRenderId( this->getRenderId() );
        imageSliceAdaptor->setPickerId( this->getPickerId() );
        imageSliceAdaptor->setTransformId( this->getTransformId() );

        ::visuVTKAdaptor::ImageSlice::sptr ISA;
        ISA = ::visuVTKAdaptor::ImageSlice::dynamicCast(imageSliceAdaptor);
        ISA->setVtkImageSource(this->getImageSource());
        ISA->setCtrlImage(image);
        ISA->setInterpolation(m_interpolation);

       ::fwComEd::helper::MedicalImageAdaptor::dynamicCast(ISA)->setOrientation((Orientation) m_orientation);

        m_imageSliceAdaptor = imageSliceAdaptor;
        this->registerService(imageSliceAdaptor);
    }
    else
    {
        imageSliceAdaptor = m_imageSliceAdaptor.lock();
    }
    return imageSliceAdaptor;
}

//------------------------------------------------------------------------------
::fwRenderVTK::IVtkAdaptorService::sptr NegatoOneSlice::getImageAdaptor()
{
    ::fwRenderVTK::IVtkAdaptorService::sptr imageAdaptor;

    if (m_imageAdaptor.expired())
    {
        OSLM_TRACE(this->getUUID() << ": Create Image Adaptor Service");
        ::fwData::Image::sptr image;
        image = this->getObject< ::fwData::Image >();
        imageAdaptor = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >(
                image,
                "::visuVTKAdaptor::Image"
                );
        imageAdaptor->setRenderService(this->getRenderService());
        imageAdaptor->setRenderId( this->getRenderId() );
        imageAdaptor->setPickerId( this->getPickerId() );
        imageAdaptor->setTransformId( this->getTransformId() );


        ::visuVTKAdaptor::Image::sptr IA;
        IA = ::visuVTKAdaptor::Image::dynamicCast(imageAdaptor);
        IA->setVtkImageRegister(this->getImageSource());
        IA->setImageOpacity(1.);
        IA->setAllowAlphaInTF(m_allowAlphaInTF);

        m_imageAdaptor = imageAdaptor;
        this->registerService(imageAdaptor);
    }
    else
    {
        imageAdaptor = m_imageAdaptor.lock();
    }
    return imageAdaptor;
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->getImageAdaptor()->start();
    this->getImageSliceAdaptor()->start();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->getImageAdaptor()->stop();
    this->getImageSliceAdaptor()->stop();
    this->unregisterServices();
    this->cleanImageSource();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doUpdate() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->getImageAdaptor()->update();
    this->getImageSliceAdaptor()->update();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doStop();
    this->doStart();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    if(m_configuration->hasAttribute("sliceIndex"))
    {
         std::string  orientation = m_configuration->getAttributeValue("sliceIndex");
         if(orientation == "axial" )
         {
             m_orientation = Z_AXIS;
         }
         else if(orientation == "frontal" )
         {
             m_orientation = Y_AXIS;
         }
         else if(orientation == "sagittal" )
         {
             m_orientation = X_AXIS;
         }
    }
    if(m_configuration->hasAttribute("transform") )
    {
        this->setTransformId( m_configuration->getAttributeValue("transform") );
    }
    if(m_configuration->hasAttribute("tfalpha") )
    {
        this->setAllowAlphaInTF(m_configuration->getAttributeValue("tfalpha") == "yes");
    }
    if (m_configuration->hasAttribute("interpolation"))
    {
        this->setInterpolation(!(m_configuration->getAttributeValue("interpolation") == "off"));
    }
    if (m_configuration->hasAttribute("vtkimagesource"))
    {
        this->setVtkImageSourceId( m_configuration->getAttributeValue("vtkimagesource") );
    }
}


//------------------------------------------------------------------------------


} //namespace visuVTKAdaptor
