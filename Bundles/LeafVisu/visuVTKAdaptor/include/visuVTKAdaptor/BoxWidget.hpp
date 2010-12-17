#ifndef _VISUVTKADAPTOR_BOXWIDGET_HPP_
#define _VISUVTKADAPTOR_BOXWIDGET_HPP_

#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include "visuVTKAdaptor/config.hpp"

class vtkTransform;
class vtkBoxWidget2;


namespace visuVTKAdaptor
{

class Transform;

// BoxWidget
class VISUVTKADAPTOR_CLASS_API BoxWidget : public ::fwRenderVTK::IVtkAdaptorService
{
public:

	fwCoreServiceClassDefinitionsMacro( (BoxWidget)(::fwRenderVTK::IVtkAdaptorService) );

	VISUVTKADAPTOR_API              BoxWidget() throw();
	VISUVTKADAPTOR_API virtual     ~BoxWidget() throw();

    VISUVTKADAPTOR_API void updateFromVtk();

protected:

	VISUVTKADAPTOR_API virtual void configuring() throw( ::fwTools::Failed );
	VISUVTKADAPTOR_API virtual void doStart()     throw( ::fwTools::Failed );
	VISUVTKADAPTOR_API virtual void doStop()      throw( ::fwTools::Failed );
	VISUVTKADAPTOR_API virtual void doSwap()      throw( ::fwTools::Failed );
	VISUVTKADAPTOR_API virtual void doUpdate()    throw( ::fwTools::Failed );
	VISUVTKADAPTOR_API virtual void doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed );

    vtkTransform* m_transform;
    WPTR(::visuVTKAdaptor::Transform) m_transformService;

private:

	::vtkBoxWidget2* m_vtkBoxWidget;
    vtkCommand* m_boxWidgetCommand;
};

} // namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_BOXWIDGET_HPP_
