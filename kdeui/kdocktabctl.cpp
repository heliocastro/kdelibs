/* This file is part of the KDE libraries
   Copyright (C) 2000 Max Judin <novaprint@mtu-net.ru>

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
#include "kdocktabctl.h"

#include <qwidgetstack.h> 
#include <qlayout.h> 
#include <qpushbutton.h>
#include <qpainter.h>
#include <qapp.h>
#include <qwmatrix.h>

struct KDockTabCtl_Private
{
  KDockTabCtl_Private( QWidget* _widget, int _id )
  { widget = _widget; id = _id; enabled = true;  }
  ~KDockTabCtl_Private(){;}

  QWidget* widget;
  int      id;
  bool     enabled;
};

struct KDockTabBar_Private
{
  KDockTabBar_Private( int _id, const QString& _label )
  {
    id = _id;
    width = 0;
    label = _label;
    pix = 0L;
    enabled = true;
    textColor = Qt::black;
  }

  ~KDockTabBar_Private()
  {
    if ( pix != 0L ) delete pix;
  }
  int      id;
  int      width;
  QString  label;
  QPixmap* pix;
  bool     enabled;
  QColor   textColor;
};

static const char* b_left_xpm[] = {
"16 16 7 1",
" 	c None",
".	c None",
"+	c #000000",
"@	c #FFFF99",
"#	c #CC6633",
"$	c #FFCC33",
"%	c #868686",
"................",
"........+.......",
".......++.......",
"......+@+.......",
".....+#@+++++++.",
"....+#$@@@@@@@+.",
"...+#$$$$$$$$@+.",
"..+#$$$$$$$$$@+.",
".%%+#$$$$$$$$@+.",
"..%%+#$$######+.",
"...%%+#$+++++++.",
"....%%+#+.......",
".....%%++.......",
"......%%+.......",
".......%........",
"................"};

KDockTabCtl::KDockTabCtl(QWidget *parent, const char *name)
: QWidget(parent, name )
{
  mainData = new QList<KDockTabCtl_Private>;
  mainData->setAutoDelete( true );

  currentPage = 0L;

  stack = new QWidgetStack( this );
  stack->installEventFilter( this );

  tabs = new KDockTabBar( this );

  tabs->hide();
  connect( tabs, SIGNAL(tabSelected(int)), this, SLOT(setVisiblePage(int)) );
  connect( tabs, SIGNAL(rightButtonPress(int,QPoint)), this, SLOT(rightButtonPress(int,QPoint)) );

  layout = 0L;
  setTabPos( KDockTabBar::TAB_TOP );
}

KDockTabCtl::~KDockTabCtl()
{
  delete mainData;
}

int KDockTabCtl::insertPage( QWidget* widget , const QString &label, int id )
{
  if ( id == -1 ){
    id = -1;
    for ( uint k = 0; k < mainData->count(); k++ )
      if ( mainData->at(k)->id > id ) id = mainData->at(k)->id;
    id++;
  }
  KDockTabCtl_Private* data = new KDockTabCtl_Private( widget, id );
  stack->addWidget( widget, id );
  mainData->append( data );
  tabs->insertTab( label, id );
  if ( !tabs->isVisible() ){
    tabs->show();
    layout->activate();
  }

  // if its page first - this page become current
  if ( mainData->count() == 1 ) setVisiblePage( widget );
  return id;
}


void KDockTabCtl::setPixmap( QWidget* widget, const QPixmap &pix )
{
  KDockTabCtl_Private* data = findData(widget);
  if ( data != 0L ){
    tabs->setPixmap( data->id, pix );
  }
}

QWidget* KDockTabCtl::getFirstPage()
{
  return mainData->first() != 0L ? mainData->first()->widget:0L;
}

QWidget* KDockTabCtl::getLastPage()
{
  return mainData->last() != 0L ? mainData->last()->widget:0L;
}

QWidget* KDockTabCtl::getNextPage( QWidget* widget )
{
  if ( mainData->count() < 2 ) return 0L;

  QWidget* found = 0L;
  for ( uint k = 0; k < mainData->count() - 1; k++ )
    if ( mainData->at(k)->widget == widget ){
      found = mainData->at( k + 1 )->widget;
      break;
    }
  return found;
}

QWidget* KDockTabCtl::getPrevPage( QWidget* widget )
{
  if ( mainData->count() < 2 ) return 0L;

  QWidget* found = 0L;
  for ( uint k = 1; k < mainData->count(); k++ )
    if ( mainData->at(k)->widget == widget ){
      found = mainData->at( k - 1 )->widget;
      break;
    }
  return found;
}

KDockTabCtl_Private* KDockTabCtl::findData( QWidget* widget )
{
  KDockTabCtl_Private* data = 0L;
  for ( uint k = 0; k < mainData->count(); k++ )
    if ( mainData->at(k)->widget == widget ){
      data = mainData->at(k);
      break;
    }
  return data;
}

void KDockTabCtl::showTabIcon( bool show )
{
  tabs->showTabIcon( show );
}

KDockTabCtl_Private* KDockTabCtl::findData( int id )
{
  KDockTabCtl_Private* data = 0L;
  for ( uint k = 0; k < mainData->count(); k++ )
    if ( mainData->at(k)->id == id ){
      data = mainData->at(k);
      break;
    }
  return data;
}

int KDockTabCtl::id( QWidget* widget )
{
  KDockTabCtl_Private* data = findData(widget);
  return data == 0L ? -1:data->id;
}

QWidget* KDockTabCtl::page( int id )
{
  KDockTabCtl_Private* data = findData( id );
  return data == 0L ? 0L:data->widget;
}

void KDockTabCtl::removePage( QWidget* widget )
{
  stack->removeWidget( widget );
  widget->hide();
  KDockTabCtl_Private* data = findData(widget);
  if ( data != 0L ){
    if ( currentPage == widget ) currentPage = 0L;
    tabs->removeTab( data->id );
    mainData->remove( data );
    if ( mainData->count() == 0 ){
      tabs->hide();
    }
  }
}

bool KDockTabCtl::eventFilter( QObject* obj, QEvent* e )
{
  if ( e->type() == QEvent::LayoutHint ){
    // change children min/max size
    stack->updateGeometry();
  }
  return QWidget::eventFilter(obj,e);
}

QWidget* KDockTabCtl::visiblePage()
{
  KDockTabCtl_Private* data = findData( currentPage );
  return  data == 0L ? 0L:data->widget;
}

void KDockTabCtl::setVisiblePage( QWidget* widget, bool allowDisable )
{
  KDockTabCtl_Private* data = findData( widget );
  if ( data != 0L ){
    if ( currentPage != widget ){
      currentPage = widget;
      showPage( widget, allowDisable );
    }
  }
}

void KDockTabCtl::showPage( QWidget* widget, bool allowDisable )
{
  emit aboutToShow( widget );

  stack->raiseWidget( widget );

  KDockTabCtl_Private* data = findData( widget );
  if ( data != 0L ) tabs->setCurrentTab( data->id, allowDisable );

  emit pageSelected( widget );
}

bool KDockTabCtl::isPageEnabled( QWidget* widget )
{
  KDockTabCtl_Private* data = findData( widget );
  return data == 0L ? false:data->enabled;
}

void KDockTabCtl::setPageEnabled( QWidget* widget, bool enabled )
{
  KDockTabCtl_Private* data = findData( widget );
  if ( data != 0L )
    if ( data->enabled != enabled ){
      data->enabled = enabled;
      tabs->setTabEnabled( data->id, enabled );
    }
}

void KDockTabCtl::show()
{
  QWidget::show();
  if ( currentPage == 0L )
    if ( !mainData->isEmpty() )
      currentPage = mainData->at(0)->widget;

  if ( currentPage != 0L ){
    showPage( currentPage );
  }
}

void KDockTabCtl::setTabFont(const QFont & font)
{
  QFont f(font);
  tabs->setFont(f);
}

void KDockTabCtl::paintEvent(QPaintEvent *)
{
  QPainter paint;
  paint.begin( this );

  paint.setPen( colorGroup().light() );
  //  top
  paint.moveTo( 0, 0 );
  paint.lineTo( 0, height() - 1 );

  // left
  paint.moveTo( width() - 1, 0 );
  paint.lineTo( 0, 0 );

  paint.setPen( colorGroup().dark() );
  // right
  paint.moveTo( 0, height() - 1 );
  paint.lineTo( width() - 1, height() - 1 );

  // botton
  paint.moveTo( width() - 1, height() - 1 );
  paint.lineTo( width() - 1, 0 );

  paint.end();
}

void KDockTabCtl::rightButtonPress( int id , QPoint pos )
{
  emit tabShowPopup(id, pos);
}

void KDockTabCtl::setTabTextColor( QWidget* widget, const QColor &color )
{
  KDockTabCtl_Private* data = findData( widget );
  if ( data != 0L ){
    tabs->setTextColor( data->id, color );
  }
}

void KDockTabCtl::setTabPos( KDockTabBar::TabPos pos )
{
  tabPos = pos;
  tabs->setTabPos( pos );

  if ( layout ){
    delete layout;
  }
  switch ( tabPos ){
    case KDockTabBar::TAB_TOP:
      layout = new QVBoxLayout( this );
      break;
    case KDockTabBar::TAB_RIGHT:
      layout = new QHBoxLayout( this );
      break;
  }
  layout->setResizeMode( QLayout::Minimum );
  layout->addWidget( tabs );

  stack_layout = new QVBoxLayout();
  stack_layout->setResizeMode( QLayout::Minimum );
  stack_layout->setMargin(3);
  stack_layout->addWidget( stack, 1 );
  layout->addLayout( stack_layout );
}

bool KDockTabCtl::isShowTabIcon()
{
  return tabs->isShowTabIcon();
}

void KDockTabCtl::setEnabled( bool enabled )
{
  QWidget::setEnabled( enabled );
  tabs->setEnabled( enabled );
}

QFont KDockTabCtl::tabFont()
{
  return tabs->font();
}

void KDockTabCtl::setPageCaption( QWidget* widget, const QString &caption )
{
  KDockTabCtl_Private* data = findData( widget );
  if ( data != 0L )
    tabs->setTabCaption( data->id, caption );
}

QString KDockTabCtl::pageCaption( QWidget* widget )
{
  KDockTabCtl_Private* data = findData( widget );
  if ( data == 0L ) return "";
  return tabs->tabCaption( data->id );
}

const QColor& KDockTabCtl::tabTextColor( QWidget* widget )
{
  KDockTabCtl_Private* data = findData( widget );
  if ( data == 0L ) return Qt::black;
  return tabs->textColor( data->id );
}


KDockTabBarPainter::KDockTabBarPainter( KDockTabBar* parent )
:QWidget( parent )
{
  delta = 0;
  buffer = new QPixmap(0,0);
}

KDockTabBarPainter::~KDockTabBarPainter()
{
  delete buffer;
}

void KDockTabBarPainter::paintEvent( QPaintEvent* )
{
  if ( buffer->isNull() ) return;
  drawBuffer();

  switch ( ((KDockTabBar*)parent())->tabPos ){
    case KDockTabBar::TAB_TOP:
      bitBlt( this, 0, 0, buffer, 0, 0, width(), height() );
      break;
    case KDockTabBar::TAB_RIGHT:{
      QWMatrix m;
      m.rotate( -90 );
      QPixmap xbuffer = buffer->xForm(m);
      bitBlt( this, 0, 0, &xbuffer, 0, 0, width(), height() );
      break;
    }
  }
}

void KDockTabBarPainter::resizeEvent( QResizeEvent* )
{
  delete buffer;
  switch ( ((KDockTabBar*)parent())->tabPos ){
    case KDockTabBar::TAB_TOP:
      buffer = new QPixmap( width(), height() );
      break;
    case KDockTabBar::TAB_RIGHT:
      buffer = new QPixmap( height(), width() );
      break;
  }
}

void KDockTabBarPainter::drawBuffer()
{
  QColor c1 = colorGroup().light();
  QColor c2 = colorGroup().dark();
  QColor c4 = colorGroup().light(); // for paint top line;

  int W = 0;
  int H = 0;
  int shadowX = 1;
  int shadowY = 1;
  switch ( ((KDockTabBar*)parent())->tabPos ){
    case KDockTabBar::TAB_TOP:
      W = width();
      H = height();
      break;
    case KDockTabBar::TAB_RIGHT:
      shadowX = -1;
      c1 = colorGroup().dark();
      c2 = colorGroup().light();
      H = width();
      W = height();
      break;
  }

  QPainter paint;
  paint.begin(buffer);
  paint.setBrushOrigin(0,0);
  paint.fillRect( 0, 0, W, H, QBrush( colorGroup().brush( QColorGroup::Background ) ));

  int x = 2;
  int curTab  = ((KDockTabBar*)parent())->_currentTab;
  int curTabNum = -1;
  int leftTab = ((KDockTabBar*)parent())->leftTab;
  int curx = -1; // start current tab ( selected )
  int curWidth = -1;
  int broken = -1;
  bool iconShow = ((KDockTabBar*)parent())->iconShow;

  QList<KDockTabBar_Private> *mainData = ((KDockTabBar*)parent())->mainData;
  for ( uint k = 0; k < mainData->count(); k++ ){
    int x1 = x;
    int y1 = 2;
    int width = mainData->at(k)->width;

    if ( mainData->at(k)->id == curTab ){  // store current tab start x
      curTabNum = k;
      curx = x;
      curWidth = width;
      x1 -= 1;
      y1 -= 1;
    }

    if ( mainData->at(k)->pix != 0L && iconShow ){
      QWMatrix m;
      switch ( ((KDockTabBar*)parent())->tabPos ){
        case KDockTabBar::TAB_TOP:
          break;
        case KDockTabBar::TAB_RIGHT:
          m.rotate( 90 );
          break;
      }
      paint.drawPixmap( x1+ 11, y1 + 2 , mainData->at(k)->pix->xForm(m) );
    }

    int ty = ( H + fontMetrics().height() ) / 2 - 2;
    int tx = ( mainData->at(k)->pix != 0L && iconShow ) ? 32:12;

    paint.setFont( parentWidget()->font() );

    if ( mainData->at(k)->enabled ){
//      if ( (int)k == curTab )
//        paint.setPen( colorGroup().buttonText() );
//      else
        paint.setPen( mainData->at(k)->textColor );
      paint.drawText( x1 + tx , ty + y1 , mainData->at(k)->label );
    } else {
      paint.setPen( colorGroup().light() );
      paint.drawText( x1 + tx + shadowX, ty + y1 + shadowY, mainData->at(k)->label );
      paint.setPen( colorGroup().mid() );
      paint.drawText( x1 + tx , ty + y1 , mainData->at(k)->label );
    }

    paint.setPen( c1 );
    paint.moveTo( x1, H + 1 -y1 );
    paint.lineTo( x1, y1 );

    paint.setPen( c4 );
    paint.lineTo( x1 + width - 1, y1 );

    paint.setPen( c2 );
    paint.lineTo( x1 + width - 1, H+1-y1 );

/***************************************************************/
    paint.setPen( c1 );
    paint.moveTo( x1 + 4, y1 + H - 5 );
    paint.lineTo( x1 + 4, 3+y1 );

    paint.moveTo( x1 + 7, y1 + H - 5 );
    paint.lineTo( x1 + 7, 3+y1 );

    paint.setPen( c2 );
    paint.moveTo( x1 + 5, y1 + H - 5 );
    paint.lineTo( x1 + 5, 3+y1 );

    paint.moveTo( x1 + 8, y1 + H - 5 );
    paint.lineTo( x1 + 8, 3+y1 );
