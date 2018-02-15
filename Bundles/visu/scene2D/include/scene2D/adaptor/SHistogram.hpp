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
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="histogram" type="::scene2D::adaptor::SHistogram" autoConnect="yes">
       <in key="histogram" uid="..." />
       <inout key="point" uid="..." optional="yes" />
       <config xAxis="xAxis" yAxis="axeHistogramY" color="gray" opacity="0.25" zValue="5"/>
   </service>
   @endcode
 *
 * @subsection In In
 * - \b histogram [::fwData::Histogram]: histogram to display.
 *
 * @subsection In-Out In-Out
 * - \b point [::fwData::Point](optional): histogram point, used to show information at the current histogram index
 * pointed by the mouse.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b color (optional, default black) : the background color of the histogram
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 */
class SCENE2D_CLASS_API SHistogram : public ::fwRenderQt::IAdaptor
{
public:
    fwCoreServiceClassDefinitionsMacro( (SHistogram)( ::fwRenderQt::IAdaptor) );

    SCENE2D_API SHistogram() noexcept;
    SCENE2D_API virtual ~SHistogram() noexcept;

protected:
    SCENE2D_API void configuring() override;
    SCENE2D_API void starting() override;
    SCENE2D_API void updating() override;
    SCENE2D_API void stopping() override;

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event ) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect SHistogram::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

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

    // Graphics items contained into m_items are also added to this group item,
    // which is then added to the scene.
    // (This is the only graphic item which has to be added into the scene).
    QGraphicsItemGroup* m_layer;
};

}   // namespace adaptor

}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_SHISTOGRAM_HPP__

