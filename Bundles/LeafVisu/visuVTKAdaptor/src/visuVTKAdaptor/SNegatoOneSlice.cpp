/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SNegatoOneSlice.hpp"

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
#include <vtkImageCheckerboard.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SNegatoOneSlice, ::fwData::Image );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT = "updateSliceType";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT      = "updateImage";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT        = "image";
static const ::fwServices::IService::KeyType s_TF_SELECTION_INOUT = "tfSelection";

//------------------------------------------------------------------------------

SNegatoOneSlice::SNegatoOneSlice() noexcept :
    m_manageImageSource(false),
    m_imageSource(nullptr),
    m_allowAlphaInTF(false),
    m_interpolation(true),
    m_actorOpacity(1.0)
{
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SNegatoOneSlice::updateSliceType, this);
    newSlot(s_UPDATE_IMAGE_SLOT, &SNegatoOneSlice::updateImage, this);
}

//------------------------------------------------------------------------------

SNegatoOneSlice::~SNegatoOneSlice() noexcept
{
    this->unregisterServices();
    this->cleanImageSource();
}

//------------------------------------------------------------------------------

vtkObject* SNegatoOneSlice::getImageSource()
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

void SNegatoOneSlice::cleanImageSource()
{
    if (m_manageImageSource && m_imageSource)
    {
        m_imageSource->Delete();
        m_imageSource = nullptr;
    }
}

//------------------------------------------------------------------------------

::fwRenderVTK::IAdaptor::sptr SNegatoOneSlice::getImageSliceAdaptor()
{
    ::fwRenderVTK::IAdaptor::sptr adaptor;

    if (m_imageSliceAdaptor.expired())
    {
        OSLM_TRACE(this->getID() << ": Create ImageSlice Adaptor Service");
        ::fwData::Image::sptr image;
        ::fwData::Composite::sptr sceneComposite;

        image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Missing image", image);

        // create the srv configuration for objects auto-connection
        IService::Config srvConfig;
        adaptor = this->createSubAdaptor("::visuVTKAdaptor::ImageSlice", srvConfig);
        this->registerServiceInOut(image, s_IMAGE_INOUT, adaptor, true, srvConfig);

        adaptor->setConfiguration(srvConfig);
        adaptor->setRenderService(this->getRenderService());
        adaptor->setRendererId( this->getRendererId() );
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

::fwRenderVTK::IAdaptor::sptr SNegatoOneSlice::getImageAdaptor()
{
    ::fwRenderVTK::IAdaptor::sptr adaptor;

    if (m_imageAdaptor.expired())
    {
        OSLM_TRACE(this->getID() << ": Create Image Adaptor Service");
        ::fwData::Image::sptr image;
        image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Missing image", image);

        // create the srv configuration for objects auto-connection
        IService::Config srvConfig;
        adaptor = this->createSubAdaptor("::visuVTKAdaptor::Image", srvConfig);
        this->registerServiceInOut(image, s_IMAGE_INOUT, adaptor, true, srvConfig);

        adaptor->setConfiguration(srvConfig);
        adaptor->setRenderService(this->getRenderService());
        adaptor->setRendererId( this->getRendererId() );
        adaptor->setPickerId( this->getPickerId() );
        adaptor->setTransformId( this->getTransformId() );
        adaptor->setAutoRender( this->getAutoRender() );

        ::visuVTKAdaptor::Image::sptr imgAdaptor;
        imgAdaptor = ::visuVTKAdaptor::Image::dynamicCast(adaptor);
        imgAdaptor->setVtkImageRegister(this->getImageSource());
        imgAdaptor->setSelectedTFKey( this->getSelectedTFKey() );

        ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite >(s_TF_SELECTION_INOUT);
        if (tfSelection)
        {
            this->registerServiceInOut(tfSelection, s_TF_SELECTION_INOUT, imgAdaptor, true, srvConfig);
        }

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

void SNegatoOneSlice::starting()
{
    this->initialize();
    if (nullptr == vtkImageBlend::SafeDownCast(this->getImageSource())
        && nullptr == vtkImageCheckerboard::SafeDownCast(this->getImageSource()))
    {
        this->getImageAdaptor()->start();
    }
    this->getImageSliceAdaptor()->start();
}

//------------------------------------------------------------------------------

void SNegatoOneSlice::stopping()
{
    this->unregisterServices();
    this->cleanImageSource();
}

//------------------------------------------------------------------------------

void SNegatoOneSlice::updating()
{
    if (nullptr == vtkImageBlend::SafeDownCast(this->getImageSource())
        && nullptr == vtkImageCheckerboard::SafeDownCast(this->getImageSource()))
    {
        this->getImageAdaptor()->update();
    }
    this->getImageSliceAdaptor()->update();
}

//-----------------------------------------------------------------------------

void SNegatoOneSlice::updateSliceType(int from, int to)
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

void SNegatoOneSlice::updateImage()
{
    this->stopping();
    this->starting();
    this->updating();
}

//------------------------------------------------------------------------------

void SNegatoOneSlice::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

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

::fwServices::IService::KeyConnectionsMap SNegatoOneSlice::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT );
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT );
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
