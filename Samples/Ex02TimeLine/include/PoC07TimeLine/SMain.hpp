/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __POC07TIMELINE_SMAIN_HPP__
#define __POC07TIMELINE_SMAIN_HPP__

#include <fwServices/IService.hpp>

#include "PoC07TimeLine/config.hpp"

namespace PoC07TimeLine
{

/**
 * @brief   This service is just a hack to block the main thread. Services on the other threads can run.
 *          Without that, the application exits immediately since we have no GUI.
 */
class POC07TIMELINE_CLASS_API SMain : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SMain)(::fwServices::IService) );

    POC07TIMELINE_API SMain() throw();
    POC07TIMELINE_API virtual ~SMain() throw();

protected:

    /// Does nothing.
    virtual void starting() throw(::fwTools::Failed);

    /// Does nothing.
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing.
    virtual void swapping() throw ( ::fwTools::Failed );

    /// Contains one input instruction to block the main thread.
    virtual void updating() throw(::fwTools::Failed);

    /// Does nothing.
    virtual void configuring() throw(fwTools::Failed);

private:

};

}  // namespace PoC07TimeLine

#endif  // __POC07TIMELINE_SMAIN_HPP__
