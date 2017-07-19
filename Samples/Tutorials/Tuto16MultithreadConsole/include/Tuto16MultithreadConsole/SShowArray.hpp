/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO16MULTITHREADCONSOLE_SSHOWARRAY_HPP__
#define __TUTO16MULTITHREADCONSOLE_SSHOWARRAY_HPP__

#include "Tuto16MultithreadConsole/config.hpp"

#include <fwServices/IController.hpp>

namespace Tuto16MultithreadConsole
{

/**
 * @brief   This service displays the array values in the log.
 *
 * @note To see the information, you need to activate log level "info".
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::Tuto16MultithreadConsole::SShowArray">
            <input key="array" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b array [::fwData::Array]: array which values should be incremented.
 */
/**
 * @brief This service displays the array values.
 *
 */
class TUTO16MULTITHREADCONSOLE_CLASS_API SShowArray : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SShowArray)(::fwServices::IController) );

    TUTO16MULTITHREADCONSOLE_API SShowArray() noexcept;
    TUTO16MULTITHREADCONSOLE_API virtual ~SShowArray() noexcept;

protected:

    /// Do nothing
    virtual void starting();

    /// Do nothing
    virtual void stopping();

    /// Display the array values
    virtual void updating();

    /// Do nothing
    virtual void configuring();
};

}  // namespace Tuto16MultithreadConsole

#endif  // __TUTO16MULTITHREADCONSOLE_SSHOWARRAY_HPP__

