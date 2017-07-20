/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_PLANESELECTOR_HPP__
#define __VISUVTKADAPTOR_PLANESELECTOR_HPP__

#ifndef ANDROID

#include "visuVTKAdaptor/config.hpp"

#include <fwData/PlaneList.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

namespace visuVTKAdaptor
{

/**
 * @brief Manage the current selected plan
 */
class VISUVTKADAPTOR_CLASS_API PlaneSelector : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (PlaneSelector)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API PlaneSelector() noexcept;

    VISUVTKADAPTOR_API virtual ~PlaneSelector() noexcept;

    VISUVTKADAPTOR_API void selectObject( ::fwData::Object::sptr object );

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();

    VISUVTKADAPTOR_API void doUpdate();

    ::fwData::Object::wptr m_currentObject;
};

} //namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_PLANESELECTOR_HPP__
