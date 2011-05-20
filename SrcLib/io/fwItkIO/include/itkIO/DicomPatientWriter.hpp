/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_DICOMPATIENTWRITER_HPP_
#define _FWITKIO_DICOMPATIENTWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/Folder.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Patient.hpp>

#include "itkIO/config.hpp"

namespace itkIO
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

    FWITKIO_API void write();

    FWITKIO_API std::string  extension();

protected:

    FWITKIO_API DicomPatientWriter();

    FWITKIO_API ~DicomPatientWriter();
};

} // namespace itkIO

#endif // _FWITKIO_DICOMPATIENTWRITER_HPP_
