#include "plotzoom.h"
#include "algo_helpers.h"
#include "colors.h"

int Plot::height() {
  return plotwin->height()+bg->height();
}

void Plot::ptplot() {

  //img[Depth].create(w,h,32);
  plots[Depth].img.create(w,h,32);

  Vector3 pt,pt2;
  RGB c;

  float pttable[w][h];
  ptmin = 10000;
  ptmax = -10000;

  float sigma;

  // make width and height dims different
  // i.e. Pts_i and Pts_j
  Vector3 Pts_i[w], Tg_i[w];
  Vector3 Pts_j[h], Tg_j[h];
  for (int j=0;j<h;j++) {
      sigma = plots[Depth].fromy+(float)j/(float)h*
              (plots[Depth].toy-plots[Depth].fromy);

      alen = sigma*length;
      if (alen>length) {
        cout << alen << " - "; alen = length;
        cout << j << " -- " << (float)j/(float)h<<endl;
        
      }
      Pts_j[j] = curve->pointAt(alen);
      Tg_j[j] = curve->tangentAt(alen);
  }

  for (int i=0;i<w;i++) {
      sigma = plots[Depth].fromx+(float)i/(float)w*
             (plots[Depth].tox-plots[Depth].fromx);

      alen = sigma*length;
      if (alen>length) {
        cout << alen << " - "; alen = length;
        cout << i << " -- " << (float)i/(float)w<<endl;
        
      }
      Pts_i[i] = curve->pointAt(alen);
      Tg_i[i] = curve->tangentAt(alen);
  }

  switch(bg->selectedId()) {
    case 0: ptype = PP_PLOT; break;
    case 1: ptype = PT_PLOT; break;
    case 2: ptype = TT_PLOT; break;
    default: cout << "No PLOT TYPE selected\n";
  }

  if (ptype==PT_PLOT) {
    for (int j=0;j<h;j++) {
      for (int i=0;i<w;i++) {

        Vector3 from = Pts_i[i], to = Pts_j[j], tg = Tg_i[i];
        Vector3 D = to - from;
        float Dlen = D.norm();
        float cosa = tg.dot(D)/Dlen;
        float sina = sqrt(1.0-cosa*cosa);
        
        if (sina==0) cur = 0.0;
        else cur = 2.*thickness*sina/Dlen;

        if (cur<0.) cur = 0.;
        else if (cur>1.) cur = 1.;

        ptmin = (cur<ptmin?cur:ptmin);
        ptmax = (cur>ptmax?cur:ptmax);

        pttable[i][j] = cur;
      }
    }
  }
  else if (ptype==TT_PLOT) {
    float z,n,tt2;
    for (int j=0;j<h;j++) {
      for (int i=0;i<w;i++) {

        Vector3 P1 = Pts_i[i], T1 = Tg_i[i];
        Vector3 P2 = Pts_j[j], T2 = Tg_j[j];
        Vector3 E = P1-P2;
        E.normalize();
        Matrix3 R;
        for (int k=0;k<3;k++)
          for (int l=0;l<3;l++)
            R[k][l] = 2.*E[k]*E[l] - (k==l?1.:0.);
        z = T1.cross(T2).dot(E); z=z*z;
        n = (((P1-P2).norm()*(P1-P2).norm()) *
             (1.0-(T2.dot(R*T1))*(T2.dot(R*T1))));
        if (T2.dot(R*T1)*T2.dot(R*T1) >= 1.0 )
          tt2 = 0;
        else
          tt2 = z/n;
        cur = sqrt(tt2)/thickness;

        // FIXME clamp tt plot to [0,1]
        // I didn't think about it, but it fixes the plot ...
        if (cur<0.) cur=0.;
        else if (cur>1.) cur = 1.;

        ptmin = (cur<ptmin?cur:ptmin);
        ptmax = (cur>ptmax?cur:ptmax);

        pttable[i][j] = cur;
      }
    }
  }

  else if (ptype==PP_PLOT) {
    for (int j=0;j<h;j++) {
      // pttable[j][j] = 0.0;
      for (int i=0;i<w;i++) {
        Vector3 from = Pts_i[i], to = Pts_j[j];
        cur = 2.*thickness/(from-to).norm();
        if (cur<0 || (from-to).norm()<1e-7) cur = 0.0;
        if (cur>1.) cur = 1.;

        ptmin = (cur<ptmin?cur:ptmin);
        ptmax = (cur>ptmax?cur:ptmax);

        pttable[i][j] = cur;
      }
    }
  }
  for (int j=0;j<h;j++) {
    for (int i=0;i<w;i++) {
      map_color_sine_end(&c,pttable[i][j],0.,1.);
      // map_bw(&c,pttable[i][j],0.,1.);
      plots[Depth].img.setPixel(i,j,qRgb(c.r,c.g,c.b));
    }
  }
  cout << "Max/Min   : " << ptmax << " / " << ptmin << endl;
  cout << "Thickness : " << thickness << endl;
  plots[Depth].img = plots[Depth].img.scale(SIZE,SIZE);
}

