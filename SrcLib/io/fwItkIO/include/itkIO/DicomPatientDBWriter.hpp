/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_DICOMPATIENTDBWRITER_HPP_
#define _FWITKIO_DICOMPATIENTDBWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/Folder.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/PatientDB.hpp>

#include "itkIO/config.hpp"

namespace itkIO
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

    FWITKIO_API void write();

    FWITKIO_API std::string  extension();

protected:

    FWITKIO_API DicomPatientDBWriter();

    FWITKIO_API ~DicomPatientDBWriter();
};

} // namespace itkIO

#endif // _FWITKIO_DICOMPATIENTDBWRITER_HPP_
