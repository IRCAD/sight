/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_SR_DICOMSRSCOORDNODE_HPP__
#define __GDCMIO_CONTAINER_SR_DICOMSRSCOORDNODE_HPP__

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

    /**
     * @brief Graphic type
     * @{ */
    const std::string& getGraphicType() const
    {
        return m_graphicType;
    }
    void setGraphicType(const std::string&  graphicType)
    {
        this->m_graphicType = graphicType;
    }
    /**  @} */

    /**
     * @brief Graphic data container
     * @{ */
    const GraphicDataContainerType& getGraphicDataContainer() const
    {
        return m_graphicDataContainer;
    }
    void setGraphicDataContainer(const GraphicDataContainerType&  graphicDataContainer)
    {
        this->m_graphicDataContainer = graphicDataContainer;
    }
    /**  @} */

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
