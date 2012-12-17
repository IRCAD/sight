#ifndef __FWGUIQT_UTIL_FUNCSLOT_HPP__
#define __FWGUIQT_UTIL_FUNCSLOT_HPP__

#include <boost/function.hpp>

#include <QObject>

#include <fwGuiQt/config.hpp>

namespace fwGuiQt
{

namespace util
{


class FWGUIQT_CLASS_API FuncSlot : public QObject
{
    Q_OBJECT

public:
    FWGUIQT_API FuncSlot();

    template< typename CALLABLE >
    FWGUIQT_API FuncSlot(CALLABLE c) : m_func(c)
    {
    }

    template< typename CALLABLE >
    FWGUIQT_API void setFunction(CALLABLE c)
    {
        m_func = c;
    }

public Q_SLOTS:
    void trigger();

protected:
    ::boost::function< void() > m_func;
};

} // namespace util

} // namespace fwGuiQt

#endif

