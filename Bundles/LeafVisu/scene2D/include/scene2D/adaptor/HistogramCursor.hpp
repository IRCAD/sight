/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_HISTOGRAMCURSOR_HPP_
#define _SCENE2D_HISTOGRAMCURSOR_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

#include <fwData/Point.hpp>

#include <QGraphicsTextItem>

#include "scene2D/data/Viewport.hpp"

namespace scene2D
{
namespace adaptor
{


/**
 * @brief
 *
 *
 * \b xAxis     : see ::scene2D::adaptor::IAdaptor
 *
 * \b yAxis     : see ::scene2D::adaptor::IAdaptor
 *
 * \b zValue    : see ::scene2D::adaptor::IAdaptor
 *
 * \b color     : inner color
 *
 * \b borderColor   : border color
 *
 * \b opacity   : from 0.0 to 1.0, default value is 0.8
 *
 * \b viewportUID : a viewport that help us to manage the scaling of the graphic object
 *
 */
class HistogramCursor : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (HistogramCursor)(::scene2D::adaptor::IAdaptor) ) ;
    SCENE2D_API HistogramCursor() throw();
    SCENE2D_API virtual ~HistogramCursor() throw();

protected:
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );
    SCENE2D_API void doStart()    throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate()    throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
    SCENE2D_API void doSwap()    throw ( ::fwTools::Failed );
    SCENE2D_API void doStop()    throw ( ::fwTools::Failed );

    SCENE2D_API void processInteraction( ::scene2D::data::Event::sptr _event );

    /// Color used for graphic item's inner color
    QPen m_color;

    /// Color used for graphic item's border color
    QPen m_borderColor;

    /// Opacity
    float m_opacity;

    // A graphics item that is located onto hsitogram's upper border and moves along this border
    // according to the position of mouse's cursor. The goal of this graphical index is to show
    // the associated value within the histogram pointed buy this index.
    QGraphicsEllipseItem* m_index;

    /// Coordinates of the current event.
    ::scene2D::data::Coord m_coord;

    ///
    float m_pointSize;

    /// The layer.
    QGraphicsItemGroup* m_layer;

    /// Curve point value at the current index of the histogram pointed by the mouse.
    std::string m_histogramPointUID;

private:

    ::fwServices::ComChannelService::sptr m_comChannel;

    ::scene2D::data::Viewport::sptr m_viewport;
};


}   // namespace adaptor
}   // namespace scene2D

#endif  // _SCENE2D_HISTOGRAMCURSOR_HPP_

