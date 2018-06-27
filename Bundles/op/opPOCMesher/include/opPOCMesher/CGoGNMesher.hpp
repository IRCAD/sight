/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "opPOCMesher/config.hpp"

#include <fwCom/Slots.hpp>

#include <fwServices/IOperator.hpp>

#include <string>

namespace opPOCMesher
{

/**
 * @brief Service used to generate a mesh from an image.
 * Example of this service configuration
 * @code{.xml}
   <service type="::opPOCMesher::CGoGNMesher" >
        <in key="image" uid="..." />
        <out key="modelSeries" uid="..." />
       <config>
            <!-- Optionnal -->
            <valueMin>1</valueMin>
            <valueMax>255</valueMax>
            <radius>5</radius>
            <adapt>5</adapt>
            <faces>5</faces>
            <percentage>true</percentage>
            <closing>true</closing>
        </config>
   </service>
   @endcode
 * @subsection Input Input
 * - \b image [::fwData::Image]: Image used to generate the mesh.
 * @subsection Output Output
 * - \b modelSeries [::fwMedData::ModelSeries]: model where the generated mesh is pushed.
 * @subsection Configuration Configuration
 * - \b valueMin : The minimum value in the image mesh
 * - \b valueMax : The maximum value to mesh
 * - \b radius : The radius of the sphere used to simplify the mesh
 * - \b adapt : The adaptivity factor used to simplify the mesh
 * - \b faces : The percentage of faces of the marching cube or the number of faces needed for the mesh
 * - \b percentage : If true the variable faces is a percentage, if false it is a number
 * - \b closing : If true the mesh is closed at the border of the image
 */
class OPPOCMESHER_CLASS_API CGoGNMesher : public ::fwServices::IOperator
{

public:

    fwCoreServiceClassDefinitionsMacro((CGoGNMesher)(::fwServices::IOperator) );

    /**
     * @brief Constructor. Initializes slots.
     */
    OPPOCMESHER_API CGoGNMesher() noexcept;

    /**
     * @brief Destructor. Does nothing.
     */
    OPPOCMESHER_API virtual ~CGoGNMesher() noexcept;

protected:

    /**
     * @brief This method starts the operator. Does nothing.
     */
    OPPOCMESHER_API void starting() override;

    /**
     * @brief This method stops the operator. Does nothing.
     */
    OPPOCMESHER_API void stopping() override;

    /**
     * @brief Declare the configuration to define percent of reduction, image source and ModelSeries destination.
     */
    OPPOCMESHER_API void configuring() override;

    /**
     * @brief This method is used to update services. Does nothing.
     */
    OPPOCMESHER_API void updating() override;

private:

    /**
     * @brief This method is used to set an integer parameter.
     */
    OPPOCMESHER_API virtual void setInteger(int val, std::string key);

    /**
     * @brief This method is used to set an boolean parameter.
     */
    OPPOCMESHER_API virtual void setBoolean(bool val, std::string key);

    ///The minimum value in the image mesh
    unsigned int m_valueMin;

    ///The maximum value to mesh
    unsigned int m_valueMax;

    ///The adaptativity factor used to simplify the mesh
    unsigned int m_adapt;

    ///The radius of the sphere used to simplify the mesh
    unsigned int m_radius;

    ///The percentage of faces of the marching cube or the number of faces needed for the mesh
    unsigned int m_faces;

    ///If true the variable m_faces is a percentage, if false it is a number
    bool m_percentage;

    ///If true the mesh is closed at the border of the image
    bool m_closing;

};

} // namespace opPOCMesher
