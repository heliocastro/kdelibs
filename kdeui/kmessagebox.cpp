/*  This file is part of the KDE libraries
    Copyright (C) 1999 Waldo Bastian (bastian@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; version 2 
    of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
/*
 * $Id$
 *
 * $Log$
 * Revision 1.9  1999/11/11 15:03:41  waba
 * WABA:
 * * Uses KDialogBase for implementation
 * * Some additions according to the KDE Style Guide
 *
 * Revision 1.8  1999/10/09 00:08:29  kalle
 * The dreaded library cleanup: getConfig() -> config() and friends (see separate mail)
 *
 * Revision 1.7  1999/09/23 18:27:45  espensa
 * Changed to the new kapp->makeStdCaption() for creating captions.
 * Result: More maintainable code and libkdeui size was reduced by
 * approx 6000 bytes :-) Note: The about dialog is not changed.
 *
 * Revision 1.6  1999/09/12 13:35:04  espensa
 * I have had problems getting the action button to be properly underlined.
 * The changes should fix this once and for all. There were errors in the
 * "box->setButtonText()" (index errors)
 *
 * One minor typo removed in kmessagebox.h as well.
 *
 * Revision 1.5  1999/08/24 13:16:17  waba
 * WABA: Adding enums, fixing default for warningYesNo
 *
 * Revision 1.4  1999/08/16 15:38:32  waba
 * WABA: Make dialogs fixed size.
 *
 * Revision 1.3  1999/08/15 10:50:30  kulow
 * adding KMessageBox::about which uses the KDE icon instead of the Qt Information
 * icon - very cool! :)
 *
 * Revision 1.2  1999/07/26 07:27:04  kulow
 * it's OK
 *
 * Revision 1.1  1999/07/25 19:38:57  waba
 * WABA: Added some i18n'ed qmessagebox'es for convenience
 *
 *
 */

#include "kmessagebox.h"
#include <qmessagebox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qcheckbox.h>

#include "kapp.h"
#include "kconfig.h"
#include "klocale.h"
#include "kdialogbase.h"

 /** 
  * Easy MessageBox Dialog. 
  *
  * Provides convenience functions for some i18n'ed standard dialogs.
  *
  * @author Waldo Bastian (bastian@kde.org)
  * @version $Id$
  */

int   
KMessageBox::questionYesNo(QWidget *parent, const QString &text,
                           const QString &caption,
                           const QString &buttonYes, 
                           const QString &buttonNo)
{
    KDialogBase dialog(caption.isEmpty() ? i18n("Question") : caption,
                       KDialogBase::Yes | KDialogBase::No,
                       KDialogBase::Yes, KDialogBase::Yes,
                       parent, "questionYesNo", true, true,
                       buttonYes, buttonNo);

    QHBox *contents = new QHBox(&dialog);
    contents->setSpacing(KDialog::spacingHint()*2);
    contents->setMargin(KDialog::marginHint()*2);

    QLabel *label1 = new QLabel( contents);
    label1->setPixmap(QMessageBox::standardIcon(QMessageBox::Information, kapp->style().guiStyle()));

    new QLabel(text, contents);

    dialog.setMainWidget(contents);
    dialog.enableButtonSeparator(false);
     
    int result = dialog.exec();
    
    switch( result )
    {
      case KDialogBase::Yes:
         return Yes;

      case KDialogBase::No:
         return No;
      
      default: // Huh?
         break;
    }

    return Yes; // Default
}

