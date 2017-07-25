/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SNegatoMPR.hpp"

#include "visuVTKAdaptor/NegatoSlicingInteractor.hpp"
#include "visuVTKAdaptor/SlicesCursor.hpp"
#include "visuVTKAdaptor/SNegatoOneSlice.hpp"
#include "visuVTKAdaptor/SNegatoWindowingInteractor.hpp"

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

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SNegatoMPR);

namespace visuVTKAdaptor
{
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT   = "updateSliceType";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_MODE_SLOT   = "updateSliceMode";
static const ::fwCom::Slots::SlotKeyType s_SHOW_SLICE_SLOT          = "showSlice";
static const ::fwCom::Slots::SlotKeyType s_SET_CROSS_SCALE_SLOT     = "setCrossScale";
static const ::fwCom::Slots::SlotKeyType S_CHANGE_IMAGE_SOURCE_SLOT = "changeImageSource";

static const std::string s_slicingStartingProxy = "slicingStartingProxy";
static const std::string s_slicingStoppingProxy = "slicingStoppingProxy";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT        = "image";
static const ::fwServices::IService::KeyType s_TF_SELECTION_INOUT = "tfSelection";

//------------------------------------------------------------------------------

SNegatoMPR::SNegatoMPR() noexcept :
    m_allowAlphaInTF(false),
    m_interpolation(false),
    m_actorOpacity(1.0),
    m_3dModeEnabled( ::boost::logic::indeterminate ),
    m_sliceMode(THREE_SLICES),
    m_backupedSliceMode(THREE_SLICES)
{
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SNegatoMPR::updateSliceType, this);
    newSlot(s_UPDATE_SLICE_MODE_SLOT, &SNegatoMPR::updateSliceMode, this);
    newSlot(s_SHOW_SLICE_SLOT, &SNegatoMPR::showSlice, this);
    newSlot(s_SET_CROSS_SCALE_SLOT, &SNegatoMPR::setCrossScale, this);
    newSlot(S_CHANGE_IMAGE_SOURCE_SLOT, &SNegatoMPR::changeImageSource, this);
}

//------------------------------------------------------------------------------

SNegatoMPR::~SNegatoMPR() noexcept
{
}

//------------------------------------------------------------------------------

void SNegatoMPR::starting()
{
    this->initialize();
    this->updating();
}

//------------------------------------------------------------------------------

void SNegatoMPR::stopping()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

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
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegatoMPR::updating()
{
    this->stopping();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if ( imageIsValid)
    {
        if(this->getSliceMode() != NO_SLICE)
        {
            if(this->getSliceMode() == ONE_SLICE)
            {
                this->addAdaptor("::visuVTKAdaptor::SNegatoOneSlice", m_orientation);
            }
            else if(this->getSliceMode() == THREE_SLICES)
            {
                this->addAdaptor("::visuVTKAdaptor::SNegatoOneSlice", X_AXIS);
                this->addAdaptor("::visuVTKAdaptor::SNegatoOneSlice", Y_AXIS);
                this->addAdaptor("::visuVTKAdaptor::SNegatoOneSlice", Z_AXIS);
            }

            ::fwRenderVTK::IAdaptor::sptr sliceCursor;
            ::fwRenderVTK::IAdaptor::sptr negatoSlicingInteractor;
            this->addAdaptor("::visuVTKAdaptor::SNegatoWindowingInteractor");
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
        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SNegatoMPR::updateSliceType(int from, int to)
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

void SNegatoMPR::updateSliceMode(int mode)
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
    this->updating();
}
//-----------------------------------------------------------------------------

void SNegatoMPR::showSlice(bool isShown)
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
    this->updating();
}

//-----------------------------------------------------------------------------

void SNegatoMPR::setCrossScale(double scale)
{
    if (!m_sliceCursor.expired())
    {
        ::fwCom::SlotBase::sptr slot = m_sliceCursor.lock()->slot(s_SET_CROSS_SCALE_SLOT);
        slot->asyncRun(scale);
    }
}

//-----------------------------------------------------------------------------

void SNegatoMPR::changeImageSource(std::string _value, std::string _key)
{
    if( _key == "ImageSource" )
    {
        // Select the right algorithm
        m_imageSourceId = _value;

        this->updating();
    }
}

//------------------------------------------------------------------------------

void SNegatoMPR::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    if (config.count("mode"))
    {
        std::string value = config.get<std::string>("mode");
        std::transform(value.begin(), value.end(), value.begin(), tolower);
        SLM_ASSERT("Bad value '" + value + "' for attribute mode, it should either be '2d' or '3d'.",
                   value == "3d" || value == "2d");
        this->set3dMode(value == "3d");
    }
    if (config.count("slices"))
    {
        const std::string value = config.get<std::string>("slices");

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
        else
        {
            SLM_FATAL("'slice' value must be '0', '1' or '3', actual: " + value);
        }
        m_backupedSliceMode = this->getSliceMode();
    }

    const std::string orientation = config.get<std::string>("sliceIndex", "axial");
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

    const std::string tfalpha = config.get<std::string>("tfalpha", "no");
    SLM_ASSERT("'tfalpha' value must be 'yes' or 'no', actual: " + tfalpha, tfalpha == "yes" || tfalpha == "no");
    this->setAllowAlphaInTF(tfalpha == "yes");

    const std::string interpolation = config.get<std::string>("interpolation", "off");
    SLM_ASSERT("'interpolation' value must be 'on' or 'off', actual: " + interpolation,
               interpolation == "on" || interpolation == "off");
    this->setInterpolation(interpolation == "yes");

    this->setVtkImageSourceId( config.get<std::string>("vtkimagesource", ""));

    m_actorOpacity = config.get<double>("actorOpacity", 1.);

    this->setSelectedTFKey(config.get<std::string>("selectedTFKey", ""));
}