Plot::~Plot() { if (curve) delete curve; curve = NULL; }

void Plot::loadPKF(QString filename) {
  if (curve!=NULL) {
    delete curve; curve = NULL;
  }
  curve = new Curve<Vector3>(filename.ascii());
  if (curve==NULL) { cout << "Could not load Knot " << filename << endl;
    exit(-1);
  }
  Depth = 0;
  FileLoaded = 1;
 // cout << filename << " loaded\n";

  curve->link();
  curve->make_default();
  length = curve->length();
  thickness = curve->thickness()/2.;

  cout << "Curve info : L = " << length << " , D = " << thickness<<endl;

  ptplot();

}

void Plot::openFile() {
  QString newfilename = QFileDialog::getOpenFileName(".",
                                                     "PKF Files (*.pkf)",
                                                     this,
						     "open file dialog",
                                                     "Load file");
  if ( !newfilename.isEmpty()) {
    loadPKF(newfilename);
  }
  update();
}

Plot::Plot(const char* pkfname, int w, int h,
       PLOT_TYPE type, QWidget *parent)
     : QWidget(parent)
  {

  FileLoaded = 0;

  // Size of the Widget (i.e. Image on Screen Size)
  SIZE = 500;

  // Start with adding the menubar
  menubar = new QMenuBar(this);
  menubar->setSeparator(QMenuBar::InWindowsStyle);
  file = new QPopupMenu(menubar);
  menubar->insertItem("&File",file);
  file->insertItem("&Open...",this,SLOT(openFile()),CTRL+Key_O);
  file->insertSeparator();
  file->insertItem("E&xit",qApp,SLOT(quit()),CTRL+Key_Q);

  ptype = type;
  this->w = w; this->h = h; Depth = 0;
  plots[Depth].fromx = 0; plots[Depth].fromy = 0;
  plots[Depth].tox = 1.; plots[Depth].toy = 1.;
  if (!plots[Depth].img.create(w,h,32)) {
    cout << "Could not generate image.\n";
    exit(-1);
  }

  // drawing part;
  QVBoxLayout *l = new QVBoxLayout(this);
  plotwin = new QWidget(this);
  plotwin->setFixedSize(SIZE,SIZE+menubar->heightForWidth(width()));
  l->addWidget(plotwin);

  // Button part
  QHBoxLayout *hl = new QHBoxLayout(l,5);
  bg = new QButtonGroup(3,QGroupBox::Horizontal, this);
  bg->setFixedWidth(400);
  QRadioButton *r1 = new QRadioButton("pp",bg);
  QRadioButton *r2 = new QRadioButton("pt",bg);
  QRadioButton *r3 = new QRadioButton("tt",bg);
  switch(ptype) {
  case PP_PLOT: r1->toggle(); break;
  case PT_PLOT: r2->toggle(); break;
  case TT_PLOT: r3->toggle(); break;
  } 
  hl->addWidget(bg);
  QPushButton *pb = new QPushButton("Zoom-Out",this);
  hl->addWidget(pb);

  // Paint plot of pkfname in plotwin
  curve = NULL;
  if (pkfname!=NULL)
    loadPKF(pkfname);
  else
    plots[Depth].img = plots[Depth].img.scale(SIZE,SIZE);

  resize(plots[Depth].img.width(),
         plots[Depth].img.height()+bg->height());

  connect(bg,SIGNAL(clicked(int)), SLOT(test(int))); // update()));
  connect(pb,SIGNAL(clicked()), SLOT(zoomout())); // update()));

  mbarh = menubar->heightForWidth(width());
}

void Plot::test(int id) {
  switch(id) {
  case 0:
    ptype = PP_PLOT;
  case 1:
    ptype = PT_PLOT;
  case 2:
    ptype = TT_PLOT;
  }
  Depth = 0;
  if (FileLoaded) {
    ptplot();
    update();
  }
}

void Plot::zoomout() {
  if (Depth>0) { Depth --; update(); }
  else cout << "Zoom-Level 0 reached\n";
}

