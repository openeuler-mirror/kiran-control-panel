/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "keycode-translator.h"
#include <qt5-log-i.h>
#include <QMetaEnum>
#include <QApplication>

static QMetaEnum keyMetaEnum = QMetaEnum::fromType<KeycodeTranslator::Key>();

// clang-format off
static const QMap<QString, QString> SpecialKeyMap = {
        {"exclam", "!"},
        {"at", "@"},
        {"numbersign", "#"},
        {"dollar", "$"},
        {"percent", "%"},
        {"asciicircum", "^"},
        {"ampersand", "&"},
        {"asterisk", "*"},
        {"parenleft", "("},
        {"parenRight", ")"},
        {"underscore", "_"},
        {"plus", "+"},
        {"braceleft", "{"},
        {"braceright", "}"},
        {"bar", "|"},
        {"colon", ":"},
        {"quotedbl", "\""},
        {"quoteleft", "`"},
        {"less", "<"},
        {"greater", ">"},
        {"question", "?"},
        {"minus", "-"},
        {"equal", "="},
        {"bracketleft", "["},
        {"bracketright", "]"},
        {"backslash", "\\"},
        {"semicolon", ";"},
        {"apostrophe", "'"},
        {"comma", ","},
        {"period", "."},
        {"slash", "/"},
        {"up", "↑"},
        {"left", "←"},
        {"down", "↓"},
        {"right", "→"},
        {"asciitilde", "~"},
        {"grave", "`"},
        {"control", "Ctrl"},
        {"super_l", "Super"},
        {"super_r", "Super"}
};

static const QMap<QString, QString> MediaKeyMap = {
        {"XF86AudioPlay", QT_TRANSLATE_NOOP("Media Key", "Audio Play")},
        {"XF86Search", QT_TRANSLATE_NOOP("Media Key", "Search")},
        {"XF86WWW", QT_TRANSLATE_NOOP("Media Key", "WWW")},
        {"XF86AudioLowerVolume", QT_TRANSLATE_NOOP("Media Key", "Audio Lower Volume")},
        {"XF86AudioRaiseVolume", QT_TRANSLATE_NOOP("Media Key", "Audio Raise Volume")},
        {"XF86AudioMicMute", QT_TRANSLATE_NOOP("Media Key", "Mic Mute")},
        {"XF86AudioStop", QT_TRANSLATE_NOOP("Media Key", "Audio Stop")},
        {"XF86Explorer", QT_TRANSLATE_NOOP("Media Key", "Explorer")},
        {"XF86Calculator", QT_TRANSLATE_NOOP("Media Key", "Calculator")},
        {"XF86AudioMute", QT_TRANSLATE_NOOP("Media Key", "Audio Mute")},
        {"XF86AudioPause", QT_TRANSLATE_NOOP("Media Key", "Audio Pause")},
        {"XF86AudioPrev", QT_TRANSLATE_NOOP("Media Key", "Audio Prev")},
        {"XF86AudioMedia", QT_TRANSLATE_NOOP("Media Key", "Audio Media")},
        {"XF86AudioNext", QT_TRANSLATE_NOOP("Media Key", "Audio Next")},
        {"XF86Mail", QT_TRANSLATE_NOOP("Media Key", "Mail")},
        {"XF86Tools", QT_TRANSLATE_NOOP("Media Key", "Tools")},
        {"XF86Eject", QT_TRANSLATE_NOOP("Media Key", "Eject")}
};
// clang-format on

KeycodeTranslator::KeycodeTranslator(QObject *parent)
    : QObject(parent)
{

}

QString KeycodeTranslator::keycode2ReadableString(const QList<int> &keycodes)
{
    QStringList keyStrings;

    for( int key : keycodes )
    {
        if( (key >= Qt::Key_0) && (key <= Qt::Key_9) )
        {
            keyStrings.append(QString::number(key-Qt::Key_0));
            continue;
        }

        const char* keyValue = keyMetaEnum.valueToKey(key);
        if( keyValue == nullptr )
        {
            KLOG_WARNING() << "can't convert key:" << key;
            continue;
        }

        QString keyStr(keyValue);
        //特殊按键经QMetaEnum翻译之后再经过SpecialKeyMap翻译
        if( SpecialKeyMap.contains(keyStr.toLower()) )
        {
            keyStrings.append(SpecialKeyMap.value(keyStr.toLower()));
        }
        //特殊按键 "_" 转换成 " "
        else if( (key >= Audio_Lower_Volume) && (key <= Audio_Mic_Mute) )
        {
            keyStrings.append(keyStr.split("_").join(" "));
        }
        else
        {
            keyStrings.append(keyStr);
        }
    }

    return keyStrings.join('+');
}

QString KeycodeTranslator::backendKeyString2Readable(const QString &keyString)
{
    QString readableString;

    if( keyString.isEmpty() )
    {
        readableString = tr("None");
    }
    else if( keyString.contains("disable",Qt::CaseInsensitive) )
    {
        readableString = tr("disabled");
    }
    else
    {
        QString temp = keyString;
        temp = temp.replace("<","");
        temp = temp.replace(">","-");
        QStringList keyList = temp.split("-",Qt::SkipEmptyParts);
        for(int i=0;i<keyList.size();i++)
        {
            if( SpecialKeyMap.contains(keyList.at(i).toLower()) )
            {
                keyList.replace(i,SpecialKeyMap.value(keyList.at(i).toLower()));
            }
            else if (MediaKeyMap.contains(keyList.at(i)))
            {
                QString media = QApplication::translate("Media Key", MediaKeyMap.value(keyList.at(i)).toStdString().c_str());
                keyList.replace(i, media);
            }
        }
        readableString = keyList.join('+');
    }

    return readableString;
}

QString KeycodeTranslator::readableKeyString2Backend(const QString &keyString)
{
    QStringList keystrings = keyString.split('+');
    for( int i=0;i<keystrings.count();i++ )
    {
        QString key = keystrings.at(i);

        //modifier
        if (!key.compare("Alt", Qt::CaseInsensitive) ||
            !key.compare("Shift", Qt::CaseInsensitive) ||
            !key.compare("Ctrl", Qt::CaseInsensitive))
        {
            QString str = "<" + key + ">";
            keystrings.replace(i, str);
        }
        //media key
        else if (key.contains(" "))
        {
            QString str = QString("XF86%1").arg(key.split(" ").join(""));
            keystrings.replace(i, str);
        }
        //special key
        else if (!key.contains(QRegExp("[A-Z]")) &&
                 !key.contains(QRegExp("[a-z]")) &&
                 !key.contains(QRegExp("[0-9]")))
        {
            keystrings.replace(i, SpecialKeyMap.key(key));
        }
    }

    return keystrings.join("");
}
