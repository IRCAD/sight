/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_VIEWPORTINTERACTOR_HPP__
#define __SCENE2D_ADAPTOR_VIEWPORTINTERACTOR_HPP__

#include <scene2D/adaptor/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{


/**
 * @brief Adaptor implementation that manages the camera on the view.
 */
class ViewportInteractor : public ::scene2D::adaptor::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ViewportInteractor)(::scene2D::adaptor::IAdaptor) );

    ViewportInteractor() throw();

    ~ViewportInteractor() throw();

protected:

    SCENE2D_API void configuring() throw ( ::fwTools::Failed );

    SCENE2D_API void doStart()    throw ( ::fwTools::Failed );

    SCENE2D_API void doUpdate()    throw ( ::fwTools::Failed );

    SCENE2D_API void doReceive( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    SCENE2D_API void doSwap()    throw ( ::fwTools::Failed );

    SCENE2D_API void doStop()    throw ( ::fwTools::Failed );

    SCENE2D_API void processInteraction( ::scene2D::data::Event::sptr _event );

    SCENE2D_API void zoom( bool zoomIn );

    bool m_viewportIsTranslated;
    ::scene2D::data::Coord m_lastCoordEvent;
};


}   // namespace adaptor
}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_VIEWPORTINTERACTOR_HPP__
