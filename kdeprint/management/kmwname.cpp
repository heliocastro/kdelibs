#include "kmwname.h"
#include "kmwizard.h"
#include "kmprinter.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <klocale.h>

KMWName::KMWName(QWidget *parent, const char *name)
: KMWInfoBase(3,parent,name)
{
	m_ID = KMWizard::Name;
	m_title = i18n("General informations");
	m_nextpage = KMWizard::End;

	setInfo(i18n("<p>Enter the informations concerning your printer or class. <b>Name</b> is mandatory, "
		     "<b>Location</b> and <b>Description</b> are not (they may even not be used on some systems).</p>"));
	setLabel(0,i18n("Name:"));
	setLabel(1,i18n("Location:"));
	setLabel(2,i18n("Description:"));
}

bool KMWName::isValid(QString& msg)
{
	if (text(0).isEmpty())
	{
		msg = i18n("You must supply at least a name !");
		return false;
	}
	return true;
}

void KMWName::initPrinter(KMPrinter *p)
{
	setText(0,p->printerName());
	setText(1,p->location());
	setText(2,p->description());
	if (text(2).isEmpty())
		if (p->option("kde-driver") == "raw")
			setText(2,i18n("Raw printer"));
		else
			setText(2,p->manufacturer() + " " + p->model());

	setCurrent(0);
}

void KMWName::updatePrinter(KMPrinter *p)
{
	p->setPrinterName(text(0));
	p->setName(text(0));
	p->setLocation(text(1));
	p->setDescription(text(2));
}