/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>

#include <fwDicomIOFilterQt/sorter/TagValueConfigurableSorter.hpp>

#include "ioDicomExt/Plugin.hpp"

namespace ioDicomExt
{
static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("ioDicomExt::Plugin");

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
{}

} // namespace ioDicomExt
