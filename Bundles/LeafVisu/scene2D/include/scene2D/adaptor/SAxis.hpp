/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SAXIS_HPP__
#define __SCENE2D_ADAPTOR_SAXIS_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{

namespace adaptor
{

/**
 * @brief Render an axis on the scene2d.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="axis" type="::scene2D::adaptor::SAxis" autoConnect="yes">
      <in key="viewport" uid="..." />
      <config xAxis="xAxis" yAxis="yAxis" zValue="4" color="cyan" align="left" min="0" max="1" step="0.1" />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b viewport [::fwRenderQt::data::Viewport]: object listened to update axis.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b align (mandatory): axis alignment, left', 'right', 'top' or 'bottom'
 *    - \b min (mandatory): min axis bound
 *    - \b max (mandatory): max axis bound
 *    - \b tickSize (optional, default=1.0): axis tick size
 *    - \b interval (optional, default=1.0): axis interval
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 *    - \b color (optional, default black): color of the axis
 */
class SCENE2D_CLASS_API SAxis : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SAxis)(::fwRenderQt::IAdaptor) );

    SCENE2D_API SAxis() noexcept;
    SCENE2D_API virtual ~SAxis() noexcept;

protected:

    void starting() override;
    void stopping() override;
    void updating() override;
    void configuring() override;

    /// Manage the given events
    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event ) override;

    SCENE2D_API KeyConnectionsMap getAutoConnections() const override;
private:

    /// Builds axis graphic items.
    void buildAxis();

    ///
    double getStartVal();

    ///
    double getEndVal();

    // Specify where the axis must be aligned: left, right, top or bottom.
    // Left and right side axis are aligned/floating relatively to the view.
    std::string m_align;

    /// The required interval between two consecutive values of the axis.
    float m_interval;

    /// Minimal value of the axis.
    float m_min;

    /// Maximal value of the axis.
    float m_max;

    /// Size of a tick.
    float m_tickSize;

    /// Color.
    QPen m_color;

    /// Tells if the line of the axis must be displayed in addition to ticks.
    bool m_showLine;

    // A layer that gathers all the graphic items.
    QGraphicsItemGroup* m_layer;

    /// The line of the axis.
    QGraphicsLineItem* m_line;

    /// The graphic items that refer to ticks of the axis.
    std::vector< QGraphicsLineItem* > m_ticks;
};

}   // namespace adaptor
}   // namespace scene2D

#endif //__SCENE2D_ADAPTOR_SAXIS_HPP__

