/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_DICOMDISTANCE_HPP__
#define __GDCMIO_READER_DICOM_DICOMDISTANCE_HPP__

#include <boost/shared_ptr.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/reader/dicom/IDicomInstance.hxx"
#include "gdcmIO/container/DicomDistance.hpp"
#include "gdcmIO/container/DicomSCoord.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
 * @brief   This class define a reader of distances taken from a DICOM file.
 *
 * It search and read a specific template from the root of the document SR.
 * Currently, the template is TID 1400 (taken from PS 3.16).
 *
 * @class   DicomDistance
 */
class GDCMIO_CLASS_API DicomDistance : public IDicomInstance< fwData::Image >
{

public :

    GDCMIO_API DicomDistance();

    GDCMIO_API ~DicomDistance();

    /**
     * @brief   Read distance(s).
     *
     * @note    setReader() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw (::gdcmIO::exception::Failed);

private :

    /**
     * @brief   Read distance(s) from its data set.
     * It will read only one distance with a non contiguous content.
     *
     * @param   a_ds            Data set of distance.
     * @param   a_continuity    Continuity of content.
     */
    void readDistance(const ::gdcm::DataSet & a_ds, const bool a_continuity = false) throw (::gdcmIO::exception::Failed);

    /**
     * @brief   Check consistency of an SCOORD.
     * It verifies graphic type and data of the SCOORD are complied with a distance.
     * It will throw an exception with invalid content.
     *
     * @param   a_scoord    SCOORD to check.
     */
    void checkSCOORDContent(const ::gdcmIO::container::DicomSCoord& a_scoord) throw (::gdcmIO::exception::Failed);


    ::boost::shared_ptr< ::gdcmIO::container::DicomDistance > m_dicomDistances;  ///< Store and transform into fwData

};

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_DICOM_DICOMDISTANCE_HPP__ */
