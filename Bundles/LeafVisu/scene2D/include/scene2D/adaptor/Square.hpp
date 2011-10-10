/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_SQUARE_HPP_
#define _SCENE2D_ADAPTOR_SQUARE_HPP_

#include <QGraphicsItem>

#include "scene2D/adaptor/IAdaptor.hpp"
#include "scene2D/data/Coord.hpp"

namespace scene2D
{
namespace adaptor
{

class SCENE2D_CLASS_API Square : public ::scene2D::adaptor::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Square)(::scene2D::adaptor::IAdaptor) ) ;

    SCENE2D_API Square() throw();
    SCENE2D_API virtual ~Square() throw();

protected:

    SCENE2D_API void configuring() throw ( ::fwTools::Failed );
    SCENE2D_API void doStart() throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate() throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
    SCENE2D_API void doSwap() throw ( ::fwTools::Failed );
    SCENE2D_API void doStop() throw ( ::fwTools::Failed );

    SCENE2D_API void processInteraction( ::scene2D::data::Event::sptr _event );

    SCENE2D_API void setColor( std::string _color );
    SCENE2D_API bool coordViewIsInItem( const ::scene2D::data::Coord & coord, QGraphicsItem * item );
    SCENE2D_API ::scene2D::data::Coord coordViewToCoordItem( const ::scene2D::data::Coord & coord, QGraphicsItem * item );

private:

    ::scene2D::data::Coord m_coord;
    ::boost::uint32_t m_size;
    QColor m_color;
    QGraphicsItemGroup* m_layer;
    QGraphicsRectItem* m_rec;
    ::scene2D::data::Coord m_oldCoord;

    bool m_pointIsCaptured;
};

} // namespace adaptor
} // namespace scene2D


#endif // _SCENE2D_ADAPTOR_SQUARE_HPP_

