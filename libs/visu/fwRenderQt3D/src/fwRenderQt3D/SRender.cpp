/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwRenderQt3D/SRender.hpp"

#include "fwRenderQt3D/GenericScene.hpp"

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <QColor>
#include <QForwardRenderer>
#include <QQmlEngine>
#include <QString>

#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DRender/QCamera>

#include <QVBoxLayout>
#include <QWidget>

#include <iostream>
#include <string>

// fwServicesRegisterMacro( ::fwRender::IRender, ::fwRenderQt3D::SRender )

//-----------------------------------------------------------------------------

namespace fwRenderQt3D
{

//-----------------------------------------------------------------------------

SRender::SRender() noexcept
{
}

//-----------------------------------------------------------------------------

SRender::~SRender() noexcept
{
}

//-----------------------------------------------------------------------------

void SRender::configuring()
{
    this->initialize();

    // Allow using GenericScene as QML type when using SRender service in QML applications
    qmlRegisterType< GenericScene >("fwRenderQt3D", 1, 0, "GenericScene");

    // Define default background color value
    std::string color = "black";

    const ConfigType config = this->getConfigTree();
    if(!config.empty())
    {
        // Get scene configuration
        SLM_ERROR_IF("One scene must be configured.", config.count("scene") != 1);
        const ConfigType sceneCfg = config.get_child("scene");

        if(!sceneCfg.empty())
        {
            // Get background tag and its attributes if it is defined
            if(sceneCfg.count("background"))
            {
                const ConfigType backgroundCfg = sceneCfg.get_child("background");

                const ConfigType attributes = backgroundCfg.get_child("<xmlattr>");
                if (attributes.count("color"))
                {
                    color = attributes.get<std::string>("color");
                }
            }
        }
    }

    m_backgroundColor = QColor(QString::fromStdString(color));
}

//------------------------------------------------------------------------------

void SRender::starting()
{
    this->create();

    m_qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    // Renders a Qt3DWindow into a QWidget and displays it
    m_3dView = new Qt3DExtras::Qt3DWindow();

    m_scene = new ::fwRenderQt3D::GenericScene(false);

    Qt3DRender::QCamera* const camera = m_3dView->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 10.0f, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    Qt3DExtras::QOrbitCameraController* const cameraController = new Qt3DExtras::QOrbitCameraController(m_scene);
    cameraController->setCamera(camera);
    cameraController->setLinearSpeed(50.0f);
    cameraController->setLookSpeed(180.0f);

    m_3dView->setRootEntity(m_scene);
    m_3dView->defaultFrameGraph()->setClearColor(m_backgroundColor);

    QWidget* viewWidget = QWidget::createWindowContainer(m_3dView);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(viewWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    m_qtContainer->setLayout(layout);
}

//------------------------------------------------------------------------------

void SRender::updating()
{
}

//------------------------------------------------------------------------------

void SRender::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SRender::setQtContainer(SPTR(::fwGuiQt::container::QtContainer)_qtContainer)
{
    m_qtContainer = _qtContainer;
}

//------------------------------------------------------------------------------

void SRender::set3DView(Qt3DExtras::Qt3DWindow* _3dView)
{
    m_3dView = _3dView;
}

//------------------------------------------------------------------------------

void SRender::setScene(::fwRenderQt3D::GenericScene* _scene)
{
    m_scene = _scene;
}

//------------------------------------------------------------------------------

SPTR(::fwGuiQt::container::QtContainer) SRender::getQtContainer()
{
    return m_qtContainer;
}

//------------------------------------------------------------------------------

Qt3DExtras::Qt3DWindow* SRender::get3DView()
{
    return m_3dView;
}

//------------------------------------------------------------------------------

::fwRenderQt3D::GenericScene* SRender::getScene()
{
    return m_scene;
}

}// namespace fwRenderQt3D