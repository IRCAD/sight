#ifndef __UIVISUOGRE_SLIGHTSELECTOR_HPP__
#define __UIVISUOGRE_SLIGHTSELECTOR_HPP__

#include "uiVisuOgre/config.hpp"

#include <fwRenderOgre/ILight.hpp>
#include <fwRenderOgre/Layer.hpp>

#include <gui/editor/IEditor.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

#include <string>
#include <vector>

namespace uiVisuOgre
{

/**
 * @brief User interface to select light adaptor from a selected layer or create a new one.
 *
 * @section Slots Slots
 * - \b initLightList(::fwRenderOgre::Layer::sptr): populates the list of available light adaptors for the current layer.
 *
 * @section XML XML Configuration
 * @code{.xml}
 *  <service uid="SLightSelectorUid" type="::uiVisuOgre::SLightSelector" />
 */
class UIVISUOGRE_CLASS_API SLightSelector : public QObject,
                                            public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (SLightSelector)(::gui::editor::IEditor) );

    /**
     * @name Signals API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Signals::SignalKeyType s_LIGHT_SELECTED_SIG;
    typedef ::fwCom::Signal<void (::fwRenderOgre::ILight::sptr)> LightSelectedSignalType;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_INIT_LIGHT_LIST_SLOT;
    /** @} */

    /// Initializes signals and slots.
    UIVISUOGRE_API SLightSelector() throw();
    UIVISUOGRE_API virtual ~SLightSelector() throw();

protected:

    /// Configure the service.
    UIVISUOGRE_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Sets the connections and the UI elements.
    UIVISUOGRE_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Destroys the connections and cleans the container.
    UIVISUOGRE_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Does nothing.
    UIVISUOGRE_API virtual void updating() throw ( ::fwTools::Failed );

protected Q_SLOTS:

    /// Slot: called when a layer is selected.
    /// Sets the current layer and initializes the light adaptors list.
    void onSelectedLayerItem(int _index);

    /// Slot: called when the checkbox for all lights is checked or unchecked.
    /// Checks or unchecks all the light adaptors according to the new state.
    void onChangedLightsState(int _state);

    /// Slot: called when a light is selected.
    /// Loads the selected light parameters in the light editor.
    void onSelectedLightItem(QListWidgetItem* _item);

    /// Slot: called when a light is checked.
    /// Switched on or off the light according to its current state.
    void onCheckedLightItem(QListWidgetItem* _item);

    /// Slot: called when the add light button is clicked.
    /// Adds a new light to the current scene.
    void onAddLight(bool _checked);

    /// Slot: called when the remove light button is clicked.
    /// Removes the selected light.
    void onRemoveLight(bool _checked);

    /// Slot: called when the scene ambient color button is clicked.
    /// Opens a color picker and lets the user choose a new ambient color.
    void onEditAmbientColor(bool _checked);

private:

    void initLightList(::fwRenderOgre::Layer::sptr _layer);

    /// Retrieves all the layers from the application thanks to the render services.
    void refreshLayers();

    /// Retrieves light adaptors used in the current layer and stores them in the list widget.
    void updateLightsList();

    /// Creates a new light adaptor.
    void createLightAdaptor(const std::string& _name);

    /// Finds the light adaptor with the specified name.
    ::fwRenderOgre::ILight::sptr retrieveLightAdaptor(const std::string& _name) const;

    QPointer<QComboBox>   m_layersBox;
    QPointer<QCheckBox>   m_lightsState;
    QPointer<QListWidget> m_lightsList;

    QPointer<QPushButton> m_addLightBtn;
    QPointer<QPushButton> m_removeLightBtn;
    QPointer<QPushButton> m_ambientColorBtn;

    std::vector< ::fwRenderOgre::Layer::wptr > m_layers;
    ::fwRenderOgre::Layer::wptr m_currentLayer;

    std::vector< ::fwRenderOgre::ILight::sptr > m_lightAdaptors;

    ::fwRenderOgre::ILight::sptr m_currentLight;

    /// Connection service, needed for slot/signal association
    ::fwCom::helper::SigSlotConnection m_connections;
};

//------------------------------------------------------------------------------

class NewLightDialog : public QDialog
{
Q_OBJECT

public:

    NewLightDialog(QWidget* parent = 0);
    ~NewLightDialog();

protected Q_SLOTS:
    void onOkBtn(bool _checked);

private:

    QPointer<QLabel> m_lightNameLbl;
    QPointer<QLineEdit> m_lightNameEdit;
    QPointer<QPushButton> m_okBtn;
};

} // namespace uiVisuOgre

#endif // __UIVISUOGRE_SLIGHTSELECTOR_HPP__