/***************************************************************/

    // fixed picture for leftTab
    if ( leftTab == (int)k + 1 ){
      paint.fillRect( x1 + width - 2, 0, 2, H - 1, QBrush( colorGroup().brush( QColorGroup::Background ) ));
    }

    // paint broken left
    if ( (int)k == leftTab && k != 0 )
    {
      int yy = y1;
      int xx = x1 - 2;
      paint.fillRect( x1, 0, 1, H - 1, QBrush( colorGroup().brush( QColorGroup::Background ) ));
      paint.setPen( c1 );
      do {
          paint.drawPoint( xx + 2, yy );
          paint.drawPoint( xx + 1, yy + 1 );
          paint.moveTo( xx + 1, yy + 1 );
          paint.lineTo( xx + 1, yy + 3 );
          paint.drawPoint( xx + 2, yy + 4 );
          paint.lineTo( xx + 2, yy + 6 );
          paint.drawPoint( xx + 3, yy + 7 );
          paint.lineTo( xx + 3, yy + 9 );
          paint.drawPoint( xx + 2, yy + 10 );
          paint.drawPoint( xx + 2, yy + 11 );
          yy+= 12;
      } while ( yy < H );
    }

    x += width;
    if ( x >= W && broken == -1 ) broken = k; // store right broken tab
  }

  // modify ( paint ) selected tab
  if ( curx != -1 && curTabNum >= leftTab ){
    curx -= 2;
    curWidth += 4;
    paint.setPen( c1 );
    paint.moveTo( curx, H-1 );
    paint.lineTo( curx, 0 );
    paint.setPen( c4 );
    paint.lineTo( curx + curWidth - 2, 0 );

    paint.setPen( c2 );
    paint.moveTo( curx + curWidth - 1, 0 );
    paint.lineTo( curx + curWidth - 1, H-1 );

    paint.fillRect( curx + 1, 1, 2, H - 1, QBrush( colorGroup().brush( QColorGroup::Background ) ));
    paint.fillRect( curx + curWidth - 4, 1, 3, H - 1, QBrush( colorGroup().brush( QColorGroup::Background ) ));
    paint.fillRect( curx + 1, 1, curWidth - 3, 2, QBrush( colorGroup().brush( QColorGroup::Background ) ));
  }

  if ( curTabNum == leftTab && curTabNum != 0 )
  {
    int yy = 0;
    int xx = curx;
    paint.fillRect( curx, 0, 1, H - 1, QBrush( colorGroup().brush( QColorGroup::Background ) ));
    paint.setPen( c1 );
    do {
        paint.drawPoint( xx + 2, yy );
        paint.drawPoint( xx + 1, yy + 1 );
        paint.moveTo( xx + 1, yy + 1 );
        paint.lineTo( xx + 1, yy + 3 );
        paint.drawPoint( xx + 2, yy + 4 );
        paint.lineTo( xx + 2, yy + 6 );
        paint.drawPoint( xx + 3, yy + 7 );
        paint.lineTo( xx + 3, yy + 9 );
        paint.drawPoint( xx + 2, yy + 10 );
        paint.drawPoint( xx + 2, yy + 11 );
        yy+= 12;
    } while ( yy < H );
  }

  // paint broken right
  if ( broken != -1 )
  {
    int yy = broken == curTabNum ? 0:2;
    int xx = W;
    paint.fillRect( xx - 2, 0, 2, H - 1, QBrush( colorGroup().brush( QColorGroup::Background ) ) );
    paint.fillRect( xx - 5, yy + 1, 5, H - 2 - yy, QBrush( colorGroup().brush( QColorGroup::Background ) ) );
    paint.setPen( c2 );
    do {
        paint.drawPoint( xx - 2, yy );
        paint.drawPoint( xx - 1, yy + 1 );
        paint.moveTo( xx - 1, yy + 1 );
        paint.lineTo( xx - 1, yy + 3 );
        paint.drawPoint( xx - 2, yy + 4 );
        paint.lineTo( xx - 2, yy + 6 );
        paint.drawPoint( xx - 3, yy + 7 );
        paint.lineTo( xx - 3, yy + 9 );
        paint.drawPoint( xx - 2, yy + 10 );
        paint.drawPoint( xx - 2, yy + 11 );
        yy+= 12;
    } while ( yy < H );
  }
  paint.end();
}

