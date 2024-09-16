/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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
// cspell:ignore qputenv

#include "tester.hpp"

#include <core/spy_log.hpp>

#include <ui/__/preferences.hpp>

#include <boost/algorithm/string.hpp>

#include <QAction>
#include <QApplication>
#include <QMutex>
#include <QScreen>
#include <QTest>
#include <QTimer>

#include <cmath>
#include <iostream>

namespace sight::ui::test
{

//------------------------------------------------------------------------------

bool tester::s_already_loaded                     = false;
std::filesystem::path tester::s_image_output_path = std::filesystem::temp_directory_path();

//------------------------------------------------------------------------------

test_event::test_event(std::function<void()> _f) :
    QEvent(QEvent::User),
    m_function(std::move(_f))
{
}

//------------------------------------------------------------------------------

std::function<void()> test_event::function()
{
    return m_function;
}

//------------------------------------------------------------------------------

bool test_event_filter::eventFilter(QObject* _obj, QEvent* _event)
{
    if(_event->type() == QEvent::User)
    {
        auto* test_event = dynamic_cast<class test_event*>(_event);
        // the event catched here might not be a TestEvent, in which case, testEvent will be nullptr
        if(test_event != nullptr)
        {
            test_event->function()();
            return true;
        }
    }

    return QObject::eventFilter(_obj, _event);
}

tester_assertion_failed::tester_assertion_failed(const std::string& _msg) :
    std::runtime_error(_msg)
{
}

//------------------------------------------------------------------------------

bool always_true(QObject* /*unused*/)
{
    return true;
}

tester::backtrace_lock::backtrace_lock(tester& _tester) :
    m_tester(_tester)
{
}

tester::backtrace_lock::~backtrace_lock()
{
    // If stack unwinding is in progress, it means the backtrace will be useful, so we shouldn't clear it
    if(std::uncaught_exceptions() == 0)
    {
        m_tester.m_backtrace.pop_back();
    }
}

tester::tester(std::string _test_name, bool _verbose_mode) :
    m_test_name(std::move(_test_name)),
    m_verbose_mode(_verbose_mode)
{
}

tester::~tester()
{
    if(m_thread.joinable())
    {
        m_thread.join();
    }
}

//------------------------------------------------------------------------------

void tester::take(
    const std::string& _component_description,
    std::function<QObject* ()> _graphic_component,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    take<QObject*>(_component_description, _graphic_component, _condition, _timeout);
}

//------------------------------------------------------------------------------

void tester::take(const std::string& _component_description, QObject* _graphic_component)
{
    m_component_description = _component_description;
    m_graphic_component     = _graphic_component;
}

//------------------------------------------------------------------------------

void tester::take(
    const std::string& _component_description,
    QObject* _parent,
    const std::string& _object_name,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    take(
        _component_description,
        [_parent, &_object_name]
        {
            return _parent->findChild<QObject*>(QString::fromStdString(_object_name));
        },
        _condition,
        _timeout
    );
}

//------------------------------------------------------------------------------

void tester::take(
    const std::string& _component_description,
    const std::string& _object_name,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    take(_component_description, get_main_window(), _object_name, _condition, _timeout);
}

//------------------------------------------------------------------------------

void tester::interact(std::unique_ptr<interaction>&& _interaction)
{
    if(m_graphic_component == nullptr)
    {
        return;
    }

    QWidget* widget = qobject_cast<QWidget*>(m_graphic_component);
    QWindow* window = qobject_cast<QWindow*>(m_graphic_component);
    if(widget == nullptr && window == nullptr)
    {
        fail("Tried to interact with \"" + m_component_description + "\", which is neither a widget nor a window.");
    }

    if(m_verbose_mode)
    {
        qDebug() << _interaction->to_string().c_str() << "on" << QString::fromStdString(m_component_description);
    }

    if(widget != nullptr)
    {
        _interaction->interact_with(widget);
    }
    else if(window != nullptr)
    {
        _interaction->interact_with(window);
    }

    m_interactions.push_back(std::move(_interaction));
}

//------------------------------------------------------------------------------

void tester::doubt(const std::string& _result_description, std::function<bool(QObject*)> _result, int _timeout)
{
    doubt<QObject*>(_result_description, _result, _timeout);
}

//------------------------------------------------------------------------------

void tester::yields(
    const std::string& _component_description,
    std::function<QObject* (QObject*)> _graphic_component,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    yields<QObject*>(_component_description, _graphic_component, _condition, _timeout);
}

//------------------------------------------------------------------------------

void tester::yields(
    const std::string& _component_description,
    const std::string& _object_name,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    yields<QObject*>(_component_description, _object_name, _condition, _timeout);
}

//------------------------------------------------------------------------------

void tester::maybe_take(
    const std::string& _component_description,
    std::function<QObject* ()> _graphic_component,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    maybe_take<QObject*>(_component_description, _graphic_component, _condition, _timeout);
}

//------------------------------------------------------------------------------

void tester::do_something(std::function<void(QObject*)> _f)
{
    do_something<QObject*>(_f);
}

//------------------------------------------------------------------------------

void tester::do_something_asynchronously(std::function<void(QObject*)> _f)
{
    do_something_asynchronously<QObject*>(_f);
}

//------------------------------------------------------------------------------

void tester::take_screenshot(const std::filesystem::path& _path)
{
    if(get<QWidget*>()->windowHandle() != nullptr)
    {
        get<QWidget*>()->screen()->grabWindow(get<QWidget*>()->winId()).save(QString::fromStdString(_path.string()));
    }
    else
    {
        do_something_asynchronously<QWidget*>(
            [_path](QWidget* _o)
            {
                _o->grab().save(QString::fromStdString(_path.string()));
            });
    }
}

//------------------------------------------------------------------------------

void tester::start(std::function<void()> _f)
{
    m_thread = std::thread(
        [this, _f]() -> void
        {
            try
            {
                if(m_verbose_mode)
                {
                    qDebug() << "Waiting up to 5000 ms for the main window to appear";
                }

                bool ok = wait_for_asynchronously(
                    [this]() -> bool
                {
                    m_main_window = qApp->activeWindow();
                    return m_main_window != nullptr && m_main_window->isVisible();
                },
                    DEFAULT_TIMEOUT
                );
                if(!ok)
                {
                    fail("The main window has never showed up");
                }

                QTest::qWait(2000); // Temporize, because the window takes time to effectively show up

                _f();

                if(m_verbose_mode)
                {
                    qDebug() << "Waiting up to 5000 ms for the main window to close";
                }

                ok = QTest::qWaitFor(
                    [this]() -> bool
                {
                    QTimer::singleShot(0, m_main_window, &QWidget::close);
                    return qApp->activeWindow() == nullptr || !m_main_window->isVisible();
                },
                    DEFAULT_TIMEOUT
                );

                if(!ok)
                {
                    fail("The main window never closed");
                }
            }
            catch(tester_assertion_failed& e)
            {
                m_failure_message = generate_failure_message() + "Failure: " + e.what();
                m_failed          = true;
                if(m_verbose_mode)
                {
                    qDebug() << m_failure_message.c_str();
                }

                if(m_main_window != nullptr)
                {
                    m_main_window->screen()->grabWindow(0).save(
                        QString::fromStdString(
                            (s_image_output_path
                             / (m_test_name + "_failure.png")).string()
                        )
                    );
                }

                qApp->exit(1);

                // If the application hasn't closed after 5 seconds, force-quit.
                bool ok = QTest::qWaitFor(
                    [this]() -> bool
                {
                    return qApp->activeWindow() == nullptr || m_main_window == nullptr || !m_main_window->isVisible();
                });
                if(!ok)
                {
                    qDebug() << "The application didn't close in 5 seconds, force-quitting.";
                    std::exit(1);
                }
            }
            catch(std::exception& e)
            {
                m_failure_message = generate_failure_message() + "Unexpected error: " + e.what();
                m_failed          = true;
                if(m_verbose_mode)
                {
                    qDebug() << m_failure_message.c_str();
                }

                qApp->exit(1);

                // If the application hasn't closed after 5 seconds, force-quit.
                bool ok = QTest::qWaitFor(
                    [this]() -> bool
                {
                    return qApp->activeWindow() == nullptr || m_main_window == nullptr || !m_main_window->isVisible();
                });
                if(!ok)
                {
                    qDebug() << "The application didn't close in 5 seconds, force-quitting.";
                    std::exit(1);
                }
            }
        });
}

//------------------------------------------------------------------------------

void tester::should_be_hidden(
    const std::string& _component_description,
    std::function<QWidget* ()> _graphic_component,
    std::function<bool(QWidget*)> _condition,
    int _timeout
)
{
    maybe_take<QWidget*>(
        _component_description,
        _graphic_component,
        [&_condition](QWidget* _obj)
        {
            return _condition(_obj) && !_obj->isVisible();
        },
        _timeout
    );
    if(exists())
    {
        fail("'" + _component_description + "' is present, though it should be hidden");
    }
}

//------------------------------------------------------------------------------

void tester::should_be_present(
    const std::string& _component_description,
    std::function<QWidget* ()> _graphic_component,
    std::function<bool(QWidget*)> _condition,
    int _timeout
)
{
    take<QWidget*>(
        _component_description,
        _graphic_component,
        [&_condition](QWidget* _obj)
        {
            return _condition(_obj) && _obj->isVisible();
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

bool tester::exists()
{
    return m_graphic_component != nullptr;
}

//------------------------------------------------------------------------------

QWidget* tester::get_main_window()
{
    return m_main_window;
}

//------------------------------------------------------------------------------

std::string tester::get_failure_message() const
{
    return m_failure_message;
}

//------------------------------------------------------------------------------

bool tester::failed() const
{
    return m_failed;
}

//------------------------------------------------------------------------------

tester::backtrace_lock tester::add_in_backtrace(const std::string& _description)
{
    m_backtrace.push_back(_description);
    return {*this};
}

//------------------------------------------------------------------------------

std::string tester::get_description() const
{
    return m_component_description;
}

//------------------------------------------------------------------------------

void tester::init()
{
    if(!s_already_loaded)
    {
        qApp->installEventFilter(new test_event_filter);
        qputenv("GUI_TESTS_ARE_RUNNING", "1");
        const std::string image_output_path = qEnvironmentVariable("IMAGE_OUTPUT_PATH").toStdString();
        if(!image_output_path.empty())
        {
            s_image_output_path = image_output_path;
        }

        sight::ui::preferences::ignore_filesystem(true);

        s_already_loaded = true;

    #ifdef WIN32
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    #endif
    }
}

//------------------------------------------------------------------------------

QWidget* tester::get_widget_from_action(QAction* _action)
{
    return _action != nullptr ? _action->associatedWidgets().last() : nullptr;
}

//------------------------------------------------------------------------------

void tester::mouse_move(
    QWidget* _widget,
    QPoint _pos,
    int _delay,
    Qt::MouseButton _button,
    Qt::KeyboardModifiers _modifiers
)
{
    // Workaround to QTBUG-5232
    if(_delay > 0)
    {
        QTest::qWait(_delay);
    }

    if(_pos.isNull())
    {
        _pos = _widget->rect().center();
    }

    QMouseEvent event(QEvent::MouseMove, _pos, _widget->mapToGlobal(_pos), Qt::NoButton, _button, _modifiers);
    qApp->sendEvent(_widget, &event);
}

//------------------------------------------------------------------------------

void tester::mouse_move(
    QWindow* _window,
    QPoint _pos,
    int _delay,
    Qt::MouseButton _button,
    Qt::KeyboardModifiers _modifiers
)
{
    // Workaround to QTBUG-5232
    if(_delay > 0)
    {
        QTest::qWait(_delay);
    }

    if(_pos.isNull())
    {
        _pos = _window->geometry().center();
    }

    QMouseEvent event(QEvent::MouseMove, _pos, _window->mapToGlobal(_pos), Qt::NoButton, _button, _modifiers);
    qApp->sendEvent(_window, &event);
}

//------------------------------------------------------------------------------

double tester::compare_images_pixel_perfect(QImage _a, QImage _b, bool _strict)
{
    if(_a.size() != _b.size())
    {
        if(_strict)
        {
            return 0;
        }

        const QSize size = _a.size().boundedTo(_b.size());
        _a = _a.scaled(size);
        _b = _b.scaled(size);
    }

    int identical_pixels = 0;
    for(int y = 0 ; y < _a.height() ; y++)
    {
        for(int x = 0 ; x < _a.width() ; x++)
        {
            identical_pixels += static_cast<int>(_a.pixelColor(x, y) == _b.pixelColor(x, y));
        }
    }

    return static_cast<double>(identical_pixels) / (_a.height() * _a.width());
}

//------------------------------------------------------------------------------

double tester::compare_images_mse(QImage _a, QImage _b, bool _strict)
{
    if(_a.size() != _b.size())
    {
        if(_strict)
        {
            return 0;
        }

        const QSize size = _a.size().boundedTo(_b.size());
        _a = _a.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _b = _b.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    double res = 0;
    for(int y = 0 ; y < _a.height() ; y++)
    {
        for(int x = 0 ; x < _a.width() ; x++)
        {
            const QColor ca = _a.pixelColor(x, y);
            const QColor cb = _b.pixelColor(x, y);
            res += std::pow(ca.red() - cb.red(), 2) + std::pow(ca.green() - cb.green(), 2) + std::pow(
                ca.blue() - cb.blue(),
                2
            );
        }
    }

    return 1 - (res / (_a.height() * _a.width())) / (3 * std::pow(255, 2));
}

//------------------------------------------------------------------------------

double tester::compare_images_cosine(QImage _a, QImage _b, bool _strict)
{
    if(_a.size() != _b.size())
    {
        if(_strict)
        {
            return 0;
        }

        const QSize size = _a.size().boundedTo(_b.size());
        _a = _a.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _b = _b.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    double pa = 0;
    double pb = 0;
    double ab = 0;
    for(int y = 0 ; y < _a.height() ; y++)
    {
        for(int x = 0 ; x < _a.width() ; x++)
        {
            const QColor ca = _a.pixelColor(x, y);
            int ra          = ca.red() + 1;
            int ga          = ca.green() + 1;
            int ba          = ca.blue() + 1;
            const QColor cb = _b.pixelColor(x, y);
            int rb          = cb.red() + 1;
            int gb          = cb.green() + 1;
            int bb          = cb.blue() + 1;
            pa += std::pow(ra, 2) + std::pow(ga, 2) + std::pow(ba, 2);
            pb += std::pow(rb, 2) + std::pow(gb, 2) + std::pow(bb, 2);
            ab += (ra * rb) + (ga * gb) + (ba * bb);
        }
    }

    return std::pow(ab, 2) / (pa * pb);
}

//------------------------------------------------------------------------------

double tester::compare_images_histogram(const QImage& _x, const QImage& _y)
{
    const QVector<QVector<QVector<double> > > histogram_a = compute_histogram(_x);
    const QVector<QVector<QVector<double> > > histogram_b = compute_histogram(_y);
    double res                                            = 0;
    for(int r = 0 ; r < 256 ; r++)
    {
        for(int g = 0 ; g < 256 ; g++)
        {
            for(int b = 0 ; b < 256 ; b++)
            {
                double addition = histogram_a[r][g][b] + histogram_b[r][g][b];
                if(addition > 0)
                {
                    res += std::pow(histogram_a[r][g][b] - histogram_b[r][g][b], 2) / addition;
                }
            }
        }
    }

    return 1 - (res / 2);
}

//------------------------------------------------------------------------------

double tester::compare_images_correlation(QImage _a, QImage _b, bool _strict)
{
    if(_a.size() != _b.size())
    {
        if(_strict)
        {
            return 0;
        }

        const QSize size = _a.size().boundedTo(_b.size());
        _a = _a.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _b = _b.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    double res = 0;
    for(int y = 0 ; y < _a.height() ; y++)
    {
        for(int x = 0 ; x < _a.width() ; x++)
        {
            const QColor ca = _a.pixelColor(x, y);
            const QColor cb = _b.pixelColor(x, y);
            res += std::pow(ca.red() - cb.red(), 2) + std::pow(ca.green() - cb.green(), 2) + std::pow(
                ca.blue() - cb.blue(),
                2
            );
        }
    }

    const int size = _a.height() * _a.width();
    return 1 - (6 * res) / (std::pow(size, 2) - 1);
}

//------------------------------------------------------------------------------

double tester::compare_images_voodoo(const QImage& _a, const QImage& _b)
{
    const QImage av = voodooize(_a);
    const QImage bv = voodooize(_b);
    return compare_images_mse(av, bv);
}

//------------------------------------------------------------------------------

void tester::fail(const std::string& _message)
{
    throw tester_assertion_failed(_message);
}

//------------------------------------------------------------------------------

std::string tester::generate_failure_message()
{
    std::string message;
    if(m_graphic_component != nullptr)
    {
        message += "GIVEN: " + m_component_description + '\n';
    }

    if(!m_interactions.empty())
    {
        message += "WHEN: ";
        for(size_t i = 0 ; i < m_interactions.size() ; i++)
        {
            if(i != 0)
            {
                message += ", ";
            }

            message += m_interactions[i]->to_string();
        }

        message += '\n';
    }

    if(!m_result_description.empty())
    {
        message += "THEN: " + m_result_description + '\n';
    }

    if(!m_backtrace.empty())
    {
        message += "BACKTRACE: " + boost::join(m_backtrace, " / ") + '\n';
    }

    return message;
}

//------------------------------------------------------------------------------

QVector<QVector<QVector<double> > > tester::compute_histogram(const QImage& _img)
{
    QVector<QVector<QVector<double> > > histogram(256, QVector(256, QVector(256, 0.0)));
    for(int y = 0 ; y < _img.height() ; y++)
    {
        for(int x = 0 ; x < _img.width() ; x++)
        {
            QColor color = _img.pixelColor(x, y);
            histogram[color.red()][color.green()][color.blue()]++;
        }
    }

    for(int r = 0 ; r < 256 ; r++)
    {
        for(int g = 0 ; g < 256 ; g++)
        {
            for(int b = 0 ; b < 256 ; b++)
            {
                histogram[r][g][b] /= _img.width() * _img.height();
            }
        }
    }

    return histogram;
}

//------------------------------------------------------------------------------

QImage tester::normalize(QImage _img)
{
    int lightest_red   = -1;
    int lightest_green = -1;
    int lightest_blue  = -1;
    int darkest_red    = 256;
    int darkest_green  = 256;
    int darkest_blue   = 256;
    for(int y = 0 ; y < _img.height() ; y++)
    {
        for(int x = 0 ; x < _img.width() ; x++)
        {
            QColor color = _img.pixelColor(x, y);

            lightest_red   = std::max(lightest_red, color.red());
            darkest_red    = std::min(darkest_red, color.red());
            lightest_green = std::max(lightest_green, color.green());
            darkest_green  = std::min(darkest_green, color.green());
            lightest_blue  = std::max(lightest_blue, color.blue());
            darkest_blue   = std::min(darkest_blue, color.blue());
        }
    }

    // If the lightest and darkest color are the same, don't modify the color
    if(lightest_red == darkest_red)
    {
        darkest_red  = 0;
        lightest_red = 255;
    }

    if(lightest_green == darkest_green)
    {
        darkest_green  = 0;
        lightest_green = 255;
    }

    if(lightest_blue == darkest_blue)
    {
        darkest_blue  = 0;
        lightest_blue = 255;
    }

    for(int y = 0 ; y < _img.height() ; y++)
    {
        for(int x = 0 ; x < _img.width() ; x++)
        {
            QColor color = _img.pixelColor(x, y);
            color.setRed((color.red() - darkest_red) * (255 / (lightest_red - darkest_red)));
            color.setGreen((color.green() - darkest_green) * (255 / (lightest_green - darkest_green)));
            color.setBlue((color.blue() - darkest_blue) * (255 / (lightest_blue - darkest_blue)));
            _img.setPixelColor(x, y, color);
        }
    }

    return _img;
}

//------------------------------------------------------------------------------

QImage tester::voodooize(const QImage& _img)
{
    QImage res = _img.scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    res = normalize(res);
    // We must put the lightest edge in the top left, the next next darker edge to the top right, the next one to the
    // bottom left, etc
    for(int i = 0 ; i < 2 ; i++)
    {
        int lightest                = 0;
        int edge                    = 0;
        const QVector<QColor> edges =
        {res.pixelColor(0, 0), res.pixelColor(63, 0), res.pixelColor(0, 63), res.pixelColor(
             63,
             63
        )
        };
        for(int j = i ; j < 4 ; j++)
        {
            int light = edges[i].red() + edges[i].green() + edges[i].blue();
            if(light > lightest)
            {
                lightest = light;
                edge     = i;
            }
        }

        // First edge:
        //  - Top left: Do nothing
        //  - Top right: Flip horizontally
        //  - Bottom left: Flip vertically
        //  - Bottom right: Flip both horizontally and vertically
        // Second edge:
        //  - Top left: Impossible (the first edge is there)
        //  - Top right: Do nothing
        //  - Bottom left: Flip vertically and rotate to the right
        //  - Bottom right: Impossible to put it to the right place
        res = res.mirrored((i == 0 && ((edge & 1) != 0)), (edge == 2 || (i == 0 && edge == 3)));
        if(i == 1 && edge == 2)
        {
            res = res.transformed(QTransform().rotate(-90));
        }
    }

    return res;
}

//------------------------------------------------------------------------------

bool tester::wait_for_asynchronously(std::function<bool()> _predicate, int _timeout)
{
    return QTest::qWaitFor(
        [_predicate]() -> bool
        {
            QMutex mutex;
            mutex.lock(); // Lock the mutex by default
            bool ok = false;
            qApp->postEvent(
                qApp,
                new test_event(
                    [&mutex, &ok, _predicate]
            {
                ok = _predicate();
                mutex.unlock(); // Unlock the mutex for the calling thread to be unlocked
            })
            );
            mutex.lock();   // Wait for the main thread to execute the predicate
            mutex.unlock(); // We must unlock it, as destroying a locked mutex is undefined behavior
            return ok;
        },
        _timeout
    );
}

//------------------------------------------------------------------------------

QPoint tester::center_of(const QWidget* _widget)
{
    return _widget->rect().center();
}

//------------------------------------------------------------------------------

QPoint tester::left_of(const QWidget* _widget)
{
    return {1, center_of(_widget).y()};
}

//------------------------------------------------------------------------------

QPoint tester::right_of(const QWidget* _widget)
{
    return {_widget->width() - 1, center_of(_widget).y()};
}

//------------------------------------------------------------------------------

QPoint tester::top_of(const QWidget* _widget)
{
    return {center_of(_widget).x(), 1};
}

//------------------------------------------------------------------------------

QPoint tester::bottom_of(const QWidget* _widget)
{
    return {center_of(_widget).x(), _widget->height() - 1};
}

//------------------------------------------------------------------------------

std::filesystem::path tester::get_image_output_path()
{
    return s_image_output_path;
}

//------------------------------------------------------------------------------

QTouchDevice* tester::get_dummy_touch_screen()
{
    static QTouchDevice* res = nullptr;
    if(res == nullptr)
    {
        res = QTest::createTouchDevice();
        res->setMaximumTouchPoints(2);
    }

    return res;
}

} // namespace sight::ui::test
