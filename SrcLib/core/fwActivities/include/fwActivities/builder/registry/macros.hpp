/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_BUILDER_REGISTRY_MACRO_HPP__
#define __FWACTIVITIES_BUILDER_REGISTRY_MACRO_HPP__

#include <boost/preprocessor/cat.hpp>

#include "fwActivities/IBuilder.hpp"
#include "fwActivities/builder/registry/detail.hpp"

namespace fwActivities
{
namespace builder
{
namespace registry
{

#define fwActivitiesBuilderRegisterMacro( BuilderClassname, BuilderFunctorKey )   \
    static ::fwActivities::IBuilder::Registrar< BuilderClassname >                \
    BOOST_PP_CAT( s__factory__record__, __LINE__) ( BuilderFunctorKey );

} // end namespace registry
} // end namespace builder
} // end namespace fwActivities

#endif /*__FWACTIVITIES_BUILDER_REGISTRY_MACRO_HPP__*/