int KDockTabBarPainter::findBarByPos( int x, int y )
{
  int dx = 5; // overlaped

  switch ( ((KDockTabBar*)parent())->tabPos ){
    case KDockTabBar::TAB_TOP:
      break;
    case KDockTabBar::TAB_RIGHT:
      x = height() - y;
      break;
  }

  KDockTabBar* bar = (KDockTabBar*)parent();

  QList<KDockTabBar_Private> *mainData = bar->mainData;
  if ( mainData->isEmpty() ) return -1;

  int end = 0;
  int find = -1;
  int findWidth = -1;
  for ( uint k = 0; k < mainData->count(); k++ ){
    end += mainData->at(k)->width;
    if ( x < end ){
      find = k;
      findWidth = mainData->at(k)->width;
      break;
    }
  }

  int idCurTab = bar->_currentTab;
  int curTab = -1;
  for ( uint k = 0; k < mainData->count(); k++ )
    if ( mainData->at(k)->id == idCurTab ){
      curTab = k;
      break;
    }

  // process first Tab manualy
  if ( x < dx && curTab != mainData->at(0)->id ) return -1;

  // process last Tab manyaly
  if ( find == -1 )
    if ( x < (end + dx) && curTab == (int)mainData->count() - 1 )
      find = mainData->count() - 1;

  if ( find == -1 ) return -1;

  // process overlaped
  if ( find > 0 )
    if ( curTab == (find - 1) &&  x < (end - findWidth + dx ) ) find -= 1;

  if ( find < (int)mainData->count() - 1 )
    if ( curTab == (find + 1) &&  x > (end - dx ) ) find += 1;

  return mainData->at(find)->id;
}

