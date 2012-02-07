/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSERIESWRITER_HPP_
#define _GDCMIO_DICOMSERIESWRITER_HPP_

#include <fwData/Acquisition.hpp>

#include "gdcmIO/writer/DicomModuleWriter.hxx"
#include "gdcmIO/DicomInstance.hpp"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class write DICOM series Modules in a data set.
 *
 * @class   DicomSeriesWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSeriesWriter : public DicomModuleWriter< ::fwData::Acquisition >
{

public:

    GDCMIO_API DicomSeriesWriter();

    GDCMIO_API virtual ~DicomSeriesWriter();

    /**
     * @brief   Write General Series Module in a data set.
     *
     * @see     PS 3.3 C.7.3.1
     *
     * @note    setObject() have to be called before this method.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     */
    GDCMIO_API virtual void write(::gdcm::DataSet & a_gDs);


    GDCMIO_API fwGettersSettersDocMacro(SeriesID, seriesID, unsigned int, The index of series);

private:

    unsigned int    m_seriesID;  ///< Index of the series (eg : 1 or 2 or ...)
};

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMSERIESWRITER_HPP_ */
