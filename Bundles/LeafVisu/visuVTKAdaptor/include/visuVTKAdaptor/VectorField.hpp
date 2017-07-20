/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_VECTORFIELD_HPP__
#define __VISUVTKADAPTOR_VECTORFIELD_HPP__

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

class VISUVTKADAPTOR_CLASS_API VectorField : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (VectorField)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API VectorField() noexcept;

    VISUVTKADAPTOR_API virtual ~VectorField() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();

    virtual void buildPipeline();
    virtual void destroyPipeline();

    vtkSmartPointer<vtkPolyDataAlgorithm> m_arrowSource;
    vtkSmartPointer<vtkImageData> m_imageData;
    vtkSmartPointer<vtkGlyph3D> glyphFilter;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_VECTORFIELD_HPP__
