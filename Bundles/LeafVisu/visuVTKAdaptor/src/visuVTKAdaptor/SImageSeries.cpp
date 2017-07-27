/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SImageSeries.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/Material.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImageSeries);

static const ::fwServices::IService::KeyType s_IMAGE_SERIES_INOUT = "imageSeries";
static const ::fwServices::IService::KeyType s_TF_SELECTION_INOUT = "tfSelection";

namespace visuVTKAdaptor
{

SImageSeries::SImageSeries() noexcept :
    m_allowAlphaInTF(false),
    m_interpolation(false),
    m_3dModeEnabled( ::boost::logic::indeterminate ),
    m_sliceMode(SNegatoMPR::THREE_SLICES)

{
}

//------------------------------------------------------------------------------

SImageSeries::~SImageSeries() noexcept
{
}

//------------------------------------------------------------------------------

void SImageSeries::configuring()
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
            this->setSliceMode(SNegatoMPR::NO_SLICE);
        }
        else if(value == "1")
        {
            this->setSliceMode(SNegatoMPR::ONE_SLICE);
        }
        else if(value == "3")
        {
            this->setSliceMode(SNegatoMPR::THREE_SLICES);
        }
        else
        {
            SLM_FATAL("'slice' value must be '0', '1' or '3', actual: " + value);
        }
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

    this->setSelectedTFKey(config.get<std::string>("selectedTFKey", ""));
}

//------------------------------------------------------------------------------

void SImageSeries::starting()
{
    this->initialize();
    this->updating();
}

//------------------------------------------------------------------------------

void SImageSeries::updating()
{
    ::fwMedData::ImageSeries::sptr series = this->getInOut< ::fwMedData::ImageSeries >(s_IMAGE_SERIES_INOUT);
    OSLM_ASSERT("Missing iamgeSeries", series);

    this->stopping();

    ::fwData::Image::sptr image = series->getImage();

    IService::Config srvConfig;
    ::fwRenderVTK::IAdaptor::sptr service = this->createSubAdaptor("::visuVTKAdaptor::SNegatoMPR", srvConfig);
    // register image
    this->registerServiceInOut(image, SNegatoMPR::s_IMAGE_INOUT, service, true, srvConfig);

    ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite >(s_TF_SELECTION_INOUT);
    if (tfSelection)
    {
        this->registerServiceInOut(tfSelection, SNegatoMPR::s_TF_SELECTION_INOUT, service, true, srvConfig);
    }

    service->setTransformId( this->getTransformId() );
    service->setRendererId( this->getRendererId() );
    service->setPickerId( this->getPickerId() );
    service->setRenderService(this->getRenderService());
    service->setAutoRender( this->getAutoRender() );
    ::visuVTKAdaptor::SNegatoMPR::sptr negato = ::visuVTKAdaptor::SNegatoMPR::dynamicCast(service);
    negato->set3dMode(this->is3dModeEnabled());
    negato->setSliceMode(this->getSliceMode());
    negato->setOrientation(this->getOrientation());
    negato->setAllowAlphaInTF(m_allowAlphaInTF);
    negato->setInterpolation(m_interpolation);
    negato->setVtkImageSourceId(m_imageSourceId);
    negato->setSelectedTFKey(this->getSelectedTFKey());
    service->start();

    this->registerService(service);
}

//------------------------------------------------------------------------------

void SImageSeries::stopping()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SImageSeries::setSliceMode(SNegatoMPR::SliceMode sliceMode)
{
    if(m_sliceMode != sliceMode)
    {
        m_sliceMode = sliceMode;
    }
}

//------------------------------------------------------------------------------

SNegatoMPR::SliceMode SImageSeries::getSliceMode()
{
    return m_sliceMode;
}

//------------------------------------------------------------------------------

::boost::logic::tribool SImageSeries::is3dModeEnabled()
{
    return m_3dModeEnabled;
}

//------------------------------------------------------------------------------

void SImageSeries::set3dMode( bool enabled )
{
    m_3dModeEnabled = enabled;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
