/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __konq_htmlsettings_h__
#define __konq_htmlsettings_h__

class KConfig;
#include <qcolor.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qfont.h>
#include <qmap.h>

class KHTMLSettingsPrivate;

/**
 * Settings for the HTML view.
 */
class KHTMLSettings
{
public:

    /**
     * This enum specifies whether Java/JavaScript execution is allowed.
     */
    enum KJavaScriptAdvice {
	KJavaScriptDunno=0,
	KJavaScriptAccept,
	KJavaScriptReject
    };

    /**
     * @internal Constructor
     */
    KHTMLSettings();
    KHTMLSettings(const KHTMLSettings &other);

    /** Called by constructor and reparseConfiguration */
    void init();

    /** Read settings from @p config.
     * @param reset if true, settings are always set; if false,
     *  settings are only set if the config file has a corresponding key.
     */
    void init( KConfig * config, bool reset = true );

    /** Destructor. Don't delete any instance by yourself. */
    virtual ~KHTMLSettings();

    // Behaviour settings
    bool changeCursor();
    bool underlineLink();
    bool hoverLink();

    // Font settings
    QString stdFontName() const;
    QString fixedFontName() const;
    QString serifFontName() const;
    QString sansSerifFontName() const;
    QString cursiveFontName() const;
    QString fantasyFontName() const;

    // these two can be set. Mainly for historical reasons (the method in KHTMLPart exists...)
    void setStdFontName(const QString &n);
    void setFixedFontName(const QString &n);

    const QValueList<int> &fontSizes() const;
    void setFontSizes(const QValueList<int> &newFontSizes );
    void resetFontSizes();

    int minFontSize() const;

    // the charset used to display the current document.
#if QT_VERSION < 300
    QFont::CharSet charset() const;
    void setCharset( QFont::CharSet c );
    QFont::CharSet script() const;
    void setScript( QFont::CharSet c );
    void resetCharset( );

    void setDefaultCharset( QFont::CharSet c, bool b );
#endif

    const QString &encoding() const;

    // Color settings
    const QColor& textColor();
    const QColor& linkColor();
    const QColor& vLinkColor();

    // Autoload images
    bool autoLoadImages();

    // Java and JavaScript
    bool isJavaEnabled( const QString& hostname = QString::null );
    bool isJavaScriptEnabled( const QString& hostname = QString::null );
    bool isJavaScriptDebugEnabled( const QString& hostname = QString::null );
    bool isPluginsEnabled( const QString& hostname = QString::null );
    bool isCSSEnabled( const QString& hostname = QString::null );

    // helpers for parsing domain-specific configuration, used in KControl module as well
    static KJavaScriptAdvice strToAdvice(const QString& _str);
    static void splitDomainAdvice(const QString& configStr, QString &domain,
				  KJavaScriptAdvice &javaAdvice, KJavaScriptAdvice& javaScriptAdvice);
    static const char* adviceToStr(KJavaScriptAdvice _advice);

    QString settingsToCSS() const;
    QString availableFamilies() const;

    QString userStyleSheet() const;

    // Form completion
    bool isFormCompletionEnabled() const;
    int maxFormCompletionItems() const;

private:
#if QT_VERSION < 300
    void setFont(const QFont::CharSet &charset, int i, const QString &n);
    QString lookupFont(const QFont::CharSet &charset, int i) const;

    void internalSetCharset( QFont::CharSet c );
#else
    void setFont(int, int i, const QString &n);
    QString lookupFont(int , int i) const;
#endif
    
    KHTMLSettingsPrivate *d;
};

#endif
