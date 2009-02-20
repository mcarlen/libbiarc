#include <qgroupbox.h>
#include <qslider.h>
#include <qlabel.h>

class ControlPanel : public QGroupBox {
  Q_OBJECT
  public slots:
    void updateRad(int Val);
    void updateSweep(int Val);
    void updateTS(int Val);
    void updateAng(int Val);
  public:
    QSlider* sliders[5];
    QLabel* slidertext[5];
    ControlPanel(QWidget *parent=0, const char *name=0,int wFlags=0);
    ~ControlPanel();
};
