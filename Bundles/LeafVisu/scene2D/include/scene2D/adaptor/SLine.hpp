/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SLINE_HPP__
#define __SCENE2D_ADAPTOR_SLINE_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief line adaptor. Draw a line on the scene2D
 */
class SCENE2D_CLASS_API SLine : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SLine)(::fwRenderQt::IAdaptor) );

    enum LineType
    {
        PLAIN,
        DOTTED
    };

    /// Basic constructor, do nothing.
    SCENE2D_API SLine() noexcept;

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~SLine() noexcept;

protected:
    /**
     * @brief Configuring the SLine adaptor.
     *
     * Example of configuration
     * @code{.xml}
       <service id="abscissa" class="::scene2D::adaptor::SLine" objectId="self">
        <config x1="-500" x2="500" y1="0" y2="0" color="white" xAxis="xAxis" yAxis="yAxis" zValue="2" />
       </service>
       @endcode
     * - \<config x1="-500" x2="500" y1="0" y2="0" color="white" xAxis="xAxis" yAxis="yAxis" zValue="2" /\> : Set the
     * config.
     *
     * \b x1 : mandatory : Set the line begining coordinate x value.
     *
     * \b x2 : mandatory : Set the line ending coordinate x value.
     *
     * \b y1 : mandatory : Set the line begining coordinate y value.
     *
     * \b y2 : mandatory : Set the line ending coordinate y value.
     *
     * \b color : no mandatory (default value : black) : Set the color of the line.
     *
     * \b xAxis : no mandatory (default value : ::fwRenderQt::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType
     *(LINEAR)) : Set the x Axis of the line layer.
     *
     * \b yAxis : no mandatory (default value : ::fwRenderQt::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType
     *(LINEAR)) : Set the y Axis of the line layer.
     *
     * \b zValue : no mandatory (default value : 0) : Set the zValue of the line layer (the higher the zValue, the
     * higher the layer is).
     */
    SCENE2D_API void configuring();

    /// Initialize the layer and call the draw() function.
    SCENE2D_API void starting();

    /// Do nothing.
    SCENE2D_API void updating();

    /// Remove the layer from the scene.
    SCENE2D_API void stopping();

private:

    /// Create the line, set it m_pen, add it to the layer, set the layer position and zValue and add it to the scene.
    void draw();

    // The coordinates of the line.
    float m_x1, m_x2, m_y1, m_y2;
    // The type of the line.
    LineType m_lineType;
    /// The pen.
    QPen m_pen;
    /// The layer.
    QGraphicsItemGroup* m_layer;
};

} // namespace adaptor
} // namespace scene2D

#endif // __SCENE2D_ADAPTOR_SLINE_HPP__

