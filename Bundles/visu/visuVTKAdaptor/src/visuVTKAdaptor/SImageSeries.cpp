/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "visuVTKAdaptor/SImageSeries.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/Material.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImageSeries);

static const ::fwServices::IService::KeyType s_IMAGE_SERIES_INOUT = "imageSeries";
static const ::fwServices::IService::KeyType s_TF_INOUT           = "tf";

namespace visuVTKAdaptor
{

SImageSeries::SImageSeries() noexcept :
    m_allowAlphaInTF(false),
    m_interpolation(false),
    m_3dModeEnabled( ::boost::logic::indeterminate ),
    m_sliceMode(SNegatoMPR::THREE_SLICES)

{
    this->registerObject(s_IMAGE_SERIES_INOUT, AccessType::INOUT, true);
    this->registerObject(s_TF_INOUT, AccessType::INOUT, false, true);
}

//------------------------------------------------------------------------------

SImageSeries::~SImageSeries() noexcept
{
}

//------------------------------------------------------------------------------

void SImageSeries::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

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
        m_helper.setOrientation(::fwDataTools::helper::MedicalImage::Z_AXIS);
    }
    else if(orientation == "frontal" )
    {
        m_helper.setOrientation(::fwDataTools::helper::MedicalImage::Y_AXIS);
    }
    else if(orientation == "sagittal" )
    {
        m_helper.setOrientation(::fwDataTools::helper::MedicalImage::X_AXIS);
    }

    const std::string tfalpha = config.get<std::string>("tfalpha", "no");
    SLM_ASSERT("'tfalpha' value must be 'yes' or 'no', actual: " + tfalpha, tfalpha == "yes" || tfalpha == "no");
    this->setAllowAlphaInTF(tfalpha == "yes");

    const std::string interpolation = config.get<std::string>("interpolation", "off");
    SLM_ASSERT("'interpolation' value must be 'on' or 'off', actual: " + interpolation,
               interpolation == "on" || interpolation == "off");
    this->setInterpolation(interpolation == "yes");

    this->setVtkImageSourceId( config.get<std::string>("vtkimagesource", ""));
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

    auto negato = this->registerService< ::visuVTKAdaptor::SNegatoMPR>("::visuVTKAdaptor::SNegatoMPR");

    // register image
    ::fwData::Image::sptr image = series->getImage();
    negato->registerInOut(image, SNegatoMPR::s_IMAGE_INOUT, true);

    ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    if(tf != nullptr)
    {
        ::fwData::mt::ObjectWriteLock tfLock(tf);
        negato->registerInOut(tf, SNegatoMPR::s_TF_INOUT, false);
    }

    negato->setTransformId( this->getTransformId() );
    negato->setRendererId( this->getRendererId() );
    negato->setPickerId( this->getPickerId() );
    negato->setRenderService(this->getRenderService());
    negato->set3dMode((negato->is3dModeEnabled().value == ::boost::tribool::true_value));
    negato->setSliceMode(this->getSliceMode());
    negato->setOrientation(m_helper.getOrientation());
    negato->setAllowAlphaInTF(m_allowAlphaInTF);
    negato->setInterpolation(m_interpolation);
    negato->setVtkImageSourceId(m_imageSourceId);
    negato->start();
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

::fwServices::IService::KeyConnectionsMap SImageSeries::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_SERIES_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
