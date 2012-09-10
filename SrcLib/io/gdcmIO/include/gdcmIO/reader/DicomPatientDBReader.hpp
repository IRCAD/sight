/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMPATIENTDBREADER_HPP_
#define _GDCMIO_DICOMPATIENTDBREADER_HPP_

#include <fwTools/ProgressAdviser.hpp>
#include <fwTools/Failed.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/PatientDB.hpp>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class adds patient(s) from DICOM file(s) to fwData::PatientDB.
 *
 * It will sort each patient from file(s) and launch DicomPatientReader.
 *
 * @class   DicomPatientDBReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomPatientDBReader :    public ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB >,
                                                 public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader > ,
                                                 public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro( (DicomPatientDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::PatientDB > ),
                                            (()),
                                            ::fwDataIO::reader::factory::New< DicomPatientDBReader >
                                             );

    /**
     * @brief   Launch patient(s) reading.
     * First, This method filters DICOM files.
     * Then, It adds all properly read patients.
     *
     * @note    setFolder() have to be called before this one.
     */
    GDCMIO_API void read() throw(::fwTools::Failed);

    GDCMIO_API DicomPatientDBReader(::fwDataIO::reader::IObjectReader::Key key);

    GDCMIO_API ~DicomPatientDBReader();
};

}  // namespace reader

}  // namespace gdcmIO

#endif /*_GDCMIO_DICOMPATIENTDBREADER_HPP_*/
