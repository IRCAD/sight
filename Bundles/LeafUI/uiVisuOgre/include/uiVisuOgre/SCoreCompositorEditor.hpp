/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_SCORECOMPOSITOREDITOR_HPP__
#define __UIVISUOGRE_SCORECOMPOSITOREDITOR_HPP__

#include <fwRenderOgre/Layer.hpp>
#include <fwRenderOgre/compositor/Core.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>
#include <QLabel>

#include "uiVisuOgre/config.hpp"

class QAbstractButton;
class QButtonGroup;
class QComboBox;
class QRadioButton;
class QLineEdit;
class QSlider;
class QCheckBox;
class QComboBox;


namespace uiVisuOgre
{

/**
 * @brief   Allows to select an Ogre Compositor and apply it to a layer
 * @class   SCoreCompositorEditor
 */
class UIVISUOGRE_CLASS_API SCoreCompositorEditor : public QObject,
                                                   public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (SCoreCompositorEditor)(::gui::editor::IEditor) );

    /// Constructor.
    UIVISUOGRE_API SCoreCompositorEditor() throw();

    /// Destructor. Does nothing
    UIVISUOGRE_API virtual ~SCoreCompositorEditor() throw();

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SCoreCompositorEditorInstance" impl="::uiVisuOgre::SCoreCompositorEditor" type="::gui::editor::IEditor">
             <parameter>param</parameter>
        </service>
       @endcode
     * - \b Parameter : parameter description.
     */
    UIVISUOGRE_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// FILL ME.
    UIVISUOGRE_API virtual void starting()  throw ( ::fwTools::Failed );

    /// FILL ME.
    UIVISUOGRE_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// FILL ME.
    UIVISUOGRE_API virtual void updating() throw ( ::fwTools::Failed );

    /// FILL ME.
    UIVISUOGRE_API void refreshRenderers();

    std::vector< ::fwRenderOgre::Layer::sptr > m_layers;
    ::fwRenderOgre::Layer::sptr m_currentLayer;
    ::fwRenderOgre::compositor::Core::sptr m_currentCoreCompositor;

protected Q_SLOTS:

    void onSelectedLayerItem(int index);
    void onUseCelShading(int state);
    void onEditTransparencyDepth(int depth);
    void onEditTransparency(int index);

private:
    QWidget* m_container;
    QPointer<QButtonGroup> m_transparencyButtonGroup;
    QPointer<QButtonGroup> m_transparencyButtonGroupShading;
    QPointer<QButtonGroup> m_normalsRadioBox;
    QPointer<QComboBox> m_layersBox;
    QPointer<QSlider> m_transparencyDepthSlider;
    QPointer<QComboBox> m_comboBox;
    QPointer<QCheckBox> m_useCelShadingCheckBox;
    QRadioButton* m_buttonDefault;
    QRadioButton* m_buttonDepthPeeling;
    QRadioButton* m_buttonDualDepthPeeling;
    QRadioButton* m_buttonWeightedBlendedOIT;
    QRadioButton* m_buttonHybridTransparency;
    QLabel* m_labelDefault;
    QLabel* m_labelDepthPeeling;
    QLabel* m_labelDualDepthPeeling;
    QLabel* m_labelWeightedBlendedOIT;
    QLabel* m_labelHybridTransparency;
    bool m_isLayerSelected;
};

} // uiVisuOgre

#endif // __UIVISUOGRE_SCORECOMPOSITOREDITOR_HPP__