int 
KMessageBox::warningYesNo(QWidget *parent, const QString &text,
                          const QString &caption,
                          const QString &buttonYes,  
                          const QString &buttonNo)
{
    KDialogBase dialog(caption.isEmpty() ? i18n("Warning") : caption,
                       KDialogBase::Yes | KDialogBase::No,
                       KDialogBase::No, KDialogBase::No,
                       parent, "warningYesNo", true, true,
                       buttonYes, buttonNo);

    QHBox *contents = new QHBox(&dialog);
    contents->setSpacing(KDialog::spacingHint()*2);
    contents->setMargin(KDialog::marginHint()*2);

    QLabel *label1 = new QLabel( contents);
    label1->setPixmap(QMessageBox::standardIcon(QMessageBox::Warning, kapp->style().guiStyle()));

    new QLabel(text, contents);

    dialog.setMainWidget(contents);
    dialog.enableButtonSeparator(false);
 
    int result = dialog.exec();

    switch( result )
    {
      case KDialogBase::Yes:
         return Yes;

      case KDialogBase::No:
         return No;
      
      default: // Huh?
         break;
    }

    return No; // Default
}

int 
KMessageBox::warningContinueCancel(QWidget *parent, const QString &text,
                          const QString &caption,
                          const QString &buttonContinue)
{
    KDialogBase dialog(caption.isEmpty() ? i18n("Warning") : caption,
                       KDialogBase::Yes | KDialogBase::No,
                       KDialogBase::Yes, KDialogBase::No,
                       parent, "warningYesNo", true, true,
                       buttonContinue, i18n("&Cancel"));

    QHBox *contents = new QHBox(&dialog);
    contents->setSpacing(KDialog::spacingHint()*2);
    contents->setMargin(KDialog::marginHint()*2);

    QLabel *label1 = new QLabel( contents);
    label1->setPixmap(QMessageBox::standardIcon(QMessageBox::Warning, kapp->style().guiStyle()));

    new QLabel(text, contents);

    dialog.setMainWidget(contents);
    dialog.enableButtonSeparator(false);
 
    int result = dialog.exec();

    switch( result )
    {
      case KDialogBase::Yes:
         return Continue;

      case KDialogBase::No:
         return Cancel;
      
      default: // Huh?
         break;
    }

    return Cancel; // Default
}


int 
KMessageBox::warningYesNoCancel(QWidget *parent, const QString &text,
                                const QString &caption,
                                const QString &buttonYes,  
                                const QString &buttonNo)
{
    KDialogBase dialog(caption.isEmpty() ? i18n("Warning") : caption,
                       KDialogBase::Yes | KDialogBase::No | KDialogBase::Cancel,
                       KDialogBase::Yes, KDialogBase::Cancel,
                       parent, "warningYesNoCancel", true, true,
                       buttonYes, buttonNo);

    QHBox *contents = new QHBox(&dialog);
    contents->setSpacing(KDialog::spacingHint()*2);
    contents->setMargin(KDialog::marginHint()*2);

    QLabel *label1 = new QLabel( contents);
    label1->setPixmap(QMessageBox::standardIcon(QMessageBox::Warning, kapp->style().guiStyle()));

    new QLabel(text, contents);

    dialog.setMainWidget(contents);
    dialog.enableButtonSeparator(false);
 
    int result = dialog.exec();

    switch( result )
    {
      case KDialogBase::Yes:
         return Yes;

      case KDialogBase::No:
         return No;
      
      case KDialogBase::Cancel:
         return Cancel;

      default: // Huh?
         break;
    }

    return Cancel; // Default
}

void
KMessageBox::error(QWidget *parent,  const QString &text,
                   const QString &caption)
{
    KDialogBase dialog(caption.isEmpty() ? i18n("Error") : caption,
                       KDialogBase::Yes,
                       KDialogBase::Yes, KDialogBase::Yes,
                       parent, "error", true, true,
                       i18n("&OK"));

    QHBox *contents = new QHBox(&dialog);
    contents->setSpacing(KDialog::spacingHint()*2);
    contents->setMargin(KDialog::marginHint()*2);

    QLabel *label1 = new QLabel( contents);
    label1->setPixmap(QMessageBox::standardIcon(QMessageBox::Critical, kapp->style().guiStyle()));

    new QLabel(text, contents);

    dialog.setMainWidget(contents);
    dialog.enableButtonSeparator(false);
 
    dialog.exec();
}

