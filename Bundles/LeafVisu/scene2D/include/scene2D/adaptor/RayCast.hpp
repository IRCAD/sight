/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_RAYCAST_HPP_
#define _SCENE2D_ADAPTOR_RAYCAST_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Vector.hpp>
#include <fwData/Integer.hpp>
#include <fwData/PointList.hpp>

#include <fwCore/macros.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/helper/Composite.hpp>
#include <fwComEd/PointListMsg.hpp>

#include <fwServices/Base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwGuiQt/container/QtContainer.hpp>



#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>


namespace scene2D
{
namespace adaptor
{


class SCENE2D_CLASS_API RayCast : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (RayCast)(::scene2D::adaptor::IAdaptor) ) ;

    /// Basic constructor, do nothing.
    SCENE2D_API RayCast() throw();

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~RayCast() throw();

protected:
    /**
    * @brief Configuring the RayCast adaptor.
    *
    * Example of configuration
    @verbatim
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
    
    SCENE2D_API void syncCrosses();


private:

    void addCross(fwData::Point::NewSptr _point, float _size , QGraphicsItemGroup* _layer, QPen _pen );
    void addCross(fwData::PointList::NewSptr _point, float _size , QGraphicsItemGroup* _layer, QPen _pen );

    void addLine(fwData::Point::NewSptr _point1, fwData::Point::NewSptr _point2 , QGraphicsItemGroup* _layer, QPen _pen );

    fwData::Point::sptr listPointAveragePosition(fwData::PointList::sptr _listPoint);

    fwData::PointList::sptr findPointRayon(
        fwData::Point::sptr _leftPoint, fwData::Point::sptr _rightPoint, fwData::Point::sptr _centerPoint, int _rayNbr );

    fwVec3d axialToSagittal(fwVec3d _vecIn);

    fwData::Point::sptr axialToSagittal(fwData::Point::sptr _pointIn);

    void addLinesFromPoint(fwData::Point::NewSptr _point, fwData::PointList::NewSptr _pointList , QGraphicsItemGroup* _layer, QPen _pen );

    
    /// The pen.
    QPen m_pen;

    int m_nbRayon;

    float m_crossSize;

    // Related objects
    std::string m_imageKey;
    std::string m_targetPointsKey;

    QGraphicsLineItem* m_baseLine;

    /// The layer.
    QGraphicsItemGroup* m_layer;

    std::vector< QGraphicsLineItem* > m_lines;
    std::vector< QGraphicsLineItem* > m_crosses;

    bool m_isMoving;

    /**
     * @brief Reference the start point of a drag and drop move.
     */
    QPointF m_pos;

    /**
     * @brief Zoom factor applied at each mouse scroll.
     */
    float m_scaleRatio;

    std::pair<double, double> m_widgetOffset;

};



} // namespace adaptor
} // namespace scene2D


#endif // _SCENE2D_ADAPTOR_RAYCAST_HPP_

