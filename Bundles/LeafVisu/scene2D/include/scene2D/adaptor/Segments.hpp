/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_SEGMENTS_HPP_
#define _SCENE2D_ADAPTOR_SEGMENTS_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

#include <QLineF>
#include <QPointF>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>

#include <fwServices/Base.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

namespace scene2D
{
namespace adaptor
{


class SCENE2D_CLASS_API Segments : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Segments)(::scene2D::adaptor::IAdaptor) ) ;

    /// Basic constructor, do nothing.
    SCENE2D_API Segments() throw();

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~Segments() throw();

protected:
    /**
    * @brief Configuring the Segments adaptor.
    *
    * Example of configuration
    * @verbatim
    @endverbatim
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

        /**
     * @brief Implemented from ::scene2D::adaptor::IAdaptor.
     *
     * Catch mouse and keyboard events and process them.
     *
     * @param _event an event from the scene 2D (keyboard/mouse event)
     */
    SCENE2D_API void processInteraction( ::scene2D::data::Event::sptr _event );

    SCENE2D_API void syncLines();

private:

    bool m_isMoving;

    /**
     * @brief Reference the start point of a drag and drop move.
     */
    QPointF m_pos;

    /**
     * @brief Zoom factor applied at each mouse scroll.
     */
    float m_scaleRatio;

    /// The pen.
    QPen m_pen;

    /// The layer.
    QGraphicsItemGroup* m_layer;

    //vector de ligne
    std::vector < QLineF > m_vecQLine;

    std::pair<double, double> m_widgetOffset;

    std::vector< QGraphicsLineItem* > m_graphicLines;
};



} // namespace adaptor
} // namespace scene2D


#endif // _SCENE2D_ADAPTOR_SEGMENTS_HPP_

