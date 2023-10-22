/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/viz/scene3d_qt/Text.hpp"

#include <core/com/slot.hpp>

#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/registry/macros.hpp>

#include <modules/viz/scene3d_qt/window_interactor.hpp>

#include <OGRE/OgreNode.h>

#include <OgreAxisAlignedBox.h>
#include <QStyle>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_SCENE3D_TEXT(sight::module::viz::scene3d_qt::Text, sight::viz::scene3d::text::REGISTRY_KEY);

//-----------------------------------------------------------------------------

namespace sight::module::viz::scene3d_qt
{

class NodeListener : public Ogre::SceneNode::Listener
{
public:

    explicit NodeListener(Text& _text, Ogre::Camera& _camera, Ogre::Node& _node) :
        m_text(_text),
        m_camera(_camera),
        m_node(_node)
    {
    }

    //------------------------------------------------------------------------------

    void nodeUpdated(const Ogre::Node* /*unused*/) override
    {
        m_text.setUnderlyingNodeRect(
            sight::viz::scene3d::helper::scene::computeBoundingRect(
                m_camera,
                &dynamic_cast<Ogre::SceneNode&>(m_node)
            )
        );
    }

    Text& m_text;
    Ogre::Camera& m_camera;
    Ogre::Node& m_node;
};

class CameraListener : public Ogre::SceneNode::Listener
{
public:

    explicit CameraListener(Ogre::Camera& _camera) :
        m_camera(_camera)
    {
    }

    //------------------------------------------------------------------------------

    void nodeUpdated(const Ogre::Node* /*unused*/) override
    {
        std::ranges::for_each(
            m_text,
            [this](auto& _p)
            {
                const auto* scene_node = dynamic_cast<Ogre::SceneNode*>(_p.second);
                SIGHT_ASSERT("cast from Ogre::Node to Ogre::SceneNode failed", scene_node);
                _p.first->setUnderlyingNodeRect(
                    sight::viz::scene3d::helper::scene::computeBoundingRect(
                        m_camera,
                        scene_node
                    )
                );
            });
    }

    //------------------------------------------------------------------------------

    void addText(Text* _text, Ogre::Node* _node)
    {
        m_text.emplace_back(_text, _node);
    }

    //------------------------------------------------------------------------------

