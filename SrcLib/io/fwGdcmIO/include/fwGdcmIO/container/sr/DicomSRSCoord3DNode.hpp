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

#ifndef __FWGDCMIO_CONTAINER_SR_DICOMSRSCOORD3DNODE_HPP__
#define __FWGDCMIO_CONTAINER_SR_DICOMSRSCOORD3DNODE_HPP__

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
 * @brief This class defines a SCOORD3D node in a SR document
 */
class FWGDCMIO_CLASS_API DicomSRSCoord3DNode : public ::fwGdcmIO::container::sr::DicomSRNode
{
public:

    typedef std::vector<float> GraphicDataContainerType;

    /// Constructor
    FWGDCMIO_API DicomSRSCoord3DNode(const DicomCodedAttribute& codedAttribute,
                                     const std::string& relationship,
                                     const std::string& graphicType,
                                     const GraphicDataContainerType graphicDataContainer,
                                     const std::string& frameOfReferenceUID);

    /// Destructor
    FWGDCMIO_API virtual ~DicomSRSCoord3DNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    FWGDCMIO_API virtual void write(::gdcm::DataSet& dataset) const;

    /// Get Frame of reference UID
    const std::string& getFrameOfReferenceUID() const
    {
        return m_frameOfReferenceUID;
    }

    ///Set Frame of reference UID
    void setFrameOfReferenceUID(const std::string& frameOfReferenceUID)
    {
        m_frameOfReferenceUID = frameOfReferenceUID;
    }

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

    /// Referenced Frame of Reference UID
    std::string m_frameOfReferenceUID;

    /// Graphic Type
    std::string m_graphicType;

    /// Graphic Data
    GraphicDataContainerType m_graphicDataContainer;
};

} //namespace sr
} //namespace container
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_SR_DICOMSRSCOORD3DNODE_HPP__ */