//------------------------------------------------------------------------------

void SNegatoMPR::setSliceMode(SliceMode sliceMode)
{
    if(m_sliceMode != sliceMode)
    {
        m_sliceMode = sliceMode;
    }
}

//------------------------------------------------------------------------------

SNegatoMPR::SliceMode SNegatoMPR::getSliceMode() const
{
    return m_sliceMode;
}

//------------------------------------------------------------------------------

::boost::logic::tribool SNegatoMPR::is3dModeEnabled() const
{
    return m_3dModeEnabled;
}

//------------------------------------------------------------------------------

void SNegatoMPR::set3dMode( bool enabled )
{
    m_3dModeEnabled = enabled;
}

//------------------------------------------------------------------------------

::fwRenderVTK::IAdaptor::sptr SNegatoMPR::addAdaptor(const std::string& adaptor, int axis)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    ::fwRenderVTK::IAdaptor::sptr service;

    // create the srv configuration for objects auto-connection
    IService::Config srvConfig;
    service = this->createSubAdaptor(adaptor, srvConfig);
    // register image
    this->registerServiceInOut(image, s_IMAGE_INOUT, service, true, srvConfig);

    if(axis >= 0)
    {
        ::fwDataTools::helper::MedicalImageAdaptor::sptr adaptorSrv =
            ::fwDataTools::helper::MedicalImageAdaptor::dynamicCast(service);
        SLM_ASSERT("adaptorSrv not instanced", adaptorSrv);
        adaptorSrv->setOrientation((Orientation) axis);
    }

    ::visuVTKAdaptor::SNegatoOneSlice::sptr negatoAdaptor;
    negatoAdaptor = ::visuVTKAdaptor::SNegatoOneSlice::dynamicCast(service);

    ::visuVTKAdaptor::SNegatoWindowingInteractor::sptr negatoWindowingAdaptor;
    negatoWindowingAdaptor = ::visuVTKAdaptor::SNegatoWindowingInteractor::dynamicCast(service);

    if (negatoAdaptor)
    {
        negatoAdaptor->setAllowAlphaInTF(m_allowAlphaInTF);
        negatoAdaptor->setInterpolation(m_interpolation);
        if (!m_imageSourceId.empty())
        {
            negatoAdaptor->setVtkImageSourceId(m_imageSourceId);
        }
        negatoAdaptor->setActorOpacity(m_actorOpacity);

        negatoAdaptor->setSelectedTFKey( this->getSelectedTFKey() );

        ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite >(s_TF_SELECTION_INOUT);
        if (tfSelection)
        {
            this->registerServiceInOut(tfSelection, s_TF_SELECTION_INOUT, negatoAdaptor, true, srvConfig);
        }
    }
    else if (negatoWindowingAdaptor)
    {
        negatoWindowingAdaptor->setSelectedTFKey( this->getSelectedTFKey() );

        ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite >(s_TF_SELECTION_INOUT);
        if (tfSelection)
        {
            this->registerServiceInOut(tfSelection, s_TF_SELECTION_INOUT, negatoWindowingAdaptor, true, srvConfig);
        }
    }

    service->setConfiguration(srvConfig);
    service->setRenderService(this->getRenderService());
    service->setRendererId( this->getRendererId() );
    service->setPickerId( this->getPickerId() );
    service->setTransformId( this->getTransformId() );
    service->setAutoRender( this->getAutoRender() );

    service->start();
    service->update();

    return service;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegatoMPR::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
