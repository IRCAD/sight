/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPVTKMESH_IMESHER_HPP__
#define __OPVTKMESH_IMESHER_HPP__

#include <fwServices/IService.hpp>

#include "opVTKMesh/config.hpp"

namespace opVTKMesh
{

/**
 * @brief   This interface defines mesher service API.
 * Does nothing particularly, to be implemented by mesher services.
 *
 * @class  IMesher
 *
 * @date   2009-2010.
 */
class OPVTKMESH_CLASS_API IMesher : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IMesher)(::fwServices::IService) );

    ///@brief IMesher constructor. Do nothing.
    OPVTKMESH_API IMesher();

    ///@brief IMesher destructor. Do nothing.
    OPVTKMESH_API virtual ~IMesher();

};

}

#endif // __OPVTKMESH_IMESHER_HPP__
