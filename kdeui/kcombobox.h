/* This file is part of the KDE libraries

   Copyright (c) 2000 Dawit Alemayehu <adawit@earthlink.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License (LGPL) as published by the Free Software Foundation; either
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

#ifndef _KCOMBOBOX_H
#define _KCOMBOBOX_H

#include <qcombobox.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qlistbox.h>

#include <kcompletion.h>


/**
 * A combined button, line-edit and a popup list widget.
 *
 * This widget inherits from QComboBox and enhances it
 * with the following functionalities : a built-in @ref
 * KCompletion object which provides automatic & manual
 * completion, the ability to change which keyboard key
 * is used to activate this feature and rotation signals
 * that can be used to iterate through some type of list.
 * They key-bindings for the rotation feature are also
 * configurable and can be interchanged between system and
 * local settings easily.
 *
 * Also since this widget inherits form QComboBox it can
 * easily be used as a drop-in replacement where the above
 * functionalities are needed and/or useful.
 *
 * KComboBox emits a few more additional signals than @ref
 * QComboBox, the main ones being the @ref comepltion and
 * the @ref rotation signal metioned above.  The completion
 * signal is intended to be connected to a slot that will
 * assist the user in filling out the remaining text while
 * the rotation signals, both @ref rotateUp and @ref rotateDown,
 * are intended to be used to transverse through some kind
 * of list in opposing directions.  The @ref returnPressed
 * signals are emitted when the user presses the return key.
 *
 * By default, when you create a completion object through
 * either @ref completionObject() or @ref setCompletionObject
 * this widget will be automatically enabled to handle the
 * signals.  If you do not need this feature, simply use the
 * appropriate accessor methods or the boolean paramters on
 * the above function to shut them off.
 *
 * The default key-binding for completion and rotation is
 * determined from the global settings in @ref KStdAccel.
 * However, these values can be set locally to override these
 * global settings.  Simply invoking @ref useGlobalSettings
 * then allows you to immediately default the bindings back
 * to the global settings again.  You can also default the
 * key-bindings by simply invoking the @ref setXXXKey method
 * without any argumet.  Note that if this widget is not
 * editable, i.e. it is constructed as a "select-only" widget,
 * then only one completion mode, CompletionAuto, is allowed.
 * All the other modes are simply ignored.  The CompletionAuto
 * mode in this case allows you to automatically select
 * an item in the list that matches the pressed key-codes.
 * For example, if you have a list of countries, typing
 * the first few letters of the name attempts to find a
 * match and if one is found it will be selected as the
 * current item.
 *
 * @sect Example:
 *
 * To enable the basic completion feature :
 *
 * <pre>
 * KComboBox *combo = new KComboBox( true, this, "mywidget" );
 * KCompletion *comp = combo->completionObject();
 * // Connect to the return pressed signal - optional
 * connect(combo,SIGNAL(returnPressed(const QString&)),comp,SLOT(addItem(const QString&));
 * </pre>
 *
 * To use a customized completion objects or your
 * own completion object :
 * <pre>
 * KComboBox *combo = new KComboBox( this,"mywidget" );
 * KURLCompletion *comp = new KURLCompletion();
 * combo->setCompletionObject( comp );
 * // Connect to the return pressed signal - optional
 * connect(combo,SIGNAL(returnPressed(const QString&)),comp,SLOT(addItem(const QString&));
 * </pre>
 *
 * Other miscelanous functions :
 *
 * <pre>
 * // Tell the widget not to handle completion and rotation
 * combo->setHandleSignals( false );
 * // set your own completion key for manual completions.
 * combo->setCompletionKey( Qt::End );
 * // Shows the context (popup) menu
 * combo->setEnableContextMenu();
 * // Temporarly disable signal emition
 * combo->disableSignals();
 * // Default the key-bindings to system settings.
 * combo->useGlobalSettings();
 * </pre>
 *
 * @short An enhanced combo box.
 * @author Dawit Alemayehu <adawit@earthlink.net>
 */
class KComboBox : public QComboBox, public KCompletionBase
{
  Q_OBJECT

public:

    /**
    * Constructs a read-only or rather select-only combo box with a parent object
    * and a name.
    *
    * @param parent the parent object of this widget
    * @param name the name of this widget
    */
    KComboBox( QWidget *parent=0, const char *name=0 );

