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

#include "kio/passdlg.h"

#include <string.h>

#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qaccel.h>

#include <klocale.h>
#include <kapp.h>

using namespace KIO;

PassDlg::PassDlg( QWidget* parent, const char* name, bool modal, WFlags wflags,
			const QString& _head, const QString& _user, const QString& _pass )
   : QDialog(parent, name, modal, wflags)
{
   QVBoxLayout *layout = new QVBoxLayout( this );

   //
   // Bei Bedarf einen kleinen Kommentar als Label einfuegen
   //
   if ( !_head.isNull() )
   {
      QLabel *l = new QLabel(_head, this);
      l->setFixedSize( l->sizeHint() );
      layout->addWidget( l );
      layout->addSpacing( 5 );
   }

   QGridLayout *grid = new QGridLayout( 3, 5 );
   layout->addLayout( grid );

   QLabel *l = new QLabel( i18n( "User" ), this );
   l->setFixedSize( l->sizeHint() );
   grid->addWidget( l, 0, 1 );
   grid->addRowSpacing( 1, 5 );
   grid->addColSpacing( 2, 5 );
   grid->addColSpacing( 0, 5 );
   grid->addColSpacing( 4, 5 );
   m_pUser = new QLineEdit( this );
   m_pUser->setFixedHeight( m_pUser->sizeHint().height() );
   m_pUser->setMinimumWidth( 200 );
   grid->addWidget( m_pUser, 0, 3 );
   l = new QLabel( i18n( "Password" ), this );
   l->setFixedSize( l->sizeHint() );
   grid->addWidget( l, 2, 1 );
   m_pPass = new QLineEdit( this );
   m_pPass->setFixedHeight( m_pPass->sizeHint().height() );
   m_pPass->setEchoMode( QLineEdit::Password );
   m_pPass->setMinimumWidth( 200 );
   grid->addWidget( m_pPass, 2, 3 );

   if ( !_user.isNull() )
     m_pUser->setText( _user );
   if ( !_pass.isNull() )
     m_pPass->setText( _pass );

   layout->addSpacing( 10 );

   //
   // Connect vom LineEdit herstellen und Accelerator
   //
   QAccel *ac = new QAccel(this);
   ac->connectItem( ac->insertItem(Key_Escape), this, SLOT(reject()) );

   connect( m_pPass, SIGNAL(returnPressed()), SLOT(accept()) );

   //
   // Die Buttons "OK" & "Cancel" erzeugen
   //
   QHBoxLayout *hlayout = new QHBoxLayout;
   layout->addLayout( hlayout );

   QPushButton *b1, *b2;
   b1 = new QPushButton( i18n("OK"), this);
   b1->setFixedSize( b1->sizeHint() );
   hlayout->addWidget( b1 );
   b2 = new QPushButton( i18n("Cancel"), this);
   b2->setFixedSize( b2->sizeHint() );
   hlayout->addWidget( b2 );


   // Buttons mit Funktionaliataet belegen
   connect( b1, SIGNAL(clicked()), SLOT(accept()) );
   connect( b2, SIGNAL(clicked()), SLOT(reject()) );

   // Fenstertitel
   setCaption( i18n("Password") );

   // Focus
   if ( _user.isEmpty() )
     m_pUser->setFocus();
   else
     m_pPass->setFocus();

   layout->addStretch( 10 );
   layout->activate();
   resize( sizeHint() );
}

#include "passdlg.moc"
