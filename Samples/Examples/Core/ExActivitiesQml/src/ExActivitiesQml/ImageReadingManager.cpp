/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "ExActivitiesQml/ImageReadingManager.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwRenderVTK/factory/new.hpp>
#include <fwRenderVTK/SRender.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <fwVTKQml/VtkRenderWindowInteractorManager.hpp>

static const std::string s_IMAGE_ID           = "image";
static const std::string s_VALIDATION_CHANNEL = "validationChannel";

static const ::fwCom::Signals::SignalKeyType s_VALIDATED_SIG = "validated";

//------------------------------------------------------------------------------

ImageReadingManager::ImageReadingManager() noexcept
{
    this->requireInput(s_IMAGE_ID, InputType::OBJECT);
    this->requireInput(s_VALIDATION_CHANNEL, InputType::CHANNEL);
    newSignal< VoidSignalType >(s_VALIDATED_SIG);
}

//------------------------------------------------------------------------------

ImageReadingManager::~ImageReadingManager() noexcept
{
    this->uninitialize();
}

//------------------------------------------------------------------------------

void ImageReadingManager::initialize()
{
    this->::fwQml::IQmlAppManager::initialize();

    if (m_isInitialized)
    {
        auto proxy = ::fwServices::registry::Proxy::getDefault();
        proxy->connect(this->getInputID(s_VALIDATION_CHANNEL), this->signal< VoidSignalType >(s_VALIDATED_SIG));

        this->startServices();
    }
}

//------------------------------------------------------------------------------

void ImageReadingManager::createVtkScene()
{
    if (!m_vtkSceneCreated)
    {
        // generic scene
        auto renderSrv = ::fwServices::add< ::fwRenderVTK::SRender >("::fwRenderVTK::SRender");
        m_imageAdaptor = ::fwServices::add("::visuVTKAdaptor::SNegatoMPR");

        /* **************************************************************************************
        *              genericScene configuration
        ****************************************************************************************/

        // create and register the render service
        ::fwServices::IService::ConfigType renderConfig;
        ::fwServices::IService::ConfigType pickerConfig;
        pickerConfig.add("<xmlattr>.vtkclass", "fwVtkCellPicker");
        pickerConfig.add("<xmlattr>.id", "picker");
        renderConfig.add_child("scene.picker", pickerConfig);
        renderConfig.add("scene.renderer.<xmlattr>.id", "default");
        renderSrv->setConfiguration(renderConfig);
        renderSrv->useContainer(false);
        renderSrv->displayAdaptor(m_imageAdaptor->getID());

        auto interactorManager = ::fwRenderVTK::factory::New< ::fwVTKQml::VtkRenderWindowInteractorManager >();
        SLM_ASSERT("Frame Buffer is not yet defined", m_frameBuffer);
        interactorManager->setFrameBuffer(m_frameBuffer);
        renderSrv->setInteractorManager(interactorManager);
        renderSrv->configure();

        ::fwServices::IService::ConfigType imageAdaptorConfig;
        imageAdaptorConfig.add("config.<xmlattr>.renderer", "default");
        imageAdaptorConfig.add("config.<xmlattr>.picker", "picker");
        imageAdaptorConfig.add("config.<xmlattr>.mode", "3d");
        imageAdaptorConfig.add("config.<xmlattr>.slice", "3");
        imageAdaptorConfig.add("config.<xmlattr>.sliceIndex", "axial");
        m_imageAdaptor->setConfiguration(imageAdaptorConfig);
        m_imageAdaptor->configure();

        m_imageAdaptor->setObjectId("image", this->getInputID(s_IMAGE_ID));

        this->addService(renderSrv, true);
        this->addService(m_imageAdaptor, true);

        m_vtkSceneCreated = true;
    }
}

//------------------------------------------------------------------------------

void ImageReadingManager::openImage()
{
    const auto& image = ::fwData::Image::dynamicCast(this->getObject(this->getInputID(s_IMAGE_ID)));
    auto imageReader  = ::fwServices::add("::uiIO::editor::SIOSelector");
    ::fwServices::IService::ConfigType imageSeriesReaderConfig;
    imageSeriesReaderConfig.put("type.<xmlattr>.mode", "reader");
    imageReader->registerInOut(image, "data");
    imageReader->setConfiguration(imageSeriesReaderConfig);
    imageReader->configure();
    imageReader->start();
    imageReader->update();
    imageReader->stop();
    ::fwServices::OSR::unregisterService(imageReader);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers:: checkImageValidity(image))
    {
        ::fwDataTools::helper::Image helper( image );

        helper.createLandmarks();
        helper.createTransferFunctionPool();
        helper.createImageSliceIndex();
    }
    m_imageAdaptor->update();

    this->signal< VoidSignalType >(s_VALIDATED_SIG)->asyncEmit();
}

//------------------------------------------------------------------------------

void ImageReadingManager::uninitialize()
{
    auto proxy = ::fwServices::registry::Proxy::getDefault();
    proxy->disconnect(this->getInputID(s_VALIDATION_CHANNEL), this->signal< VoidSignalType >(s_VALIDATED_SIG));

    // stop the started services and unregister all the services
    this->destroy();
}

//------------------------------------------------------------------------------
