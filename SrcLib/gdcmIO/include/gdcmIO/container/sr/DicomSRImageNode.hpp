/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_SR_DICOMSRIMAGENODE_HPP__
#define __GDCMIO_CONTAINER_SR_DICOMSRIMAGENODE_HPP__

#include <string>
#include <vector>

#include <gdcmDataSet.h>

#include <fwData/macros.hpp>

#include "gdcmIO/container/DicomCodedAttribute.hpp"
#include "gdcmIO/container/sr/DicomSRNode.hpp"
#include "gdcmIO/config.hpp"


namespace gdcmIO
{
namespace container
{
namespace sr
{

/**
 * @class DicomSRImageNode
 * @brief This class defines an IMAGE node in a SR document
 */
class GDCMIO_CLASS_API DicomSRImageNode : public ::gdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    GDCMIO_API DicomSRImageNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship,
                                const std::string& sopClassUID, const std::string& sopInstanceUID, int frameNumber);

    /// Destructor
    GDCMIO_API virtual ~DicomSRImageNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    GDCMIO_API virtual void write(::gdcm::DataSet &dataset) const;

    GDCMIO_API fwGettersSettersDocMacro(SOPClassUID, sopClassUID, std::string, SOP Class UID);
    GDCMIO_API fwGettersSettersDocMacro(SOPInstanceUID, sopInstanceUID, std::string, SOP Instance UID);
    GDCMIO_API fwGettersSettersDocMacro(FrameNumber, frameNumber, int, Frame number);

protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /**
     * @brief Write a referenced SOP sequence
     * @see PS 3.3  Table C.18.3
     * @param[in] dataset Destination dataset
     */
    void writeReferencedSOPSequence(::gdcm::DataSet &dataset) const;

    /// Referenced SOP Class UID (0008,1150)
    std::string m_sopClassUID;

    /// Referenced SOP Instance UID (0008,1155)
    std::string m_sopInstanceUID;

    /// Referenced Frame Number (0008,1199)
    int m_frameNumber;
};

} //namespace sr
} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_SR_DICOMSRIMAGENODE_HPP__ */
