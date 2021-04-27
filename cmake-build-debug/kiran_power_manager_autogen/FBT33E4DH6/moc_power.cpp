/****************************************************************************
** Meta object code from reading C++ file 'power.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/dbus/power.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'power.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PowerInterface_t {
    QByteArrayData data[22];
    char stringdata0[318];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PowerInterface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PowerInterface_t qt_meta_stringdata_PowerInterface = {
    {
QT_MOC_LITERAL(0, 0, 14), // "PowerInterface"
QT_MOC_LITERAL(1, 15, 17), // "BrightnessChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 6), // "device"
QT_MOC_LITERAL(4, 41, 18), // "EventActionChanged"
QT_MOC_LITERAL(5, 60, 5), // "event"
QT_MOC_LITERAL(6, 66, 17), // "IdleActionChanged"
QT_MOC_LITERAL(7, 84, 6), // "supply"
QT_MOC_LITERAL(8, 91, 13), // "GetBrightness"
QT_MOC_LITERAL(9, 105, 22), // "QDBusPendingReply<int>"
QT_MOC_LITERAL(10, 128, 14), // "GetEventAction"
QT_MOC_LITERAL(11, 143, 13), // "GetIdleAction"
QT_MOC_LITERAL(12, 157, 29), // "QDBusPendingReply<IdleAction>"
QT_MOC_LITERAL(13, 187, 13), // "SetBrightness"
QT_MOC_LITERAL(14, 201, 19), // "QDBusPendingReply<>"
QT_MOC_LITERAL(15, 221, 21), // "brightness_percentage"
QT_MOC_LITERAL(16, 243, 14), // "SetEventAction"
QT_MOC_LITERAL(17, 258, 6), // "action"
QT_MOC_LITERAL(18, 265, 13), // "SetIdleAction"
QT_MOC_LITERAL(19, 279, 12), // "idle_timeout"
QT_MOC_LITERAL(20, 292, 14), // "lid_is_present"
QT_MOC_LITERAL(21, 307, 10) // "on_battery"

    },
    "PowerInterface\0BrightnessChanged\0\0"
    "device\0EventActionChanged\0event\0"
    "IdleActionChanged\0supply\0GetBrightness\0"
    "QDBusPendingReply<int>\0GetEventAction\0"
    "GetIdleAction\0QDBusPendingReply<IdleAction>\0"
    "SetBrightness\0QDBusPendingReply<>\0"
    "brightness_percentage\0SetEventAction\0"
    "action\0SetIdleAction\0idle_timeout\0"
    "lid_is_present\0on_battery"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PowerInterface[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       2,  100, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,
       6,    2,   65,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   70,    2, 0x0a /* Public */,
      10,    1,   73,    2, 0x0a /* Public */,
      11,    2,   76,    2, 0x0a /* Public */,
      13,    2,   81,    2, 0x0a /* Public */,
      16,    2,   86,    2, 0x0a /* Public */,
      18,    4,   91,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    7,

 // slots: parameters
    0x80000000 | 9, QMetaType::Int,    3,
    0x80000000 | 9, QMetaType::Int,    5,
    0x80000000 | 12, QMetaType::Int, QMetaType::Int,    3,    7,
    0x80000000 | 14, QMetaType::Int, QMetaType::Int,    3,   15,
    0x80000000 | 14, QMetaType::Int, QMetaType::Int,    5,   17,
    0x80000000 | 14, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    7,   19,   17,

 // properties: name, type, flags
      20, QMetaType::Bool, 0x00095001,
      21, QMetaType::Bool, 0x00095001,

       0        // eod
};

void PowerInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PowerInterface *_t = static_cast<PowerInterface *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->BrightnessChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->EventActionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->IdleActionChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: { QDBusPendingReply<int> _r = _t->GetBrightness((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<int>*>(_a[0]) = std::move(_r); }  break;
        case 4: { QDBusPendingReply<int> _r = _t->GetEventAction((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<int>*>(_a[0]) = std::move(_r); }  break;
        case 5: { QDBusPendingReply<IdleAction> _r = _t->GetIdleAction((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<IdleAction>*>(_a[0]) = std::move(_r); }  break;
        case 6: { QDBusPendingReply<> _r = _t->SetBrightness((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<>*>(_a[0]) = std::move(_r); }  break;
        case 7: { QDBusPendingReply<> _r = _t->SetEventAction((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<>*>(_a[0]) = std::move(_r); }  break;
        case 8: { QDBusPendingReply<> _r = _t->SetIdleAction((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<>*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PowerInterface::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PowerInterface::BrightnessChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PowerInterface::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PowerInterface::EventActionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PowerInterface::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PowerInterface::IdleActionChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        PowerInterface *_t = static_cast<PowerInterface *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->lid_is_present(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->on_battery(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject PowerInterface::staticMetaObject = {
    { &QDBusAbstractInterface::staticMetaObject, qt_meta_stringdata_PowerInterface.data,
      qt_meta_data_PowerInterface,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *PowerInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PowerInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PowerInterface.stringdata0))
        return static_cast<void*>(this);
    return QDBusAbstractInterface::qt_metacast(_clname);
}

int PowerInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void PowerInterface::BrightnessChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PowerInterface::EventActionChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PowerInterface::IdleActionChanged(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
