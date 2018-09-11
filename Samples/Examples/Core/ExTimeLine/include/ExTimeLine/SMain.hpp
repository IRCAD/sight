/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ExTimeLine/config.hpp"

#include <fwServices/IService.hpp>

namespace ExTimeLine
{

/**
 * @brief   This service is just a hack to block the main thread. Services on the other threads can run.
 *          Without that, the application exits immediately since we have no GUI.
 */
class EXTIMELINE_CLASS_API SMain : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro( (SMain)(::fwServices::IService) );

    EXTIMELINE_API SMain() noexcept;
    EXTIMELINE_API virtual ~SMain() noexcept;

protected:

    /// Does nothing.
    virtual void starting() override;

    /// Does nothing.
    virtual void stopping() override;

    /// Does nothing.
    virtual void swapping() override;

    /// Contains one input instruction to block the main thread.
    virtual void updating() override;

    /// Does nothing.
    virtual void configuring() override;

private:

};

}  // namespace ExTimeLine
