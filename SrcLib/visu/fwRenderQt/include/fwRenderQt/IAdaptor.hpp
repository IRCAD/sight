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

#pragma once

#include "fwRenderQt/config.hpp"
#include "fwRenderQt/data/Axis.hpp"
#include "fwRenderQt/data/Event.hpp"
#include "fwRenderQt/SRender.hpp"

#include <fwServices/IService.hpp>

namespace fwRenderQt
{

/**
 * @brief Root class for all scene2d adaptors.
 */
class FWRENDERQT_CLASS_API IAdaptor : public ::fwServices::IService
{

public:

    /// Point2D coordinate <X, Y>
    typedef std::pair< double, double > Point2DType;

    /// <width, height>
    typedef std::pair<float, float> ViewSizeRatio;

    /// <width, height>
    typedef std::pair<float, float> ViewportSizeRatio;

    /// <width, height>
    typedef std::pair<float, float> Scene2DRatio;

    fwCoreServiceMacro(IAdaptor, ::fwServices::IService);

    /// Set the zValue.
    FWRENDERQT_API void setZValue(float _zValue);

    /// Get the zValue.
    FWRENDERQT_API float getZValue() const;

    /// Get the render that manages the IAdaptor.
    FWRENDERQT_API SPTR(::fwRenderQt::SRender) getScene2DRender() const;

    /// Interact with the mouse events catched on the IAdaptor
    /// (virtual function, its behavior is only defined in the specific adaptors).
    FWRENDERQT_API virtual void processInteraction(::fwRenderQt::data::Event& _event );

protected:

    /// Constructor, set the zValue to 0.
    FWRENDERQT_API IAdaptor() noexcept;

    /// Basic destructor, do nothing.
    FWRENDERQT_API virtual ~IAdaptor() noexcept;

    /**
     * @brief Parse the xml configuration for Axis, z value and opacity
     * @code{.xml}
       <config xAxis="x" yAxis="y" zValue="1" opacity="1.0" />
       @endcode
     * @subsection Configuration Configuration:
     * - \b xAxis (optional): x axis associated to the adaptor
     * - \b yAxis (optional): y axis associated to the adaptor
     * - \b zValue (optional, default=0): z value of the layer
     * - \b opacity (optional, default=1.0): adaptor opacity (float)
     */
    FWRENDERQT_API void configureParams();

    /// Get a pair of doubles (a point), two axis, and convert the pair of doubles values from adaptor
    /// coordinates to scene coordinates
    FWRENDERQT_API Point2DType mapAdaptorToScene(const Point2DType& _xy,
                                                 const ::fwRenderQt::data::Axis::sptr& _xAxis,
                                                 const ::fwRenderQt::data::Axis::sptr& _yAxis) const;

    /// Get a pair of doubles (a point), two axis, and convert the pair of doubles values from scene
    /// coordinates to adaptor coordinates
    FWRENDERQT_API Point2DType mapSceneToAdaptor(const Point2DType& _xy,
                                                 const ::fwRenderQt::data::Axis::sptr& _xAxis,
                                                 const ::fwRenderQt::data::Axis::sptr& _yAxis) const;

    /// Return the ratio between view's initial size and its current size
    FWRENDERQT_API ViewSizeRatio getViewSizeRatio() const;

    /// Return the ratio between viewport's initial size and its current size
    FWRENDERQT_API ViewportSizeRatio getViewportSizeRatio() const;

    /// Initialize the source values used for computing view's size ratio.
    FWRENDERQT_API void initializeViewSize();

    /// Initialize the source values used for computing viewport's size ratio.
    FWRENDERQT_API void initializeViewportSize();

    FWRENDERQT_API Scene2DRatio getRatio() const;

    /// The x Axis.
    ::fwRenderQt::data::Axis::sptr m_xAxis;

    /// The y Axis.
    ::fwRenderQt::data::Axis::sptr m_yAxis;

    /// The adaptor zValue (depth within the scene).
    /// The adaptor with the highest zValue is displayed on top of all adaptors.
    float m_zValue;

    /// Opacity of the adaptor. Default value set to 1 (opaque).
    float m_opacity;

    /// Initial size of the widget (view). The goal of keeping a reference on the initial size is to
    /// avoid unwanted scaling onto some objects (such as transfer function points, histogram cursor,
    /// etc) when a resize event is caught.
    ViewSizeRatio m_viewInitialSize;

    /// Initial size of the viewport. The goal of keeping a reference on the initial size of the
    /// viewport is the same as preceding.
    ViewportSizeRatio m_viewportInitialSize;
};

//------------------------------------------------------------------------------

} // namespace fwRenderQt
