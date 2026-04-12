/****************************************************************************
** Meta object code from reading C++ file 'process_viewmodel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/presentation/viewmodels/process_viewmodel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'process_viewmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN7my_comp12presentation10viewmodels16ProcessViewModelE_t {};
} // unnamed namespace

template <> constexpr inline auto my_comp::presentation::viewmodels::ProcessViewModel::qt_create_metaobjectdata<qt_meta_tag_ZN7my_comp12presentation10viewmodels16ProcessViewModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "my_comp::presentation::viewmodels::ProcessViewModel",
        "processesChanged",
        "",
        "filterChanged",
        "sortFieldChanged",
        "sortAscendingChanged",
        "isBusyChanged",
        "refresh",
        "killProcess",
        "pid",
        "signal",
        "setPriority",
        "nice",
        "processes",
        "QVariantList",
        "filter",
        "sortField",
        "sortAscending",
        "isBusy"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'processesChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'filterChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sortFieldChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sortAscendingChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'isBusyChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'refresh'
        QtMocHelpers::MethodData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'killProcess'
        QtMocHelpers::MethodData<void(int, int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 }, { QMetaType::Int, 10 },
        }}),
        // Method 'killProcess'
        QtMocHelpers::MethodData<void(int)>(8, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Method 'setPriority'
        QtMocHelpers::MethodData<void(int, int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 }, { QMetaType::Int, 12 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'processes'
        QtMocHelpers::PropertyData<QVariantList>(13, 0x80000000 | 14, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'filter'
        QtMocHelpers::PropertyData<QString>(15, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'sortField'
        QtMocHelpers::PropertyData<QString>(16, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'sortAscending'
        QtMocHelpers::PropertyData<bool>(17, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'isBusy'
        QtMocHelpers::PropertyData<bool>(18, QMetaType::Bool, QMC::DefaultPropertyFlags, 4),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ProcessViewModel, qt_meta_tag_ZN7my_comp12presentation10viewmodels16ProcessViewModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject my_comp::presentation::viewmodels::ProcessViewModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7my_comp12presentation10viewmodels16ProcessViewModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7my_comp12presentation10viewmodels16ProcessViewModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7my_comp12presentation10viewmodels16ProcessViewModelE_t>.metaTypes,
    nullptr
} };

void my_comp::presentation::viewmodels::ProcessViewModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ProcessViewModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->processesChanged(); break;
        case 1: _t->filterChanged(); break;
        case 2: _t->sortFieldChanged(); break;
        case 3: _t->sortAscendingChanged(); break;
        case 4: _t->isBusyChanged(); break;
        case 5: _t->refresh(); break;
        case 6: _t->killProcess((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->killProcess((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->setPriority((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ProcessViewModel::*)()>(_a, &ProcessViewModel::processesChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProcessViewModel::*)()>(_a, &ProcessViewModel::filterChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProcessViewModel::*)()>(_a, &ProcessViewModel::sortFieldChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProcessViewModel::*)()>(_a, &ProcessViewModel::sortAscendingChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProcessViewModel::*)()>(_a, &ProcessViewModel::isBusyChanged, 4))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QVariantList*>(_v) = _t->processes(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->filter(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->sortField(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->sortAscending(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->isBusy(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setFilter(*reinterpret_cast<QString*>(_v)); break;
        case 2: _t->setSortField(*reinterpret_cast<QString*>(_v)); break;
        case 3: _t->setSortAscending(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *my_comp::presentation::viewmodels::ProcessViewModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *my_comp::presentation::viewmodels::ProcessViewModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7my_comp12presentation10viewmodels16ProcessViewModelE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int my_comp::presentation::viewmodels::ProcessViewModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void my_comp::presentation::viewmodels::ProcessViewModel::processesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void my_comp::presentation::viewmodels::ProcessViewModel::filterChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void my_comp::presentation::viewmodels::ProcessViewModel::sortFieldChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void my_comp::presentation::viewmodels::ProcessViewModel::sortAscendingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void my_comp::presentation::viewmodels::ProcessViewModel::isBusyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
