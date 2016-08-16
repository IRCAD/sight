/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicom/Plugin.hpp"

#include <fwDicomIOFilterQt/sorter/TagValueConfigurableSorter.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>

namespace ioDicom
{

using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("ioDicom::Plugin");

Plugin::~Plugin() throw()
{
    // Hack to force link with Qt filters
    ::fwDicomIOFilterQt::sorter::TagValueConfigurableSorter::sptr t =
        ::fwDicomIOFilterQt::sorter::TagValueConfigurableSorter::New();
}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
}

void Plugin::stop() throw()
{
}

} // namespace ioDicom
