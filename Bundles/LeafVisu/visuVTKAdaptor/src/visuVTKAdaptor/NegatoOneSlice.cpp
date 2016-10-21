/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/NegatoOneSlice.hpp"

#include "visuVTKAdaptor/Image.hpp"
#include "visuVTKAdaptor/ImageSlice.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>


fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::NegatoOneSlice, ::fwData::Image );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT = "updateSliceType";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT      = "updateImage";

//------------------------------------------------------------------------------

NegatoOneSlice::NegatoOneSlice() throw() :
    m_manageImageSource(false),
    m_imageSource(nullptr),
    m_allowAlphaInTF(false),
    m_interpolation(true),
    m_actorOpacity (1.0)
{
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &NegatoOneSlice::updateSliceType, this);
    newSlot(s_UPDATE_IMAGE_SLOT, &NegatoOneSlice::updateImage, this);
}

//------------------------------------------------------------------------------

NegatoOneSlice::~NegatoOneSlice() throw()
{
    this->unregisterServices();
    this->cleanImageSource();
}

//------------------------------------------------------------------------------

vtkObject* NegatoOneSlice::getImageSource()
{
    if ( !m_imageSource )
    {
        OSLM_TRACE(this->getID() << ": Create ImageSource");
        if (!m_imageSourceId.empty())
        {
            m_imageSource = this->getVtkObject(m_imageSourceId);
        }
        else
        {
            m_imageSource       = vtkImageMapToColors::New();
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
        m_imageSource = nullptr;
    }
}

//------------------------------------------------------------------------------

::fwRenderVTK::IVtkAdaptorService::sptr NegatoOneSlice::getImageSliceAdaptor()
{
    ::fwRenderVTK::IVtkAdaptorService::sptr adaptor;

    if (m_imageSliceAdaptor.expired())
    {
        OSLM_TRACE(this->getID() << ": Create ImageSlice Adaptor Service");
        ::fwData::Image::sptr image;
        ::fwData::Composite::sptr sceneComposite;

        image          = this->getObject< ::fwData::Image >();
        sceneComposite = this->getRenderService()->getComposite();

        adaptor = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >(
            sceneComposite,
            "::visuVTKAdaptor::ImageSlice");
        adaptor->setRenderService(this->getRenderService());
        adaptor->setRenderId( this->getRenderId() );
        adaptor->setPickerId( this->getPickerId() );
        adaptor->setTransformId( this->getTransformId() );
        adaptor->setAutoRender( this->getAutoRender() );

        ::visuVTKAdaptor::ImageSlice::sptr imgSliceAdaptor;
        imgSliceAdaptor = ::visuVTKAdaptor::ImageSlice::dynamicCast(adaptor);
        imgSliceAdaptor->setVtkImageSource(this->getImageSource());
        imgSliceAdaptor->setCtrlImage(image);
        imgSliceAdaptor->setInterpolation(m_interpolation);
        imgSliceAdaptor->setActorOpacity(m_actorOpacity);
        imgSliceAdaptor->setOrientation((Orientation) m_orientation);

        m_imageSliceAdaptor = adaptor;
        this->registerService(adaptor);
    }
    else
    {
        adaptor = m_imageSliceAdaptor.lock();
    }
    return adaptor;
}

//------------------------------------------------------------------------------

::fwRenderVTK::IVtkAdaptorService::sptr NegatoOneSlice::getImageAdaptor()
{
    ::fwRenderVTK::IVtkAdaptorService::sptr adaptor;

    if (m_imageAdaptor.expired())
    {
        OSLM_TRACE(this->getID() << ": Create Image Adaptor Service");
        ::fwData::Image::sptr image;
        image   = this->getObject< ::fwData::Image >();
        adaptor = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >(
            image,
            "::visuVTKAdaptor::Image");
        adaptor->setRenderService(this->getRenderService());
        adaptor->setRenderId( this->getRenderId() );
        adaptor->setPickerId( this->getPickerId() );
        adaptor->setTransformId( this->getTransformId() );
        adaptor->setAutoRender( this->getAutoRender() );

        ::visuVTKAdaptor::Image::sptr imgAdaptor;
        imgAdaptor = ::visuVTKAdaptor::Image::dynamicCast(adaptor);
        imgAdaptor->setVtkImageRegister(this->getImageSource());
        imgAdaptor->setSelectedTFKey( this->getSelectedTFKey() );
        imgAdaptor->setTFSelectionFwID( this->getTFSelectionFwID() );

        imgAdaptor->setImageOpacity(1.);
        imgAdaptor->setAllowAlphaInTF(m_allowAlphaInTF);

        m_imageAdaptor = adaptor;
        this->registerService(adaptor);
    }
    else
    {
        adaptor = m_imageAdaptor.lock();
    }
    return adaptor;
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if (!vtkImageBlend::SafeDownCast(this->getImageSource()))
    {
        this->getImageAdaptor()->start();
    }
    this->getImageSliceAdaptor()->start();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
    this->cleanImageSource();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doSwap() throw(fwTools::Failed)
{
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doUpdate() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if (!vtkImageBlend::SafeDownCast(this->getImageSource()))
    {
        this->getImageAdaptor()->update();
    }
    this->getImageSliceAdaptor()->update();
}

//-----------------------------------------------------------------------------

void NegatoOneSlice::updateSliceType(int from, int to)
{
    if( to == static_cast<int>(m_orientation) )
    {
        setOrientation( static_cast< Orientation >( from ));
    }
    else if(from == static_cast<int>(m_orientation))
    {
        setOrientation( static_cast< Orientation >( to ));
    }
}

//-----------------------------------------------------------------------------

void NegatoOneSlice::updateImage()
{
    this->doStop();
    this->doStart();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void NegatoOneSlice::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT("Configuration must begin with <config>", m_configuration->getName() == "config");
    if(m_configuration->hasAttribute("sliceIndex"))
    {
        std::string orientation = m_configuration->getAttributeValue("sliceIndex");
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
    if(m_configuration->hasAttribute("actorOpacity") )
    {
        m_actorOpacity = ::boost::lexical_cast<double>(m_configuration->getAttributeValue("actorOpacity"));
    }

    this->parseTFConfig( m_configuration );
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType NegatoOneSlice::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------


} //namespace visuVTKAdaptor
