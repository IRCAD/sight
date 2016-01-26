/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_REGISTRY_MACROS_HPP__
#define __FWRENDERVTK_REGISTRY_MACROS_HPP__

#include <boost/preprocessor/cat.hpp>

#include "fwRenderVTK/IVtkRenderWindowInteractorManager.hpp"
#include "fwRenderVTK/registry/detail.hpp"

namespace fwRenderVTK
{
namespace registry
{

#define fwRenderVTKRegisterMacro( VTKRenderInteractorMngClassname, FunctorKey )                            \
    static ::fwRenderVTK::IVtkRenderWindowInteractorManager::Registrar< VTKRenderInteractorMngClassname >  \
    BOOST_PP_CAT( s__factory__record__, __LINE__) ( FunctorKey );

} // end namespace registry
} // end namespace fwRenderVTK

#endif /*__FWRENDERVTK_REGISTRY_MACROS_HPP__*/
