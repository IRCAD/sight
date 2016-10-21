/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/NegatoMPR.hpp"

#include "visuVTKAdaptor/NegatoOneSlice.hpp"
#include "visuVTKAdaptor/NegatoSlicingInteractor.hpp"
#include "visuVTKAdaptor/NegatoWindowingInteractor.hpp"
#include "visuVTKAdaptor/SlicesCursor.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <fwTools/fwID.hpp>

#include <string>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::NegatoMPR, ::fwData::Image );

namespace visuVTKAdaptor
{
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT = "updateSliceType";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_MODE_SLOT = "updateSliceMode";
static const ::fwCom::Slots::SlotKeyType s_SHOW_SLICE_SLOT        = "showSlice";
static const ::fwCom::Slots::SlotKeyType s_SET_CROSS_SCALE_SLOT   = "setCrossScale";

static const std::string s_slicingStartingProxy = "slicingStartingProxy";
static const std::string s_slicingStoppingProxy = "slicingStoppingProxy";

//------------------------------------------------------------------------------

NegatoMPR::NegatoMPR() throw() :
    m_allowAlphaInTF(false),
    m_interpolation(false),
    m_actorOpacity(1.0),
    m_3dModeEnabled ( ::boost::logic::indeterminate ),
    m_sliceMode(THREE_SLICES),
    m_backupedSliceMode(THREE_SLICES)
{
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &NegatoMPR::updateSliceType, this);
    newSlot(s_UPDATE_SLICE_MODE_SLOT, &NegatoMPR::updateSliceMode, this);
    newSlot(s_SHOW_SLICE_SLOT, &NegatoMPR::showSlice, this);
    newSlot(s_SET_CROSS_SCALE_SLOT, &NegatoMPR::setCrossScale, this);
}

//------------------------------------------------------------------------------

NegatoMPR::~NegatoMPR() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void NegatoMPR::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void NegatoMPR::doStop() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    //disconnect proxy
    ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

    for (auto srv : this->getRegisteredServices())
    {
        NegatoSlicingInteractor::sptr negatoSlicingInteractor = NegatoSlicingInteractor::dynamicCast(srv.lock());
        SlicesCursor::sptr sliceCursor                        = SlicesCursor::dynamicCast(srv.lock());
        if (negatoSlicingInteractor)
        {
            proxy->disconnect(m_slicingStartingProxy, negatoSlicingInteractor->signal(
                                  NegatoSlicingInteractor::s_SLICING_STARTED_SIG));
            proxy->disconnect(m_slicingStoppingProxy, negatoSlicingInteractor->signal(
                                  NegatoSlicingInteractor::s_SLICING_STOPPED_SIG));
        }

        if (sliceCursor)
        {
            proxy->disconnect(m_slicingStartingProxy, sliceCursor->slot(
                                  SlicesCursor::s_SHOW_FULL_CROSS_SLOT));


            proxy->disconnect(m_slicingStoppingProxy, sliceCursor->slot(
                                  SlicesCursor::s_SHOW_NORMAL_CROSS_SLOT));
        }
    }

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void NegatoMPR::doSwap() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if ( imageIsValid)
    {
        if(m_subServices.empty())
        {
            doStart();
        }
        else
        {
            for( ServiceVector::value_type service :  m_subServices)
            {
                OSLM_ASSERT("sub services expired in service : " << this->getSptr()->getID(), !service.expired());
                service.lock()->swap(image);
                service.lock()->update();
            }
        }
    }
    else
    {
        doStop();
    }
}

//------------------------------------------------------------------------------

