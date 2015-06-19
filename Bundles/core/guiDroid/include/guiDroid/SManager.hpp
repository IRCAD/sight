/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIDROID_SMANAGER_HPP__
#define __GUIDROID_SMANAGER_HPP__

#include "guiDroid/config.hpp"

#include <fwServices/IService.hpp>

namespace guiDroid
{

/**
 * @brief   Description of the class.
 * @class   SManager
 */
class GUIDROID_CLASS_API SManager : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SManager)(::fwServices::IService) );

    /// Constructor.
    GUIDROID_API SManager() throw();

    /// Destructor. Does nothing
    GUIDROID_API virtual ~SManager() throw();

protected:

    /**
     * @brief method description:
     * @verbatim
        <service uid="SManagerInstance" impl="::guiDroid::SManager" type="::fwServices::IService">
            <gui>
                <layout align="right|bottom" orientation="horizontal">
                    <widget sid="buttonUid" type="button" label="doSomething"/>
                </layout>
            </gui>
        </service>
       @endverbatim
     * - \b layout : layout definition and attributes.
     * - \b widget : widget definition and attributes.
     */
    GUIDROID_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// FILL ME.
    GUIDROID_API virtual void starting()  throw ( ::fwTools::Failed );

    /// FILL ME.
    GUIDROID_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// FILL ME.
    GUIDROID_API virtual void updating() throw ( ::fwTools::Failed );

};

} // guiDroid

#endif // __GUIDROID_SMANAGER_HPP__
