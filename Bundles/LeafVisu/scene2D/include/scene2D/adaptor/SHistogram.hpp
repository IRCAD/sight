/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SHISTOGRAM_HPP__
#define __SCENE2D_ADAPTOR_SHISTOGRAM_HPP__

#include "scene2D/config.hpp"

#include <fwData/Point.hpp>

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{

namespace adaptor
{

/**
 * @brief   IAdaptor implementation for histogram data.
 *
 * Configuration example:
 *
   @code{.xml}
   <service uid="histogram" type="::scene2D::adaptor::SHistogram">
       <in key="histogram" uid="histogramUID" />
       <inout key="point" uid="pointUID" optional="yes" />
       <config xAxis="xAxis" yAxis="axeHistogramY" color="gray" opacity="0.25" zValue="5"/>
   </service>
   @endcode
 *
 * \b color (mandatory)     : the background color of the histogram
 *
 * \b opacity (mandatory)   : the opacity of the histogram (from 0.0 to 1.0)
 *
 * \b xAxis                 : see ::fwRenderQt::IAdaptor
 *
 * \b yAxis                 : see ::fwRenderQt::IAdaptor
 *
 * \b zValue                : see ::scene1D::adaptor::IAdaptor
 */
class SCENE2D_CLASS_API SHistogram : public ::fwRenderQt::IAdaptor
{
public:
    fwCoreServiceClassDefinitionsMacro( (SHistogram)( ::fwRenderQt::IAdaptor) );

    SCENE2D_API SHistogram() noexcept;
    SCENE2D_API virtual ~SHistogram() noexcept;

protected:
    SCENE2D_API void configuring();
    SCENE2D_API void starting();
    SCENE2D_API void updating();
    SCENE2D_API void stopping();

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect SHistogram::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Ratio used for vertical scaling (default value: 1.1)
    static const float SCALE;

private:
    /// Update the value of m_ordinateValueUID according to the value pointed by mouse cursor.
    void updateCurrentPoint( ::fwRenderQt::data::Event& _event, const ::fwData::Point::sptr& point );

    /// Color used for graphic item's inner and border color
    QPen m_color;

    /// Opacity
    float m_opacity;

    /// Current vertical scaling ratio
    float m_scale;

    // Graphis items contained into m_items are also added to this group item,
    // which is then added to the scene.
    // (This is the only graphic item which has to be added into the scene).
    QGraphicsItemGroup* m_layer;
};

}   // namespace adaptor

}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_SHISTOGRAM_HPP__

