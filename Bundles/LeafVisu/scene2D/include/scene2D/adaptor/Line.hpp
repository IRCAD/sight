/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_LINE_HPP_
#define _SCENE2D_ADAPTOR_LINE_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{


class SCENE2D_CLASS_API Line : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Line)(::scene2D::adaptor::IAdaptor) ) ;

    enum LineType
   {
      PLAIN,
      DOTTED
   };

    /// Basic constructor, do nothing.
    SCENE2D_API Line() throw();

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~Line() throw();

protected:
    /**
    * @brief Configuring the Line adaptor.
    *
    * Example of configuration
    * @verbatim
    <adaptor id="abscissa" class="::scene2D::adaptor::Line" objectId="self">
        <config x1="-500" x2="500" y1="0" y2="0" color="white" xAxis="xAxis" yAxis="yAxis" zValue="2" />
    </adaptor>
    @endverbatim
    * - <config x1="-500" x2="500" y1="0" y2="0" color="white" xAxis="xAxis" yAxis="yAxis" zValue="2" /> : Set the config.
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
    * \b xAxis : no mandatory (default value : ::scene2D::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType (LINEAR)) : Set the x Axis of the line layer.
    *
    * \b yAxis : no mandatory (default value : ::scene2D::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType (LINEAR)) : Set the y Axis of the line layer.
    *
    * \b zValue : no mandatory (default value : 0) : Set the zValue of the line layer (the higher the zValue, the higher the layer is).
    */
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );

    /// Initialize the layer and call the draw() function.
    SCENE2D_API void doStart()    throw ( ::fwTools::Failed );

    /// Do nothing.
    SCENE2D_API void doUpdate()    throw ( ::fwTools::Failed );

    /// Do nothing.
    SCENE2D_API void doUpdate( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Do nothing.
    SCENE2D_API void doSwap()    throw ( ::fwTools::Failed );

    /// Remove the layer from the scene.
    SCENE2D_API void doStop()    throw ( ::fwTools::Failed );

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


#endif // _SCENE2D_ADAPTOR_LINE_HPP_

