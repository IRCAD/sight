/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_TRANSFORM_HPP__
#define __VISUVTKADAPTOR_TRANSFORM_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <vtkSmartPointer.h>

#include <vector>

class vtkTransform;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to manage a TransformationMatrix3D from a vtkTransform and vice versa
 * @class Transform
 */
class VISUVTKADAPTOR_CLASS_API Transform : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Transform)(::fwRenderVTK::IVtkAdaptorService) );

    /// Constructor
    VISUVTKADAPTOR_API Transform() throw();
    /// Destructor
    VISUVTKADAPTOR_API virtual ~Transform() throw();

    /// Set a new vtkTransform
    VISUVTKADAPTOR_API void setTransform(vtkTransform *t);

    /// Returns the current vtkTransform
    VISUVTKADAPTOR_API vtkTransform *getTransform();

    /// Updates the TransformationMatrix3D from the vtkTransform
    VISUVTKADAPTOR_API void updateFromVtk();

    /// Returns the priority of the adaptor - some adaptors may have to be started before other ones
    VISUVTKADAPTOR_API virtual int getStartPriority();

protected:

    /// Calls doUpdate()
    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);

    /**
     * @brief Configure the adaptor.
     * @code{.xml}
       <adaptor id="tmAdaptor" class="::visuVTKAdaptor::Transform" objectId="tm3dKey" >
        <config transform="transform" autoRender="true" parent="parentTransform" />
       </adaptor>
       @endcode
     * With :
     *  - \b transform (mandatory) : the vtkTransform to associate to the adaptor
     *  - \b autoRender (optional, "true" by default): if autoRender=true,  the scene is automatically rendered after
     *    doStart, doUpdate, doSwap and doStop if m_vtkPipelineModified=true.
     *  - \b parent (optional): id of the parent vtkTransform, it will be concatenated with this current vtkTransform.
     */
    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Calls doUpdate()
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Updates the vtkTransform from the TransformationMatrix3D
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /// Does nothing
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    /// Current vtkTransform
    vtkTransform* m_transform;

    /// Command used to listen m_transform changes
    vtkCommand* m_transformCommand;

    /// Parent vtkTransform id (optional)
    std::string m_parentId;

    /// Parent vtkTransform (optional)
    vtkSmartPointer<vtkTransform> m_parentTransform;
};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_TRANSFORM_HPP__
