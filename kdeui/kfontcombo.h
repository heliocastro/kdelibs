/* This file is part of the KDE libraries
   Copyright (c) 2001 Malte Starostik <malte.starostik@t-online.de>

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

// $Id$

#ifndef _KFONTCOMBO_H_
#define _KFONTCOMBO_H_

#include <kcombobox.h>

/**
 * A combobox that lists the available fonts.
 *
 * The items are painted using the respective font itself, so the user
 * can easily choose a font based on its look. This can be turned off
 * globally if the user wishes so.
 *
 * @short A combobox to choose a font
 * @author Malte Starostik <malte.starostik@t-online.de>
 * @version $Id$
 */
class KFontCombo : public KComboBox
{
    Q_OBJECT
public:
    /**
     * Constructor
     *
     * @param parent The parent widget
     * @param name The object name for the widget
     */
    KFontCombo(QWidget *parent, const char *name = 0);
    /**
     * Constructor that takes an already initialzed font list
     *
     * @param fonts A list of fonts to show
     * @param parent The parent widget
     * @param name The object name for the widget
     */
    KFontCombo(const QStringList &fonts, QWidget *parent, const char *name = 0);
    /**
     * Destructor
     */
    virtual ~KFontCombo();

    /**
     * Sets the font list
     *
     * @param fonts Font list to show
     */
    void setFonts(const QStringList &fonts);
    /**
     * Set the listed fonts to bold or normal
     *
     * @param bold Set to true to display fonts in bold
     */
    void setBold(bool bold);
    /**
     * Retrieve the current bold status
     *
     * @return True if fonts are bold
     */
    bool bold() const;
    /**
     * Set the listed fonts to italic or regular
     *
     * @param italic Set to true to display fonts italic
     */
    void setItalic(bool bold);
    /**
     * Retrieve the current italic status
     *
     * @return True if fonts are italic
     */
    bool italic() const;
    /**
     * Set the listed fonts to underlined or not underlined
     *
     * @param underline Set to true to display fonts underlined
     */
    void setUnderline(bool bold);
    /**
     * Retrieve the current underline status
     *
     * @return True if fonts are underlined
     */
    bool underline() const;
    /**
     * Set the listed fonts to striked out or not
     *
     * @param strikeOut Set to true to display fonts striked out
     */
    void setStrikeOut(bool bold);
    /**
     * Retrieve the current strike out status
     *
     * @return True if fonts are striked out
     */
    bool strikeOut() const;
    /**
     * Set the listed fonts' size
     *
     * @param size Set to the point size to display the fonts in
     */
    void setSize(int size);
    /**
     * Retrieve the current font size
     *
     * @return The point size of the fonts
     */
    int size() const;

    /**
     * Retrieve the user's setting of whether the items should be painted
     * in the respective fonts or not
     *
     * @return True if the respective fonts are used for painting
     */
    static bool displayFonts();

protected:
    /**
     * Updated the combo's @ref listBox() to reflect changes made to the
     * fonts' attributed
     */
    void updateFonts();

private:
    void init();

private:
    friend class KFontListItem;
    struct KFontComboPrivate *d;
};

#endif

