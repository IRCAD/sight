/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMDISTANCEWRITER_HPP_
#define _GDCMIO_DICOMDISTANCEWRITER_HPP_

#include <fwData/Image.hpp>

#include "gdcmIO/writer/DicomModuleWriter.hxx"
#include "gdcmIO/DicomDistance.hpp"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class define a writer of distances in DICOM form.
 *
 * It completes a data set with specific template.
 *
 * @note    Currently, the template is TID 1400 (taken from PS 3.16).
 *
 * @class   DicomDistanceWriter
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomDistanceWriter : public DicomModuleWriter< fwData::Image >
{

public :

    GDCMIO_API DicomDistanceWriter();

    GDCMIO_API virtual ~DicomDistanceWriter();

    /**
     * @brief   Write distance(s) into a data set.
     *
     * @param   a_gDs   Root of a data set for a document SR.
     */
    GDCMIO_API void write(::gdcm::DataSet & a_gDs) throw (::fwTools::Failed);

private :

    /**
     * @brief   Write one distance in a separate Structured Reporting form.
     *
     * @param   idx     Index of distance.
     * @param   a_gSq   Sequence of items where new distance will be inserted.
     */
    void writeDistance(const unsigned int                               idx,
                       ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >  a_gSq);


    ::boost::shared_ptr<DicomDistance>  m_distances;    ///< Distances container and converter.
};

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMDISTANCEWRITER_HPP_ */
