#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QGroupBox>
#include <QVector>
#include <QRandomGenerator64>
#include <QPushButton>
#include <QSlider>
#include <QLabel>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    //Functions
protected slots:
    void onGenerate();
    void onOutline();

private:
    void init();
    void createBtnGroupBox();
    void createSliderGroupBox();

    void draw();
    void drawPoints();
    void drawLines();
    void drawPoly();
    void generateRandomPoints(unsigned int points);
    void createPoly();
    void connectPoints();
    void connects();

    void clearVectors();

    virtual void resizeEvent(QResizeEvent *event);

//Variable
private:
    //UI
    Ui::Widget *ui;
    QHBoxLayout *mainLayout;
    QVBoxLayout *sideLayout;
    QGroupBox *btnGroupBox;
    QGroupBox *sliderGroupBox;

    QGraphicsScene *scene;
    QGraphicsView *view;
    QPushButton *btnGenerate;
    QPushButton *btnOutline;
    QLabel *lblSlider;
    QSlider *vSlider;

    //Other
    QVector<QPointF*> points;
    QVector<QLineF*> lines;
    QPolygonF polygon;

    const int diameter = 4;
};

#endif // WIDGET_H
