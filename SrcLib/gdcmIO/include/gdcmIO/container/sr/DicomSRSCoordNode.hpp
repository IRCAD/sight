/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_SR_DICOMSRSCOORDNODE_HPP__
#define __GDCMIO_CONTAINER_SR_DICOMSRSCOORDNODE_HPP__

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
 * @class DicomSRSCoordNode
 * @brief This class defines a SCOORD node in a SR document
 */
class GDCMIO_CLASS_API DicomSRSCoordNode : public ::gdcmIO::container::sr::DicomSRNode
{
public:

    typedef std::vector<float> GraphicDataContainerType;

    /// Constructor
    GDCMIO_API DicomSRSCoordNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship,
                                 const std::string& graphicType, GraphicDataContainerType graphicDataContainer);

    /// Destructor
    GDCMIO_API virtual ~DicomSRSCoordNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    GDCMIO_API virtual void write(::gdcm::DataSet &dataset) const;

    GDCMIO_API fwGettersSettersDocMacro(GraphicType, graphicType, std::string, Graphic type);
    GDCMIO_API fwGettersSettersDocMacro(GraphicDataContainer, graphicDataContainer, GraphicDataContainerType,
                                        Graphic data container);

protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /// Graphic Type
    std::string m_graphicType;

    /// Graphic Data
    GraphicDataContainerType m_graphicDataContainer;
};

} //namespace sr
} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_SR_DICOMSRSCOORDNODE_HPP__ */
