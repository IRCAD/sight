#ifndef __UIVISUOGRE_SLIGHTEDITOR_HPP__
#define __UIVISUOGRE_SLIGHTEDITOR_HPP__

#include "uiVisuOgre/config.hpp"

#include <fwRenderOgre/ILight.hpp>

#include <gui/editor/IEditor.hpp>

#include <QComboBox>
#include <QPointer>
#include <QPushButton>
#include <QSlider>

namespace uiVisuOgre
{

/**
 * @brief User interface to edit a light adaptor.
 *
 * @section Slots Slots
 * - \b editLight(::fwRenderOgre::ILight::sptr): loads the editor with the parameters from the selected light.
 *
 * @section XML XML Configuration
 * @code{.xml}
 *  <service uid=SLightEditorUid" type="uiVisuOgre::SLightEditor" />
 */
class UIVISUOGRE_CLASS_API SLightEditor : public QObject,
                                          public ::gui::editor::IEditor
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SLightEditor)(::gui::editor::IEditor) );

    /**
     * @name Slots API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_EDIT_LIGHT_SLOT;
    /** @} */

    UIVISUOGRE_API SLightEditor() throw();
    UIVISUOGRE_API virtual ~SLightEditor() throw();

protected:

    /// Configure the service.
    UIVISUOGRE_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Sets the connections and the UI elements.
    UIVISUOGRE_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Destroys the connections and cleans the container.
    UIVISUOGRE_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Does nothing.
    UIVISUOGRE_API virtual void updating() throw ( ::fwTools::Failed );

private:

    void editLight(::fwRenderOgre::ILight::sptr _lightAdaptor);

    QPointer<QComboBox> m_lightTypeBox;

    QPointer<QPushButton> m_diffuseColorBtn;
    QPointer<QPushButton> m_specularColorBtn;

    QPointer<QSlider> m_thetaSlider;
    QPointer<QSlider> m_phiSlider;

    static const std::string s_POINT_LIGHT;
    static const std::string s_DIRECTIONAL_LIGHT;
    static const std::string s_SPOT_LIGHT;
};

} // namespace uiVisuOgre

#endif // __UIVISUOGRE_SLIGHTEDITOR_HPP__
