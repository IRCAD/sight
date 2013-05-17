/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>        // for SLM_TRACE_FUNC()

#include "gdcmIO/container/DicomSCoord.hpp"

namespace gdcmIO
{
namespace container
{

//------------------------------------------------------------------------------

DicomSCoord::DicomSCoord():
        m_graphicData(1,0),
        m_graphicType(""),
        m_context()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSCoord::~DicomSCoord()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

unsigned int DicomSCoord::getGraphicDataSize() const
{
    return this->m_graphicData.size();
}

//------------------------------------------------------------------------------

void DicomSCoord::setGraphicData(const std::vector< float > & a_coords)
{
    const unsigned int nbCoords = a_coords.size();

    this->m_graphicData.resize(nbCoords);
    for (unsigned int i = 0; i < nbCoords; ++i)
        this->m_graphicData[i] = toGraphicData<float>( a_coords[i] );
}

//------------------------------------------------------------------------------

void DicomSCoord::setGraphicData(const float * a_coords, const unsigned int length)
{
    this->m_graphicData.resize(length);
    for (unsigned int i = 0; i < length; ++i)
        this->m_graphicData[i] = toGraphicData<float>( a_coords[i] );
}

//------------------------------------------------------------------------------

float DicomSCoord::operator[](const unsigned int i) const
{
    return m_graphicData[i];
}

} //namespace container
} //namespace gdcmIO
