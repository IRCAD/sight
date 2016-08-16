/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_REGISTRY_MACROS_HPP__
#define __FWDICOMIOFILTER_REGISTRY_MACROS_HPP__

#include "fwDicomIOFilter/registry/detail.hpp"

namespace fwDicomIOFilter
{
namespace registry
{

#define fwDicomIOFilterRegisterMacro( classname )                                  \
    static ::fwDicomIOFilter::IFilter::Registrar< classname > s__factory__record__ ## __LINE__;

} // end namespace registry
} // end namespace fwDicomIOFilter

#endif /*__FWDICOMIOFILTER_REGISTRY_MACROS_HPP__*/
