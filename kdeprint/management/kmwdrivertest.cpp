#include "kmwdrivertest.h"
#include "kmprinter.h"
#include "kmwizard.h"
#include "driver.h"
#include "kmfactory.h"
#include "kmmanager.h"
#include "kmdriverdialog.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <klocale.h>

KMWDriverTest::KMWDriverTest(QWidget *parent, const char *name)
: KMWizardPage(parent,name)
{
	m_ID = KMWizard::DriverTest;
	m_title = i18n("Printer test");
	m_nextpage = KMWizard::Name;
	m_driver = 0;

	m_manufacturer = new QLabel(this);
	m_model = new QLabel(this);
	m_driverinfo = new QLabel(this);
	m_driverinfo->setTextFormat(Qt::RichText);
	QLabel	*l1 = new QLabel(i18n("<b>Manufacturer:</b>"), this);
	QLabel	*l2 = new QLabel(i18n("<b>Model:</b>"), this);
	QLabel	*l3 = new QLabel(i18n("<b>Description:</b>"), this);

	m_test = new QPushButton(i18n("Test"), this);
	m_settings = new QPushButton(i18n("Settings"), this);

	QLabel	*l0 = new QLabel(this);
	l0->setText(i18n("<p>Now you can test the printer before finishing installation. "
			 "Use the <b>Settings</b> button to configure the printer driver and "
			 "the <b>Test</b> button to test your configuration. Use the <b>Back</b> "
			 "button to change the driver (you current configuration will be discarded).</p>"));

	QVBoxLayout	*lay1 = new QVBoxLayout(this, 0, 30);
	QGridLayout	*lay2 = new QGridLayout(0, 3, 3, 0, 0);
	QHBoxLayout	*lay3 = new QHBoxLayout(0, 0, 10);
	lay1->addWidget(l0,0);
	lay1->addLayout(lay2,0);
	lay1->addLayout(lay3,0);
	lay1->addStretch(1);
	lay2->setColStretch(2,1);
	lay2->addColSpacing(1,10);
	lay2->addWidget(l1,0,0);
	lay2->addWidget(l2,1,0);
	lay2->addWidget(l3,2,0,Qt::AlignLeft|Qt::AlignTop);
	lay2->addWidget(m_manufacturer,0,2);
	lay2->addWidget(m_model,1,2);
	lay2->addWidget(m_driverinfo,2,2);
	lay3->addWidget(m_test,0);
	lay3->addWidget(m_settings,0);
	lay3->addStretch(1);

	connect(m_test,SIGNAL(clicked()),SLOT(slotTest()));
	connect(m_settings,SIGNAL(clicked()),SLOT(slotSettings()));
}

KMWDriverTest::~KMWDriverTest()
{
	delete m_driver;
}

void KMWDriverTest::initPrinter(KMPrinter *p)
{
	m_manufacturer->setText(p->manufacturer());
	m_model->setText(p->model());
	m_driverinfo->setText(p->driverInfo());

	delete m_driver;
	m_driver = 0;

	QString	drfile = p->option("kde-driver");
	if (!drfile.isEmpty() && drfile != "raw")
		m_driver = KMFactory::self()->manager()->loadFileDriver(drfile);
	else if (p->dbEntry() != NULL)
		m_driver = KMFactory::self()->manager()->loadDbDriver(p->dbEntry());

	m_settings->setEnabled((m_driver != 0));
}

void KMWDriverTest::updatePrinter(KMPrinter *p)
{
	// Give the DrMain structure to the driver and don't care about it anymore.
	// It will be destroyed either when giving another structure, or when the
	// printer object will be destroyed.
	p->setDriver(m_driver);
	m_driver = 0;
}

void KMWDriverTest::slotTest()
{
}

void KMWDriverTest::slotSettings()
{
	if (m_driver)
	{
		KMDriverDialog	dlg(this);
		dlg.setDriver(m_driver);
		dlg.showButtonCancel(false);	// only OK button
		dlg.exec();
	}
}
#include "kmwdrivertest.moc"