#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPointF>
#include <QResizeEvent>
#include <QGroupBox>
#include <QVector>
#include <QRandomGenerator64>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>


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
    bool sortByAngle();

protected slots:
    void onGenerate();
    void onOutline();
    void onExchange();
    void selectLine(int value);

private:
    void init();
    void createBtnGroupBox();
    void createSliderGroupBox();
    void createPointGroupBox();

    void draw();
    void drawPoints();
    void drawLines();
    void drawPoly();
    void generateRandomPoints(int points);
    void createPoly();

    void connectPoints();
    void connects();

    void clearVectors();

    virtual void resizeEvent(QResizeEvent *event);

//Variable
private:
    //UI
    Ui::Widget *ui;

    //Layouts
    QHBoxLayout *mainLayout;
    QVBoxLayout *sideLayout;

    //Groupbox
    QGroupBox *btnGroupBox;
    QGroupBox *sliderGroupBox;
    QGroupBox *pointGroupBox;

    QGraphicsScene *scene;
    QGraphicsView *view;

    //PointBox
    //First Layer
    QLabel *lblPoint;
    QLabel *lblTextAngle;
    QLabel *lblAngle;
    QSpinBox *spinPoint;

    //Second Layer
    QPushButton *btnExchange;
    QSpinBox *spinPoint1;
    QSpinBox *spinPoint2;

    //SliderBox
    QLabel *lblSlider;
    QSlider *vSlider;

    //ButtonBox
    QPushButton *btnGenerate;
    QPushButton *btnOutline;


    //Other
    QVector<QPointF*> points;
    QVector<QLineF*> lines;
    QVector<QGraphicsEllipseItem*> pointsItem;
    QVector<QGraphicsLineItem*> linesItem;
    QPolygonF polygon;

    const int diameter = 4;
};


#endif // WIDGET_H
