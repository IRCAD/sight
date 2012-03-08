/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_REGISTRY_MACRO_HPP_
#define _FWDATA_REGISTRY_MACRO_HPP_

#include <fwCore/Demangler.hpp>
#include <fwCore/concept_checks.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/registry/macros.hpp"

namespace fwData
{
namespace registry
{

#define fwDataRegisterMacro( ObjectImpl )                                                                                               \
    REGISTER_BINDING_BYCLASSNAME( ::fwData::Object, ObjectImpl, ObjectImpl );                                                     \
    REGISTER_BINDING_BYCLASSNAME_ID( ::fwTools::Object, ObjectImpl, ObjectImpl, BOOST_PP_CAT( fwToolObj,  __LINE__ )  );

} // end namespace registry
} // end namespace fwData

#endif /*MACRO_HPP_*/