    /**
    * Constructs a "read-write" or "read-only" combo box depending on the value of
    * the first argument( bool rw ) with a parent, a name.
    *
    * @param string text to be shown in the edit widget
    * @param parent the parent object of this widget
    * @param name the name of this widget
    */
    KComboBox( bool rw, QWidget *parent=0, const char *name=0 );

    /**
    * Destructor.
    */
    virtual ~KComboBox();

    /**
    * Returns the current cursor position.
    *
    * This method always returns a -1 if the combo-box is NOT
    * editable (read-write).
    *
    * @return current cursor position.
    */
    int cursorPosition() const { return ( m_pEdit ) ? m_pEdit->cursorPosition() : -1; }

    /**
    * Re-implemented from QComboBox.
    *
    * If true, the completion mode will be set to automatic.
    * Otherwise, it is defaulted to the gloabl setting.  This
    * methods has been replaced by the more comprehensive @ref
    * setCompletionMode.
    *
    * @param autocomplete flag to enable/disable automatic completion mode.
    */
    virtual void setAutoCompletion( bool autocomplete );

    /**
    * Re-implemented from QComboBox.
    *
    * Returns true if the current completion mode is set
    * to automatic.  See its more comprehensive replacement
    * @ref completionMode.
    *
    * @return true when completion mode is automatic.
    */
    bool autoCompletion() const { return m_iCompletionMode == KGlobalSettings::CompletionAuto; }

    /**
    * Re-implemented from QComboBox.
    *
    * This function now always returns 0.  All the
    * functions needed to manipulate the line edit
    * with the execption of echomode are supplied
    * here.  Methods that affect the funcationality
    * of this widget are not made available.
    *
    * @return always a NULL pointer.
    */
    QLineEdit* lineEdit() const { return 0; }

    /**
    * Enables or disables the popup (context) menu.
    *
    * This method only works if this widget is editable, i.e.
    * read-write and allows you to enable/disable the context
    * menu.  It does nothing if invoked for a none-editable
    * combo-box.  Note that by default the mode changer item
    * is made visiable if the context menu is enabled.  Use
    * @ref hideModechanger() if you want to hide this item.
    *
    * @param showMenu if true, show the context menu.
    * @param showMode if true, show the mode changer.
    */
    virtual void setEnableContextMenu( bool showMenu = true );

    /**
    * Marks the text in the line edit beginning
    * from @p start for @p length characters long.
    *
    * Note that this method is only valid if the
    * combo-box is editable, i.e. read-write mode.
    *
    * @param start  begin selection fron this point.
    * @param length select this many characters.
    */
    virtual void setSelection( int start, int length );

    /**
    * Sets the cursor position.
    *
    * Note that this method is only valid if the
    * combo-box is editable, i.e. read-write mode.
    *
    * @param pos places cursor at this position.
    */
    virtual void setCursorPosition( int pos ) { if( m_pEdit != 0 ) m_pEdit->setCursorPosition( pos ); }

    /**
    * Places the cursor at a new postion and marks
    * the caracters between the indicated positions.
    *
    * Note that this method is only valid if the
    * combo-box is editable, i.e. read-write mode.
    *
    * @param newPos place cursor at this position.
    * @param bMark begin selection at this point.
    * @param eMark end selection at this point.
    */
    virtual void setSelection( int newPos, int bMark, int eMark );

    /**
    * Returns true when the context menu is enabled.
    *
    * @return true if context menu is enabled.
    */
    bool isContextMenuEnabled() const { return m_bEnableMenu; }

signals:

    /**
    * This signal is emitted when the user presses
    * the return key.  It is emitted if and only if
    * the widget is editable (read-write).
    *
    * Note that this widget consumes the RETURN key
    * event.  If you need to process this event be
    * sure to connect to this signal or its cousin
    * signal below.
    */
    void returnPressed();

    /**
    * This signal is emitted when the user presses
    * the return key.  The argument is the current
    * text being edited.  This signal is just like
    * @ref returnPressed() except it contains the
    * current text as its argument.
    *
    * Note that this signal is only emitted if this
    * widget is editable.  Also this widget consumes
    * the RETURN/ENTER key event.  If you need to
    * process this event, be sure to connect to this
    * signal or its cousin signal above.
    */
    void returnPressed( const QString& );

