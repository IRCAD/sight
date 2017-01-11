/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_CONTAINER_SR_DICOMSRCODENODE_HPP__
#define __FWGDCMIO_CONTAINER_SR_DICOMSRCODENODE_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/container/sr/DicomSRNode.hpp"

#include <gdcmDataSet.h>

#include <string>
#include <vector>

namespace fwGdcmIO
{
namespace container
{
namespace sr
{

/**
 * @brief This class defines a CODE node in a SR document
 */
class FWGDCMIO_CLASS_API DicomSRCodeNode : public ::fwGdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    FWGDCMIO_API DicomSRCodeNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship,
                                 const DicomCodedAttribute& codedEntry);

    /// Destructor
    FWGDCMIO_API virtual ~DicomSRCodeNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    FWGDCMIO_API virtual void write(::gdcm::DataSet& dataset) const;

    /// Get Coded Entry
    const DicomCodedAttribute& getCodedEntry() const
    {
        return m_codedEntry;
    }

    /// Set Coded Entry
    void setCodedEntry(const DicomCodedAttribute& codedEntry)
    {
        m_codedEntry = codedEntry;
    }


protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /// Coded Entry
    DicomCodedAttribute m_codedEntry;

};

} //namespace sr
} //namespace container
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_SR_DICOMSRCODENODE_HPP__ */
