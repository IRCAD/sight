/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_FWCONTAINER_HPP_
#define _FWGUI_FWCONTAINER_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the generic container for IHM.
 * @class   fwContainer.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API fwContainer : public ::fwTools::Object
{

public:

    fwCoreServiceClassDefinitionsMacro ( (fwContainer)(::fwTools::Object) ) ;

    FWGUI_API virtual void clean() = 0;
    FWGUI_API virtual void destroyContainer() = 0;

};

} // namespace fwGui

#endif /*_FWGUI_FWCONTAINER_HPP_*/


