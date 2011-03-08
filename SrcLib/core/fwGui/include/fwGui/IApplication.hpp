/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IAPPLICATION_HPP_
#define _FWGUI_IAPPLICATION_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the application manipulation API.
 * @class   IApplication.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IApplication : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IApplication)(::fwCore::BaseObject) )

    /// Constructor. Do nothing.
    FWGUI_API IApplication();
    /// Destructor. Do nothing.
    FWGUI_API virtual ~IApplication();

    /// Tells the application to exit with a returncode
    FWGUI_API virtual void exit( int returncode ) = 0;

    typedef std::string FactoryRegistryKeyType;
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

};

} // namespace fwGui

#endif /*_FWGUI_IAPPLICATION_HPP_*/

