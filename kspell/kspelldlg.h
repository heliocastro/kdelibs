/* This file is part of the KDE libraries
   Copyright (C) 1997 David Sweet <dsweet@kde.org>

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
#ifndef __KSPELLDLG_H__
#define __KSPELLDLG_H__

//#include "dialog1.h"
#include <qdialog.h>
#include <qpushbutton.h>
#include <qbutton.h>
#include <qlabel.h>
#include <qlayout.h>

#include <klistbox.h>
#include <klineedit.h>
#include <kprogress.h>

//Possible result codes
enum KS_RESULT {
  KS_CANCEL=     0,
  KS_REPLACE=    1,
  KS_REPLACEALL= 2,
  KS_IGNORE=     3,
  KS_IGNOREALL=  4,
  KS_ADD=        5,
  KS_STOP=       7
};

enum Geometries {buttonwidth=80, labelwidth=100};

class QStringList;

class KSpellDlg : public QWidget
{
  Q_OBJECT

  class KSpellDlgPrivate;
  KSpellDlgPrivate *d;
  KLineEdit *editbox;
  KProgress *progbar;
  KListBox *listbox;
  QStringList *sugg;
  QPushButton *qpbrep, *qpbrepa;
  QLabel *wordlabel;
  QList<QWidget> *children;
  QGridLayout *layout;
  QString word, newword;
  bool progressbar;

public:
  KSpellDlg (QWidget *parent, const char *name,
	     bool _progressbar = FALSE, bool _modal = FALSE );

  inline QString replacement ()
    { return newword; }

  /**
   * Change the misspelled word and suggested replacements
   *  and enable the disabled buttons on the dialog box.
   * (Buttons are disabled by @ref standby().)
   **/
  void init (const QString& _word, QStringList *_sugg);

  /**
   * Disable some buttons and gray out the misspelled word.
   **/
  void standby ();


  public slots:
  /**
   * Adjust the progress bar to @p p percent.
   **/
  void slotProgress (unsigned int p);

protected:
	virtual void closeEvent ( QCloseEvent * e );
  void done (int i);

 signals:
    /**
      This signal is emitted when a button is pressed.
      */
  void command (int);

protected slots:
  void ignore();
  void add();
  void ignoreAll();  
  void cancel();
  void replace();
  void replaceAll();
  void stop();
  void help();

  void textChanged (const QString &);

  void selected (int i);
  void highlighted (int i);


};

#endif
