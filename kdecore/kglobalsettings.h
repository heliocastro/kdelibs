/* This file is part of the KDE libraries
   Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef _KGLOBALSETTINGS_H
#define _KGLOBALSETTINGS_H

#include <qstring.h>

/**
 * Access the KDE global configuration.
 *
 * @author David Faure <faure@kde.org>
 * @version $Id$
 */
class KGlobalSettings
{
  public:

    /**
     * Returns a treshold in pixels for drag & drop operations.
     * As long as the mouse movement has not exceeded this number
     * of pixels in either X or Y direction no drag operation may
     * be started. This prevents spurious drags when the user intended
     * to click on something but moved the mouse a bit while doing so.
     *
     * For this to work you must save the position of the mouse (@p oldPos)
     * in the @ref QWidget::mousePressEvent().
     * When the position of the mouse (@p newPos)
     * in a  @ref QWidget::mouseMoveEvent() exceeds this treshold
     * you may start a drag
     * which should originate from @ref oldPos.
     *
     * Example code:
     * <pre>
     * void KColorCells::mousePressEvent( QMouseEvent *e )
     * {
     *    mOldPos = e->pos();
     * }
     *
     * void KColorCells::mouseMoveEvent( QMouseEvent *e )
     * {
     *    if( !(e->state() && LeftButton)) return;
     *
     *    int delay = KGlobalSettings::dndEventDelay();
     *    QPoint newPos = e->pos();
     *    if(newPos->x() > mOldPos.x()+delay || newPos->x() < mOldPos.x()-delay ||
     *       newPos->y() > mOldPos.y()+delay || newPos->y() < mOldPos.y()-delay)
     *    {
     *       // Drag color object
     *       int cell = posToCell(mOldPos); // Find color at mOldPos
     *       if ((cell != -1) && colors[cell].isValid())
     *       {
     *          KColorDrag *d = KColorDrag::makeDrag( colors[cell], this);
     *          d->dragCopy();
     *       }
     *    }
     * }
     * </pre>
     *
     */

    static int dndEventDelay();

    /**
     * Returns whether KDE runs in single (default) or double click
     * mode.
     *
     * @return @p true if single click mode, or @p false if double click mode.
     *
     * see @ref http://developer.kde.org/documentation/standards/kde/style/mouse/index.html
     **/
    static bool singleClick();

    /**
     * @return the KDE setting for "change cursor over icon"
     */
    static bool changeCursorOverIcon();

    /**
     * Returns the KDE setting for the auto-select option
     *
     * @return the auto-select delay or -1 if auto-select is disabled.
     */
    static int autoSelectDelay();

    /**
     * Retrieve the configured completion mode.
     *
     * see @ref http://developer.kde.org/documentation/standards/kde/style/keys/completion.html
     *
     * @return CompletionNone:  Completion should be disabled
     *         CompletionAuto:  Automatic completion
     *         CompletionMan:   Like automatic completion except the user initiates
     *                          the completion using the completion key as in CompletionEOL
     *         CompletionShell: Attempts to mimic the completion feature found in
     *                          typical *nix shell enviornments.
     **/

    enum Completion { CompletionNone=1, CompletionAuto, CompletionMan, CompletionShell };

    static Completion completionMode();

    /**
     * This returns whether or not KDE should use certain GNOME
     * resources.  The resources, in this case, are things like pixmap
     * directories, applnk paths, etc.
     *
     * @return Whether or not KDE should use certain GNOME resources
     */
    static bool honorGnome();

    /**
     * The path to the desktop directory of the current user.
     */
    static QString desktopPath() { initStatic(); return *s_desktopPath; }

    /**
     * The path to the templates directory of the current user.
     */
    static QString templatesPath() { initStatic(); return *s_templatesPath; }

    /**
     * The path to the autostart directory of the current user.
     */
    static QString autostartPath() { initStatic(); return *s_autostartPath; }

    /**
     * The path to the trash directory of the current user.
     */
    static QString trashPath() { initStatic(); return *s_trashPath; }


private:
    /**
     * reads in all paths from kdeglobals
     */
    static void initStatic();

    static QString* s_desktopPath;
    static QString* s_templatesPath;
    static QString* s_autostartPath;
    static QString* s_trashPath;

};

#endif
