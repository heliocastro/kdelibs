/* This file is part of the KDE libraries

   Copyright (c) 2000 Dawit Alemayehu <adawit@earthlink.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License (LGPL) as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qobjcoll.h>

#include <klocale.h>
#include <kstdaccel.h>

#include "kcombobox.h"
#include "kcombobox.moc"


KComboBox::KComboBox( QWidget *parent, const char *name ) :QComboBox( parent, name )
{
    m_pEdit = 0;
    m_pContextMenu = 0;
    m_bShowContextMenu = false;
    m_bShowModeChanger = false;
    initialize();
}

KComboBox::KComboBox( bool rw, QWidget *parent, const char *name,
                      bool showContext, bool showModeChanger ) :QComboBox( rw, parent, name )
{
    QObjectList *list = queryList( "QLineEdit" );
    QObjectListIt it ( *list );
    m_pEdit = (QLineEdit*) it.current();
    list = queryList( "QPopupMenu" );
    it = QObjectListIt( *list );
    m_pContextMenu = (QPopupMenu*) it.current();
    connect( m_pEdit, SIGNAL( returnPressed() ), this, SIGNAL( returnPressed() ) );
    connect( m_pEdit, SIGNAL( returnPressed() ), this, SLOT( returnKeyPressed() ) );
    m_pEdit->installEventFilter( this );
    delete list;
    initialize();
    m_bShowModeChanger = showModeChanger;
    if ( rw && showContext )
        showContextMenu();
}

KComboBox::~KComboBox()
{
    if( m_pEdit != 0 )
        m_pEdit->removeEventFilter( this );

    delete m_pEdit;
    delete m_pSubMenu;
    if( m_bAutoDelCompObj )
        delete m_pCompObj;
}

void KComboBox::initialize()
{
    // Permanently set some parameters in the parent object.
    setAutoCompletion( false );

    // Determines whether the completion object should
    // be deleted or not.
    m_bAutoDelCompObj = false;

    // Determine whether items in a select-only mode are
    //auto-selectable
    m_bAutoSelect = false;

    // Initialize all key-bindings to 0 by default so that
    // the event filter will use the global settings.
    m_iCompletionKey = 0;

    // Initalize Variables used in auto-completion mode.
    // These values greatly simplify the logic used to
    // determine whether completion is needed when the
    // widget is in in auto-completion mode.
    m_iPrevpos = 0; // keeps cursor position whenever it changes.
    m_iPrevlen = 0; // keeps length of text as it changes.

    // Initialize the context Menu.  By default the popup
    // menu as well as the mode switching entry are enabled.
    m_pSubMenu = 0;
    m_iSubMenuId = -1;

    // Assign the default completion type to use.
    m_iCompletionMode = KGlobal::completionMode();

    // Initialize the pointer to the completion object.
    m_pCompObj = 0;

    // Connect the signals and slots.
    connect( this, SIGNAL( textChanged( const QString& ) ), this, SLOT( itemChanged( const QString& ) ) );
    connect( this, SIGNAL( completion( const QString& ) ), this, SLOT( makeCompletion( const QString& ) ) );
    connect( listBox(), SIGNAL( clicked( QListBoxItem* ) ), this, SLOT( clickItemEvent( QListBoxItem* ) ) );
    connect( listBox(), SIGNAL( pressed( QListBoxItem* ) ), this, SLOT( pressedItemEvent( QListBoxItem* ) ) );
    connect( listBox(), SIGNAL( returnPressed( QListBoxItem* ) ), this, SLOT( itemSelected( QListBoxItem* ) ) );
}

void KComboBox::setAutoCompletion( bool autocomplete )
{
    m_iCompletionMode = autocomplete ? KGlobal::CompletionAuto : KGlobal::completionMode();
}

void KComboBox::setCompletionObject ( KCompletion* obj, bool autoDelete )
{
    m_pCompObj = obj;
    m_bAutoDelCompObj = autoDelete;
}

void KComboBox::disableCompletion()
{
    delete m_pCompObj;
    m_pCompObj = 0;
}

void KComboBox::enableCompletion( bool autoDelete )
{
    if( m_pCompObj == 0 )
    {
        setCompletionObject( new KCompletion(), autoDelete );
        setCompletionMode( m_iCompletionMode );  // forces a completion mode sync w/ KCompletion.
    }
    m_bAutoDelCompObj = autoDelete;
}

void KComboBox::setCompletionMode( KGlobal::Completion mode )
{
    m_iCompletionMode = mode;
    // Always sync up KCompletion mode with ours as long as we
    // are performing completions.
    if( m_pCompObj != 0 && m_iCompletionMode != KGlobal::CompletionNone )
        m_pCompObj->setCompletionMode( m_iCompletionMode );
}

bool KComboBox::setCompletionKey( int ckey )
{
    if( m_pEdit != 0 && ( ckey >= 0 && ckey != m_iRotateDnKey && ckey != m_iRotateUpKey) )
    {
        m_iCompletionKey = ckey;
        return true;
    }
    return false;
}

bool KComboBox::setRotateUpKey( int rUpKey )
{
    if( rUpKey == 0 || (rUpKey > 0 && rUpKey != m_iRotateDnKey && rUpKey != m_iCompletionKey) )
    {
        m_iRotateUpKey = rUpKey;
        return true;
    }
    return false;
}

bool KComboBox::setRotateDownKey( int rDnKey )
{
    if ( rDnKey == 0 || (rDnKey > 0 && rDnKey != m_iRotateUpKey && rDnKey != m_iCompletionKey) )
    {
        m_iRotateDnKey = rDnKey;
        return true;
    }
    return false;
}

bool KComboBox::showContextMenu()
{
    if( m_pEdit != 0 )
    {
        connect ( m_pContextMenu, SIGNAL( aboutToShow() ), this, SLOT( aboutToShowMenu() ) );
        m_bShowContextMenu = true;
        return m_bShowContextMenu;
    }
    return false;
}

void KComboBox::setEditText( const QString& text )
{
    if( m_pEdit != 0 && !text.isNull() )
    {
        int pos = cursorPosition();
        m_pEdit->validateAndSet( text, pos, 0, text.length() );
    }
    else
        QComboBox::setEditText( text );
}

void KComboBox::autoHighlightItems( bool highlight )
{
    if( highlight )
        connect( listBox(), SIGNAL( onItem( QListBoxItem* ) ), this, SLOT( mouseOverItem( QListBoxItem* ) ) );
    else
        disconnect( listBox(), SIGNAL( onItem( QListBoxItem* ) ), this, SLOT( mouseOverItem( QListBoxItem* ) ) );
}

void KComboBox::hideContextMenu()
{
    if( m_pContextMenu != 0 )
    {
        disconnect ( m_pContextMenu, SIGNAL(aboutToShow()), this, SLOT( aboutToShowMenu()) );
        if( m_iSubMenuId != -1 )
            disconnect ( m_pContextMenu, SIGNAL( highlighted( int ) ), this, SLOT( aboutToShowSubMenu( int ) ) );
        delete m_pSubMenu;
        m_pSubMenu = 0;
        m_bShowContextMenu = false;
    }
}

void KComboBox::aboutToShowMenu()
{
    if( m_bShowModeChanger && m_pCompObj != 0 )
    {
        if( m_pSubMenu == 0 )
            m_pSubMenu = new QPopupMenu();
        // Dummy place holder so that "-->" is shown !!!
        if( m_iSubMenuId == -1 )
        {
            m_pContextMenu->insertSeparator( m_pContextMenu->count() - 1 );
            m_iSubMenuId = m_pContextMenu->insertItem( i18n("Completion Mode"), m_pSubMenu, -1, m_pContextMenu->count() - 2 );
            connect ( m_pContextMenu, SIGNAL( highlighted( int ) ), this, SLOT( aboutToShowSubMenu( int ) ) );
        }
    }
}

void KComboBox::aboutToShowSubMenu( int itemID )
{
    if( itemID == m_iSubMenuId )
    {
        m_pSubMenu->clear();
        int id = m_pSubMenu->insertItem( i18n("None"), this, SLOT(modeNone()),0 , 1 );
        m_pSubMenu->setItemChecked( id, m_iCompletionMode == KGlobal::CompletionNone );
        id = m_pSubMenu->insertItem( i18n("Automatic"), this, SLOT(modeAuto()), 0, 3 );
        m_pSubMenu->setItemChecked( id, m_iCompletionMode == KGlobal::CompletionAuto );
        // The following items are not
        if( m_pEdit != 0 )
        {
            id = m_pSubMenu->insertItem( i18n("Manual"), this, SLOT(modeShell()), 0, 2 );
            m_pSubMenu->setItemChecked( id, m_iCompletionMode == KGlobal::CompletionShell );
            id = m_pSubMenu->insertItem( i18n("Semi-Automatic"), this, SLOT(modeManual()), 0, 4 );
            m_pSubMenu->setItemChecked( id, m_iCompletionMode == KGlobal::CompletionMan );
        }
    }
}

void KComboBox::itemChanged( const QString& text )
{
    if( m_iCompletionMode == KGlobal::CompletionAuto )
    {
        int pos = cursorPosition();
        int len = text.length();
        if( pos > m_iPrevpos && len >= m_iPrevlen )
            emit completion( text );
        m_iPrevpos = pos;
        m_iPrevlen = len;
    }
}

void KComboBox::makeCompletion( const QString& text )
{
    if( m_pEdit !=0 && m_pCompObj != 0 )
    {
        QString match = m_pCompObj->makeCompletion( text );

        // If no match or the same match, simply return
        // without completing.
        if( match.isNull() || match == text )
            return;

        if ( m_iCompletionMode == KGlobal::CompletionShell )
        {
            m_pEdit->setText( match );
            // TODO : deal with partial matches (multiple possible completions)
        }
        else
        {
            m_iPrevlen = match.length();
            m_iPrevpos = cursorPosition();
            m_pEdit->validateAndSet( match, m_iPrevpos, m_iPrevpos, m_iPrevlen );
        }
    }
    else if( m_pEdit ==0 && m_pCompObj != 0 &&
             m_iCompletionMode == KGlobal::CompletionAuto )
    {
       if( text.isNull() )
        return;

       int index = listBox()->index( listBox()->findItem( text ) );
        if( index >= 0 )
            setCurrentItem( index );
    }
}

void KComboBox::multipleCompletions( const QStringList& )
{
    //TODO: POP-UP LISTBOX FOR MULTIPLE MATCHES
}

void KComboBox::returnKeyPressed()
{
    if( m_pEdit && m_pEdit->text().length() != 0 )
        emit returnPressed( m_pEdit->text() );
}

void KComboBox::setSelectedItem( QListBoxItem *item )
{
    if( item != 0 )
    {
        setCurrentItem( listBox()->index( item ) );
        if( m_pEdit != 0 )
            m_pEdit->setSelection(0, m_pEdit->text().length() );
    }
    listBox()->hide();
}

void KComboBox::setSelectedItem( const QString& item  )
{
    if( item.length() != 0 )
    {
        setCurrentItem( listBox()->index( listBox()->findItem( item ) ) );
        if( m_pEdit != 0 )
            m_pEdit->setSelection(0, m_pEdit->text().length() );
    }
    listBox()->hide();
}

void KComboBox::setSelectedItem( int index  )
{
    if( index != -1 )
    {
        setCurrentItem( index );
        if( m_pEdit != 0 )
            m_pEdit->setSelection(0, m_pEdit->text().length() );
    }
    listBox()->hide();
}

void KComboBox::clickItemEvent( QListBoxItem *item )
{
    if( item != 0 )
        emit clicked( listBox()->index( item ) );
}

void KComboBox::pressedItemEvent( QListBoxItem *item )
{
    if( item != 0 )
        emit pressed( listBox()->index( item ) );
}

void KComboBox::mouseOverItem( QListBoxItem* item )
{
    if( item != 0 )
        listBox()->setCurrentItem( item );
}

void KComboBox::itemSelected( QListBoxItem* item )
{
    if( item != 0 && m_pEdit != 0 )
        m_pEdit->setSelection(0, m_pEdit->text().length() );
}

bool KComboBox::eventFilter( QObject *o, QEvent *ev )
{
    if ( o == m_pEdit )
    {
        if( ev->type() == QEvent::KeyPress )
        {
            QKeyEvent *e = (QKeyEvent *) ev;
            // Filter key-events if EchoMode is normal and
            // the completion mode is not CompletionNone.
            if( m_iCompletionMode != KGlobal::CompletionNone )
            {
                // Handles completion.
                int key = ( m_iCompletionKey == 0 ) ? KStdAccel::completion() : m_iCompletionKey;
                if( KStdAccel::isEqual( e, key ) )
                {
                    // Emit completion if the completion mode is NOT
                    // CompletionAuto and if the mode is CompletionShell,
                    // the cursor is at the end of the string.
                    if( m_iCompletionMode == KGlobal::CompletionMan ||
                        (m_iCompletionMode == KGlobal::CompletionShell &&
                         m_pEdit->cursorPosition() == (int) m_pEdit->text().length() ))
                    {
                        emit completion( m_pEdit->text() );
                        return true;
                    }
                }
                // Handles rotateUp.
                key = ( m_iRotateUpKey == 0 ) ? KStdAccel::rotateUp() : m_iRotateUpKey;
                if( KStdAccel::isEqual( e, key ) )
                {
                    emit rotateUp ();
                    return true;
                }
                // Handles rotateDown.
                key = ( m_iRotateDnKey == 0 ) ? KStdAccel::rotateDown() : m_iRotateDnKey;
                if( KStdAccel::isEqual( e, key ) )
                {
                    emit rotateDown();
                    return true;
                }
                // Always update the position holder if the user
                // pressed the END key in auto completion mode.
                if( m_iCompletionMode == KGlobal::CompletionAuto )
                {
                    int pos = cursorPosition();
                    int len = m_pEdit->text().length();
                    if( m_iPrevpos != pos && pos == len )
                    m_iPrevpos = pos;
                }
            }
        }
        else if ( ev->type() == QEvent::MouseButtonPress )
        {
            QMouseEvent *e = (QMouseEvent *) ev;
            if( e->button() == Qt::RightButton )
            {
                    if( m_bShowContextMenu )
                        return false;
                    return true;
            }
        }
    }
    return false;
}

void KComboBox::keyPressEvent ( QKeyEvent * e )
{
    if( m_bAutoSelect && m_pEdit == 0 &&
        m_iCompletionMode == KGlobal::CompletionAuto )
    {
        QString keycode = e->text();
        if ( !keycode.isNull() && keycode.unicode()->isPrint() )
        {
            emit completion ( keycode );
            return;
        }
    }
    QComboBox::keyPressEvent( e );
}