void
KMessageBox::sorry(QWidget *parent, const QString &text,
                   const QString &caption)
{
    KDialogBase dialog(caption.isEmpty() ? i18n("Sorry") : caption,
                       KDialogBase::Yes,
                       KDialogBase::Yes, KDialogBase::Yes,
                       parent, "sorry", true, true,
                       i18n("&OK"));

    QHBox *contents = new QHBox(&dialog);
    contents->setSpacing(KDialog::spacingHint()*2);
    contents->setMargin(KDialog::marginHint()*2);

    QLabel *label1 = new QLabel( contents);
    label1->setPixmap(QMessageBox::standardIcon(QMessageBox::Warning, kapp->style().guiStyle()));

    new QLabel(text, contents);

    dialog.setMainWidget(contents);
    dialog.enableButtonSeparator(false);
 
    dialog.exec();
}

void
KMessageBox::information(QWidget *parent,const QString &text,
                   const QString &caption, const QString &dontShowAgainName)
{
    KConfig *config = 0;
    bool showMsg = true;
    QString originalConfigGroup;
    QCheckBox *checkbox = 0;
    
    if (!dontShowAgainName.isEmpty())
    {
       config = kapp->config();
       originalConfigGroup = config->group();
       config->setGroup("Notification Messages");
       showMsg = config->readBoolEntry( dontShowAgainName, true);
       if (!showMsg)
       {
          config->setGroup(originalConfigGroup);
          return;
       }
    }

    KDialogBase dialog(caption.isEmpty() ? i18n("Information") : caption,
                       KDialogBase::Yes,
                       KDialogBase::Yes, KDialogBase::Yes,
                       parent, "information", true, true,
                       i18n("&OK"));

    QVBox *contents = new QVBox(&dialog);
    contents->setSpacing(KDialog::spacingHint()*2);
    contents->setMargin(KDialog::marginHint()*2);

    QHBox *info = new QHBox(contents);
    info->setSpacing(KDialog::spacingHint()*2);
    info->setMargin(0);

    QLabel *label1 = new QLabel( info);
    label1->setPixmap(QMessageBox::standardIcon(QMessageBox::Information, kapp->style().guiStyle()));

    new QLabel(text, info);

    if (!dontShowAgainName.isEmpty())
    {
       checkbox = new QCheckBox(i18n("Do not show this message again"), contents);
    }

    dialog.setMainWidget(contents);
    dialog.enableButtonSeparator(false);
 
    dialog.exec();

    if (!dontShowAgainName.isEmpty())
    {
       showMsg = !checkbox->isChecked(); 
       if (!showMsg)
       {
          config->writeEntry( dontShowAgainName, showMsg);
       }
       config->setGroup(originalConfigGroup);
       config->sync();
    }

    return;
}

void
KMessageBox::enableAllMessages()
{
   KConfig *config = kapp->config();
   QString originalConfigGroup = config->group();
   if (!config->hasGroup("Notification Messages"))
      return;

   config->setGroup("Notification Messages");

   typedef QMap<QString, QString> configMap;
   
   configMap map = config->entryMap("Notification Messages");

   configMap::Iterator it;
   for (it = map.begin(); it != map.end(); ++it)
      config->writeEntry( it.key(), true);
   config->setGroup(originalConfigGroup);
   config->sync();
}

void
KMessageBox::about(QWidget *parent, const QString &text,
                   const QString &caption)
{
    QString _caption = caption;
    if (_caption.isEmpty())
        _caption = i18n("About %1").arg(kapp->caption());

    QMessageBox *box = new QMessageBox( _caption, text,
              QMessageBox::Information,
              QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape, 
              0, 0,
              parent, "about" );

    box->setButtonText(QMessageBox::Ok, i18n("&OK"));
    box->setIconPixmap(kapp->icon());
    box->adjustSize();
    box->setFixedSize(box->size());

    box->exec();
    delete box;
    return;
}

