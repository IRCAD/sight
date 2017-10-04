/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SSQUARE_HPP__
#define __SCENE2D_ADAPTOR_SSQUARE_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/data/Coord.hpp>
#include <fwRenderQt/IAdaptor.hpp>

#include <QGraphicsItem>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief Create a square on the scene2D
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="square" type="::scene2D::adaptor::SSquare">
      <config x="20" y="20" size="30" color="blue" zValue="3" />
   </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b x (mandatory): specify x square coordinate
 *    - \b y (mandatory): specify y square coordinate
 *    - \b size (mandatory): specify size of the square
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b color (optional, default black): color of the square
 */
class SCENE2D_CLASS_API SSquare : public ::fwRenderQt::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SSquare)(::fwRenderQt::IAdaptor) );

    SCENE2D_API SSquare() noexcept;
    SCENE2D_API virtual ~SSquare() noexcept;

protected:

    SCENE2D_API void configuring() override;
    SCENE2D_API void starting() override;
    SCENE2D_API void updating() override;
    SCENE2D_API void stopping() override;

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event ) override;

    SCENE2D_API void setColor(const std::string& _color );
    SCENE2D_API bool coordViewIsInItem( const ::fwRenderQt::data::Coord& coord, QGraphicsItem* item );
    SCENE2D_API ::fwRenderQt::data::Coord coordViewToCoordItem( const ::fwRenderQt::data::Coord& coord,
                                                                QGraphicsItem* item );

private:

    ::fwRenderQt::data::Coord m_coord;
    std::uint32_t m_size;
    QColor m_color;
    QGraphicsItemGroup* m_layer;
    QGraphicsRectItem* m_rec;
    ::fwRenderQt::data::Coord m_oldCoord;

    bool m_pointIsCaptured;
};

} // namespace adaptor
} // namespace scene2D

#endif // __SCENE2D_ADAPTOR_SSQUARE_HPP__

