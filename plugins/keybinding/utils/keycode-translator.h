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

#ifndef KIRAN_CONTROL_PANEL_KEYCODE_TRANSLATOR_H
#define KIRAN_CONTROL_PANEL_KEYCODE_TRANSLATOR_H

#include <QObject>

class KeycodeTranslator : public  QObject
{
    Q_OBJECT
public:
    //TODO:该处的枚举值应和Qt::Key对应，不应直接赋值
    enum Key
    {
        Escape = 0x01000000,
        Tab = 0x01000001,
        Backtab = 0x01000002,
        Backspace = 0x01000003,
        Return = 0x01000004,
        Enter = 0x01000005,
        Insert = 0x01000006,
        Delete = 0x01000007,
        Pause = 0x01000008,
        Print = 0x01000009,
        SysReq = 0x0100000a,
        Clear = 0x0100000b,
        Home = 0x01000010,
        End = 0x01000011,
        Left = 0x01000012,
        Up = 0x01000013,
        Right = 0x01000014,
        Down = 0x01000015,
        PgUp = 0x01000016,
        PgDown = 0x01000017,
        Shift = 0x01000020,
        Ctrl = 0x01000021,
        Meta = 0x01000022,
        Alt = 0x01000023,
        CapsLock = 0x01000024,
        NumLock = 0x01000025,
        ScrollLock = 0x01000026,
        F1 = 0x01000030,
        F2 = 0x01000031,
        F3 = 0x01000032,
        F4 = 0x01000033,
        F5 = 0x01000034,
        F6 = 0x01000035,
        F7 = 0x01000036,
        F8 = 0x01000037,
        F9 = 0x01000038,
        F10 = 0x01000039,
        F11 = 0x0100003a,
        F12 = 0x0100003b,
        Super_L = 0x01000053,
        Super_R = 0x01000054,
        Menu = 0x01000055,
        Hyper_L = 0x01000056,
        Hyper_R = 0x01000057,
        Help = 0x01000058,
        Direction_L = 0x01000059,
        Direction_R = 0x01000060,
        Space = 0x20,
        Any = Space,
        Exclam = 0x21,      //!
        QuoteDbl = 0x22,    //
        NumberSign = 0x23,  //#
        Dollar = 0x24,      //$
        Percent = 0x25,
        Ampersand = 0x26,  //&
        Apostrophe = 0x27,
        ParenLeft = 0x28,
        ParenRight = 0x29,
        Asterisk = 0x2a,  //*
        Plus = 0x2b,
        Comma = 0x2c,  // ,
        Minus = 0x2d,
        Period = 0x2e,  //.
        Slash = 0x2f,   // /
        Key_0 = 0x30,
        Key_1 = 0x31,
        Key_2 = 0x32,
        Key_3 = 0x33,
        Key_4 = 0x34,
        Key_5 = 0x35,
        Key_6 = 0x36,
        Key_7 = 0x37,
        Key_8 = 0x38,
        Key_9 = 0x39,
        Colon = 0x3a,
        Semicolon = 0x3b,
        Less = 0x3c,
        Equal = 0x3d,
        Greater = 0x3e,
        Question = 0x3f,
        At = 0x40,
        A = 0x41,
        B = 0x42,
        C = 0x43,
        D = 0x44,
        E = 0x45,
        F = 0x46,
        G = 0x47,
        H = 0x48,
        I = 0x49,
        J = 0x4a,
        K = 0x4b,
        L = 0x4c,
        M = 0x4d,
        N = 0x4e,
        O = 0x4f,
        P = 0x50,
        Q = 0x51,
        R = 0x52,
        S = 0x53,
        T = 0x54,
        U = 0x55,
        V = 0x56,
        W = 0x57,
        X = 0x58,
        Y = 0x59,
        Z = 0x5a,
        BracketLeft = 0x5b,  // [
        Backslash = 0x5c,
        BracketRight = 0x5d,  // ]
        AsciiCircum = 0x5e,
        Underscore = 0x5f,
        QuoteLeft = 0x60,
        BraceLeft = 0x7b,   //{
        Bar = 0x7c,         //|
        BraceRight = 0x7d,  //}
        AsciiTilde = 0x7e,  //~
        //media
        Audio_Lower_Volume = 0x01000070,
        Audio_Mute = 0x01000071,
        Audio_Raise_Volume = 0x01000072,
        Audio_Play = 0x01000080,
        Audio_Stop = 0x01000081,
        Audio_Prev = 0x01000082,
        Audio_Next = 0x01000083,
        Audio_Record = 0x01000084,
        Audio_Pause = 0x1000085,
        Media_TogglePlay_Pause = 0x1000086,
        Home_Page = 0x01000090,
        Favorites = 0x01000091,
        Search = 0x01000092,
        Standby = 0x01000093,
        OpenUrl = 0x01000094,
        Mail = 0x010000a0,
        Audio_Media = 0x010000a1,
        My_Computer = 0x010000a2,  // My Computer
        Calculator = 0x010000a3,   // Calculator
        Eject = 0x010000b9,
        WWW = 0x010000bb,
        Explorer = 0x010000d5,
        Tools = 0x010000f1,
        Audio_Mic_Mute = 0x01000113,
    };
    Q_ENUM(Key)

private:
    KeycodeTranslator(QObject* parent);

public:
    static QString keycode2ReadableString(const QList<int>& keycodes);
    static QString backendKeyString2Readable(const QString& keyString);
    static QString readableKeyString2Backend(const QString& keyString);
};


#endif //KIRAN_CONTROL_PANEL_KEYCODE_TRANSLATOR_H
