/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/IVtkRenderWindowInteractorManager.hpp"
#include "fwRenderVTK/factory/new.hpp"


namespace fwRenderVTK
{

namespace factory
{

::fwRenderVTK::IVtkRenderWindowInteractorManager::sptr New( const ::fwRenderVTK::registry::KeyType & classname )
{
    return ::fwRenderVTK::registry::get()->create(classname);
}

} // namespace factory

} // namespace fwRenderVTK