void NegatoMPR::doUpdate() throw(::fwTools::Failed)
{
    this->doStop();

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if ( imageIsValid)
    {
        if(this->getSliceMode() != NO_SLICE)
        {
            if(this->getSliceMode() == ONE_SLICE)
            {
                this->addAdaptor("::visuVTKAdaptor::NegatoOneSlice", m_orientation);
            }
            else if(this->getSliceMode() == THREE_SLICES)
            {
                this->addAdaptor("::visuVTKAdaptor::NegatoOneSlice", X_AXIS);
                this->addAdaptor("::visuVTKAdaptor::NegatoOneSlice", Y_AXIS);
                this->addAdaptor("::visuVTKAdaptor::NegatoOneSlice", Z_AXIS);
            }

            ::fwRenderVTK::IVtkAdaptorService::sptr sliceCursor;
            ::fwRenderVTK::IVtkAdaptorService::sptr negatoSlicingInteractor;
            this->addAdaptor("::visuVTKAdaptor::NegatoWindowingInteractor");
            negatoSlicingInteractor = this->addAdaptor("::visuVTKAdaptor::NegatoSlicingInteractor", m_orientation);
            sliceCursor             = this->addAdaptor("::visuVTKAdaptor::SlicesCursor", m_orientation);
            this->addAdaptor("::visuVTKAdaptor::ProbeCursor", m_orientation);

            /// Connect slicing signals/slots from NegatoSlicingInteractor to SlicesCursor using the image slicing proxy
            ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();
            m_slicingStartingProxy                    = image->getID() + s_slicingStartingProxy;
            m_slicingStoppingProxy                    = image->getID() + s_slicingStoppingProxy;
            proxy->connect(m_slicingStartingProxy, negatoSlicingInteractor->signal(
                               NegatoSlicingInteractor::s_SLICING_STARTED_SIG));
            proxy->connect(m_slicingStartingProxy, sliceCursor->slot(
                               SlicesCursor::s_SHOW_FULL_CROSS_SLOT));

            proxy->connect(m_slicingStoppingProxy, negatoSlicingInteractor->signal(
                               NegatoSlicingInteractor::s_SLICING_STOPPED_SIG));
            proxy->connect(m_slicingStoppingProxy, sliceCursor->slot(
                               SlicesCursor::s_SHOW_NORMAL_CROSS_SLOT));
            m_sliceCursor = sliceCursor;
        }
        if(this->is3dModeEnabled())
        {
            this->addAdaptor("::visuVTKAdaptor::Medical3DCamera", m_orientation);
        }
        else if(!this->is3dModeEnabled())
        {
            this->addAdaptor("::visuVTKAdaptor::SliceFollowerCamera", m_orientation);
        }
        else
        {
            SLM_TRACE("No 2D/3D mode specified.");
        }
        this->setVtkPipelineModified();
    }

}

//-----------------------------------------------------------------------------

void NegatoMPR::updateSliceType(int from, int to)
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

void NegatoMPR::updateSliceMode(int mode)
{
    switch(mode)
    {
        case 0:
        {
            this->setSliceMode(NO_SLICE);
            break;
        }
        case 1:
        {
            this->setSliceMode(ONE_SLICE);
            break;
        }
        case 3:
        {
            this->setSliceMode(THREE_SLICES);
            break;
        }
        default:
        {
            FW_RAISE("Slice mode " << mode << " is not implemented.");
        }
    }
    m_backupedSliceMode = this->getSliceMode();
    this->doUpdate();
}
//-----------------------------------------------------------------------------

void NegatoMPR::showSlice(bool isShown)
{
    if(isShown)
    {
        this->setSliceMode(m_backupedSliceMode);
    }
    else
    {
        m_backupedSliceMode = this->getSliceMode();
        this->setSliceMode(NO_SLICE);
    }
    this->doUpdate();
}

//-----------------------------------------------------------------------------

void NegatoMPR::setCrossScale(double scale)
{
    if (!m_sliceCursor.expired())
    {
        ::fwCom::SlotBase::sptr slot = m_sliceCursor.lock()->slot(s_SET_CROSS_SCALE_SLOT);
        slot->asyncRun(scale);
    }
}

//------------------------------------------------------------------------------

