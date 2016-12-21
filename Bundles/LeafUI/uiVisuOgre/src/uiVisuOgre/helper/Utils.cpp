#include "uiVisuOgre/helper/Utils.hpp"

namespace uiVisuOgre
{

namespace helper
{

//------------------------------------------------------------------------------

QColor Utils::converOgreColorToQColor(const ::Ogre::ColourValue& _ogreColor)
{
    int r = static_cast<int>(_ogreColor.r * 255);
    int g = static_cast<int>(_ogreColor.g * 255);
    int b = static_cast<int>(_ogreColor.b * 255);
    int a = static_cast<int>(_ogreColor.a * 255);

    return QColor(r, g, b, a);
}

//------------------------------------------------------------------------------

::Ogre::ColourValue Utils::convertQColorToOgreColor(const QColor& _qColor)
{
    float r = static_cast<float>(_qColor.red()) / 255.f;
    float g = static_cast<float>(_qColor.green()) / 255.f;
    float b = static_cast<float>(_qColor.blue()) / 255.f;
    float a = static_cast<float>(_qColor.alpha()) / 255.f;

    return ::Ogre::ColourValue(r, g, b, a);
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace uiVisuOgre
