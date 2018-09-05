/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiVisuOgre/config.hpp"

#include <fwRenderOgre/IParameter.hpp>

#include <fwServices/IService.hpp>

namespace uiVisuOgre
{

namespace helper
{

class ParameterEditor
{
public:

    /**
     * @brief Create a config that will be used to expose an shader parameter adaptor in a SParameters.
     *
     * @param[in] _adaptor adaptor to bind.
     * @param[in] _paramSrv editor service.
     * @param[inout] _connections helper that stores connections between the editor and the adaptors.
     */
    UIVISUOGRE_API static ::fwServices::IService::ConfigType
    createConfig(const ::fwRenderOgre::IParameter::csptr& _adaptor,
                 const ::fwServices::IService::csptr& _paramSrv,
                 ::fwCom::helper::SigSlotConnection& _connections);
};

} // namespace helper

} // namespace uiVisuOgre
