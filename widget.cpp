#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QGraphicsTextItem>
#include <algorithm>
#include <QList>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    init();
    connects();
}

Widget::~Widget()
{
    clearVectors();
    delete ui;
}

void Widget::resizeEvent(QResizeEvent *event)
{
    view->setSceneRect(0,0,view->width(),view->height());
    scene->setSceneRect(view->sceneRect());
    view->fitInView(0,0,view->width(),view->height(), Qt::KeepAspectRatio);
    QWidget::resizeEvent(event);
}

void Widget::createBtnGroupBox()
{
    btnGroupBox = new QGroupBox("Functions");
    btnGenerate = new QPushButton("Generate");
    btnOutline = new QPushButton("Outline");
    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(btnGenerate);
    layout->addWidget(btnOutline);

    btnGroupBox->setLayout(layout);
}

void Widget::createSliderGroupBox()
{
    sliderGroupBox = new QGroupBox("Generate");
    lblSlider = new QLabel("2");
    vSlider = new QSlider(Qt::Horizontal);
    QHBoxLayout *layout = new QHBoxLayout;

    vSlider->setRange(2, 100);
    vSlider->setSingleStep(2);

    layout->addWidget(lblSlider);
    layout->addWidget(vSlider);

    sliderGroupBox->setLayout(layout);
}

void Widget::createPointGroupBox()
{
    QVBoxLayout *pointLayout = new QVBoxLayout;
    QHBoxLayout *firstLayer = new QHBoxLayout;
    QHBoxLayout *secondLayer = new QHBoxLayout;
    pointGroupBox = new QGroupBox("Point");
    lblPoint = new QLabel("Point Nr.");
    lblTextAngle = new QLabel("Angle: ");
    lblAngle = new QLabel("0°");
    spinPoint = new QSpinBox();
    spinPoint1 = new QSpinBox();
    spinPoint2 = new QSpinBox();
    btnExchange = new QPushButton("Exchange");

    spinPoint->setEnabled(false);
    spinPoint1->setEnabled(false);
    spinPoint2->setEnabled(false);
    btnExchange->setEnabled(false);

    firstLayer->addWidget(lblPoint);
    firstLayer->addWidget(spinPoint);
    firstLayer->addWidget(lblTextAngle);
    firstLayer->addWidget(lblAngle);

    secondLayer->addWidget(spinPoint1);
    secondLayer->addWidget(spinPoint2);
    secondLayer->addWidget(btnExchange);

    pointLayout->addLayout(firstLayer);
    pointLayout->addLayout(secondLayer);

    pointGroupBox->setLayout(pointLayout);
}

void Widget::init()
{
    //Layout
    mainLayout = new QHBoxLayout(this);
    sideLayout = new QVBoxLayout();

    //GView
    scene = new QGraphicsScene();
    view = new QGraphicsView(scene);

    //GroupBox
    createBtnGroupBox();
    createSliderGroupBox();
    createPointGroupBox();

    //Properties
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Add Grougbox
    sideLayout->addWidget(btnGroupBox);
    sideLayout->addWidget(sliderGroupBox);
    sideLayout->addWidget(pointGroupBox);
    sideLayout->setAlignment(Qt::AlignTop);

    //Add Main Widgets
    mainLayout->addWidget(view);
    mainLayout->addLayout(sideLayout);

    //Basic Properties
    setWindowTitle("Geometry");
    setLayout(mainLayout);
    resize(sizeHint());
}

void Widget::connects()
{
    connect(btnGenerate, SIGNAL(pressed()), this, SLOT(onGenerate()));
    connect(btnOutline, SIGNAL(pressed()), this, SLOT(onOutline()));
    connect(btnExchange, SIGNAL(pressed()), this, SLOT(onExchange()));
    connect(vSlider, SIGNAL(valueChanged(int)), lblSlider, SLOT(setNum(int)));
    connect(spinPoint, SIGNAL(valueChanged(int)), this, SLOT(selectLine(int)));
}