void NegatoMPR::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT("Configuration must begin with <config>", m_configuration->getName() == "config");

    if (m_configuration->hasAttribute("mode"))
    {
        std::string value(m_configuration->getAttributeValue("mode"));
        std::transform(value.begin(), value.end(), value.begin(), tolower);
        OSLM_ASSERT("Bad value "<<value<<" for attribute mode, it should either be '2d' or '3d'.",
                    value == "3d" || value == "2d");
        this->set3dMode(value == "3d");
    }
    if (m_configuration->hasAttribute("slices"))
    {
        std::string value(m_configuration->getAttributeValue("slices"));
        std::transform(value.begin(), value.end(), value.begin(), tolower);

        if(value == "0")
        {
            this->setSliceMode(NO_SLICE);
        }
        else if(value == "1")
        {
            this->setSliceMode(ONE_SLICE);
        }
        else if(value == "3")
        {
            this->setSliceMode(THREE_SLICES);
        }
        m_backupedSliceMode = this->getSliceMode();
    }
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

void NegatoMPR::setSliceMode(SliceMode sliceMode)
{
    if(m_sliceMode != sliceMode)
    {
        m_sliceMode = sliceMode;
    }
}

//------------------------------------------------------------------------------

NegatoMPR::SliceMode NegatoMPR::getSliceMode() const
{
    return m_sliceMode;
}

//------------------------------------------------------------------------------

::boost::logic::tribool NegatoMPR::is3dModeEnabled() const
{
    return m_3dModeEnabled;
}

//------------------------------------------------------------------------------

void NegatoMPR::set3dMode( bool enabled )
{
    m_3dModeEnabled = enabled;
}

//------------------------------------------------------------------------------

::fwRenderVTK::IVtkAdaptorService::sptr NegatoMPR::addAdaptor(std::string adaptor, int axis)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwRenderVTK::IVtkAdaptorService::sptr service;

    if(axis >= 0)
    {
        service = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( image, adaptor );
        SLM_ASSERT("service not instanced", service);
        ::fwDataTools::helper::MedicalImageAdaptor::sptr adaptorSrv =
            ::fwDataTools::helper::MedicalImageAdaptor::dynamicCast(service);
        SLM_ASSERT("adaptorSrv not instanced", adaptorSrv);
        adaptorSrv->setOrientation((Orientation) axis);
    }
    else
    {
        service = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( image, adaptor );
        SLM_ASSERT("service not instanced", service);
    }


    ::visuVTKAdaptor::NegatoOneSlice::sptr negatoAdaptor;
    negatoAdaptor = ::visuVTKAdaptor::NegatoOneSlice::dynamicCast(service);

    ::visuVTKAdaptor::NegatoWindowingInteractor::sptr negatoWindowingAdaptor;
    negatoWindowingAdaptor = ::visuVTKAdaptor::NegatoWindowingInteractor::dynamicCast(service);

    if (negatoAdaptor)
    {
        negatoAdaptor->setAllowAlphaInTF(m_allowAlphaInTF);
        negatoAdaptor->setInterpolation(m_interpolation);
        if (!m_imageSourceId.empty())
        {
            negatoAdaptor->setVtkImageSourceId(m_imageSourceId);
        }
        negatoAdaptor->setSelectedTFKey( this->getSelectedTFKey() );
        negatoAdaptor->setTFSelectionFwID( this->getTFSelectionFwID() );
        negatoAdaptor->setActorOpacity(m_actorOpacity);
    }
    else if (negatoWindowingAdaptor)
    {
        negatoWindowingAdaptor->setSelectedTFKey( this->getSelectedTFKey() );
        negatoWindowingAdaptor->setTFSelectionFwID( this->getTFSelectionFwID() );
    }

    service->setRenderService(this->getRenderService());
    service->setRenderId( this->getRenderId() );
    service->setPickerId( this->getPickerId() );
    service->setTransformId( this->getTransformId() );
    service->setAutoRender( this->getAutoRender() );

    service->start();
    service->update();
    this->registerService(service);

    return service;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType NegatoMPR::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
