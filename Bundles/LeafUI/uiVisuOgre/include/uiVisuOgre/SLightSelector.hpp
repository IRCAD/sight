#ifndef __UIVISUOGRE_SLIGHTSELECTOR_HPP__
#define __UIVISUOGRE_SLIGHTSELECTOR_HPP__

#include "uiVisuOgre/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderOgre/Layer.hpp>

#include <gui/editor/IEditor.hpp>

#include <QComboBox>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

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
     * @name Slots API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_INIT_LIGHT_LIST_SLOT;
    /** @} */

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

    /// Slot: called when a layer is selected
    /// Sets the current layer and initializes the light adaptors list
    void onSelectedLayerItem(int index);

private:

    void initLightList(::fwRenderOgre::Layer::sptr layer);

    /// Retrieves all the layers from the application thanks to the render services.
    void refreshLayers();

    /// Retrieves light adaptors used in the current layer and stores them in the list widget.
    void updateLightList();

    QPointer<QComboBox> m_layersBox;

    QPointer<QListWidget> m_lightsList;

    QPointer<QPushButton> m_addLightBtn;

    std::vector< ::fwRenderOgre::Layer::wptr > m_layers;
    ::fwRenderOgre::Layer::wptr m_currentLayer;

    ///Connection service, needed for slot/signal association
    ::fwCom::helper::SigSlotConnection m_connections;
};


} // namespace uiVisuOgre

#endif // __UIVISUOGRE_SLIGHTSELECTOR_HPP__
