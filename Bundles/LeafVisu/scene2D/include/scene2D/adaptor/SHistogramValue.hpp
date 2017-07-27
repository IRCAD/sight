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
 *
 * Configuration example:
 *
   @code{.xml}
   <service uid="histogram" type="::scene2D::adaptor::SHistogramValue">
       <in key="histogram" uid="..." />
       <in key="point" uid="..." />
       <in key="viewport"  uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" color="gray" opacity="0.25" zValue="5"/>
   </service>
   @endcode
 * \b xAxis     : see ::fwRenderQt::IAdaptor
 *
 * \b yAxis     : see ::fwRenderQt::IAdaptor
 *
 * \b zValue    : see ::fwRenderQt::IAdaptor
 *
 * \b color     : inner color, default value is white
 *
 * \b fontSize  : size of the font used to display the current value, default value is 8
 *
 * \b opacity   : from 0.0 to 1.0, default value is 0.8
 *
 */
class SHistogramValue : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SHistogramValue)(::fwRenderQt::IAdaptor) );

    SCENE2D_API SHistogramValue() noexcept;
    SCENE2D_API virtual ~SHistogramValue() noexcept;

    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

protected:
    SCENE2D_API void configuring();
    SCENE2D_API void doStart();
    SCENE2D_API void doUpdate();
    SCENE2D_API void doSwap();
    SCENE2D_API void doStop();

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