void KDockTabBarPainter::mousePressEvent( QMouseEvent* e )
{
  int cur = findBarByPos( e->x(), e->y() );
  if ( e->button() == RightButton )
    emit ((KDockTabBar*)parent())->rightButtonPress( cur, e->globalPos() );
  else
    mousePressTab = cur;
}

void KDockTabBarPainter::mouseReleaseEvent( QMouseEvent* e )
{
  int cur = findBarByPos( e->x(), e->y() );
  if ( cur != -1 && cur == mousePressTab ){
    ((KDockTabBar*)parent())->setCurrentTab( cur );
  }
}

/***************************************************************************/

KDockTabBar::KDockTabBar( QWidget * parent, const char * name )
:QWidget( parent, name )
{
  /* Set up bitmaps */
  left_xpm = new QPixmap( b_left_xpm );

  QWMatrix m;
  m.scale( -1, 1 );
  right_xpm = new QPixmap( left_xpm->xForm(m) );

  m.reset();
  m.rotate( 90 );
  down_xpm = new QPixmap( left_xpm->xForm(m) );

  m.reset();
  m.scale( 1, -1 );
  up_xpm = new QPixmap( down_xpm->xForm(m) );
/****************************************************************/

  tabPos = TAB_TOP;
  iconShow = true;

  barPainter = new KDockTabBarPainter( this );
  move( 0, 0 );

  mainData = new QList<KDockTabBar_Private>;
  mainData->setAutoDelete( true );
  _currentTab = -1;
  leftTab = 0;

  left = new QPushButton(this);// left->hide();
  left->setAutoRepeat( true );
  connect( left, SIGNAL(clicked()), SLOT( leftClicked()) );
  right = new QPushButton(this); right->hide();
  right->setAutoRepeat( true );
  connect( right, SIGNAL(clicked()), SLOT( rightClicked()) );

  setFixedHeight( fontMetrics().height() + 10 );

  setButtonPixmap();
}

