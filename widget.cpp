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
    scene->setSceneRect(view->sceneRect());
    QWidget::resizeEvent(event);

    qDebug() << view->sceneRect() << " " << scene->sceneRect();
}

void Widget::init()
{
    //Layout
    mainLayout = new QGridLayout(this);
    sideLayout = new QHBoxLayout();

    //Buttons
    btnGenerate = new QPushButton("Generate");
    btnOutline = new QPushButton("Outline");

    //Extra
    vSlider = new QSlider(Qt::Horizontal);

    //GView
    scene = new QGraphicsScene();
    view = new QGraphicsView(scene);

    //Properties
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    vSlider->setMinimum(0);
    vSlider->setMaximum(100);
    vSlider->setTickInterval(2);

    sideLayout->addWidget(btnGenerate);
    sideLayout->addWidget(btnOutline);

    mainLayout->addWidget(view,0,0,mainLayout->rowCount(),1);
    mainLayout->addLayout(sideLayout,0,3);
    mainLayout->addWidget(vSlider,1,3);

    scene->setSceneRect(view->rect());

    setLayout(mainLayout);
}

void Widget::connects()
{
    connect(btnGenerate, SIGNAL(pressed()), this, SLOT(onGenerate()));
    connect(btnOutline, SIGNAL(pressed()), this, SLOT(onOutline()));
}

void Widget::generateRandomPoints(unsigned int points)
{
    if(points % 2 != 0)
        points++;

    for(unsigned int i = 0; i < points; i++)
    {
        QRandomGenerator64 random = QRandomGenerator64::securelySeeded();

        int x = random.bounded(0, scene->width());
        int y = random.bounded(0, scene->height());

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
    std::sort(points.begin(),points.end());
    for(QPointF *point : points)
        polygon << *point;
}

void Widget::draw()
{
    if(!points.empty())
    {
        int i = 0;
        for(QPointF *point : points)
        {
            scene->addEllipse(point->rx() - diameter / 2, point->ry() - diameter / 2, diameter, diameter);
            QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(i));
            text->setPos(*point);
            text->setScale(1.5);
            scene->addItem(text);
            i++;
        }
    }

    if(!lines.empty())
        for(QLineF *line : lines)
            scene->addLine(*line);

    if(!polygon.empty())
        scene->addPolygon(polygon);
}

void Widget::clearVectors()
{
    for(auto point : points)
        delete point;
    points.clear();

    for(auto line : lines)
        delete line;
    lines.clear();

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
//    createPoly();
//    draw();
}
