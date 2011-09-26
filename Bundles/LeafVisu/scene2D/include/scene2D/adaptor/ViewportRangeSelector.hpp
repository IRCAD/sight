/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_VIEWPORTRANGESELECTOR_HPP_
#define _SCENE2D_VIEWPORTRANGESELECTOR_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{


/**
 * @brief <p>The viewport range selector adaptor allows to select a delimited range of a viewport.
 * It uses a graphical delimiter (called shutter) that can be moved from both left to right 
 * and right to left directions (in those cases, shutter's width is changing).</p>
 *
 * <p>Clicking onto the approximative center of the shutter allows the user to change its position, 
 * according to mouse's cursor position (width won't change).</p>
 *
 * <p>Clicking onto the approximative left/right (respectively) border of the shutter allows the 
 * user to change the width of the shutter: the right/left (respectively) border doesn't move
 * during resizing.</p>
 *
 * <p>Each change onto the shutter will cause this adaptor to update the managed
 * ::scene2D::data::Viewport object.</p>
 *
 * Configuration example:
   @verbatim
   <adaptor id="viewSelector" class="::scene2D::adaptor::ViewportRangeSelector" objectId="myViewport">
       <config xAxis="xAxis" yAxis="yAxis" zValue="5" />
   </adaptor>
   @endverbatim
 * 
 * \b objectId  :   the viewport object that is updated by this adaptor
 *
 * \b xAxis     : see ::scene2D::adaptor::IAdaptor
 *
 * \b yAxis     : see ::scene2D::adaptor::IAdaptor
 *
 * \b zValue    : see ::scene2D::adaptor::IAdaptor
 *
 * \b initialX  : initial position of the shutter on the X axis
 *
 * \b initialWidth  : initial width of the shutter
 *
 * This adaptor is intended to be used with a ::scene2D::adaptor::ViewportUpdater adaptor.
 */
class ViewportRangeSelector : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (ViewportRangeSelector)(::scene2D::adaptor::IAdaptor) ) ;
    ViewportRangeSelector() throw();
    ~ViewportRangeSelector() throw();

protected:
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );
    SCENE2D_API void doStart()    throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate()    throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
    SCENE2D_API void doSwap()    throw ( ::fwTools::Failed );
    SCENE2D_API void doStop()    throw ( ::fwTools::Failed );

    SCENE2D_API void processInteraction( ::scene2D::data::Event::sptr _event );

    /// Update the viewport object according to the current state of the shutter (coordinates are
    /// mapped from the scene).
    SCENE2D_API void updateViewportFromShutter( double x, double y, double width, double height );
 
    QGraphicsRectItem* m_shutter;
    bool m_isLeftInteracting;                       // interaction onto shutter's left border
    bool m_isRightInteracting;                      // interaction onto shutter's right border
    bool m_isInteracting;                           // interaction onto the whole shutter
    ::scene2D::data::Coord m_dragStartPoint;        // dragging origin point
    ::scene2D::data::Coord m_dragStartShutterPos;   // shutter position when dragging starts

    /// A spacing value for an easier picking onto shutter borders
    const int m_clickCatchRange;
        
    /// (This is the only graphic item which has to be added into the scene).
    QGraphicsItemGroup* m_layer;

    /// Initial position of the shutter on the X axis
    float m_initialX;

    /// Initial width of the shutter
    float m_initialWidth;

private:

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's left border
    bool mouseOnShutterLeft( ::scene2D::data::Coord _coord);
    
    /// Tells if the mouse cursor is at the good position to start interacting on shutter's right border
    bool mouseOnShutterRight( ::scene2D::data::Coord _coord);
    
    /// Tells if the mouse cursor is at the good position to start interacting on shutter's middle part
    bool mouseOnShutterMiddle( ::scene2D::data::Coord _coord);
};


}   // namespace adaptor
}   // namespace scene2D

#endif  // _SCENE2D_VIEWPORTRANGESELECTOR_HPP_

