/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SSCALEVALUES_HPP__
#define __SCENE2D_ADAPTOR_SSCALEVALUES_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief SScaleValues adaptor. Display values and units on the axis.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="scaleValues" class="::scene2D::adaptor::SScaleValues" autoConnect="yes">
        <in key="viewport"  uid="..." />
        <config xMin="-1100" xMax="750" yMin="0" yMax="1" xSpacing="100" ySpacing="0.1"
            color="lightGray" xAxis="xAxis" yAxis="yAxis" zValue="1"/>
   </service>
   @endcode
 *
 * @subsection In In
 * - \b viewport [::fwRenderQt::data::Viewport]: object listened to update adaptor.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b color (optional, default white): specify text color
 *    - \b xMin (mandatory): set the minimum X value to display.
 *    - \b xMax (mandatory): set the maximum X value to display.
 *    - \b yMin (mandatory): set the minimum Y value to display.
 *    - \b yMax (mandatory): set the maximum Y value to display.
 *    - \b interval (optional, default value 10): set the interval between 2 consecutive values.
 *    - \b fontSize (optional, default value : 8.): specify font size.
 *    - \b unit (optional): displayed unit.
 *    - \b showUnit (optional, default true): tells if unit has to be shown (value: "true" or "false")
 *    - \b align (mandatory): axis alignment, left', 'right', 'top' or 'bottom'
 */
class SCENE2D_CLASS_API SScaleValues : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SScaleValues)(::fwRenderQt::IAdaptor) );

    /// Constructor, set the x and y spacing to 10
    SCENE2D_API SScaleValues() noexcept;

    /// Basic destructor, do nothing
    SCENE2D_API virtual ~SScaleValues() noexcept;

protected:

    SCENE2D_API void configuring() override;

    /// Initialize the layer, set the pen style to DashLine and call the draw() function.
    SCENE2D_API void starting() override;

    /// Do nothing.
    SCENE2D_API void updating() override;

    /// Clean the lines vector and remove the layer from the scene.
    SCENE2D_API void stopping() override;

    /// Manage the given events
    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );

    SCENE2D_API KeyConnectionsMap getAutoConnections() const override;

private:

    void buildValues();

    void rescaleValues();

    double getStartVal();

    double getEndVal();

    /// Show or hide values which don't have enough space to be displayed correctly.
    void showHideScaleValues();

    /// Bounds of the grid and spacing values for each axis.
    float m_min, m_max;

    /// Interval between 2 consecutive values
    double m_interval;

    /// Tells if the unit of the axis must be displayed.
    bool m_showUnit;

    /// The unit of this values as text.
    QGraphicsSimpleTextItem* m_unit;

    /// The unit  of the axis (as text) that will be displayed.
    std::string m_displayedUnit;

    /// The layer.
    QGraphicsItemGroup* m_layer;

    /// Specify where the axis must be aligned: left, right, top or bottom.
    /// Left and right side axis are aligned/floating relatively to the view.
    std::string m_align;

    /// The font applied to grid's values.
    QFont m_font;

    /// The color applied to grid's values.
    QBrush m_brush;

    /// A vector containing QGraphicsItem-s representing the scale values of the axis.
    std::vector<QGraphicsItem*> m_values;

    /// The pen.
    QPen m_pen;

    /// The step which defines when a value of the axis should be displayed if there is not
    /// enough space to display all the values.
    int m_step;

    /// Size of the font used for rendering grid values.
    float m_fontSize;
};

} // namespace adaptor
} // namespace scene2D

#endif // __SCENE2D_ADAPTOR_SSCALEVALUES_HPP__
