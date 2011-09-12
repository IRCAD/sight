/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKGDCMIO_DICOMPATIENTWRITER_HPP_
#define _VTKGDCMIO_DICOMPATIENTWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/Folder.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Patient.hpp>

#include "vtkGdcmIO/config.hpp"

namespace vtkGdcmIO
{

class DicomPatientWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Patient >,
                             public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((DicomPatientWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Patient>),
                                           (()),
                                           new  DicomPatientWriter
                                          );
    fwCoreAllowSharedFromThis();

    VTKGDCMIO_API DicomPatientWriter();

    VTKGDCMIO_API ~DicomPatientWriter();

    VTKGDCMIO_API void write();

    VTKGDCMIO_API std::string  extension();

};

} // namespace VTKGDCMIO

#endif // _VTKGDCMIO_DICOMPATIENTWRITER_HPP_
