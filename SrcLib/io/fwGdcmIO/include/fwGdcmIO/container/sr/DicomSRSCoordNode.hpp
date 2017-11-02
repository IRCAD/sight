/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_CONTAINER_SR_DICOMSRSCOORDNODE_HPP__
#define __FWGDCMIO_CONTAINER_SR_DICOMSRSCOORDNODE_HPP__

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
 * @brief This class defines a SCOORD node in a SR document
 */
class FWGDCMIO_CLASS_API DicomSRSCoordNode : public ::fwGdcmIO::container::sr::DicomSRNode
{
public:

    typedef std::vector<float> GraphicDataContainerType;

    /// Constructor
    FWGDCMIO_API DicomSRSCoordNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship,
                                   const std::string& graphicType, GraphicDataContainerType graphicDataContainer);

    /// Destructor
    FWGDCMIO_API virtual ~DicomSRSCoordNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    FWGDCMIO_API virtual void write(::gdcm::DataSet& dataset) const;

    /// Get Graphic type
    const std::string& getGraphicType() const
    {
        return m_graphicType;
    }

    ///Set Graphic type
    void setGraphicType(const std::string& graphicType)
    {
        m_graphicType = graphicType;
    }

    /// Get Graphic data container
    const GraphicDataContainerType& getGraphicDataContainer() const
    {
        return m_graphicDataContainer;
    }

    ///Set Graphic data container
    void setGraphicDataContainer(const GraphicDataContainerType& graphicDataContainer)
    {
        m_graphicDataContainer = graphicDataContainer;
    }

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
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_SR_DICOMSRSCOORDNODE_HPP__ */
