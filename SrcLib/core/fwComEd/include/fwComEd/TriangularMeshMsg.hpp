/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TRIANGULARMESHMSG_HPP_
#define _FWCOMED_TRIANGULARMESHMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for TriangularMesh : store modification information
 * @class   TriangularMeshMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API TriangularMeshMsg : public ::fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (TriangularMeshMsg)(::fwServices::ObjectMsg), (( )) , new TriangularMeshMsg );

    /// Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_MESH;

    /// Constructor. Do nothing.
    FWCOMED_API TriangularMeshMsg() throw();

    /// Destructor. Do nothing.
    FWCOMED_API virtual ~TriangularMeshMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_TRIANGULARMESHMSG_HPP_

