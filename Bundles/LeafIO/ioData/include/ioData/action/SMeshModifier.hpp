/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODATA_ACTION_SMESHMODIFIER_HPP__
#define __IODATA_ACTION_SMESHMODIFIER_HPP__

#include "ioData/config.hpp"

#include <fwDataTools/AlgoMeshDeformation.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

namespace ioData
{
namespace action
{

/**
 * @brief   This action modifies a mesh using specified functor in configuration.
 * The purpose is to test all possibilities provide by the new mesh structure.

 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="ioData/action/SMeshModifier >
         <inout key="mesh" uid="..." />
         <config functor="ShakeMeshPoint" />
     </service>
    @endcode
 * @subsection InOut InOut
 * - \b mesh [::fwData::Mesh]: mesh to modify.
 * @subsection Configuration Configuration
 * - \b functor
 * Functor available :
 *  - ShakeMeshPoint
 *  - ColorizeMeshPoints
 *  - ColorizeMeshCells
 *  - ComputePointNormals
 *  - ComputeCellNormals
 *  - ShakePointNormals
 *  - ShakeCellNormals
 *  - MeshDeformation
 */
class IODATA_CLASS_API SMeshModifier : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SMeshModifier)(::fwGui::IActionSrv) );

    /**
     * @brief Constructor. Do nothing.
     */
    IODATA_API SMeshModifier() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    IODATA_API virtual ~SMeshModifier() noexcept;

protected:

    IODATA_API virtual void configuring();

    IODATA_API virtual void starting();

    IODATA_API virtual void stopping();

    /**
     * @brief Process the action: modifies the mesh using the selected functor.
     */
    IODATA_API virtual void updating();

    /**
     * @brief This method gives information about the class.
     */
    IODATA_API virtual void info(std::ostream& _sstream );

    ///@}

private:

    /// Functor name used to generate mesh.
    std::string m_functor;

    /// Algo use to animate a deformation on the mesh
    ::fwDataTools::AlgoMeshDeformation m_animator;
};

} // namespace action
} // namespace ioData

#endif /*__IODATA_ACTION_SMESHMODIFIER_HPP__*/
