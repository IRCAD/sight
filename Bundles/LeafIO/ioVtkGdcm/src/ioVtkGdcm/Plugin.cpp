/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// ioVtkGdcm - copyright (C) 2008 IRCAD.
//
// author   Jean-Baptiste Fasquel


#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>
#include <io/IReader.hpp>
#include <fwData/Image.hpp>
#include <fwData/Patient.hpp>
#include <fwData/PatientDB.hpp>

#include <fwServices/macros.hpp>

#include "ioVtkGdcm/Plugin.hpp"
#include "ioVtkGdcm/DicomPatientDBReaderService.hpp"
#include "ioVtkGdcm/DicomPatientDBWriterService.hpp"
#include "ioVtkGdcm/DicomPatientWriterService.hpp"
#include "ioVtkGdcm/DicomImageWriterService.hpp"

namespace ioVtkGdcm
{
using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("ioVtkGdcm::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
}

void Plugin::stop() throw()
{}

} // namespace operators
