#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QGraphicsTextItem>
#include <algorithm>

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
    sliderGroupBox = new QGroupBox("Points");
    lblSlider = new QLabel("2");
    vSlider = new QSlider(Qt::Horizontal);
    QHBoxLayout *layout = new QHBoxLayout;

    vSlider->setRange(2, 100);
    vSlider->setSingleStep(2);

    layout->addWidget(lblSlider);
    layout->addWidget(vSlider);

    sliderGroupBox->setLayout(layout);
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

    //Properties
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    sideLayout->addWidget(btnGroupBox);
    sideLayout->addWidget(sliderGroupBox);
    sideLayout->setAlignment(Qt::AlignTop);

    mainLayout->addWidget(view);
    mainLayout->addLayout(sideLayout);

    setWindowTitle("Geometry");
    setLayout(mainLayout);
    resize(sizeHint());
}

void Widget::connects()
{
    connect(btnGenerate, SIGNAL(pressed()), this, SLOT(onGenerate()));
    connect(btnOutline, SIGNAL(pressed()), this, SLOT(onOutline()));
    connect(vSlider, SIGNAL(valueChanged(int)), lblSlider, SLOT(setNum(int)));
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

void Widget::createPoly()
{
    polygon.clear();
    std::sort(points.begin(), points.end()); //Dummy function

    for(QPointF *point : points)
        polygon << *point;
}

void Widget::draw()
{
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
            scene->addEllipse(point->rx() - diameter / 2, point->ry() - diameter / 2, diameter, diameter);
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
            scene->addLine(*line);
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
    polygon.clear();
}

void Widget::onGenerate()
{
    scene->clear();
    clearVectors();

    generateRandomPoints(vSlider->value());
    connectPoints();
    draw();
}

void Widget::onOutline()
{
    scene->clear();

    createPoly();
    drawPoly();
}
