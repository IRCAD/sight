/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoFilter/config.hpp"

#include <fwServices/IService.hpp>

namespace videoFilter
{
/**
 * @brief   This interface defines video filters.
 *          Must be implemented for services that filter videos.
 *
 */
class VIDEOFILTER_CLASS_API IVideoFilter : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro((IVideoFilter)(::fwServices::IService));

    /**
     * @brief Constructor.
     */
    IVideoFilter() noexcept
    {
    }

    /**
     * @brief Destructor.
     */
    virtual ~IVideoFilter() noexcept
    {
    }

};

} //namespace videoFilter
