/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_HELPER_POINTLIST_HPP__
#define __FWDATATOOLS_HELPER_POINTLIST_HPP__

#include "fwDataTools/config.hpp"

#include <fwData/Array.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

namespace fwDataTools
{
namespace helper
{

/**
 * @brief Defines a helper to modify a ::fwData::PointList.
 */
class FWDATATOOLS_CLASS_API PointList
{

public:

    /// Constructor. Does nothing.
    FWDATATOOLS_API PointList();

    /// Destructor. Does nothing.
    FWDATATOOLS_API ~PointList();

    /// Computes the point-to-point distance between 2 pointlists
    /// Return an array of the size of one the pointlists (they must have the same size)
    FWDATATOOLS_API static ::fwData::Array::sptr
    computeDistance(::fwData::PointList::sptr pointList1, ::fwData::PointList::sptr pointList2);

    /// Associate 2 point lists
    /// Take 2 point lists as input and re-order the second one, so that the points at the
    /// same index on both lists are the closest to each other
    /// The points in each point lists will be transformed by the associated matrix
    FWDATATOOLS_API static void associatePointLists(const ::fwData::PointList::sptr pointList1,
                                                    ::fwData::TransformationMatrix3D::csptr matrix1,
                                                    ::fwData::PointList::sptr pointList2,
                                                    ::fwData::TransformationMatrix3D::csptr matrix2);

};

} // namespace helper
} // namespace fwDataTools

#endif // __FWDATATOOLS_HELPER_POINTLIST_HPP__
