﻿/*
 * Copyright (c) 2016 J-P Nurmi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "statusbar_p.h"

#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>

// WindowManager.LayoutParams
#define FLAG_FULLSCREEN 0x00000400
#define FLAG_LAYOUT_NO_LIMITS 0x00000200
#define FLAG_TRANSLUCENT_STATUS 0x04000000
#define FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS 0x80000000
// View
#define SYSTEM_UI_FLAG_LIGHT_STATUS_BAR 0x00002000

static QJniObject getAndroidWindow()
{

    QJniObject obj = QtAndroidPrivate::activity();
    QJniObject window=obj.callObjectMethod("getWindow", "()Landroid/view/Window;");
    window.callMethod<void>("addFlags", "(I)V", FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
    window.callMethod<void>("clearFlags", "(I)V",  FLAG_LAYOUT_NO_LIMITS);
    //window.callMethod<void>("addFlags", "(I)V", FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);

    return window;
}


bool StatusBarPrivate::isAvailable_sys()
{
    return QtAndroidPrivate::androidSdkVersion() >= 21;
}
void StatusBarPrivate::addFLAG(StatusBar::FLAG flag){
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=](){
        QJniObject window = getAndroidWindow();
        if(flag==StatusBar::FLAG::SET_FULLSCREEN){
            window.callMethod<void>("addFlags", "(I)V", FLAG_FULLSCREEN);
        }else if(flag==StatusBar::NOT_FULLSCREEN){
            window.callMethod<void>("clearFlags", "(I)V", FLAG_FULLSCREEN);
        }
    });
}
void StatusBarPrivate::setColor_sys(const QColor &color)
{
    if (QtAndroidPrivate::androidSdkVersion() < 21)
        return;
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=]() {
        QJniObject window = getAndroidWindow();
        window.callMethod<void>("setStatusBarColor", "(I)V", color.rgba());
    });
}

void StatusBarPrivate::setTheme_sys(StatusBar::Theme theme)
{
    if (QtAndroidPrivate::androidSdkVersion() < 23) return;
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=](){
        QJniObject window = getAndroidWindow();
        QJniObject view = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
        int visibility = view.callMethod<int>("getSystemUiVisibility", "()I");
        if (theme == StatusBar::Theme::Light){
            visibility |= SYSTEM_UI_FLAG_LIGHT_STATUS_BAR;
        }else{
            visibility &= ~SYSTEM_UI_FLAG_LIGHT_STATUS_BAR;
        }
        view.callMethod<void>("setSystemUiVisibility", "(I)V", visibility);
    });
}
