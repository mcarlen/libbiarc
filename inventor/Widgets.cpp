#include "Widgets.h"

Aux2DPlotWindow::Aux2DPlotWindow(QWidget *parent, const char *name, Qt::WindowFlags wFlags)
    : QWidget( parent, wFlags ),
      pop_info( 0 )
{

  std::cout << "init aux2dplot\n" << std::flush;
  pickx = -1; pickx = -1;
  clickx = -1; clicky = -1;
  releasex = -1; releasey = -1;

  // Init Menu Actions
  // File open
  openFileAct = new QAction(tr("&Open..."), this);
  openFileAct->setShortcut(tr("Ctrl+O"));
  openFileAct->setStatusTip(tr("Open file"));
  connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

  // Exit action
  quitAct = new QAction(tr("E&xit"), this);
  quitAct->setShortcut(tr("Ctrl+Q"));
  quitAct->setStatusTip(tr("Quit"));
  connect(quitAct, SIGNAL(triggered()), this, SLOT(quit()));

  // Menubar
  menubar = new QMenuBar(this);
  menubar->addSeparator();

  // File Menu
  file = menubar->addMenu(tr("&File"));
  file->addAction(openFileAct);
  file->addSeparator();
  file->addAction(quitAct);

  // Extras Menu
/*
  extras =  new QPopupMenu( menubar );
  menubar->insertItem( "&Extras", extras );
  e1 = extras->insertItem( "Extra 1" );
  e2 = extras->insertItem( "Extra 2" );
  e3 = extras->insertItem( "Extra 3" );
  extras->insertSeparator();
  oe1 = extras->insertItem( "Other Extra 1" );
  oe2 = extras->insertItem( "Other Extra 2" );
*/

  status = new QLabel(this);
  status->setFrameStyle(QFrame::WinPanel|QFrame::Sunken);
  status->setFixedHeight(fontMetrics().height()+4);

  // Own file dialog
#ifdef WITH_ICON_PROVIDER
  iip = new ImageIconProvider;
#endif

  fd = new QFileDialog(this);
  fd->setViewMode(QFileDialog::Detail);
  fd->setFileMode(QFileDialog::ExistingFile);
  fd->setNameFilter(tr("Image Files (*.png *.jpg *.bmp)"));
  fd->setDirectory("$LIBBIARC");
#ifdef WITH_ICON_PROVIDER
  fd->setIconProvider(iip);
#endif

  setMouseTracking(TRUE);

  std::cout << "done\n" << std::flush;
}

Aux2DPlotWindow::~Aux2DPlotWindow() {
/*
  if ( alloc_context )
    QColor::destroyAllocContext( alloc_context );
*/
}

void Aux2DPlotWindow::updateStatus() {
  if ( image.size() == QSize( 0, 0 ) ) {
    if ( !filename.isEmpty() )
      status->setText("Could not load image");
    else
      status->setText("No image - select Open from File menu.");
  }
  else {
    QString message, moremsg;
    message.sprintf("%dx%d", image.width(), image.height());
/*
    if ( pm.size() != pmScaled.size() ) {
      moremsg.sprintf(" [%dx%d]", pmScaled.width(),
      pmScaled.height());
      message += moremsg;
    }
*/
    // moremsg.sprintf(", %d bits ", image.depth());
    message += moremsg;
    if (image.valid(pickx,picky)) {
      moremsg.sprintf(" (%d,%d) ", //=#%0*x ",
                      pickx, picky);
//                      image.hasAlphaBuffer() ? 8 : 6,
//                      image.pixel(pickx,picky));
      message += moremsg;
    }
/*
    if ( image.numColors() > 0 ) {
      if (image.valid(pickx,picky)) {
        moremsg.sprintf(", %d/%d colors", image.pixelIndex(pickx,picky),                    image.numColors());
      }
      else {
        moremsg.sprintf(", %d colors", image.numColors());
      }
      message += moremsg;
    }
    if ( image.hasAlphaBuffer() ) {
      if ( image.depth() == 8 ) {
        int i;
        bool alpha[256];
        int nalpha=0;
        for (i=0; i<256; i++)
          alpha[i] = FALSE;

        for (i=0; i<image.numColors(); i++) {
          int alevel = image.color(i) >> 24;
          if (!alpha[alevel]) {
            alpha[alevel] = TRUE;
            nalpha++;
          }
        }
        moremsg.sprintf(", %d alpha levels", nalpha);
      }
      else {
        // Too many pixels to bother counting.
        moremsg = ", 8-bit alpha channel";
      }
      message += moremsg;
    }
*/
    status->setText(message);
  }
}

