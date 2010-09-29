/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Region.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Region , ::fwData::Region);
namespace fwData
{
Region::Region()
{
}

//------------------------------------------------------------------------------

Region::Region(boost::uint8_t  _dim):
m_vOrigin(_dim,0),
m_vSize(_dim,0)
{
}

//------------------------------------------------------------------------------

Region::~Region()
{
}


//------------------------------------------------------------------------------
Region::sptr Region::RegionFactory(boost::uint8_t _dim)
{
    sptr region = ::fwData::Region::New();
    region->m_vOrigin = OriginType(_dim,0);
    region->m_vSize = SizeType(_dim,0);
    return region;
}

//------------------------------------------------------------------------------
//
//std::vector<double> & Region::origin()
//{
//  return m_vOrigin;
//}
//
////------------------------------------------------------------------------------
//
//const std::vector<double> & Region::origin() const
//{
//  return m_vOrigin;
//}
//
////------------------------------------------------------------------------------
//
//std::vector<boost::int32_t>  & Region::size()
//{
//  return m_vSize;
//}
//
////------------------------------------------------------------------------------
//
//const std::vector<boost::int32_t>  & Region::size() const
//{
//  return m_vSize;
//}

//------------------------------------------------------------------------------

} // namespace fwData
