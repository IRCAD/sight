/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_STRANSFORM_HPP__
#define __VISUVTKADAPTOR_STRANSFORM_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkSmartPointer.h>

#include <vector>

class vtkTransform;
class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor binding a TransformationMatrix3D to a vtkTransform.
 * @code{.xml}
    <service type="::visuVTKAdaptor::STransform" >
        <inout key="tm3d" uid="..." />
        <config transform="transform" autoRender="true" parent="parentTransform" />
    </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b tm3d [::fwData::TransformationMatrix3D]: f4s transform matrix.
 *
 * @subsection Configuration Configuration
 *  - \b transform (mandatory) : the vtkTransform to associate to the adaptor
 *  - \b autoRender (optional, "true" by default): if autoRender=true,  the scene is automatically rendered after
 *    doStart, doUpdate, doSwap and doStop if m_vtkPipelineModified=true.
 *  - \b parent (optional): id of the parent vtkTransform, it will be concatenated with this current vtkTransform.
 */
class VISUVTKADAPTOR_CLASS_API STransform : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (STransform)(::fwRenderVTK::IAdaptor) );

    /// Constructor
    VISUVTKADAPTOR_API STransform() noexcept;

    /// Destructor
    VISUVTKADAPTOR_API virtual ~STransform() noexcept;

    /// Set a new vtkTransform
    VISUVTKADAPTOR_API void setTransform(vtkTransform* t);

    /// Returns the current vtkTransform
    VISUVTKADAPTOR_API vtkTransform* getTransform();

    /// Updates the TransformationMatrix3D from the vtkTransform
    VISUVTKADAPTOR_API void updateFromVtk();

    /// Returns the priority of the adaptor - some adaptors may have to be started before other ones
    VISUVTKADAPTOR_API virtual int getStartPriority();

protected:

    /// Calls doUpdate()
    VISUVTKADAPTOR_API void starting();

    VISUVTKADAPTOR_API void configuring();

    /// Calls doUpdate()
    VISUVTKADAPTOR_API void swapping();

    /// Updates the vtkTransform from the TransformationMatrix3D
    VISUVTKADAPTOR_API void updating();

    /// Does nothing
    VISUVTKADAPTOR_API void stopping();

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

#endif // __VISUVTKADAPTOR_STRANSFORM_HPP__
