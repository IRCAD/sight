/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "scene2D/config.hpp"

#include <fwRenderQt/data/Coord.hpp>
#include <fwRenderQt/IAdaptor.hpp>

#include <fwServices/macros.hpp>

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
 *
 * @section Slots Slots
 * -\b setDoubleParameter(double, std::string): set the double parameters 'x' and 'y'
 */
class SCENE2D_CLASS_API SSquare : public ::fwRenderQt::IAdaptor
{

public:

    fwCoreServiceMacro(SSquare, ::fwRenderQt::IAdaptor)

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
    static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    void setDoubleParameter(const double val, std::string key);
};

} // namespace adaptor
} // namespace scene2D