void Aux2DPlotWindow::openFile() {
  QStringList newfilenames;
  if (fd->exec())
    newfilenames = fd->selectedFiles();

  if ( !newfilenames.isEmpty() ) {
    loadImage(newfilenames.at(0)) ;
    repaint();
  }
}

bool Aux2DPlotWindow::loadImage(const QString & fileName) {
  filename = fileName;
  bool ok = FALSE;
  if (!filename.isEmpty()) {
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    ok = image.load(filename, 0);
    pickx = -1; picky = -1;
    clickx = -1; clicky = -1;
    releasex = -1; releasey = -1;

    //if ( ok ) {
    //  ok = reconvertImage();
    //}

    if ( ok ) {
      setWindowTitle( filename );
      int w = image.width();
      int h = image.height();

/*
      const int reasonable_width = 128;
      if ( w < reasonable_width ) {
        int multiply = ( reasonable_width + w - 1 ) / w;
        w *= multiply;
        h *= multiply;
      }
*/
      h += menubar->heightForWidth(w) + status->height();
      resize( w, h );
    } else {
      // image.resize(0,0);                    // couldn't load image
      update();
    }
    QApplication::restoreOverrideCursor();
  }
  updateStatus();
  // setMenuItemFlags();

 /*
  for (int i=100;i<=200;i++)
    for (int j=220;j<=340;j++) {
      QColor c(image.pixel(i,j));
      image.setPixel(i,j,c.dark().pixel());
    }
  repaint();
*/
  return ok;
}

void Aux2DPlotWindow::paintEvent( QPaintEvent *e) {
  if ( image.size() != QSize( 0, 0 ) ) {         // is an image loaded?
    QPainter painter(this);
    painter.setClipRect(e->rect());
    painter.drawImage(0, menubar->heightForWidth( width() ), image);
  }
}

/*
make it fixed size
*/
void Aux2DPlotWindow::resizeEvent( QResizeEvent * ) {
   status->setGeometry(0, height() - status->height(),
                        width(), status->height());

}

void Aux2DPlotWindow::mousePressEvent( QMouseEvent *e) {
  //my_be_other = convertEvent(e,clickx,clicky);
  convertEvent(e,clickx,clicky);
  releasex = releasey = -1;
std::cout << "Clicked : " << clickx << ", "<<clicky<<std::endl;

/*
  if (image.valid(clickx,clicky)) {
    QColor c(image.pixel(clickx,clicky));
    image.setPixel(clickx,clicky,c.dark().pixel());
    update();
  }
*/
}

void Aux2DPlotWindow::mouseReleaseEvent( QMouseEvent *e) {
  //if (may_be_other) other = this;
  convertEvent(e,releasex,releasey);
  popcoords();

  if (image.valid(clickx,clicky) &&image.valid(releasex,releasey)) {

    int sx,sy,tx,ty;
    if (clickx<releasex) {sx=clickx;tx=releasex;}
    else {sx=releasex;tx=clickx;}

    if (clicky<releasey) {sy=clicky;ty=releasey;}
    else {sy=releasey;ty=clicky;}

    for (int i=sx;i<=tx;i++){
      for (int j=sy;j<=ty;j++) {
        QColor c(image.pixel(i,j));
        image.setPixel(i,j,c.dark().rgb());
      }
    }

    //std::cout << "cl : " << clickx << ", "<<clicky<<std::endl
//<<"rel: "<<releasex<<", "<<releasey<<std::endl
 std::cout << " x : " << sx << ", "<<tx
         << std::endl << " y : "<<sy
         << ", "<<ty<<std::endl
<<"dif: "<< (tx-sx) << ", "<<(ty-sy)<<std::endl;
    int ws = tx-sx, hs = ty-sy;
std::cout << ws << " " << hs << std::endl;
 //   this->update(sx,sy,ws,hs);
update();
  }
}

void Aux2DPlotWindow::mouseMoveEvent( QMouseEvent *e) {
  if (convertEvent(e,pickx,picky)) {
    updateStatus();
  /*  
  if (image.valid(pickx,picky)&&image.valid(clickx,clicky)) {
    for (int i=clickx;i<=pickx;i++)
      for (int j=clicky;j<=picky;j++) {
        QColor c(image.pixel(i,j));
        image.setPixel(i,j,c.dark().pixel());
      }
    update();
  }
*/
/*
    if ((e->state()&LeftButton)) {
      may_be_other = FALSE;
      if ( clickx >= 0 && other) {
        copyFrom(other);
      }
    }
*/
  }
}

bool Aux2DPlotWindow::convertEvent(QMouseEvent* e, int& x, int& y) {
  if ( image.size() != QSize( 0, 0 ) ) {
    int h = height() - menubar->heightForWidth( width() ) - status->height();
    int nx = e->x() * image.width() / width();
    int ny = (e->y()-menubar->heightForWidth( width() )) * image.height() /
      h;
    if (nx != x || ny != y ) {
      x = nx;
      y = ny;
      updateStatus();
      return TRUE;
    }
  }
  return FALSE;
}

