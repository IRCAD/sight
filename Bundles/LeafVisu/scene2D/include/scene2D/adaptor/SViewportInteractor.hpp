/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SVIEWPORTINTERACTOR_HPP__
#define __SCENE2D_ADAPTOR_SVIEWPORTINTERACTOR_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief Adaptor implementation that manages the camera on the view.
 */
class SViewportInteractor : public ::fwRenderQt::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SViewportInteractor)(::fwRenderQt::IAdaptor) );

    SViewportInteractor() noexcept;

    ~SViewportInteractor() noexcept;

protected:

    SCENE2D_API void configuring();

    SCENE2D_API void starting();

    SCENE2D_API void updating();

    SCENE2D_API void stopping();

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );

    SCENE2D_API void zoom( bool zoomIn );

private:

    bool m_viewportIsTranslated;
    ::fwRenderQt::data::Coord m_lastCoordEvent;
};

}   // namespace adaptor
}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_SVIEWPORTINTERACTOR_HPP__
