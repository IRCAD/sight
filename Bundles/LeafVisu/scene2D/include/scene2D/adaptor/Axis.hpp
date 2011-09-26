/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_AXIS_HPP_
#define _SCENE2D_AXIS_HPP_

#include <scene2D/config.hpp>
#include <scene2D/adaptor/IAdaptor.hpp>

namespace scene2D
{

namespace adaptor
{


class SCENE2D_CLASS_API Axis : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Axis)(::scene2D::adaptor::IAdaptor) ) ;

    SCENE2D_API Axis() throw();
    SCENE2D_API virtual ~Axis() throw();

protected:

    void doStart() throw( ::fwTools::Failed);
    void doStop() throw( ::fwTools::Failed);
    void doSwap() throw( ::fwTools::Failed);
    void doUpdate() throw( ::fwTools::Failed);
    void doUpdate( ::fwServices::ObjectMsg::csptr _msg) throw( ::fwTools::Failed);
    void configuring() throw( ::fwTools::Failed);

    /// Manage the given events
    SCENE2D_API void processInteraction( SPTR(::scene2D::data::Event) _event );

private:

    /// Builds axis graphic items.
    void buildAxis();

    ///
    double getStartVal();

    ///
    double getEndVal();

    // Specify where the axis must be aligned: left, right, top or bottom.
    // Left and right side axis are aligned/floating relatively to the view.
    std::string m_align;

    /// The required interval between two consecutive values of the axis.
    float m_interval;

    /// Minimal value of the axis.
    float m_min;

    /// Maximal value of the axis.
    float m_max;

    /// Size of a tick.
    float m_tickSize;

    /// Color.
    QPen m_color;

    /// Tells if the line of the axis must be displayed in addition to ticks.
    bool m_showLine;

    // A layer that gathers all the graphic items.
    QGraphicsItemGroup* m_layer;

    /// The line of the axis.
    QGraphicsLineItem* m_line;

    /// The graphic items that refer to ticks of the axis.
    std::vector< QGraphicsLineItem* > m_ticks;

    /// Communication channel between this adaptor and the viewport.
    ::fwServices::ComChannelService::sptr m_comChannel;

    /// The viewport that help us to scale the axis.
    ::scene2D::data::Viewport::sptr m_viewport;
};


}   // namespace adaptor

}   // namespace scene2D

#endif

