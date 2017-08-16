/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SGRIDFROMFLOAT_HPP__
#define __SCENE2D_ADAPTOR_SGRIDFROMFLOAT_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief Create a Grid in the scen2D with float min/max values.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="grid1" type="::scene2D::adaptor::SGridFromFloat">
    <in key="float" uid="floatUID" />
    <config xMin="-750" xMax="750" yMin="-0.7" yMax="1.7" xSpacing="100" ySpacing="0.1" color="lightGray"
            xAxis="xAxis" yAxis="yAxis" zValue="1" />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b float [::fwData::Float]: value used to set xSpacing.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b xMin (mandatory): Set the minimum x value of the grid.
 *    - \b xMax (mandatory): Set the maximum x value of the grid.
 *    - \b yMin (mandatory): Set the minimum y value of the grid.
 *    - \b yMax (mandatory): Set the maximum y value of the grid.
 *    - \b xSpacing (optional, default value : 10) : Set the grid spacing (space between 2 consecutive lines) in x.
 *    - \b ySpacing (optional, default value : 10) : Set the grid spacing (space between 2 consecutive lines) in y.
 *    - \b color (optional, default value : black) : Set the color of the lines.
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 */
class SCENE2D_CLASS_API SGridFromFloat : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SGridFromFloat)(::fwRenderQt::IAdaptor) );

    enum LineType
    {
        PLAIN,
        DOTTED
    };

    /// Constructor, set the x and y spacing to 10
    SCENE2D_API SGridFromFloat() noexcept;

    /// Basic destructor, do nothing
    SCENE2D_API virtual ~SGridFromFloat() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Float::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

protected:

    SCENE2D_API void configuring();

    /// Initialize the layer, set the pen style to DashLine and call the draw() function.
    SCENE2D_API void starting();

    /// Do nothing.
    SCENE2D_API void updating();

    /// Clean the lines vector and remove the layer from the scene.
    SCENE2D_API void stopping();

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

#endif // __SCENE2D_ADAPTOR_SGRIDFROMFLOAT_HPP__

