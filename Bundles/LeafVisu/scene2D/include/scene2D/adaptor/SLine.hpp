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
 * @brief Line adaptor. Draw a line on the scene2D
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service id="abscissa" class="::scene2D::adaptor::SLine" objectId="self">
    <config x1="-500" x2="500" y1="0" y2="0" color="white" xAxis="xAxis" yAxis="yAxis" zValue="2" />
   </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b x1 (mandatory): Set the line beginning coordinate x value.
 *    - \b x2 (mandatory): Set the line ending coordinate x value.
 *    - \b y1 (mandatory): Set the line beginning coordinate y value.
 *    - \b y2 (mandatory): Set the line ending coordinate y value.
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b color (optional, default black): color of the line
 */
class SCENE2D_CLASS_API SLine : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SLine)(::fwRenderQt::IAdaptor) );

    /// Basic constructor, do nothing.
    SCENE2D_API SLine() noexcept;

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~SLine() noexcept;

protected:

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

    /// The coordinates of the line.
    float m_x1, m_x2, m_y1, m_y2;

    /// The pen.
    QPen m_pen;

    /// The layer.
    QGraphicsItemGroup* m_layer;
};

} // namespace adaptor
} // namespace scene2D

#endif // __SCENE2D_ADAPTOR_SLINE_HPP__

