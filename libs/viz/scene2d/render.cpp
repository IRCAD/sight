/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "render.hpp"

#include "viz/scene2d/adaptor.hpp"
#include "viz/scene2d/registry/adaptor.hpp"
#include "viz/scene2d/Scene2DGraphicsView.hpp"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/tools/color.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QGraphicsRectItem>
#include <QVBoxLayout>

#include <cmath>

namespace sight::viz::scene2d
{

render::render() noexcept :
    m_sceneStart(-100., -100.),
    m_sceneWidth(200., 200.),
    m_background("#000000")
{
}

//-----------------------------------------------------------------------------

render::~render() noexcept =
    default;

//-----------------------------------------------------------------------------

QGraphicsScene* render::getScene() const
{
    return m_scene;
}

//-----------------------------------------------------------------------------

Scene2DGraphicsView* render::getView() const
{
    return m_view;
}

//-----------------------------------------------------------------------------

scene2d::data::Axis::sptr render::getAxis(const std::string& _id) const
{
    scene2d::data::Axis::sptr axis;
    const auto iter = m_axisMap.find(_id);
    if(iter != m_axisMap.end())
    {
        axis = iter->second;
    }

    return axis;
}

//-----------------------------------------------------------------------------

void render::dispatchInteraction(scene2d::data::Event& _event)
{
    if(!_event.isAccepted())
    {
        // Get all started adaptors.
        std::vector<viz::scene2d::adaptor::sptr> ordered_adaptors;

        const auto& registry = viz::scene2d::registry::get_adaptor_registry();
        for(const auto& elt : registry)
        {
            if(elt.second == this->get_id())
            {
                viz::scene2d::adaptor::sptr adaptor =
                    std::dynamic_pointer_cast<viz::scene2d::adaptor>(core::tools::id::get_object(elt.first));
                if(adaptor != nullptr && adaptor->started())
                {
                    ordered_adaptors.push_back(adaptor);
                }
            }
        }

        // Sort adaptors by z value.
        std::sort(
            ordered_adaptors.begin(),
            ordered_adaptors.end(),
            [&](viz::scene2d::adaptor::sptr _a1, viz::scene2d::adaptor::sptr _a2)
            {
                return _a1->getZValue() > _a2->getZValue();
            });

        // Process interaction on all adaptors until one has accepted the event.
        for(const viz::scene2d::adaptor::sptr& adaptor : ordered_adaptors)
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

bool render::contains(const scene2d::vec2d_t& _coord) const
{
    /// Returns the viewport coordinate point mapped to scene coordinates.
    const QPoint qp(static_cast<int>(_coord.x), static_cast<int>(_coord.y));
    QPointF qps = m_view->mapToScene(qp);

    QRectF rect = m_view->sceneRect();

    return rect.contains(qps);
}

//-----------------------------------------------------------------------------

scene2d::vec2d_t render::mapToScene(const scene2d::vec2d_t& _coord, bool _clip) const
{
    /// Returns the viewport coordinate point mapped to scene coordinates.
    const QPoint qp(static_cast<int>(_coord.x), static_cast<int>(_coord.y));
    QPointF qps = m_view->mapToScene(qp);
    if(_clip)
    {
        QRectF rect = m_view->sceneRect();
        if(!rect.contains(qps))
        {
            // Keep the item inside the scene rect.
            qps.setX(qMin(rect.right(), qMax(qps.x(), rect.left())));
            qps.setY(qMin(rect.bottom(), qMax(qps.y(), rect.top())));
        }
    }

    return {qps.x(), qps.y()};
}

//-----------------------------------------------------------------------------

void render::configuring()
{
    this->initialize();

    const auto& config = this->get_config();

    const auto scene_cfg = config.get_child_optional("scene");
    SIGHT_ASSERT("There is no implementation between \"scene\" tags", scene_cfg.has_value());

    for(const auto& iter : scene_cfg.value())
    {
        if(iter.first == "axis")
        {
            this->configureAxis(iter.second.get_child("<xmlattr>"));
        }
        else if(iter.first == "scene")
        {
            this->configureScene(iter.second.get_child("<xmlattr>"));
        }
        else if(iter.first == "adaptor")
        {
            this->configureAdaptor(iter.second.get_child("<xmlattr>"));
        }
        else
        {
            SIGHT_ASSERT("Bad scene const base&, unknown xml node : " + iter.first, false);
        }
    }
}

//-----------------------------------------------------------------------------

void render::starting()
{
    this->create();

    this->startContext();
}

//-----------------------------------------------------------------------------

void render::updating()
{
}

//-----------------------------------------------------------------------------

void render::stopping()
{
    m_axisMap.clear();

    this->stopContext();
    this->destroy();
}

//-----------------------------------------------------------------------------

void render::startContext()
{
    auto qt_container = std::dynamic_pointer_cast<ui::qt::container::widget>(this->getContainer());

    // Convert the background color
    std::array<std::uint8_t, 4> color {};
    sight::data::tools::color::hexaStringToRGBA(m_background, color);

    m_scene = new QGraphicsScene(m_sceneStart.x, m_sceneStart.y, m_sceneWidth.x, m_sceneWidth.y);
    m_scene->setBackgroundBrush(QBrush(QColor(color[0], color[1], color[2], color[3])));
    m_scene->setFocus(Qt::MouseFocusReason);

    m_view = new Scene2DGraphicsView(m_scene, qt_container->getQtContainer());
    m_view->setSceneRender(std::dynamic_pointer_cast<viz::scene2d::render>(this->get_sptr()));
    m_view->setRenderHint(QPainter::Antialiasing, m_antialiasing);

    auto* layout = new QVBoxLayout;
    layout->addWidget(m_view);
    qt_container->setLayout(layout);

    viz::scene2d::data::Viewport init_viewport;
    init_viewport.setX(m_sceneStart.x);
    init_viewport.setY(m_sceneStart.y);
    init_viewport.setWidth(m_sceneWidth.x);
    init_viewport.setHeight(m_sceneWidth.y);
    m_view->updateFromViewport(init_viewport);
}

//-----------------------------------------------------------------------------

void render::stopContext()
{
    delete m_scene;
    delete m_view;
}

//-----------------------------------------------------------------------------

Qt::AspectRatioMode render::getAspectRatioMode() const
{
    return m_aspectRatioMode;
}

//-----------------------------------------------------------------------------

void render::configureAxis(const config_t& _conf)
{
    const auto id         = _conf.get<std::string>("id");
    const auto scale_type = _conf.get<std::string>("scaleType");
    const auto origin     = _conf.get<float>("origin");
    const auto scale      = _conf.get<float>("scale");

    scene2d::data::Axis::sptr axis = std::make_shared<scene2d::data::Axis>();
    axis->setOrigin(origin);
    axis->setScale(scale);
    axis->set_scale_type(scale_type == "LINEAR" ? scene2d::data::Axis::LINEAR : scene2d::data::Axis::LOG);
    m_axisMap[id] = axis;
}

//-----------------------------------------------------------------------------

void render::configureScene(const config_t& _conf)
{
    m_sceneStart.x = _conf.get<float>("x");
    m_sceneStart.y = _conf.get<float>("y");
    m_sceneWidth.x = _conf.get<float>("width");
    m_sceneWidth.y = _conf.get<float>("height");

    m_antialiasing = _conf.get<bool>("antialiasing", m_antialiasing);

    if(const auto aspect_ratio = _conf.get_optional<std::string>("aspectRatioMode"); aspect_ratio.has_value())
    {
        if(*aspect_ratio == "KeepAspectRatioByExpanding")
        {
            m_aspectRatioMode = Qt::KeepAspectRatioByExpanding;
        }
        else if(*aspect_ratio == "KeepAspectRatio")
        {
            m_aspectRatioMode = Qt::KeepAspectRatio;
        }
        else
        {
            SIGHT_ERROR_IF(
                "Unknown aspect ratio ("
                << *aspect_ratio
                << "). Possible values are: KeepAspectRatio, KeepAspectRatioByExpanding or IgnoreAspectRatio.",
                *aspect_ratio != "IgnoreAspectRatio"
            );
            m_aspectRatioMode = Qt::IgnoreAspectRatio;
        }
    }

    if(const auto hexa_color = _conf.get<std::string>("background", ""); !hexa_color.empty())
    {
        SIGHT_ASSERT(
            "Color string should start with '#' and followed by 6 or 8 "
            "hexadecimal digits. Given color: " << hexa_color,
            hexa_color[0] == '#'
            && (hexa_color.length() == 7 || hexa_color.length() == 9)
        );
        m_background = hexa_color;
    }
}

//-----------------------------------------------------------------------------

void render::configureAdaptor(const config_t& _conf)
{
    const auto adaptor_id = _conf.get<std::string>("uid");

    auto& registry = viz::scene2d::registry::get_adaptor_registry();
    registry[adaptor_id] = this->get_id();
}

//-----------------------------------------------------------------------------

void render::updateSceneSize(float _ratio_percent)
{
    QRectF rec = m_scene->itemsBoundingRect();
    qreal x    = NAN;
    qreal y    = NAN;
    qreal w    = NAN;
    qreal h    = NAN;
    rec.getRect(&x, &y, &w, &h);

    if(_ratio_percent != 0)
    {
        qreal center_x = x + w / 2.0;
        qreal center_y = y + h / 2.0;
        w = w + w * _ratio_percent;
        h = h + h * _ratio_percent;
        x = center_x - w / 2.0;
        y = center_y - h / 2.0;
        rec.setRect(x, y, w, h);
    }

    m_sceneStart.x = x;
    m_sceneStart.y = y;
    m_sceneWidth.x = w;
    m_sceneWidth.y = h;

    m_scene->setSceneRect(rec);
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d
