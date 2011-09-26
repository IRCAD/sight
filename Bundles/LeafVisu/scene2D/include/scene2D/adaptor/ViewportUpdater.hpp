/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_VIEWPORTUPDATER_HPP_
#define _SCENE2D_VIEWPORTUPDATER_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{


/**
 * @brief Adaptor implementation that manages the camera on the view (by updating the viewport object).
 *
 * The ViewportUpdater updates the viewport of the scene it is attached to, by picking values from the
 * managed ::scene2D::data::Viewport object.
 *
 * Configuration example:
   @verbatim
   <adaptor id="viewportRangeUpdater" class="::scene2D::adaptor::ViewportUpdater" objectId="myViewport">
       <config xAxis="xAxis" yAxis="yAxis" zValue="7" />
   </adaptor>
   @endverbatim
 *
 * \b objectID  : the viewport object this updater takes values from.
 *
 * \b xAxis     : see ::scene2D::adaptor::IAdaptor
 *
 * \b yAxis     : see ::scene2D::adaptor::IAdaptor
 *
 * \b zValue    : see ::scene2D::adaptor::IAdaptor
 *
 * This adaptor is intended to be used with a ::scene2D::adaptor::ViewportRangeSelector adaptor.
 */
class ViewportUpdater : public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (ViewportUpdater)(::scene2D::adaptor::IAdaptor) ) ;
    ViewportUpdater() throw();
    ~ViewportUpdater() throw();

protected:
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );
    SCENE2D_API void doStart()    throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate()    throw ( ::fwTools::Failed );
    SCENE2D_API void doUpdate( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
    SCENE2D_API void doSwap()    throw ( ::fwTools::Failed );
    SCENE2D_API void doStop()    throw ( ::fwTools::Failed );

    SCENE2D_API void processInteraction( ::scene2D::data::Event::sptr _event );
};


}   // namespace adaptor
}   // namespace scene2D

#endif  // _SCENE2D_VIEWPORTUPDATER_HPP_

