/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_VIEWPORTINTERACTOR_HPP__
#define __SCENE2D_ADAPTOR_VIEWPORTINTERACTOR_HPP__

#include "scene2D/adaptor/IAdaptor.hpp"

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

    ViewportInteractor() noexcept;

    ~ViewportInteractor() noexcept;

protected:

    SCENE2D_API void configuring();

    SCENE2D_API void doStart();

    SCENE2D_API void doUpdate();

    SCENE2D_API void doSwap();

    SCENE2D_API void doStop();

    SCENE2D_API void processInteraction( ::scene2D::data::Event::sptr _event );

    SCENE2D_API void zoom( bool zoomIn );

    bool m_viewportIsTranslated;
    ::scene2D::data::Coord m_lastCoordEvent;
};


}   // namespace adaptor
}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_VIEWPORTINTERACTOR_HPP__