KDockTabBar::~KDockTabBar()
{
  delete mainData;
}

void KDockTabBar::paintEvent(QPaintEvent *)
{
  QPainter paint;
  paint.begin( this );

  // find current ( selected ) tab data
  KDockTabBar_Private* data = 0L;
  int curx = 2 - barPainter->delta;
  int curWidth = 0;
  for ( uint k = 0; k < mainData->count(); k++ ){
    data = mainData->at(k);
    if ( data->id == _currentTab ){
      curWidth = data->width + 4 ;
      curx -= 2;
      break;
    }
    curx += data->width;
  }

  if ( curWidth == 0 ) curx = 0; // no tab selected

  // paint button line
  switch ( tabPos ){
    case TAB_TOP:
      paint.fillRect( 0, height()-1, width(), 1, QBrush( colorGroup().brush( QColorGroup::Background ) ));
      paint.setPen( colorGroup().light() );
      paint.moveTo( 0, height()-1 );
      paint.lineTo( curx, height()-1 );
      paint.moveTo( QMIN( curx + curWidth, width() - 50 ), height()-1 );
      paint.lineTo( width() - 1, height()-1 );
      break;
    case TAB_RIGHT:
      paint.fillRect( width() - 1, 0, 1, height(), QBrush( colorGroup().brush( QColorGroup::Background ) ));
      curx = height() - curx;
      paint.setPen( colorGroup().dark() );
      paint.drawPoint( width() - 1, height()-1 );

      paint.moveTo( width() - 1, height()-2 );
      paint.setPen( colorGroup().light() );
      if ( curx != height() ) paint.lineTo( width() - 1, curx );
      paint.moveTo( width() - 1, QMAX( curx - curWidth, 50 ) );
      paint.lineTo( width() - 1, 0 );
      break;
  }
  paint.end();
  barPainter->repaint( false );
}