void Widget::generateRandomPoints(int points)
{
    if(points % 2 != 0)
        points++;

    for(int i = 0; i < points; i++)
    {
        QRandomGenerator64 random = QRandomGenerator64::securelySeeded();

        int x = random.bounded(0, scene->width() - 50);
        int y = random.bounded(0, scene->height() - 50);

        QPointF *point = new QPointF(x,y);
        this->points.push_back(point);
    }
}

void Widget::connectPoints()
{
    if(points.size() != 0)
    {
        for(int x = 0; x < points.size() - 1; x++)
        {
            QLineF *line = new QLineF(*points[x], *points[x+1]);
            lines.push_back(line);
        }

        QLineF *line = new QLineF(*points.last(), *points.first());
        lines.push_back(line);
    }

}

bool Widget::sortByAngle()
{
    //Nähersten Punkt finden (0,0) -> Linie zu jedem Punkt und den Winkel messen
    QPointF *topLeft = points[0];
    for(QPointF *point : points)
    {
        if(point->rx() < topLeft->rx() && point->ry() < topLeft->ry())
            topLeft = point;
    }

    for(int x = 0; x < points.size(); x++)
    {
        if(topLeft != points[x])
        {
            QLineF *line = new QLineF(*topLeft, *points[x]);

            qDebug() << line->angle();

            delete line;
        }
    }
}

void Widget::createPoly()
{
    polygon.clear();
    sortByAngle();
    std::sort(points.begin(), points.end()); //Dummy function

    for(QPointF *point : points)
        polygon << *point;
}

void Widget::draw()
{
    scene->clear();
    drawPoints();
    drawLines();
}

void Widget::drawPoints()
{
    if(!points.empty())
    {
        int i = 0;
        for(QPointF *point : points)
        {
            pointsItem.append(scene->addEllipse(point->rx() - diameter / 2, point->ry() - diameter / 2, diameter, diameter));
            QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(i + 1));
            text->setPos(*point);
            text->setScale(1.5);
            scene->addItem(text);
            i++;
        }
    }
}

void Widget::drawLines()
{
    if(!lines.empty())
        for(QLineF *line : lines)
            linesItem.append(scene->addLine(*line));
}

void Widget::drawPoly()
{
    if(!polygon.empty())
    {
        int i = 0;
        scene->addPolygon(polygon);
        for(QPointF point : polygon)
        {
            QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(i + 1));
            text->setPos(point);
            text->setScale(1.5);
            scene->addItem(text);
            i++;
        }
    }
}

void Widget::clearVectors()
{
    for(auto point : points)
        delete point;
    points.clear();

    for(auto line : lines)
        delete line;
    lines.clear();

    for(auto poly : polygon)
        delete &poly;

    for(auto point : pointsItem)
        delete point;
    pointsItem.clear();

    for(auto line : linesItem)
        delete line;
    linesItem.clear();

    polygon.clear();
}

void Widget::onGenerate()
{
    clearVectors();

    generateRandomPoints(vSlider->value());
    connectPoints();
    draw();

    btnExchange->setEnabled(true);
    spinPoint->setEnabled(true);
    spinPoint1->setEnabled(true);
    spinPoint2->setEnabled(true);

    spinPoint->setMinimum(1);
    spinPoint->setMaximum(lines.length());

    spinPoint1->setMinimum(1);
    spinPoint1->setMinimum(1);
    spinPoint1->setMaximum(points.length());
    spinPoint2->setMaximum(points.length());
}

void Widget::onOutline()
{
    scene->clear();

    createPoly();
    drawPoly();
}

void Widget::onExchange()
{

    QPointF *p1 = points[spinPoint1->value() - 1];
    QPointF *p2 = points[spinPoint2->value() - 1];

    points[spinPoint2->value() - 1] = p1;
    points[spinPoint1->value() - 1] = p2;

    draw();
    //selectLine(spinPoint->value());
}

void Widget::selectLine(int value)
{
    QGraphicsLineItem *lineItem = linesItem[value - 1];

    if(value - 1 > 0)
    {
        QGraphicsLineItem *prev = linesItem[value -  2];
        prev->setPen(QPen(Qt::black));
    }

    QString angle = QString::number(lineItem->line().angle());
    lblAngle->setText(angle + "°");

    QPen pen(Qt::red, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    lineItem->setPen(pen);
}