    void removeText(Text* _text)
    {
        std::erase_if(m_text, [&](const auto& _p){return _p.first == _text;});
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool empty() const
    {
        return m_text.empty();
    }

private:

    std::vector<std::pair<Text*, Ogre::Node*> > m_text;
    Ogre::Camera& m_camera;
};

std::map<Ogre::Camera*, CameraListener*> s_camera_listeners;

//------------------------------------------------------------------------------

Text::Text(const sight::viz::scene3d::layer::sptr& _layer)
{
    m_resizeSlot = core::com::new_slot(
        [this](int, int)
        {
            if(m_nodeListener != nullptr)
            {
                m_nodeListener->nodeUpdated({});
            }

            this->adjustSize();
        });
    m_resizeSlot->set_worker(core::thread::get_default_worker());

    m_resizeConnection = _layer->signal(sight::viz::scene3d::layer::RESIZE_LAYER_SIG)->connect(m_resizeSlot);

    auto interactor    = _layer->getRenderService()->getInteractorManager();
    auto qt_interactor = std::dynamic_pointer_cast<sight::module::viz::scene3d_qt::window_interactor>(interactor);

    auto* parent_widget = qt_interactor->getQtWidget();

    m_text = new QLineEdit("", parent_widget);
    m_text->setReadOnly(true);
    m_text->setAttribute(Qt::WA_TransparentForMouseEvents);
    Qt::Alignment alignment;
    if(m_horizontalAlignment == "left")
    {
        alignment |= Qt::AlignLeft;
    }
    else if(m_horizontalAlignment == "center")
    {
        alignment |= Qt::AlignHCenter;
    }
    else if(m_horizontalAlignment == "right")
    {
        alignment |= Qt::AlignRight;
    }

    if(m_verticalAlignment == "top")
    {
        alignment |= Qt::AlignTop;
    }
    else if(m_verticalAlignment == "center")
    {
        alignment |= Qt::AlignVCenter;
    }
    else if(m_verticalAlignment == "bottom")
    {
        alignment |= Qt::AlignBottom;
    }

    m_text->setAlignment(alignment);
    QObject::connect(
        m_text,
        &QLineEdit::textEdited,
        [this](QString _text)
        {
            signal<text::TextEditedSignal>(text::TEXT_EDITED_SIGNAL)->async_emit(_text.toStdString());
            adjustSize();
        });
    QObject::connect(
        m_text,
        &QLineEdit::editingFinished,
        [this]
        {
            signal<text::EditingFinishedSignal>(text::EDITING_FINISHED_SIGNAL)->async_emit();
        });
    m_text->show();
}

//------------------------------------------------------------------------------

Text::~Text()
{
    m_resizeConnection.disconnect();

    SIGHT_ASSERT("Node should be detached first with detachFromNode()", m_nodeListener == nullptr);

    delete m_text;
}

//------------------------------------------------------------------------------

void Text::attachToNode(Ogre::SceneNode* _node, Ogre::Camera* _camera)
{
    SIGHT_ASSERT("Camera is null", _camera);
    SIGHT_ASSERT("Node is null", _node);

    m_nodeListener = new NodeListener(*this, *_camera, *_node);
    _node->setListener(m_nodeListener);

    auto it_listener = s_camera_listeners.find(_camera);
    if(it_listener == s_camera_listeners.end())
    {
        s_camera_listeners[_camera] = new CameraListener(*_camera);
        _camera->getParentSceneNode()->setListener(s_camera_listeners[_camera]);
    }

    s_camera_listeners[_camera]->addText(this, _node);
}

//------------------------------------------------------------------------------

void Text::detachFromNode()
{
    SIGHT_ASSERT("Node is null", m_nodeListener != nullptr);

    auto* camera = &m_nodeListener->m_camera;
    s_camera_listeners[camera]->removeText(this);
    m_nodeListener->m_node.setListener(nullptr);
    delete m_nodeListener;
    m_nodeListener = nullptr;

    if(s_camera_listeners[camera]->empty())
    {
        s_camera_listeners.erase(camera);
    }
}

//------------------------------------------------------------------------------

void Text::setText(const std::string& _text)
{
    QString q_text = QString::fromStdString(_text);
    if(m_text->text() == q_text)
    {
        return;
    }

    std::optional<int> cursor_position;
    if(m_text->hasFocus())
    {
        cursor_position = m_text->cursorPosition();
    }

    m_text->setText(q_text);
    if(cursor_position.has_value())
    {
        m_text->setCursorPosition(*cursor_position);
    }

    this->adjustSize();
}

//------------------------------------------------------------------------------

void Text::setPosition(float _x, float _y)
{
    m_position = {Ogre::Vector2(_x, _y), Ogre::Vector2(_x, _y)};
    this->adjustSize();
}

//------------------------------------------------------------------------------

void Text::setTextColor(const Ogre::ColourValue& _color)
{
    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(8) << _color.getAsARGB();
    m_textColor = QString::fromStdString(ss.str());
    this->adjustStyle();
}

//------------------------------------------------------------------------------

void Text::setTextColor(const std::string& _color)
{
    m_textColor = QString::fromStdString(_color);
    this->adjustStyle();
}

//------------------------------------------------------------------------------

void Text::setVisible(bool _visible)
{
    if(_visible)
    {
        m_text->show();
    }
    else
    {
        m_text->hide();
    }
}

//------------------------------------------------------------------------------

void Text::setTextAlignment(const std::string& _h_align, const std::string& _v_align)
{
    m_horizontalAlignment = _h_align;
    m_verticalAlignment   = _v_align;
    this->adjustSize();
}

//------------------------------------------------------------------------------

void Text::setFont(const std::string& _font_family)
{
    m_fontFamily = QString::fromStdString(_font_family);
    this->adjustStyle();
}

//------------------------------------------------------------------------------

void Text::setFontSize(std::size_t _size)
{
    m_fontSize = _size;
    this->adjustStyle();
}

//------------------------------------------------------------------------------

void Text::setEditMode(bool _edit_mode)
{
    m_text->setReadOnly(!_edit_mode);
    m_text->setAttribute(Qt::WA_TransparentForMouseEvents, !_edit_mode);
}

//------------------------------------------------------------------------------

void Text::setUnderlyingNodeRect(std::pair<Ogre::Vector2, Ogre::Vector2> _rect)
{
    m_position = _rect;
    this->adjustSize();
}

//------------------------------------------------------------------------------

void Text::adjustSize()
{
    QLabel dummy_label(m_text->text());
    dummy_label.setStyleSheet(computeStyle());
    dummy_label.adjustSize();
    m_text->resize(dummy_label.size());

    QPoint origin;
    if(m_nodeListener != nullptr)
    {
        QRectF position_rect = {QPointF(m_position.first.x, m_position.first.y), QPointF(
                                    m_position.second.x,
                                    m_position.second.y
        )
        };
        int x {};
        if(m_horizontalAlignment == "center")
        {
            x = static_cast<int>(position_rect.center().x() - static_cast<float>(m_text->width()) / 2.F);
        }
        else
        {
            x = static_cast<int>(position_rect.bottomRight().x());
        }

        origin = QPoint(x, static_cast<int>(position_rect.bottom())) / m_text->devicePixelRatioF();
    }
    else
    {
        Qt::Alignment alignment;
        if(m_horizontalAlignment == "left")
        {
            alignment |= Qt::AlignLeft;
        }
        else if(m_horizontalAlignment == "center")
        {
            alignment |= Qt::AlignHCenter;
        }
        else if(m_horizontalAlignment == "right")
        {
            alignment |= Qt::AlignRight;
        }

        if(m_verticalAlignment == "top")
        {
            alignment |= Qt::AlignTop;
        }
        else if(m_verticalAlignment == "center")
        {
            alignment |= Qt::AlignVCenter;
        }
        else if(m_verticalAlignment == "bottom")
        {
            alignment |= Qt::AlignBottom;
        }

        QRect const parent_rect {QPoint {0, 0}, m_text->parentWidget()->size()};
        origin = QStyle::alignedRect(Qt::LeftToRight, alignment, m_text->size(), parent_rect).topLeft();
    }

    m_text->move(origin);
}

//------------------------------------------------------------------------------

QString Text::computeStyle()
{
    static const QString s_textCss =
        R"(
        * {
            color: %1;
            font-size: %2px;
            font-family: %3;
            background: transparent;
            border: 0;
        }
        QLineEdit:!read-only {
            color: black;
            background: white;
        })";
    return s_textCss.arg(m_textColor).arg(m_fontSize).arg(m_fontFamily);
}

//------------------------------------------------------------------------------

void Text::adjustStyle()
{
    m_text->setStyleSheet(computeStyle());
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d_qt
