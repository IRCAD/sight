/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_GRIDFROMFLOAT_HPP__
#define __SCENE2D_ADAPTOR_GRIDFROMFLOAT_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief Create a Grid in the scen2D with float min/max values
 */
class SCENE2D_CLASS_API GridFromFloat : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (GridFromFloat)(::fwRenderQt::IAdaptor) );

    enum LineType
    {
        PLAIN,
        DOTTED
    };

    /// Constructor, set the x and y spacing to 10
    SCENE2D_API GridFromFloat() noexcept;

    /// Basic destructor, do nothing
    SCENE2D_API virtual ~GridFromFloat() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Float::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

protected:
    /**
     * @brief Configuring the GridFromFloat adaptor.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="grid1" type="::scene2D::adaptor::GridFromFloat">
        <in key="float" uid="floatUID" />
        <config xMin="-750" xMax="750" yMin="-0.7" yMax="1.7" xSpacing="100" ySpacing="0.1" color="lightGray"
                xAxis="xAxis" yAxis="yAxis" zValue="1"/>
       </service>
       @endcode
     *
     * - objectId : ::fwData::Float
     *
     * - \<config xMin="-750" xMax="750" yMin="-0.7" yMax="1.7" xSpacing="100" ySpacing="0.1" color="lightGray"
     * xAxis="xAxis" yAxis="yAxis" zValue="1"/\> : Set the config.
     *
     * \b xMin : mandatory : Set the minimum x value of the grid.
     *
     * \b xMax : mandatory : Set the maximum x value of the grid.
     *
     * \b yMin : mandatory : Set the minimum y value of the grid.
     *
     * \b yMax : mandatory : Set the maximum y value of the grid.
     *
     * \b xSpacing : no mandatory (default value : 10) : Set the grid spacing (space between 2 consecutive lines) in x.
     *
     * \b ySpacing : no mandatory (default value : 10) : Set the grid spacing (space between 2 consecutive lines) in y.
     *
     * \b color : no mandatory (default value : black) : Set the color of the lines.
     *
     * \b xAxis : no mandatory (default value : ::fwRenderQt::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType
     *(LINEAR)) : Set the x Axis of the grid layer.
     *
     * \b yAxis : no mandatory (default value : ::fwRenderQt::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType
     *(LINEAR)) : Set the y Axis of the grid layer.
     *
     * \b zValue : no mandatory (default value : 0) : Set the zValue of the grid layer (the higher the zValue, the
     * higher the layer is).
     */
    SCENE2D_API void configuring();

    /// Initialize the layer, set the pen style to DashLine and call the draw() function.
    SCENE2D_API void doStart();

    /// Do nothing.
    SCENE2D_API void doUpdate();

    /// Do nothing.
    SCENE2D_API void doSwap();

    /// Clean the lines vector and remove the layer from the scene.
    SCENE2D_API void doStop();

private:

    /// Remove the lines from the scene, clear the lines vector, calculate the x/y start/end values, create the lines,
    /// set'em m_pen, push'em back in the m_lines vector, add'em to the layer, set the layer position and zValue and add
    /// it to the scene.
    void draw();

    float m_xMin, m_xMax, m_yMin, m_yMax, m_xSpacing, m_ySpacing;

    /// The type of the lines.
    LineType m_lineType;
    /// The pen.
    QPen m_pen;
    /// A vector containing QGraphicsItems representing the lines of the grid.
    std::vector<QGraphicsItem*> m_lines;
    /// The layer.
    QGraphicsItemGroup* m_layer;
};

} // namespace adaptor
} // namespace scene2D

#endif // __SCENE2D_ADAPTOR_GRIDFROMFLOAT_HPP__

