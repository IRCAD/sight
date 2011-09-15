/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_HISTOGRAM_HPP_
#define _SCENE2D_ADAPTOR_HISTOGRAM_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

#include <fwData/Histogram.hpp>


namespace scene2D
{

namespace adaptor
{

/**
 * @brief   IAdaptor implementation for histogram data.
 *
 * Configuration example:
 *
   @verbatim
   <adaptor id="histogram" class="::scene2D::adaptor::Histogram" objectId="myHistogram">
       <config xAxis="xAxis" yAxis="axeHistogramY" color="gray" opacity="0.25" zValue="5"/>
   </adaptor>
   @endverbatim
 * 
 * \b color (mandatory)     : the background color of the histogram
 *
 * \b opacity (mandatory)   : the opacity of the histogram (from 0.0 to 1.0)
 *
 * \b xAxis                 : see ::scene2D::adaptor::IAdaptor
 *
 * \b yAxis                 : see ::scene2D::adaptor::IAdaptor
 *
 * \b zValue                : see ::scene1D::adaptor::IAdaptor
 */
class SCENE2D_CLASS_API Histogram : public ::scene2D::adaptor::IAdaptor
{
    public:
        fwCoreServiceClassDefinitionsMacro( (Histogram)( ::scene2D::adaptor::IAdaptor) );

        SCENE2D_API Histogram() throw();
        SCENE2D_API virtual ~Histogram() throw();

    protected:
        SCENE2D_API void configuring()  throw ( ::fwTools::Failed );
        SCENE2D_API void doStart()      throw ( ::fwTools::Failed );
        SCENE2D_API void doUpdate()     throw ( ::fwTools::Failed );
        SCENE2D_API void doUpdate( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
        SCENE2D_API void doSwap()       throw ( ::fwTools::Failed );
        SCENE2D_API void doStop()       throw ( ::fwTools::Failed );

        SCENE2D_API void processInteraction( SPTR(::scene2D::data::Event) _event );

        /// Ratio used for vertical scaling (default value: 1.1)
        static const float SCALE;
        
    private:
        /// Update the value of m_ordinateValueUID according to the value pointed by mouse cursor.
        void updateCurrentPoint( ::scene2D::data::Event::sptr _event );
        
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

        // Curve point at the current index of the histogram pointed by the mouse. This adaptor looks for 
        // mouse move events: when the mouse cursor is onto the histogram, the corresponding point of the 
        // histogram is informed into the object this UID is all about.
        std::string m_histogramPointUID;
};


}   // namespace adaptor

}   // namespace scene2D

#endif  // _SCENE2D_ADAPTOR_HISTOGRAM_HPP_

