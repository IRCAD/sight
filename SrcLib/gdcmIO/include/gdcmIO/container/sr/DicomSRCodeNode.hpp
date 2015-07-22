/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_SR_DICOMSRCODENODE_HPP__
#define __GDCMIO_CONTAINER_SR_DICOMSRCODENODE_HPP__

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
 * @class DicomSRCodeNode
 * @brief This class defines a CODE node in a SR document
 */
class GDCMIO_CLASS_API DicomSRCodeNode : public ::gdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    GDCMIO_API DicomSRCodeNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship,
                               const DicomCodedAttribute& codedEntry);

    /// Destructor
    GDCMIO_API virtual ~DicomSRCodeNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    GDCMIO_API virtual void write(::gdcm::DataSet &dataset) const;

    GDCMIO_API fwGettersSettersDocMacro(CodedEntry, codedEntry, DicomCodedAttribute, Coded Entry);

protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /// Coded Entry
    DicomCodedAttribute m_codedEntry;

};

} //namespace sr
} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_SR_DICOMSRCODENODE_HPP__ */
