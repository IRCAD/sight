/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SHISTOGRAMVALUE_HPP__
#define __SCENE2D_ADAPTOR_SHISTOGRAMVALUE_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/data/Viewport.hpp>
#include <fwRenderQt/IAdaptor.hpp>

#include <QGraphicsTextItem>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief IAdaptor implementation to show the histogram values clicked by mouse.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="histogram" type="::scene2D::adaptor::SHistogramValue" autoConnect="yes">
       <in key="histogram" uid="..." />
       <in key="point" uid="..." />
       <in key="viewport"  uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" color="gray" opacity="0.25" zValue="5" fontSize="8"/>
   </service>
   @endcode
 *
 * @subsection In In
 * - \b histogram [::fwData::Histogram]: histogram to display.
 * - \b point [::fwData::Point]: histogram point, used to show information at the current histogram index pointed by the
 * mouse.
 * - \b viewport [::fwRenderQt::data::Viewport]: object listened to update adaptor.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 *    - \b color (optional, default white): color of the text
 *    - \b fontSize (optional, default 8): size of the font used to display the current value
 *
 */
class SHistogramValue : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SHistogramValue)(::fwRenderQt::IAdaptor) );

    SCENE2D_API SHistogramValue() noexcept;
    SCENE2D_API virtual ~SHistogramValue() noexcept;

    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:
    SCENE2D_API void configuring() override;
    SCENE2D_API void starting() override;
    SCENE2D_API void updating() override;
    SCENE2D_API void stopping() override;

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );

    /// Color used for graphic item's inner color
    QPen m_color;

    /// An item which display the current values of the associated histogram pointed by this cursor.
    QGraphicsSimpleTextItem* m_text;

    /// If true, display the currently pointed intensity value
    bool m_isInteracting;

    ///
    QFont m_font;

    /// Coordinates of the current event.
    ::fwRenderQt::data::Coord m_coord;

    /// Size of the font used for rendering the current value of this tracker.
    float m_fontSize;

    /// The layer.
    QGraphicsItemGroup* m_layer;

};

}   // namespace adaptor
}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_SHISTOGRAMVALUE_HPP__

