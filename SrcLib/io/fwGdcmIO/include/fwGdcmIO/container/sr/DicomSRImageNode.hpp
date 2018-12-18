/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWGDCMIO_CONTAINER_SR_DICOMSRIMAGENODE_HPP__
#define __FWGDCMIO_CONTAINER_SR_DICOMSRIMAGENODE_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/container/sr/DicomSRNode.hpp"

#include <gdcmDataSet.h>

namespace fwGdcmIO
{
namespace container
{
namespace sr
{

/**
 * @brief This class defines an IMAGE node in a SR document
 */
class FWGDCMIO_CLASS_API DicomSRImageNode : public ::fwGdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    FWGDCMIO_API DicomSRImageNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship,
                                  const std::string& sopClassUID, const std::string& sopInstanceUID, int frameNumber);

    /// Destructor
    FWGDCMIO_API virtual ~DicomSRImageNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    FWGDCMIO_API virtual void write(::gdcm::DataSet& dataset) const;

    /// Get SOP Class UID
    const std::string& getSOPClassUID() const
    {
        return m_sopClassUID;
    }

    /// Set SOP Class UID
    void setSOPClassUID(const std::string& sopClassUID)
    {
        m_sopClassUID = sopClassUID;
    }

    /// Get SOP Instance UID
    const std::string& getSOPInstanceUID() const
    {
        return m_sopInstanceUID;
    }

    /// Set SOP Instance UID
    void setSOPInstanceUID(const std::string& sopInstanceUID)
    {
        m_sopInstanceUID = sopInstanceUID;
    }

    /// Get Frame number
    const int& getFrameNumber() const
    {
        return m_frameNumber;
    }

    /// Set Frame number
    void setFrameNumber(const int& frameNumber)
    {
        m_frameNumber = frameNumber;
    }

protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /**
     * @brief Write a referenced SOP sequence
     * @see PS 3.3  Table C.18.3
     * @param[in] dataset Destination dataset
     */
    void writeReferencedSOPSequence(::gdcm::DataSet& dataset) const;

    /// Referenced SOP Class UID (0008,1150)
    std::string m_sopClassUID;

    /// Referenced SOP Instance UID (0008,1155)
    std::string m_sopInstanceUID;

    /// Referenced Frame Number (0008,1199)
    int m_frameNumber;
};

} //namespace sr
} //namespace container
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_SR_DICOMSRIMAGENODE_HPP__ */