int KDockTabBar::insertTab( const QString &label, int id )
{
  if ( id == -1 ){
    id = 0;
    for ( uint k = 0; k < mainData->count(); k++ )
      if ( mainData->at(k)->id > id ) id = mainData->at(k)->id;
  }
  KDockTabBar_Private* data = new KDockTabBar_Private( id, label );
  data->textColor = colorGroup().text();

  data->width = 4 + fontMetrics().width( label ) + 14;
  mainData->append( data );

  resizeEvent(0);
  repaint( false );
  return id;
}

void KDockTabBar::showTabIcon( bool show )
{
  if ( iconShow == show ) return;
  iconShow = show;
  updateHeight();
  tabsRecreate();
}

KDockTabBar_Private* KDockTabBar::findData( int id )
{
  KDockTabBar_Private* data = 0L;
  for ( uint k = 0; k < mainData->count(); k++ )
    if ( mainData->at(k)->id == id ){
      data = mainData->at(k);
      break;
    }
  return data;
}

void KDockTabBar::setPixmap( int id, const QPixmap &pix )
{
  KDockTabBar_Private* data = findData( id );
  if ( data != 0L ){
    if ( data->pix != 0L ) delete data->pix;
    data->pix = new QPixmap( pix );
    if ( iconShow ) data->width += 16 + 4;
    tabsRecreate();
  }
}

