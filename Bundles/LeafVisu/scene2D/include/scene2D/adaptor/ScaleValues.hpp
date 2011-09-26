/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_SCALEVALUES_HPP_
#define _SCENE2D_ADAPTOR_SCALEVALUES_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{



class SCENE2D_CLASS_API ScaleValues : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (ScaleValues)(::scene2D::adaptor::IAdaptor) ) ;

    /// Constructor, set the x and y spacing to 10
    SCENE2D_API ScaleValues() throw();

    /// Basic destructor, do nothing
    SCENE2D_API virtual ~ScaleValues() throw();

protected:
    /**
    * @brief Configuring the ScaleValues adaptor.
    *
    * Example of configuration
    * @verbatim
    <adaptor id="scaleValues" class="::scene2D::adaptor::ScaleValues" objectId="self">
        <config xMin="-1100" xMax="750" yMin="0" yMax="1" xSpacing="100" ySpacing="0.1" 
            color="lightGray" xAxis="xAxis" yAxis="yAxis" zValue="1"/>
    </adaptor>
    @endverbatim
    * - <config xMin="-1100" xMax="750" yMin="-0.7" yMax="1.7" xSpacing="100" ySpacing="0.1" 
    *   color="lightGray" xAxis="xAxis" yAxis="yAxis" zValue="1"/> : Set the config.
    *
    * \b xMin : mandatory : Set the minimum X value to display.
    *
    * \b xMax : mandatory : Set the maximum X value to display.
    *
    * \b yMin : mandatory : Set the minimum Y value to display.
    *
    * \b yMax : mandatory : Set the maximum Y value to display.
    *
    * \b xInterval : no mandatory (default value : 10) : Set the interval between 2 consecutive values on the X axis.
    *
    * \b yInterval : no mandatory (default value : 10) : Set the interval between 2 consecutive values on the Y axis.
    *
    * \b color : no mandatory (default value : white) : Set the foreground color of the values.
    *
    * \b xAxis : no mandatory (default value : ::scene2D::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType (LINEAR)) : 
    * Set the X axis of this adaptor.
    *
    * \b yAxis : no mandatory (default value : ::scene2D::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType (LINEAR)) : 
    * Set the Y axis of this adaptor.
    *
    * \b zValue : no mandatory (default value : 0) : Set the zValue of this adaptor
    * (the higher the zValue, the higher the layer is).
    *
    * \b viewportUID : mandatory : a viewport that help us to manage the scaling of the graphic object
    *
    * \b showXScale : no mandatory : tells if the scale on the X axis has to be shown (value: "true" or "false")
    *
    * \b showYScale : no mandatory : tells if the scale on the Y axis has to be shown (value: "true" or "false")
    *
    */
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );

    /// Initialize the layer, set the pen style to DashLine and call the draw() function.
    SCENE2D_API void doStart()    throw ( ::fwTools::Failed );

    /// Do nothing.
    SCENE2D_API void doUpdate()    throw ( ::fwTools::Failed );

    /// Do nothing.
    SCENE2D_API void doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Do nothing.
    SCENE2D_API void doSwap()    throw ( ::fwTools::Failed );

    /// Clean the lines vector and remove the layer from the scene.
    SCENE2D_API void doStop()    throw ( ::fwTools::Failed );

    /// Manage the given events
    SCENE2D_API void processInteraction( SPTR(::scene2D::data::Event) _event );

private:

    void buildValues();

    ///
    void rescaleValues();

    ///
    double getStartVal();

    ///
    double getEndVal();

    /// Show or hide values which don't have enough space to be displayed correctly.
    void showHideScaleValues();

    /// Bounds of the grid and spacing values for each axis.
    float m_min, m_max;

    ///
    double m_interval;

    /// Tells if the unit of the axis must be displayed.
    bool m_showUnit;

    /// The unit of this values as text.
    QGraphicsSimpleTextItem* m_unit;

    /// The unit  of the axis (as text) that will be displayed.
    std::string m_displayedUnit;

    /// The layer.
    QGraphicsItemGroup* m_layer;

    // Specify where the axis must be aligned: left, right, top or bottom.
    // Left and right side axis are aligned/floating relatively to the view.
    std::string m_align;

    /// The font applied to grid's values.
    QFont m_font;

    /// The color applied to grid's values.
    QBrush m_brush;

    /// A vector containing QGraphicsItem-s representing the scale values of the axis.
    std::vector<QGraphicsItem*> m_values;

    /// Communication channel between this adaptor and the viewport.
    ::fwServices::ComChannelService::sptr m_comChannel;

    /// The viewport that help us to scale the graphic items which represent axis values.
    ::scene2D::data::Viewport::sptr m_viewport;

    /// The pen.
    QPen m_pen;

    // The step which defines when a value of the axis should be displayedn if there is not
    // enough space to display all the values.
    int m_step;

    /// Size of the font used for rendering grid values.
    float m_fontSize;
};



} // namespace adaptor
} // namespace scene2D


#endif // _SCENE2D_ADAPTOR_GRID2D_HPP_
