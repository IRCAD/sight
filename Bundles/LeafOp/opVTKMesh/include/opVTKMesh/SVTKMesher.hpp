/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPVTKMESH_SVTKMESHER_HPP_
#define _OPVTKMESH_SVTKMESHER_HPP_

#include <string>

#include "opVTKMesh/IMesher.hpp"
#include "opVTKMesh/config.hpp"

namespace opVTKMesh
{

class OPVTKMESH_CLASS_API SVTKMesher : public ::opVTKMesh::IMesher
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SVTKMesher)(::opVTKMesh::IMesher) ) ;

    OPVTKMESH_API SVTKMesher() throw() ;

    OPVTKMESH_API virtual ~SVTKMesher() throw() ;

protected:

    OPVTKMESH_API virtual void starting() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual void stopping() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual void receiving( ::fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );

    /**
     * @brief Declare the configuration to define percent of reduction, image source and ModelSeries destination.
     *
     * Example of this service configuration
     * @verbatim
       <service impl="::opVTKMesh::SVTKMesher" type="::opVTKMesh::IMesher">
           <config>
                <percentReduction>80</percentReduction>
                <image>IMAGE_KEY</image>
                <modelSeries>MODELSERIES_KEY</modelSeries>
            </config>
       </service>
        @endverbatim
     */
    OPVTKMESH_API virtual void configuring() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual void updating() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual  void info ( std::ostream &_sstream ) ;

private :

    unsigned int m_reduction;
    std::string m_imageKey;
    std::string m_modelSeriesKey;

};

} // namespace opVTKMesh


#endif // _OPVTKMESH_SVTKMESHER_HPP_