void KDockTabBar::setTextColor( int id, const QColor &color )
{
  KDockTabBar_Private* data = findData( id );
  if ( data != 0L ){
    data->textColor = color;
    repaint( false );
  }
}

void KDockTabBar::removeTab( int id )
{
  KDockTabBar_Private* data = findData( id );
  if ( data != 0L ){
    if ( _currentTab == data->id )
    {
      for ( uint k = 0; k < mainData->count(); k++ )
      {
        if ( mainData->at(k)->id == data->id ){
          if ( mainData->count() == 1 ){
            setCurrentTab( -1 );
          } else {
            setCurrentTab( mainData->at(k+1)->id );
          }
          break;
        }

        if ( mainData->at(k+1)->id == data->id ){
          setCurrentTab( mainData->at(k)->id );
          break;
        }
      }
    }
    mainData->remove( data );
    leftTab = QMIN( leftTab, (int)mainData->count() - 1 );

    resizeEvent(0);
    repaint( false );
  }
}

void KDockTabBar::setCurrentTab( int id, bool allowDisable )
{
  KDockTabBar_Private* data = findData( id );
  if ( data != 0L )
    if ( (data->enabled || allowDisable ) && _currentTab != data->id )
    {
      _currentTab = data->id;
      repaint( false );

      int curx = 2; // _currentTab start here 
      for ( uint k = 0; k < mainData->count(); k++ ){
        KDockTabBar_Private* data  = mainData->at(k);
        if ( data->id == _currentTab ){
          break;
        }
        curx += data->width;
      }
      // curx : _currentTab start here 
      
      int count;
      switch ( tabPos ){
        case TAB_TOP:
          count = mainData->count();
          while ( count > 0 && -barPainter->delta + curx < 0 ){
            leftClicked();
            count--;
          }
          count = mainData->count();
          while ( count > 0 && -barPainter->delta + curx > width()- 100 ){
            rightClicked();
            count--;
          }
          break;
        case TAB_RIGHT:
          count = mainData->count();
          while ( count > 0 && -barPainter->delta + curx < 0 ){
            leftClicked();
            count--;
          }
          count = mainData->count();
          while ( count > 0 && -barPainter->delta + curx > height() - 100 ){
            rightClicked();
            count--;
          }
          break;
      }
      emit tabSelected( id );
    }
}

void KDockTabBar::setTabEnabled( int id , bool enabled )
{
  KDockTabBar_Private* data = findData( id );
  if ( data == 0L ) return;

  if ( data->enabled != enabled )
  {
    data->enabled = enabled;
    if ( _currentTab == data->id ){
      for ( uint k = 0; k < mainData->count(); k++ ){
        if ( mainData->at(k)->enabled ){
          setCurrentTab( mainData->at(k)->id );
          break;
        }
      }
    }
    if ( enabled ){
      data = findData( _currentTab );
      if ( !data->enabled ) setCurrentTab( id );
    }
    repaint( false );
  }
}

bool KDockTabBar::isTabEnabled( int id )
{
  KDockTabBar_Private* data = findData( id );
  return data == 0L ? false:data->enabled;
}

void KDockTabBar::show()
{
  if ( _currentTab == 0 )
    if ( !mainData->isEmpty() )
      _currentTab = mainData->at(0)->id;

  if ( _currentTab != 0 ){
    setCurrentTab( _currentTab );
  }
  QWidget::show();
}

int KDockTabBar::tabsWidth()
{
  int width = 0;
  for ( uint k = 0; k < mainData->count(); k++ ){
    width += mainData->at(k)->width;
  }
  return width == 0 ? 0:width + 4;
}