void Aux2DPlotWindow::setImage(const QImage& newimage) {
  image = newimage;
  pickx = -1; picky = -1;
  clickx = -1; clicky = -1;
  releasex = -1; releasey = -1;
  setWindowTitle( filename );
  int w = image.width();
  int h = image.height();
  if ( !w )
    return;
/*
  const int reasonable_width = 128;
  if ( w < reasonable_width ) {
    // Integer scale up to something reasonable
    int multiply = ( reasonable_width + w - 1 ) / w;
    w *= multiply;
    h *= multiply;
  }
*/
  h += menubar->heightForWidth(w) + status->height();
  resize( w, h );

  // reconvertImage();
//  repaint( image.hasAlphaBuffer() );
  repaint();
  updateStatus();
//  setMenuItemFlags();

}

bool Aux2DPlotWindow::reconvertImage() {
  bool success = FALSE;
  if ( image.isNull() ) return FALSE;

/*
  if ( alloc_context ) {
    QColor::destroyAllocContext( alloc_context );
    alloc_context = 0;
  }
  if ( useColorContext() ) {
    // alloc_context = QColor::enterAllocContext();
    // Clear the image to hide flickering palette
    QPainter painter(this);
    painter.eraseRect(0, menubar->heightForWidth( width() ), width(), height());
  }
*/

  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
  pm = QPixmap::fromImage(image);
  if ( 1 ) //, conversion_flags) )
  {
/*
    pmScaled = QPixmap();
    scale();
    resize( width(), height() );
*/
    success = TRUE;                         // load successful
  } else {
    pm = QPixmap(0,0);                         // couldn't load image
  }
  updateStatus();
  // setMenuItemFlags();
  QApplication::restoreOverrideCursor();      // restore original cursor

/*
  if ( useColorContext() )
    QColor::leaveAllocContext();
*/

  return success;                             // TRUE if loaded OK
}

void Aux2DPlotWindow::popcoords() {
/*
  QString infotext = "Clicked at : \n";
  infotext += QString().setNum(pickx);
  infotext += " ";
  infotext += QString().setNum(picky);

  if (!pop_info) {
    pop_info = new QMessageBox( "Clicked", infotext,
      QMessageBox::Information, QMessageBox::Ok, 0, 0, 0, 0, FALSE );
  }
  else
    pop_info->setText(infotext);
  pop_info->show();
  pop_info->raise();
*/
  if (clickx==releasex && clicky==releasey) {
    std::cout << "(X,Y) = " << clickx << ", " << clicky <<std::endl<<std::flush;
    if (clickx>=0&&clicky>=0)
      emit pos_changed((float)clickx/(float)image.width(),
                       (float)clicky/(float)image.height(),
                       -1.0,-1.0);
  }
  else {
    if (clickx>=0&&clicky>=0&&releasex>=0&&releasey>=0)
      emit pos_changed((float)clickx/(float)image.width(),
                       (float)clicky/(float)image.height(),
                       (float)releasex/(float)image.width(),
                       (float)releasey/(float)image.height());
  }
}

#ifdef WITH_ICON_PROVIDER
/* XPM */
static const char *image_xpm[] = {
"17 15 9 1",
"	c #7F7F7F",
".	c #FFFFFF",
"X	c #00B6FF",
"o	c #BFBFBF",
"O	c #FF6C00",
"+	c #000000",
"@	c #0000FF",
"#	c #6CFF00",
"$	c #FFB691",
"             ..XX",
" ........o   .XXX",
" .OOOOOOOo.  XXX+",
" .O@@@@@@+++XXX++",
" .O@@@@@@O.XXX+++",
" .O@@@@@@OXXX+++.",
" .O######XXX++...",
" .O#####XXX++....",
" .O##$#$XX+o+....",
" .O#$$$$$+.o+....",
" .O##$$##O.o+....",
" .OOOOOOOO.o+....",
" ..........o+....",
" ooooooooooo+....",
"+++++++++++++...."
};

ImageIconProvider::ImageIconProvider( QWidget *parent, const char *name ) :
    QFileIconProvider( ),
    imagepm(image_xpm)
{
    fmts = QImageReader::supportedImageFormats();
}

ImageIconProvider::~ImageIconProvider()
{
}

QIcon ImageIconProvider::icon( const QFileInfo &fi ) const
{
    QByteArray ext = fi.suffix().toUpper().toUtf8();
    if ( fmts.contains(ext) ) {
	return imagepm;
    } else {
	return QFileIconProvider::icon(fi);
    }
}
#endif
