/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKGDCMIO_DICOMPATIENTDBWRITER_HPP_
#define _VTKGDCMIO_DICOMPATIENTDBWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/Folder.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/PatientDB.hpp>

#include "vtkGdcmIO/config.hpp"

namespace vtkGdcmIO
{

class DicomPatientDBWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::PatientDB >,
                             public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((DicomPatientDBWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::PatientDB>),
                                           (()),
                                           new  DicomPatientDBWriter
                                          );
    fwCoreAllowSharedFromThis();


    VTKGDCMIO_API void write();

    VTKGDCMIO_API std::string  extension();

protected:

    VTKGDCMIO_API DicomPatientDBWriter();

    VTKGDCMIO_API ~DicomPatientDBWriter();
};

} // namespace vtkGdcmIO

#endif // _VTKGDCMIO_DICOMPATIENTDBWRITER_HPP_
