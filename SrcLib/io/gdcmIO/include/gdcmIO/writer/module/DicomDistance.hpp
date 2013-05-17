/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_MODULE_DICOMDISTANCE_HPP__
#define __GDCMIO_WRITER_MODULE_DICOMDISTANCE_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/writer/module/DicomModule.hxx"
#include "gdcmIO/container/DicomDistance.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

/**
 * @brief   This class define a writer of distances in DICOM form.
 *
 * It completes a data set with specific template.
 *
 * @note    Currently, the template is TID 1400 (taken from PS 3.16).
 *
 * @class   DicomDistance
 */
class GDCMIO_CLASS_API DicomDistance : public DicomModule< ::fwData::Image >
{

public :

    GDCMIO_API DicomDistance();

    GDCMIO_API virtual ~DicomDistance();

    /**
     * @brief   Write distance(s) into a data set.
     *
     * @param   a_gDs   Root of a data set for a document SR.
     */
    GDCMIO_API void write(::gdcm::DataSet & a_gDs) throw (::gdcmIO::exception::Failed);

private :

    /**
     * @brief   Write one distance in a separate Structured Reporting form.
     *
     * @param   idx     Index of distance.
     * @param   a_gSq   Sequence of items where new distance will be inserted.
     */
    void writeDistance(const unsigned int                               idx,
                       ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >  a_gSq);


    ::boost::shared_ptr< ::gdcmIO::container::DicomDistance>  m_distances;    ///< Distances container and converter.
};

} // namespace module
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_MODULE_DICOMDISTANCE_HPP__ */
