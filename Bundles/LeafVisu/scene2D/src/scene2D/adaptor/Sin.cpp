/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/Sin.hpp"
#include "scene2D/data/InitQtPen.hpp"

#include <fwServices/macros.hpp>
#include <fwData/Composite.hpp>


fwServicesRegisterMacro( ::scene2D::adaptor::IAdaptor, ::scene2D::adaptor::Sin, ::fwData::Composite );


namespace scene2D
{
namespace adaptor
{

Sin::Sin() throw() :
    m_xMin(0.f),
    m_xMax(0.f)
{
}

//---------------------------------------------------------------------------------------------------------

Sin::~Sin() throw()
{
}

//---------------------------------------------------------------------------------------------------------

void Sin::configuring() throw (::fwTools::Failed)
{
    assert(m_configuration->getName() == "config");

    std::string xMinStr(m_configuration->getAttributeValue("xMinBound"));
    std::string xMaxStr(m_configuration->getAttributeValue("xMaxBound"));
    std::istringstream xMinISS(xMinStr);
    std::istringstream xMaxISS(xMaxStr);
    float xMin;
    float xMax;
    xMinISS >> xMin;
    xMaxISS >> xMax;
    m_xMin = xMin;
    m_xMax = xMax;
}

//---------------------------------------------------------------------------------------------------------

void Sin::doStart()
{
}

//---------------------------------------------------------------------------------------------------------

void Sin::doUpdate()
{
}

//---------------------------------------------------------------------------------------------------------

void Sin::doSwap()
{
}

//---------------------------------------------------------------------------------------------------------

void Sin::doStop()
{
}

} // namespace adaptor
} // namespace scene2D



