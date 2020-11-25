/****************************************************************************
** Meta object code from reading C++ file 'kiran-module-widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/kiran-module-widget/kiran-module-widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kiran-module-widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KiranModuleWidget_t {
    QByteArrayData data[9];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KiranModuleWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KiranModuleWidget_t qt_meta_stringdata_KiranModuleWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "KiranModuleWidget"
QT_MOC_LITERAL(1, 18, 26), // "onSelectedClassItemChanged"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 21), // "QMap<int,ModuleItem>*"
QT_MOC_LITERAL(4, 68, 7), // "modules"
QT_MOC_LITERAL(5, 76, 39), // "on_listWidget_module_currentI..."
QT_MOC_LITERAL(6, 116, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 133, 7), // "current"
QT_MOC_LITERAL(8, 141, 8) // "previous"

    },
    "KiranModuleWidget\0onSelectedClassItemChanged\0"
    "\0QMap<int,ModuleItem>*\0modules\0"
    "on_listWidget_module_currentItemChanged\0"
    "QListWidgetItem*\0current\0previous"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KiranModuleWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       5,    2,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 6,    7,    8,

       0        // eod
};

void KiranModuleWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KiranModuleWidget *_t = static_cast<KiranModuleWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onSelectedClassItemChanged((*reinterpret_cast< QMap<int,ModuleItem>*(*)>(_a[1]))); break;
        case 1: _t->on_listWidget_module_currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KiranModuleWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KiranModuleWidget.data,
      qt_meta_data_KiranModuleWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KiranModuleWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KiranModuleWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KiranModuleWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KiranModuleWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
