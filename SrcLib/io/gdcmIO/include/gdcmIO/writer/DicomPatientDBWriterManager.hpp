/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMPATIENTDBWRITERMANAGER_HPP_
#define _GDCMIO_DICOMPATIENTDBWRITERMANAGER_HPP_

#include <fwTools/ProgressAdviser.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/location/Folder.hpp>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class manages patient writing, in DICOM file format.
 *
 * @class   DicomPatientDBWriterManager
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class DicomPatientDBWriterManager : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::PatientDB >,
                                    public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                                    public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro( (DicomPatientDBWriterManager)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::PatientDB >),
                                            (()),
                                            new  DicomPatientDBWriterManager
                                          );

    GDCMIO_API DicomPatientDBWriterManager();

    GDCMIO_API ~DicomPatientDBWriterManager();

    /**
     * @brief    Manage writing tools to save all patients.
     *
     * It launches DicomGlobalWriterManager for each patient.
     */
    GDCMIO_API void write();

    /**
     * Overriding
     * @brief   Do nothing
     */
    GDCMIO_API std::string extension();

};

}   // namespace writer

}   // namespace gdcmIO

#endif // _GDCMIO_DICOMPATIENTDBWRITERMANAGER_HPP_
