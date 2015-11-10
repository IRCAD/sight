/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_SR_DICOMSRCONTAINERNODE_HPP__
#define __GDCMIO_CONTAINER_SR_DICOMSRCONTAINERNODE_HPP__

#include "gdcmIO/container/DicomCodedAttribute.hpp"
#include "gdcmIO/container/sr/DicomSRNode.hpp"
#include "gdcmIO/config.hpp"

#include <gdcmDataSet.h>

#include <string>
#include <vector>


namespace gdcmIO
{
namespace container
{
namespace sr
{

/**
 * @class DicomSRContainerNode
 * @brief This class defines a CONTAINER node in a SR document
 */
class GDCMIO_CLASS_API DicomSRContainerNode : public ::gdcmIO::container::sr::DicomSRNode
{
public:

    /// Constructor
    GDCMIO_API DicomSRContainerNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship = "");

    /// Destructor
    GDCMIO_API virtual ~DicomSRContainerNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    GDCMIO_API virtual void write(::gdcm::DataSet &dataset) const;

};

} //namespace sr
} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_SR_DICOMSRCONTAINERNODE_HPP__ */
