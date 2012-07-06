/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/Patient.hpp>
#include <fwData/PatientDB.hpp>

#include <io/IReader.hpp>

#include "ioGdcm/Plugin.hpp"
#include "ioGdcm/DicomPatientDBReaderService.hpp"
#include "ioGdcm/DicomPatientDBWriterService.hpp"
#include "ioGdcm/DicomPatientWriterService.hpp"

namespace ioGdcm
{
using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("ioGdcm::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
}

void Plugin::stop() throw()
{}

} // namespace operators
