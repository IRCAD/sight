/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SRender.hpp"

#include "viz/scene2d/IAdaptor.hpp"
#include "viz/scene2d/registry/Adaptor.hpp"
#include "viz/scene2d/Scene2DGraphicsView.hpp"

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/tools/Color.hpp>

#include <service/helper/Config.hpp>
#include <service/macros.hpp>
#include <service/op/Add.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QGraphicsRectItem>
#include <QVBoxLayout>

namespace sight::viz::scene2d
{

SRender::SRender() noexcept :
    m_sceneStart(-100., -100.),
    m_sceneWidth(200., 200.),
    m_scene(nullptr),
    m_view(nullptr),
    m_antialiasing(false),
    m_background("#000000"),
    m_aspectRatioMode(Qt::IgnoreAspectRatio)
{
}

//-----------------------------------------------------------------------------

SRender::~SRender() noexcept
{
}

//-----------------------------------------------------------------------------

QGraphicsScene* SRender::getScene() const
{
    return m_scene;
}

//-----------------------------------------------------------------------------

Scene2DGraphicsView* SRender::getView() const
{
    return m_view;
}

//-----------------------------------------------------------------------------

scene2d::data::Viewport::sptr SRender::getViewport() const
{
    return m_viewport;
}

//-----------------------------------------------------------------------------

scene2d::data::Axis::sptr SRender::getAxis(const std::string& id) const
{
    scene2d::data::Axis::sptr axis;
    const auto iter = m_axisMap.find(id);
    if(iter != m_axisMap.end())
    {
        axis = iter->second;
    }

    return axis;
}

//-----------------------------------------------------------------------------

void SRender::dispatchInteraction(scene2d::data::Event& _event)
{
    if(!_event.isAccepted())
    {
        // Get all started adaptors.
        std::vector<viz::scene2d::IAdaptor::sptr> orderedAdaptors;

        const auto& registry = viz::scene2d::registry::getAdaptorRegistry();
        for(const auto& elt : registry)
        {
            if(elt.second == this->getID())
            {
                viz::scene2d::IAdaptor::sptr adaptor =
                    viz::scene2d::IAdaptor::dynamicCast(core::tools::fwID::getObject(elt.first));
                if(adaptor != nullptr && adaptor->isStarted())
                {
                    orderedAdaptors.push_back(adaptor);
                }
            }
        }

        // Sort adaptors by z value.
        std::sort(
            orderedAdaptors.begin(),
            orderedAdaptors.end(),
            [&](viz::scene2d::IAdaptor::sptr _a1, viz::scene2d::IAdaptor::sptr _a2)
            {
                return _a1->getZValue() > _a2->getZValue();
            });

        // Process interaction on all adaptors until one has accepted the event.
        for(viz::scene2d::IAdaptor::sptr adaptor : orderedAdaptors)
        {
            adaptor->processInteraction(_event);
            if(_event.isAccepted())
            {
                return;
            }
        }
    }
}

//-----------------------------------------------------------------------------

scene2d::data::Coord SRender::mapToScene(const scene2d::data::Coord& coord) const
{
    /// Returns the viewport coordinate point mapped to scene coordinates.
    const QPoint qp(static_cast<int>(coord.getX()), static_cast<int>(coord.getY()));
    const QPointF qps = m_view->mapToScene(qp);
    return scene2d::data::Coord(qps.x(), qps.y());
}

//-----------------------------------------------------------------------------

void SRender::configuring()
{
    this->initialize();

    std::vector<SPTR(core::runtime::ConfigurationElement)> vectConfig = m_configuration->find("scene");
    SIGHT_ASSERT("There is no implementation between \"scene\" tags", !vectConfig.empty());
    m_sceneConfiguration = vectConfig.at(0);

    core::runtime::ConfigurationElementContainer::Iterator iter;
    for(iter = m_sceneConfiguration->begin() ; iter != m_sceneConfiguration->end() ; ++iter)
    {
        if((*iter)->getName() == "axis")
        {
            this->configureAxis(*iter);
        }
        else if((*iter)->getName() == "viewport")
        {
            this->configureViewport(*iter);
        }
        else if((*iter)->getName() == "scene")
        {
            this->configureScene(*iter);
        }
        else if((*iter)->getName() == "adaptor")
        {
            this->configureAdaptor(*iter);
        }
        else
        {
            SIGHT_ASSERT("Bad scene configurationType, unknown xml node : " + (*iter)->getName(), false);
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::starting()
{
    this->create();

    this->startContext();
}

//-----------------------------------------------------------------------------

void SRender::updating()
{
}

//-----------------------------------------------------------------------------

void SRender::stopping()
{
    m_axisMap.clear();

    this->stopContext();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SRender::startContext()
{
    SPTR(ui::qt::container::QtContainer) qtContainer =
        ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    // Convert the background color
    std::uint8_t color[4];
    sight::data::tools::Color::hexaStringToRGBA(m_background, color);

    m_scene = new QGraphicsScene(m_sceneStart.getX(), m_sceneStart.getY(), m_sceneWidth.getX(), m_sceneWidth.getY());
    m_scene->setBackgroundBrush(QBrush(QColor(color[0], color[1], color[2], color[3])));
    m_scene->setFocus(Qt::MouseFocusReason);

    m_view = new Scene2DGraphicsView(m_scene, qtContainer->getQtContainer());
    m_view->setViewport(m_viewport);
    m_view->setSceneRender(viz::scene2d::SRender::dynamicCast(this->getSptr()));
    m_view->setRenderHint(QPainter::Antialiasing, m_antialiasing);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_view);
    qtContainer->setLayout(layout);

    m_view->updateFromViewport();
}

//-----------------------------------------------------------------------------

void SRender::stopContext()
{
    delete m_scene;
    delete m_view;
}

//-----------------------------------------------------------------------------

Qt::AspectRatioMode SRender::getAspectRatioMode() const
{
    return m_aspectRatioMode;
}

//-----------------------------------------------------------------------------

void SRender::configureAxis(ConfigurationType _conf)
{
    SIGHT_ASSERT("\"axis\" tag required", _conf->getName() == "axis");

    const std::string id        = _conf->getAttributeValue("id");
    const std::string origin    = _conf->getAttributeValue("origin");
    const std::string scale     = _conf->getAttributeValue("scale");
    const std::string scaleType = _conf->getAttributeValue("scaleType");

    scene2d::data::Axis::sptr axis = std::make_shared<scene2d::data::Axis>();
    axis->setOrigin(std::stof(origin));
    axis->setScale(std::stof(scale));
    axis->setScaleType(scaleType == "LINEAR" ? scene2d::data::Axis::LINEAR : scene2d::data::Axis::LOG);
    m_axisMap[id] = axis;
}

//-----------------------------------------------------------------------------

void SRender::configureViewport(ConfigurationType _conf)
{
    SIGHT_ASSERT("\"viewport\" tag required", _conf->getName() == "viewport");

    const std::string id     = _conf->getAttributeValue("id");
    const std::string x      = _conf->getAttributeValue("x");
    const std::string y      = _conf->getAttributeValue("y");
    const std::string width  = _conf->getAttributeValue("width");
    const std::string height = _conf->getAttributeValue("height");

    m_viewport = scene2d::data::Viewport::New();
    m_viewport->setX(std::stof(x));
    m_viewport->setY(std::stof(y));
    m_viewport->setWidth(std::stof(width));
    m_viewport->setHeight(std::stof(height));
}

//-----------------------------------------------------------------------------

void SRender::configureScene(ConfigurationType _conf)
{
    SIGHT_ASSERT("\"scene\" tag required", _conf->getName() == "scene");

    const std::string x      = _conf->getAttributeValue("x");
    const std::string y      = _conf->getAttributeValue("y");
    const std::string width  = _conf->getAttributeValue("width");
    const std::string height = _conf->getAttributeValue("height");

    m_sceneStart.setX(std::stof(x));
    m_sceneStart.setY(std::stof(y));
    m_sceneWidth.setX(std::stof(width));
    m_sceneWidth.setY(std::stof(height));

    if(_conf->hasAttribute("antialiasing"))
    {
        if(_conf->getAttributeValue("antialiasing") == "true")
        {
            m_antialiasing = true;
        }
    }

    if(_conf->hasAttribute("aspectRatioMode"))
    {
        const std::string aspectRatio = _conf->getAttributeValue("aspectRatioMode");
        if(aspectRatio == "KeepAspectRatioByExpanding")
        {
            m_aspectRatioMode = Qt::KeepAspectRatioByExpanding;
        }
        else if(aspectRatio == "KeepAspectRatio")
        {
            m_aspectRatioMode = Qt::KeepAspectRatio;
        }
        else
        {
            SIGHT_ERROR_IF(
                "Unknown aspect ratio ("
                << aspectRatio
                << "). Possible values are: KeepAspectRatio, KeepAspectRatioByExpanding or IgnoreAspectRatio.",
                aspectRatio != "IgnoreAspectRatio"
            );
            m_aspectRatioMode = Qt::IgnoreAspectRatio;
        }
    }

    if(_conf->hasAttribute(("background")))
    {
        m_background = _conf->getAttributeValue("background");
        SIGHT_ASSERT("Color format must be hexadecimal.", m_background[0] == '#');
    }
}

//-----------------------------------------------------------------------------

void SRender::configureAdaptor(ConfigurationType _conf)
{
    SIGHT_ASSERT("\"adaptor\" tag required", _conf->getName() == "adaptor");

    const std::string adaptorId = _conf->getAttributeValue("uid");

    auto& registry = viz::scene2d::registry::getAdaptorRegistry();
    registry[adaptorId] = this->getID();
}

//-----------------------------------------------------------------------------

void SRender::updateSceneSize(float ratioPercent)
{
    QRectF rec = m_scene->itemsBoundingRect();
    qreal x, y, w, h;
    rec.getRect(&x, &y, &w, &h);

    if(ratioPercent != 0)
    {
        qreal centerX = x + w / 2.0;
        qreal centerY = y + h / 2.0;
        w = w + w * ratioPercent;
        h = h + h * ratioPercent;
        x = centerX - w / 2.0;
        y = centerY - h / 2.0;
        rec.setRect(x, y, w, h);
    }

    m_sceneStart.setX(x);
    m_sceneStart.setY(y);
    m_sceneWidth.setX(w);
    m_sceneWidth.setY(h);

    m_scene->setSceneRect(rec);
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d
