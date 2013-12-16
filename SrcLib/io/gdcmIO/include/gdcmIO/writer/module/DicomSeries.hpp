/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_MODULE_DICOMSERIES_HPP__
#define __GDCMIO_WRITER_MODULE_DICOMSERIES_HPP__

#include <fwMedData/Series.hpp>

#include "gdcmIO/writer/module/DicomModule.hxx"
#include "gdcmIO/container/DicomInstance.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

/**
 * @brief   This class write DICOM series Modules in a data set.
 *
 * @class   DicomSeries
 */
class GDCMIO_CLASS_API DicomSeries : public DicomModule< ::fwMedData::Series >
{

public:

    GDCMIO_API DicomSeries();

    GDCMIO_API virtual ~DicomSeries();

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

};

} // namespace module
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_MODULE_DICOMSERIES_HPP__ */
