/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMSCOORD_HPP__
#define __GDCMIO_CONTAINER_DICOMSCOORD_HPP__

#include <string>
#include <vector>
#include <cmath>

#include <fwData/macros.hpp>

#include "gdcmIO/config.hpp"
#include "gdcmIO/container/DicomCodedAttribute.hpp"

namespace gdcmIO
{
namespace container
{

/**
 * @class DicomSCoord
 * @brief This class defines Spatial coordinates.
 * @see PS 3.3 C.18.6
 */
class GDCMIO_CLASS_API DicomSCoord
{

public:

    typedef std::vector< float > GraphicDataContainerType;

    /// Constructor
    GDCMIO_API DicomSCoord();

    /// Destructor
    GDCMIO_API ~DicomSCoord();

    /// Return number of coordinates.
    GDCMIO_API unsigned int getGraphicDataSize() const;

    /**
     * @brief Set graphic data
     * @param[in] coords Coordinates
     */
    GDCMIO_API void setGraphicData(const GraphicDataContainerType &coords);

    /**
     * @brief Set graphic data
     * @param[in] coords Coordinates
     * @param[in] length Length
     */
    GDCMIO_API void setGraphicData(const float * coords, const unsigned int length);

    /**
     * @brief Access to graphic data at index.
     * @param[in] index Index of the data to access
     */
    GDCMIO_API float operator[](const unsigned int index) const;

    GDCMIO_API fwGettersMacro(GraphicData, graphicData, std::vector<float>);

    GDCMIO_API fwGettersSettersDocMacro(Context, context, DicomCodedAttribute,
            The SCOORD context (eg : Path, Path Vertex, ...));

    GDCMIO_API fwGettersSettersDocMacro(GraphicType, graphicType, std::string,
            The type of spatial coordinates (eg : "POINT", "CIRCLE"));

private :

    /**
     * @brief Convert a number into graphic data.
     * This method converts a floating coordinate into a number with one decimal.
     * This decimal has to be equal to 0 or 5 in order to refer to the side or the middle of a pixel.
     * @see PS 3.3 C.10.5.1.2
     * @param[in] coord Coordinate that must be converted
     */
    template <typename T>
    static float toGraphicData(const T & coord);

    /// The spatial coordinates (eg : [x1,y1,x2,y2,...]).
    GraphicDataContainerType m_graphicData;

    /// The SCOORD type (eg : POINT, POLYLINE, CIRCLE).
    std::string m_graphicType;

    /// The SCOORD context (eg : Path, Path Vertex, ...).
    DicomCodedAttribute m_context;

};

//------------------------------------------------------------------------------

template< typename T >
float DicomSCoord::toGraphicData(const T & coord)
{
    int integerPart = std::floor(coord);
    int sign = ((integerPart > 0) ? 1 : -1);
    float decimals = sign * (sign * coord - sign * integerPart); // <=> abs( abs(float) - abs(integer_part) )

    float result = static_cast<float>(integerPart);
    if (decimals > 0.75f)
    {
        result += sign;
    }
    else if (decimals > 0.25f)
    {
        result += sign * 0.5f;
    }

    return result;
}

} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMSCOORD_HPP__ */
