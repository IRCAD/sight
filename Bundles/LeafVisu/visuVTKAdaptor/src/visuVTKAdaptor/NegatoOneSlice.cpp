/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Contributors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#include <fwTools/helpers.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwServices/Base.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/Color.hpp>
#include <fwData/String.hpp>
#include <fwData/Float.hpp>

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
    ::fwRuntime::ConfigurationElement::sptr cfg;
    //To be managed by ::fwRenderVTK::VtkRenderService
    if(m_configuration->getName() == "config")
    {
        cfg = m_configuration;
    }
    //When directly declared as an image service
    else if( m_configuration->findConfigurationElement("config") )
    {
        cfg = m_configuration->findConfigurationElement("config") ;
    }
    else
    {
        assert(false);
    }

    if( cfg->hasAttribute("scene") )
    {
        OSLM_TRACE("m_configuration->hasAttributeValue scene: true");
        this->setSceneId(cfg->getAttributeValue("scene"));
    }
    else
    {
        OSLM_TRACE("m_configuration->hasAttributeValue scene: false");
    }

    this->setRenderId( cfg->getAttributeValue("renderer") );
    this->setPickerId( cfg->getAttributeValue("picker") );
    if(cfg->hasAttribute("sliceIndex"))
    {
         std::string  orientation = cfg->getAttributeValue("sliceIndex");
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

    if( cfg->hasAttribute("transform") )
    {
        this->setTransformId( cfg->getAttributeValue("transform") );
    }
    if(cfg->hasAttribute("tfalpha") )
    {
        this->setAllowAlphaInTF(cfg->getAttributeValue("tfalpha") == "yes");
    }
    if (cfg->hasAttribute("interpolation"))
    {
        this->setInterpolation(!(cfg->getAttributeValue("interpolation") == "off"));
    }
    if (cfg->hasAttribute("vtkimagesource"))
    {
        this->setVtkImageSourceId( cfg->getAttributeValue("vtkimagesource") );
    }
}


//------------------------------------------------------------------------------


} //namespace visuVTKAdaptor
