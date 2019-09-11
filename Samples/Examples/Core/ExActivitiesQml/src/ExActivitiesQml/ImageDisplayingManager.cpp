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

#include "ExActivitiesQml/ImageDisplayingManager.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwQml/IQmlEditor.hpp>

#include <fwRenderVTK/factory/new.hpp>
#include <fwRenderVTK/SRender.hpp>

#include <fwServices/op/Add.hpp>

#include <fwVTKQml/VtkRenderWindowInteractorManager.hpp>

static const std::string s_IMAGE_ID = "image";

//------------------------------------------------------------------------------

ImageDisplayingManager::ImageDisplayingManager() noexcept
{
    this->requireInput(s_IMAGE_ID, InputType::OBJECT);
}

//------------------------------------------------------------------------------

ImageDisplayingManager::~ImageDisplayingManager() noexcept
{
    this->uninitialize();
}

//------------------------------------------------------------------------------

void ImageDisplayingManager::initialize()
{
    this->create();

    if (this->checkInputs())
    {
        this->startServices();
    }
    else
    {
        const std::string msg = "All the required inputs are not present, '" + this->getInputID("") +
                                "' activity will not be launched";
        ::fwGui::dialog::MessageDialog::showMessageDialog("Manager Initialization",
                                                          msg,
                                                          ::fwGui::dialog::IMessageDialog::CRITICAL);
    }
}

//------------------------------------------------------------------------------

void ImageDisplayingManager::createVtkScene()
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

void ImageDisplayingManager::onServiceCreated(const QVariant& obj)
{
    ::fwQml::IQmlEditor::sptr srv(obj.value< ::fwQml::IQmlEditor* >());
    if (srv)
    {
        if (srv->isA("::uiImageQml::SSliceIndexPositionEditor"))
        {
            srv->setObjectId("image", this->getInputID(s_IMAGE_ID));
            this->addService(srv, true);
        }
        else
        {
            OSLM_FATAL("service '" + srv->getClassname() + "' is not managed");
        }
    }
}

//------------------------------------------------------------------------------

void ImageDisplayingManager::onUpdateSliceMode(int mode)
{
    m_imageAdaptor->slot("updateSliceMode")->asyncRun(mode);
}

//------------------------------------------------------------------------------

void ImageDisplayingManager::onShowScan(bool isShown)
{
    m_imageAdaptor->slot("showSlice")->asyncRun(isShown);
}

//------------------------------------------------------------------------------

void ImageDisplayingManager::uninitialize()
{
    // stop the started services and unregister all the services
    this->destroy();
}

//------------------------------------------------------------------------------
