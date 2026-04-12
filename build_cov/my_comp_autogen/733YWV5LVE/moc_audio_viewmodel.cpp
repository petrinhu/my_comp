/****************************************************************************
** Meta object code from reading C++ file 'audio_viewmodel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/presentation/viewmodels/audio_viewmodel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audio_viewmodel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN7my_comp12presentation10viewmodels14AudioViewModelE_t {};
} // unnamed namespace

template <> constexpr inline auto my_comp::presentation::viewmodels::AudioViewModel::qt_create_metaobjectdata<qt_meta_tag_ZN7my_comp12presentation10viewmodels14AudioViewModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "my_comp::presentation::viewmodels::AudioViewModel",
        "audioDataChanged",
        "",
        "isCapturingChanged",
        "start",
        "stop",
        "waveform",
        "QVariantList",
        "spectrum",
        "rms",
        "peak",
        "isCapturing"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'audioDataChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'isCapturingChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'start'
        QtMocHelpers::MethodData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'stop'
        QtMocHelpers::MethodData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'waveform'
        QtMocHelpers::PropertyData<QVariantList>(6, 0x80000000 | 7, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'spectrum'
        QtMocHelpers::PropertyData<QVariantList>(8, 0x80000000 | 7, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'rms'
        QtMocHelpers::PropertyData<float>(9, QMetaType::Float, QMC::DefaultPropertyFlags, 0),
        // property 'peak'
        QtMocHelpers::PropertyData<float>(10, QMetaType::Float, QMC::DefaultPropertyFlags, 0),
        // property 'isCapturing'
        QtMocHelpers::PropertyData<bool>(11, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AudioViewModel, qt_meta_tag_ZN7my_comp12presentation10viewmodels14AudioViewModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject my_comp::presentation::viewmodels::AudioViewModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7my_comp12presentation10viewmodels14AudioViewModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7my_comp12presentation10viewmodels14AudioViewModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7my_comp12presentation10viewmodels14AudioViewModelE_t>.metaTypes,
    nullptr
} };

void my_comp::presentation::viewmodels::AudioViewModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AudioViewModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->audioDataChanged(); break;
        case 1: _t->isCapturingChanged(); break;
        case 2: _t->start(); break;
        case 3: _t->stop(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AudioViewModel::*)()>(_a, &AudioViewModel::audioDataChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AudioViewModel::*)()>(_a, &AudioViewModel::isCapturingChanged, 1))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QVariantList*>(_v) = _t->waveform(); break;
        case 1: *reinterpret_cast<QVariantList*>(_v) = _t->spectrum(); break;
        case 2: *reinterpret_cast<float*>(_v) = _t->rms(); break;
        case 3: *reinterpret_cast<float*>(_v) = _t->peak(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->isCapturing(); break;
        default: break;
        }
    }
}

const QMetaObject *my_comp::presentation::viewmodels::AudioViewModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *my_comp::presentation::viewmodels::AudioViewModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7my_comp12presentation10viewmodels14AudioViewModelE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int my_comp::presentation::viewmodels::AudioViewModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
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
void my_comp::presentation::viewmodels::AudioViewModel::audioDataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void my_comp::presentation::viewmodels::AudioViewModel::isCapturingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
