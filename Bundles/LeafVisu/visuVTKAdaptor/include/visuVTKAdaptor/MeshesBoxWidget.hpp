/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_MESHESBOXWIDGET_HPP__
#define __VISUVTKADAPTOR_MESHESBOXWIDGET_HPP__

#include <fwCom/Connection.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkBoxWidget2;
class vtkCommand;
class vtkPolyData;


namespace visuVTKAdaptor
{

/**
 * @brief   Create a Box widget around the meshes contained in the composite.
 * @class   MeshesBoxWidget
 *
 * @date    2010.
 */
class VISUVTKADAPTOR_CLASS_API MeshesBoxWidget : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (MeshesBoxWidget)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API MeshesBoxWidget() throw();

    VISUVTKADAPTOR_API virtual ~MeshesBoxWidget() throw();

    /// Updates meshes transformation matrix from vtk box widget transform
    void updateFromVtk();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    /// Updates vtk transformation from data meshes
    void updateMeshTransform();

    /// Updates map from composite meshes
    void updateMeshMapFromComposite(::fwData::Composite::sptr composite);


private:

    typedef std::map< std::string, ::fwCom::Connection > ConnectionMapType;
    typedef std::map< std::string, vtkActor* > MeshMapType;

    MeshMapType m_meshMap;
    ConnectionMapType m_connections;

    vtkAssembly * m_assembly;
    vtkCommand* m_boxWidgetCommand;
    vtkBoxWidget2* m_vtkBoxWidget;
};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_MESHESBOXWIDGET_HPP__
