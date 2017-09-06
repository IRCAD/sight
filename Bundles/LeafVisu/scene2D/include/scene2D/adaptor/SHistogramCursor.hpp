/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SHISTOGRAMCURSOR_HPP__
#define __SCENE2D_ADAPTOR_SHISTOGRAMCURSOR_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/data/Viewport.hpp>
#include <fwRenderQt/IAdaptor.hpp>

#include <QGraphicsTextItem>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief IAdaptor implementation to show a cursor on histogram pointed by mouse.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="histogram" type="::scene2D::adaptor::SHistogramCursor" autoConnect="yes">
       <in key="histogram" uid="..." />
       <in key="point"     uid="..." />
       <in key="viewport"  uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" color="gray" opacity="0.25" zValue="5"/>
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
 *    - \b color (optional, default black): inner color
 *    - \b borderColor (optional, default black): border color
 *    - \b pointSize (optional, default 6.0): point size
 *
 */
class SHistogramCursor : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SHistogramCursor)(::fwRenderQt::IAdaptor) );

    SCENE2D_API SHistogramCursor() noexcept;
    SCENE2D_API virtual ~SHistogramCursor() noexcept;

    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

protected:
    SCENE2D_API void configuring();
    SCENE2D_API void starting();
    SCENE2D_API void updating();
    SCENE2D_API void stopping();

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );

    /// Color used for graphic item's inner color
    QPen m_color;

    /// Color used for graphic item's border color
    QPen m_borderColor;

    // A graphics item that is located onto hsitogram's upper border and moves along this border
    // according to the position of mouse's cursor. The goal of this graphical index is to show
    // the associated value within the histogram pointed buy this index.
    QGraphicsEllipseItem* m_index;

    /// Coordinates of the current event.
    ::fwRenderQt::data::Coord m_coord;

    /// Point size
    float m_pointSize;

    /// The layer.
    QGraphicsItemGroup* m_layer;
};

}   // namespace adaptor
}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_SHISTOGRAMCURSOR_HPP__

