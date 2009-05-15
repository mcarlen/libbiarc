#include "pp.h"

PPPlotWindow::PPPlotWindow(Curve<Vector3>* c, QWidget *parent, const char *name, Qt::WindowFlags wFlags)
    : QWidget( parent, wFlags )
{

  std::cout << "init aux2dplot\n" << std::flush;
  pickx = -1; picky = -1;
  clickx = -1; clicky = -1;
  releasex = -1; releasey = -1;
	PRESSED = false;

  setMouseTracking(TRUE);

  resize(500,500);

  // Make pp plot
	Curve<Vector3> c2 = *c;
	c2.link();
	c2.make_default();
	float *table = new float[500*500];
	float min, max;
	unsigned char* px = new unsigned char[3*500*500];
	unsigned char* px32 = new unsigned char[4*500*500];
	Vector3 pt_i[500], pt_j[500], tg_i[500], tg_j[500];
	samplePtTg(0,1,500,c2,pt_i,tg_i);
	samplePtTg(0,1,500,c2,pt_j,tg_j);
	computePP(500,500,pt_i,pt_j,c2.thickness_fast(),table,&min,&max);
	convertToColoring(500,500,min,max,table,map_color_sine_end,px);
//	sp_png_write_rgb("out.png",px,500,500,0.1,0.1,3*500);
  // convert to RGB24 to XRGB32
	  for (int i=0;i<500*500;++i) {
			px32[i*4+3] = 255;
			px32[i*4+2] = px[i*3+0];
			px32[i*4+1] = px[i*3+1];
			px32[i*4+0] = px[i*3+2];
		}
	image = QImage(px32,500,500,QImage::Format_RGB32);
	orig = image;

  update();
  std::cout << "done\n" << std::flush;
}

PPPlotWindow::~PPPlotWindow() {
/*
  if ( alloc_context )
    QColor::destroyAllocContext( alloc_context );
*/
}

void PPPlotWindow::paintEvent( QPaintEvent *e) {
  if ( image.size() != QSize( 0, 0 ) ) {         // is an image loaded?
    QPainter painter(this);
//    painter.setClipRect(e->rect());
    painter.drawImage(e->rect(),image);
  }
}

/*
make it fixed size
*/
void PPPlotWindow::resizeEvent( QResizeEvent * ) {
	/*
   status->setGeometry(0, height() - status->height(),
                        width(), status->height());
*/
}

void PPPlotWindow::mousePressEvent( QMouseEvent *e) {
  //my_be_other = convertEvent(e,clickx,clicky);
  convertEvent(e,clickx,clicky);
  if (releasex >= 0 && releasey >=0) {
    image = orig;
		repaint();
    releasex = releasey = -1;
	}
	PRESSED = true;
std::cout << "Clicked : " << clickx << ", "<<clicky<<std::endl;

/*
  if (image.valid(clickx,clicky)) {
    QColor c(image.pixel(clickx,clicky));
    image.setPixel(clickx,clicky,c.dark().pixel());
    update();
  }
*/
}

void PPPlotWindow::mouseReleaseEvent( QMouseEvent *e) {
  //if (may_be_other) other = this;
  convertEvent(e,releasex,releasey);
  popcoords();

  if (image.valid(clickx,clicky) &&image.valid(releasex,releasey)) {
		image = orig;

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
  PRESSED = false;
update();
  }
}

void PPPlotWindow::mouseMoveEvent( QMouseEvent *e) {
  if (PRESSED && convertEvent(e,pickx,picky)) {
 //   updateStatus();
 
    if (image.valid(pickx,picky)&&image.valid(clickx,clicky)) {
	  	image = orig;
      for (int i=clickx;i<=pickx;i++)
        for (int j=clicky;j<=picky;j++) {
          QColor c(image.pixel(i,j));
          image.setPixel(i,j,c.dark().rgb());
        }
      repaint();
  	}

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

bool PPPlotWindow::convertEvent(QMouseEvent* e, int& x, int& y) {
  if ( image.size() != QSize( 0, 0 ) ) {
    int h = height();
    int nx = e->x() * image.width() / width();
    int ny = e->y() * image.height() / h;
    if (nx != x || ny != y ) {
      x = nx;
      y = ny;
      return TRUE;
    }
  }
  return FALSE;
}

bool PPPlotWindow::reconvertImage() {
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

#if 0
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
  // setMenuItemFlags();
  QApplication::restoreOverrideCursor();      // restore original cursor

/*
  if ( useColorContext() )
    QColor::leaveAllocContext();
*/
#endif
  return success;                             // TRUE if loaded OK
}

void PPPlotWindow::popcoords() {
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