void Plot::paintEvent(QPaintEvent *e) {
  QPainter painter(plotwin);
  painter.setClipRect(e->rect());
  painter.drawImage(0,menubar->heightForWidth(width()),plots[Depth].img);
}

void Plot::keyReleaseEvent ( QKeyEvent *e) {
  switch(e->key()) {
    case Key_Backspace:
      zoomout();
      break;
    case Key_Q:
      exit(-1);
  }
}

void Plot::mousePressEvent ( QMouseEvent *e ) {
if (FileLoaded) {
  clickx = e->x();
  clicky = e->y()-mbarh;
  if (clicky<0) clicky = 0;
  else if (clicky>(SIZE-1)) clicky = SIZE-1;
  if (clickx<0) clickx = 0;
  else if (clickx>(SIZE-1)) clickx = SIZE-1;
  fx = plots[Depth].fromx + (plots[Depth].tox-
       plots[Depth].fromx)*(float)clickx/(SIZE-1.);
  fy = plots[Depth].fromy + (plots[Depth].toy-
       plots[Depth].fromy)*(float)clicky/(SIZE-1.);
}
}

void Plot::mouseReleaseEvent (QMouseEvent *e) {
  if (FileLoaded) {
  int ex = e->x(); int ey = e->y()-mbarh;
  if (ey<0) ey = 0;
  else if (ey>SIZE-1) ey = SIZE-1;
  if (ex<0) ex = 0;
  else if (ex>SIZE-1) ex = SIZE-1;
  tx = plots[Depth].fromx + (plots[Depth].tox-
        plots[Depth].fromx)*(float)ex/(SIZE-1.);
  ty = plots[Depth].fromy + (plots[Depth].toy-
        plots[Depth].fromy)*(float)ey/(SIZE-1.);
  // ok zoom
  if (Depth<9) Depth++;
  else cout << "Max zoom-level reached.\n";

  plots[Depth].tox = tx; plots[Depth].fromx = fx;
  plots[Depth].toy = ty; plots[Depth].fromy = fy;

    ptplot();
    update();
  }
}

void Plot::mouseMoveEvent( QMouseEvent *e) {
  if (FileLoaded) {
    int fx = clickx, fy = clicky;
    int tx = e->x(), ty = e->y()-mbarh;
    if (ty<0) ty = 0;
    else if (ty>SIZE-1) ty = SIZE-1;
    if (tx<0) tx = 0;
    else if (tx>SIZE-1) tx = SIZE-1;

    int tmp;
    if (fx>tx) { tmp = fx; fx = tx; tx = tmp; }
    if (fy>ty) { tmp = fy; fy = ty; ty = tmp; }

    if (tx-fx<5 || ty-fy<5) return;

    QImage pic(plots[Depth].img.copy());
    for (int i=fx;i<=tx;i++) {
      for (int j=fy;j<=ty;j++) {
        QColor col(plots[Depth].img.pixel(i,j));
        pic.setPixel(i,j,col.dark().pixel());
      }
    }
    bitBlt(plotwin,0,mbarh,new QPixmap(pic));
  }
}

void usage(char* prog) {
    cout << "Usage : " << prog << " [option]\n";
    cout << "\n  -file name\tplot for the pkf file name.\n"
         << "  -plot type\ttype is either pp, pt or tt.\n"
         << "  -res N\tN is the resolution of the plot. Default 500\n";
    exit(-1);
}

int main(int argc,char **argv) {

  if (argc>7) {
    usage(argv[0]);
  }

  QApplication a(argc,argv);
  a.setStyle("platinum");
  int Res = 500;
  char *cname = NULL;
  PLOT_TYPE pType = PT_PLOT;

  int arg = 1;
  while (arg<argc) {
    if (argv[arg][0] == '-') {
      if (!strncmp(&argv[arg][1],"file",4)) cname = argv[arg+1];
      else if (!strncmp(&argv[arg][1],"res",3)) Res = atoi(argv[arg+1]);
      else if (!strncmp(&argv[arg][1],"plot",4)) {
        if (!strncmp("pp",argv[arg+1],2))
          pType = PP_PLOT;
        else if (!strncmp("pt",argv[arg+1],2))
          pType = PT_PLOT;
        else if (!strncmp("tt",argv[arg+1],2))
          pType = TT_PLOT;
        else {
          cout << "Unsuported or unknown plot type\n";
          exit(-1);
        }
      }
      arg+=2;
    }
    else usage(argv[0]);
  }

  Plot b(cname,Res,Res,pType);

  a.setMainWidget(&b);
  b.setFixedSize(b.SIZE,b.height());
  b.show();
  b.repaint();

  return a.exec();
}
