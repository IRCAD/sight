/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_MESHMSG_HPP_
#define _FWCOMED_MESHMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{

/**
 * @brief   Object event message specialized for Mesh : store modification information
 * @class   MeshMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API MeshMsg : public ::fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MeshMsg)(::fwServices::ObjectMsg), (( )) , new MeshMsg );

    /// Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_MESH;

    /// Constructor. Do nothing.
    FWCOMED_API MeshMsg() throw();

    /// Destructor. Do nothing.
    FWCOMED_API virtual ~MeshMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_MESHMSG_HPP_

