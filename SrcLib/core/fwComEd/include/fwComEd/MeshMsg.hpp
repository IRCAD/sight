/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_MESHMSG_HPP__
#define __FWCOMED_MESHMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"

namespace fwComEd
{

/**
 * @brief   Object event message specialized for Mesh : store modification information
 * @class   MeshMsg
 *
 * @date    2011.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API MeshMsg : public ::fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MeshMsg)(::fwServices::ObjectMsg), (( )),
                                            ::fwServices::factory::message::New< MeshMsg > );

    /// Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_MESH;
    FWCOMED_API static std::string VERTEX_MODIFIED;
    FWCOMED_API static std::string POINT_COLORS_MODIFIED;
    FWCOMED_API static std::string CELL_COLORS_MODIFIED;
    FWCOMED_API static std::string POINT_NORMALS_MODIFIED;
    FWCOMED_API static std::string CELL_NORMALS_MODIFIED;
    FWCOMED_API static std::string POINT_TEXCOORDS_MODIFIED;
    FWCOMED_API static std::string CELL_TEXCOORDS_MODIFIED;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API MeshMsg(::fwServices::ObjectMsg::Key key);

    /// Destructor. Do nothing.
    FWCOMED_API virtual ~MeshMsg() throw();

};

} // namespace fwComEd

#endif //__FWCOMED_MESHMSG_HPP__

