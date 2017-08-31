/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SVECTORFIELD_HPP__
#define __VISUVTKADAPTOR_SVECTORFIELD_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkSmartPointer.h>

class vtkPolyDataAlgorithm;
class vtkImageData;
class vtkGlyph3D;

namespace visuVTKAdaptor
{

/**
 * @brief This adaptor displays vector field contains in a ::fwData::Image
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SVectorField" autoConnect="yes">
       <in key="image" uid="..." />
       <config renderer="default" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b image [::fwData::Image]: image containing vector field (image with 3 components).
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the mesh is displayed
 */
class VISUVTKADAPTOR_CLASS_API SVectorField : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SVectorField)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SVectorField() noexcept;

    VISUVTKADAPTOR_API virtual ~SVectorField() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;

    virtual void buildPipeline();
    virtual void destroyPipeline();

    vtkSmartPointer<vtkPolyDataAlgorithm> m_arrowSource;
    vtkSmartPointer<vtkImageData> m_imageData;
    vtkSmartPointer<vtkGlyph3D> glyphFilter;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SVECTORFIELD_HPP__
