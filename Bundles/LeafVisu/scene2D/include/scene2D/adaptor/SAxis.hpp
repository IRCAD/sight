/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SAXIS_HPP__
#define __SCENE2D_ADAPTOR_SAXIS_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{

namespace adaptor
{

class SCENE2D_CLASS_API SAxis : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SAxis)(::fwRenderQt::IAdaptor) );

    SCENE2D_API SAxis() noexcept;
    SCENE2D_API virtual ~SAxis() noexcept;

protected:

    void doStart();
    void doStop();
    void doSwap();
    void doUpdate();
    void configuring();

    /// Manage the given events
    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );

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
};

}   // namespace adaptor
}   // namespace scene2D

#endif //__SCENE2D_ADAPTOR_SAXIS_HPP__