    /**
    * This signal is emitted when the completion key
    * is pressed.  The argument is the current text
    * being edited.
    *
    * Note that this signal is NOT available if this
    * widget is non-editable or the completion mode is
    * set to KGlobalSettings::CompletionNone.
    */
    void completion( const QString& );

    /**
    * This signal is emitted when the rotate up key is pressed.
    *
    * Note that this signal is NOT available if this widget is non-editable
    * or the completion mode is set to KGlobalSettings::CompletionNone.
    */
    void rotateUp();

    /**
    * This signal is emitted when the rotate down key is pressed.
    *
    * Note that this signal is NOT available if this widget is non-editable
    * or the completion mode is set to KGlobalSettings::CompletionNone.
    */
    void rotateDown();


public slots:

    /*
    * Re-implemented from QComboBox for internal reasons.
    *
    * See @ref QComobBox::setEditText.
    */
    virtual void setEditText( const QString& );

    /**
    * Iterates in the up (previous match) direction through the
    * completion list if it is available.
    *
    * This slot is intended to make it easy to connect the rotate
    * up signal in order to make the widget itself handle rotation
    * events internally.  Note that no action is taken if there is
    * no completion object or the completion object does not contain
    * a next match.
    */
    virtual void iterateUpInList();

    /**
    * Iterates in the down (next match) direction through the
    * completion list if it is available.
    *
    * This slot is intended to make it easy to connect the rotate
    * down signal in order to make the widget itself handle rotation
    * events internally.  Note that no action is taken if there is
    * no completion object or the completion object does not contain
    * a next match.
    */
    virtual void iterateDownInList();

    /**
    * Selects the text in the lined edit.  Does nothing
    * if this widget is not editable.
    */
    virtual void selectText()     { if( m_pEdit != 0 ) m_pEdit->selectAll(); }

    /**
    * Un-marks all selected text in the lined edit.  Does
    * nothing if this widget is not editable.
    */
    virtual void deselectText()   { if( m_pEdit != 0 ) m_pEdit->deselect(); }

protected slots:

    /**
    * Accepts the "aboutToShow" signal from the completion
    * sub-menu.
    *
    * This implementation allows this widget to handle
    * the requests for changing the completion mode on
    * the fly by the user. See @ref showCompletionMenu().
    */
    virtual void selectedItem( int itemID );

    /**
    * Populates the sub menu before it is displayed.
    */
    virtual void showCompletionMenu() { insertCompletionItems( this, SLOT( selectedItem( int ) ) ); }

    /**
    * Deals with text changing in the line edit in
    * editable mode.
    */
    virtual void entryChanged( const QString& );

    /**
    * Deals with highlighting the seleted item when return
    * is pressed in the list box (editable-mode only).
    */
    virtual void itemSelected( QListBoxItem* );

    /**
    * Deals with text changes in auto completion mode.
    */
    virtual void makeCompletion( const QString& );

    /**
    * Correctly populates the context menu with the
    * completion entry before it is displayed.
    */
    virtual void aboutToShowMenu();

protected:
    /**
    * Initializes the variables upon construction.
    */
    virtual void init();

    /*
    * Rotates the text on rotation events
    */
    void rotateText( const QString& );

    /**
    * Implementation of @ref KCompletionBase::connectSignals().
    *
    * This function simply connects the signals to appropriate
    * slots when they are handled internally.
    *
    * @param handle if true, handle completion & roation internally.
    */
    virtual void connectSignals( bool handle ) const;

    /**
    * Overridden from QComboBox to provide automatic selection
    * in "select-only" mode.
    */
    virtual void keyPressEvent ( QKeyEvent* );


private :
    // Holds the length of the entry.
    int m_iPrevlen;
    // Holds the current cursor position.
    int m_iPrevpos;

    // Flag that indicates whether we enable/disable
    // the context (popup) menu.
    bool m_bEnableMenu;
    // Flag that indicates whether we show/hide the mode
    // changer item in the context menu.
    bool m_bShowModeChanger;

    // Pointer to the line editor.
    QLineEdit* m_pEdit;
    // Context Menu items.
    QPopupMenu *m_pContextMenu;
    // Event Filter to trap events
    virtual bool eventFilter( QObject* o, QEvent* e );
};

#endif
