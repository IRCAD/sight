/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IODATA_ACTION_MESHGENERATORSRV_HPP_
#define _IODATA_ACTION_MESHGENERATORSRV_HPP_

#include <fwServices/IService.hpp>

#include <fwGui/IActionSrv.hpp>

#include "ioData/export.hpp"

namespace ioData
{
namespace action
{

/**
 * @brief   This action generates a mesh using specified functor in configuration.
 * The purpose is to test all possibilities provide by the new mesh structure.
 * @class   MeshGeneratorSrv.
 * @author  IRCAD (Research and Development Team).

 * @date    2011.
 */
class IODATA_CLASS_API MeshGeneratorSrv : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (MeshGeneratorSrv)(::fwGui::IActionSrv) ) ;

    /**
    * @brief Constructor. Do nothing.
    */
    IODATA_API MeshGeneratorSrv() throw() ;

    /**
    * @brief Destructor. Do nothing.
    */
    IODATA_API virtual ~MeshGeneratorSrv() throw() ;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
      * @brief Configure the functor used to generate the mesh.
      *
      * Example of configuration :
      * @verbatim
         <service ... >
             <config functor="GenTriangle" />
         </service>
        @endverbatim
      * - functor available : "GenTriangle", "GenQuad" and "GenTriangleQuad"
      */
    IODATA_API virtual void configuring() throw( ::fwTools::Failed ) ;

    IODATA_API virtual void starting() throw(::fwTools::Failed);

    IODATA_API virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief This method is used to update services on notification. Do nothing.
     */
    IODATA_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /**
     * @brief Process the action: generate the mesh using the selected functor.
     * GenTriangle functor generate a random mesh with cells are triangle.
     * GenQuad functor generate a random mesh with cells are quad.
     * GenTriangleQuad functor generate a random mesh which contains triangle and quad cells.
     */
    IODATA_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief This method gives information about the class.
     */
    IODATA_API virtual void info(std::ostream &_sstream ) ;

    ///@}

private:

    /// Functor name used to generate mesh.
    std::string m_functor;
};


} // namespace action
} // namespace ioData


#endif /*_IODATA_ACTION_MESHGENERATORSRV_HPP_*/
