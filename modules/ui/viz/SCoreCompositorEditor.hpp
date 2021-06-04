/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/ui/viz/config.hpp"

#include <viz/scene3d/Layer.hpp>

#include <ui/base/IEditor.hpp>

#include <QLabel>
#include <QObject>
#include <QPointer>

class QAbstractButton;
class QButtonGroup;
class QComboBox;
class QRadioButton;
class QLineEdit;
class QSlider;
class QCheckBox;
class QComboBox;
// needed for the radius
class QDoubleSpinBox;
// needed for the samples number
class QSpinBox;

namespace sight::module::ui::viz
{

/**
 * @brief   Allows to select an Ogre Compositor and apply it to a layer
 */
class MODULE_UI_VIZ_CLASS_API SCoreCompositorEditor : public QObject,
                                                      public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SCoreCompositorEditor, sight::ui::base::IEditor);

    /// Constructor.
    MODULE_UI_VIZ_API SCoreCompositorEditor() noexcept;

    /// Destructor. Does nothing
    MODULE_UI_VIZ_API virtual ~SCoreCompositorEditor() noexcept;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SCoreCompositorEditorInstance" type="sight::module::ui::viz::SCoreCompositorEditor">
             <parameter>param</parameter>
        </service>
       @endcode
     * - \b Parameter : parameter description.
     */
    MODULE_UI_VIZ_API void configuring() override;

    /// FILL ME.
    MODULE_UI_VIZ_API void starting() override;

    /// FILL ME.
    MODULE_UI_VIZ_API void stopping() override;

    /// FILL ME.
    MODULE_UI_VIZ_API void updating() override;

    /// FILL ME.
    MODULE_UI_VIZ_API void refreshRenderers();

    std::vector<sight::viz::scene3d::Layer::wptr> m_layers;
    sight::viz::scene3d::Layer::wptr m_currentLayer;

protected Q_SLOTS:

    void onSelectedLayerItem(int index);
    void onEditTransparencyDepth(int depth);
    void onEditTransparency(int index);

private:

    QPointer<QButtonGroup> m_transparencyButtonGroup;
    QPointer<QButtonGroup> m_transparencyButtonGroupShading;
    QPointer<QButtonGroup> m_normalsRadioBox;
    QPointer<QComboBox> m_layersBox;
    QPointer<QSlider> m_transparencyDepthSlider;
    QPointer<QComboBox> m_comboBox;
    QRadioButton* m_buttonDefault;
    QRadioButton* m_buttonDepthPeeling;
    QRadioButton* m_buttonDualDepthPeeling;
    QRadioButton* m_buttonWeightedBlendedOIT;
    QRadioButton* m_buttonHybridTransparency;
    QRadioButton* m_buttonCelShadingDepthPeeling;
    QLabel* m_labelDefault;
    QLabel* m_labelDepthPeeling;
    QLabel* m_labelDualDepthPeeling;
    QLabel* m_labelWeightedBlendedOIT;
    QLabel* m_labelHybridTransparency;
    QLabel* m_labelCelShadingDepthPeeling;
    bool m_isLayerSelected;
};

} // uiVisuOgre
