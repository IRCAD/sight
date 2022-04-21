/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "IAdaptor.hpp"

#include "viz/scene2d/registry/Adaptor.hpp"
#include "viz/scene2d/Scene2DGraphicsView.hpp"

#include <service/macros.hpp>
#include <service/registry/ObjectService.hpp>

#include <QGraphicsItemGroup>

namespace sight::viz::scene2d
{

IAdaptor::IAdaptor() noexcept :
    m_zValue(0.f),
    m_opacity(1.f)
{
}

//-----------------------------------------------------------------------------

IAdaptor::~IAdaptor() noexcept
{
}

//-----------------------------------------------------------------------------

void IAdaptor::setZValue(float _zValue)
{
    m_zValue = _zValue;
}

//-----------------------------------------------------------------------------

float IAdaptor::getZValue() const
{
    return m_zValue;
}

//-----------------------------------------------------------------------------

viz::scene2d::SRender::sptr IAdaptor::getScene2DRender() const
{
    const auto& registry = viz::scene2d::registry::getAdaptorRegistry();
    const auto& iter     = registry.find(this->getID());
    SIGHT_ASSERT("Adaptor " + this->getID() + " not registered", iter != registry.end());

    viz::scene2d::SRender::sptr render =
        viz::scene2d::SRender::dynamicCast(core::tools::fwID::getObject(iter->second));
    SIGHT_ASSERT("Service SRender " + iter->second + " not instanced", render);
    return render;
}

//-----------------------------------------------------------------------------

double IAdaptor::getViewSizeRatio() const
{
    return static_cast<double>(this->getScene2DRender()->getView()->width())
           / static_cast<double>(this->getScene2DRender()->getView()->height());
}

//-----------------------------------------------------------------------------

IAdaptor::ViewportSizeRatio IAdaptor::getViewportSizeRatio(const scene2d::data::Viewport& viewport) const
{
    return ViewportSizeRatio(
        static_cast<double>(viewport.getWidth()),
        static_cast<double>(viewport.getHeight())
    );
}

//-----------------------------------------------------------------------------

IAdaptor::Point2DType IAdaptor::pixelsToViewport(
    const IAdaptor::Point2DType& _xy,
    const scene2d::data::Viewport& viewport
) const
{
    auto view = this->getScene2DRender()->getView();

    const double viewportHeight = viewport.getHeight();
    const double viewportWidth  = viewport.getWidth();

    const double viewportViewRatio = viewport.getWidth() / view->width();
    const double viewportSizeRatio = viewportHeight / viewportWidth;
    const double viewSizeRatio     = static_cast<double>(view->width()) / static_cast<double>(view->height());

    const double x = _xy.first * viewportViewRatio;
    const double y = _xy.second * viewportSizeRatio * viewSizeRatio * viewportViewRatio;

    return {x, y};
}

//-----------------------------------------------------------------------------

IAdaptor::Point2DType IAdaptor::mapAdaptorToScene(
    const Point2DType& _xy
) const
{
    double x, y;

    if(m_xAxis->getScaleType() == scene2d::data::Axis::LOG)
    {
        // Logarithm 10 cannot get negative values
        if(_xy.first <= 0.)
        {
            x = 0.;
        }
        else
        {
            // Apply the x scale and the log to the x value
            x = m_xAxis->getScale() * log10(_xy.first);
        }
    }
    else
    {
        // Apply just the x scale to the x value
        x = m_xAxis->getScale() * _xy.first;
    }

    if(m_yAxis->getScaleType() == scene2d::data::Axis::LOG)
    {
        // Logarithm 10 cannot get negative values
        if(_xy.second <= 0.)
        {
            y = 0.;
        }
        else
        {
            // Apply the y scale and the log to the y value
            y = m_yAxis->getScale() * log10(_xy.second);
        }
    }
    else
    {
        // Apply just the y scale to the y value
        y = m_yAxis->getScale() * _xy.second;
    }

    return Point2DType(x, y);
}

//-----------------------------------------------------------------------------

IAdaptor::Point2DType IAdaptor::mapSceneToAdaptor(const Point2DType& _xy) const
{
    // Do the reverse operation of the mapAdaptorToScene function
    double x, y;
    if(m_xAxis->getScaleType() == scene2d::data::Axis::LOG)
    {
        x = 10. * std::exp(_xy.first) / m_xAxis->getScale();
    }
    else
    {
        x = (_xy.first) / m_xAxis->getScale();
    }

    if(m_yAxis->getScaleType() == scene2d::data::Axis::LOG)
    {
        y = 10. * std::exp(_xy.second) / m_yAxis->getScale();
    }
    else
    {
        y = _xy.second / m_yAxis->getScale();
    }

    return Point2DType(x, y);
}

//-----------------------------------------------------------------------------

void IAdaptor::configureParams()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    // If the corresponding attributes are present in the config, set the xAxis, yAxis and the adaptor zValue
    if(config.count("xAxis"))
    {
        m_xAxis = this->getScene2DRender()->getAxis(config.get<std::string>("xAxis"));
        SIGHT_ASSERT("xAxis not found", m_xAxis);
    }
    else
    {
        m_xAxis = std::make_shared<scene2d::data::Axis>();
    }

    if(config.count("yAxis"))
    {
        m_yAxis = this->getScene2DRender()->getAxis(config.get<std::string>("yAxis"));
        SIGHT_ASSERT("yAxis not found", m_xAxis);
    }
    else
    {
        m_yAxis = std::make_shared<scene2d::data::Axis>();
    }

    if(config.count("zValue"))
    {
        m_zValue = config.get<float>("zValue");
    }

    if(config.count("opacity"))
    {
        m_opacity = config.get<float>("opacity");
    }
}

//-----------------------------------------------------------------------------

void IAdaptor::processInteraction(scene2d::data::Event&)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d