void KDockTabBar::resizeEvent(QResizeEvent *)
{
  int maxAllowWidth = 0;
  int maxAllowHeight = 0;

  /* reset bar position to 0 if it allowed or if _currentTab == -1 ( no current tab ) */
  switch ( tabPos ){
    case TAB_TOP:
      if ( width() - 50 > tabsWidth() || _currentTab == -1 ){
        barPainter->delta = 0;
        leftTab = 0;
      }
      maxAllowWidth = width() - 50 + barPainter->delta;
      barPainter->move( -barPainter->delta, 0 );
      barPainter->resize( QMIN(tabsWidth(),maxAllowWidth),  height() - 1 );
      break;
    case TAB_RIGHT:
      if ( height() - 50 > tabsWidth() || _currentTab == -1 ){
        barPainter->delta = 0;
        leftTab = 0;
      }
      maxAllowHeight = height() - 50 + barPainter->delta;
      barPainter->resize( width() - 1, QMIN(tabsWidth(),maxAllowHeight) );
      barPainter->move( 0, height() - barPainter->height() + barPainter->delta );
      break;
  }

  if ( tabPos == TAB_TOP ){
    int wh = height()-4;//18;
    left->setGeometry( width()-2*wh-2, height()-wh-2, wh, wh );
    right->setGeometry( width()-wh, height()-wh-2, wh, wh );

    if ( barPainter->delta > 0 ||  tabsWidth() > maxAllowWidth ){
      left->show();
      right->show();
    } else {
      left->hide();
      right->hide();
    }
  }

  if ( tabPos == TAB_RIGHT ){
    int wh = width()-4;//18;
    left->setGeometry( width()-wh-2, wh+2, wh, wh );
    right->setGeometry( width()-wh-2, 0, wh, wh );

    if ( barPainter->delta > 0 ||  tabsWidth() > maxAllowHeight ){
      left->show();
      right->show();
    } else {
      left->hide();
      right->hide();
    }
  }
}

void KDockTabBar::leftClicked()
{
  if ( leftTab > 0 ){
    leftTab--;
    int dx = mainData->at( leftTab )->width;
    barPainter->delta -= dx;
    barPainter->move( barPainter->x() + dx, barPainter->y() );
    resizeEvent(0);
    repaint( false );
  }
}

void KDockTabBar::rightClicked()
{
  if ( leftTab != (int)mainData->count() - 1 ){
    int dx = mainData->at( leftTab )->width;
    barPainter->delta += dx;
    leftTab++;
    barPainter->move( barPainter->x() - dx, barPainter->y() );
    resizeEvent(0);
    repaint( false );
  }
}

void KDockTabBar::setTabPos( TabPos pos )
{
  tabPos = pos;
  updateHeight();
  setButtonPixmap();
  tabsRecreate();
}

void KDockTabBar::updateHeight()
{
  switch ( tabPos ){
    case TAB_TOP:
      setMaximumWidth(32767);
      if ( iconShow )
        setFixedHeight( fontMetrics().height() + 10 );
      else
        setFixedHeight( fontMetrics().height() + 4 );
      break;
    case TAB_RIGHT:
      setMaximumHeight(32767);
      if ( iconShow )
        setFixedWidth( fontMetrics().height() + 10 );
      else
        setFixedWidth( fontMetrics().height() + 4 );
      break;
  }
}

void KDockTabBar::setButtonPixmap()
{
  switch ( tabPos ){
    case TAB_TOP:
      left->setPixmap( *left_xpm );
      right->setPixmap( *right_xpm );
      break;
    case TAB_RIGHT:
      left->setPixmap( *up_xpm );
      right->setPixmap( *down_xpm );
      break;
  }
}

void KDockTabBar::setFont( const QFont &f )
{
  QWidget::setFont( f );
  tabsRecreate();
}

void KDockTabBar::setTabCaption( int id, const QString &caption )
{
  KDockTabBar_Private* data = findData( id );
  if ( data != 0L ){
    data->label = caption;
    tabsRecreate();
  }
}

QString KDockTabBar::tabCaption( int id )
{
  KDockTabBar_Private* data = findData( id );
  return data == 0L ? QString(""):data->label;
}

void KDockTabBar::tabsRecreate()
{
  for ( uint k = 0; k < mainData->count(); k++ ){
    KDockTabBar_Private* data = mainData->at(k);
    data->width = 4 + fontMetrics().width( data->label ) + 14;
    if ( iconShow && data->pix != 0L ) data->width += 16 + 4;
  }
  resizeEvent(0);
  repaint( false );
}

const QColor& KDockTabBar::textColor( int id )
{
  KDockTabBar_Private* data = findData( id );
  if ( data != 0L ){
    return data->textColor;
  }
  return Qt::black;
}
