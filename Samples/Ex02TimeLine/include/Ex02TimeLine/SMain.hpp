/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EX02TIMELINE_SMAIN_HPP__
#define __EX02TIMELINE_SMAIN_HPP__

#include <fwServices/IService.hpp>

#include "Ex02TimeLine/config.hpp"

namespace Ex02TimeLine
{

/**
 * @brief   This service is just a hack to block the main thread. Services on the other threads can run.
 *          Without that, the application exits immediately since we have no GUI.
 */
class EX02TIMELINE_CLASS_API SMain : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SMain)(::fwServices::IService) );

    EX02TIMELINE_API SMain() noexcept;
    EX02TIMELINE_API virtual ~SMain() noexcept;

protected:

    /// Does nothing.
    virtual void starting();

    /// Does nothing.
    virtual void stopping();

    /// Does nothing.
    virtual void swapping();

    /// Contains one input instruction to block the main thread.
    virtual void updating();

    /// Does nothing.
    virtual void configuring();

private:

};

}  // namespace Ex02TimeLine

#endif  // __EX02TIMELINE_SMAIN_HPP__
