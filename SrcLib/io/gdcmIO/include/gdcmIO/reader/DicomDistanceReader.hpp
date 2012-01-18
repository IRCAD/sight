/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMDISTANCEREADER_HPP_
#define _GDCMIO_DICOMDISTANCEREADER_HPP_

#include "gdcmIO/reader/DicomInstanceReader.hxx"
#include "gdcmIO/DicomDistance.hpp"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class define a reader of distances taken from a DICOM file.
 *
 * It search and read a specific template from the root of the document SR.
 * Currently, the template is TID 1400 (taken from PS 3.16).
 *
 * @class   DicomDistanceReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomDistanceReader : public DicomInstanceReader< fwData::Image >
{

public :

    GDCMIO_API DicomDistanceReader();

    GDCMIO_API ~DicomDistanceReader();

    /**
     * @brief   Read distance(s).
     *
     * @note    setReader() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw (::fwTools::Failed);

private :

    /**
     * @brief   Read distance(s) from its data set.
     * It will read only one distance with a non contiguous content.
     *
     * @param   a_ds            Data set of distance.
     * @param   a_continuity    Continuity of content.
     */
    void readDistance(const ::gdcm::DataSet & a_ds, const bool a_continuity = false) throw (::fwTools::Failed);

    /**
     * @brief   Check consistency of an SCOORD.
     * It verifies graphic type and data of the SCOORD are complied with a distance.
     * It will throw an exception with invalid content.
     *
     * @param   a_scoord    SCOORD to check.
     */
    void checkSCOORDContent(const SCoord & a_scoord) throw (::fwTools::Failed);


    ::boost::shared_ptr< DicomDistance > m_dicomDistances;  ///< Store and transform into fwData

};

}  // namespace reader

}  // namespace gdcmIO

#endif /* _GDCMIO_DICOMDISTANCEREADER_HPP_ */
