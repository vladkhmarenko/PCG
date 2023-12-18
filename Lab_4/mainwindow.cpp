#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),algorithms(this)
{
    ui->setupUi(this);

    InitializaAlgorithms();
}

void MainWindow::InitializaAlgorithms() {
    algorithms.addAction(ui->a_StepByStep);
    algorithms.addAction(ui->a_Bresenham);
    algorithms.addAction(ui->a_DDA);
    algorithms.addAction(ui->a_circle);

    algorithms.setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

    for (auto action: algorithms.actions()) {
        action->setCheckable(true);
    }

    ui->a_StepByStep->setChecked(true);
    algorithm = STEP_BY_STEP;

    connect(ui->comboBox,SIGNAL(currentIndexChanged(int index)),this,SLOT(on_comboBox_currentIndexChanged(int index)));
}




void MainWindow::on_comboBox_currentIndexChanged(int index) {
    ui->doubleSpinBox_4->setVisible(true);
    ui->label_5->setVisible(true);
    ui->label_4->setText("x2");

    if (ui->comboBox->currentIndex()==0)
        algorithm = STEP_BY_STEP;
    else if (ui->comboBox->currentIndex()==2)
        algorithm = BRESENHAM;

    else if (ui->comboBox->currentIndex()==1)
        algorithm = DDA;
    else {
        algorithm = BRESENHAM_CIRCLE;
        ui->doubleSpinBox_4->setVisible(false);
        ui->label_5->setVisible(false);
        ui->label_4->setText("R");
    }

    if (ui->comboBox->currentIndex()==3) {
        algorithm = BRESENHAM_CIRCLE;
        ui->doubleSpinBox_4->setVisible(false);
        ui->label_5->setVisible(false);
        ui->label_4->setText("R");
    }
}



MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::swap(double&a,double&b) {
    double tmp = a;
    a = b;
    b= tmp;
}

void MainWindow::paintEvent(QPaintEvent *) {

    double x0 = ui->doubleSpinBox->value(),x1 = ui->doubleSpinBox_2->value(),
           y0 = ui->doubleSpinBox_3->value(), y1=ui->doubleSpinBox_4->value();

    QPixmap pix(650,650);
    pix.fill(Qt::white);
    QPainter p(&pix);
    p.setBrush(Qt::black);

    int max = qMax(abs(x0),qMax(abs(x1),qMax(abs(y0),abs(y1))));
    int range = 10;

    while (range <= max) {
        range *= 2;
    }


    double step = 0.9 / 20;
    double stp = 0.9 / (2 * range);

    p.drawLine(pix.width()*0.05,pix.height()/2,pix.width()*0.95,pix.height()/2);
    p.drawLine(pix.width()/2,pix.height()*0.95,pix.width()/2,pix.height()*0.05);

    for (int i = 0; i < 20; i++) {
        p.setPen(QPen(Qt::black,1));
        p.drawLine(pix.width()*(0.05+step*i),pix.height()*0.05,pix.width()*(0.05+step*i),pix.height()*0.95);
        p.drawLine(pix.width()*0.05,pix.height()*(0.05+step*i),pix.width()*0.95,pix.height()*(0.05+step*i));
        p.drawText(pix.width()*(0.04+step*i),pix.height()/1.90,QString::number(range/10*(i-10)));
        if (i != 10)
            p.drawText(pix.width()/2.10,pix.height()*(0.955-step*i),QString::number(range/10*(i-10)));
    }

    QVector<QPoint> line;
    QVector<QVector<QPoint>> circle;

    switch (algorithm) {
    case STEP_BY_STEP:
        line = StepByStepLine(x0, y0, x1, y1);
        break;
    case BRESENHAM:
        line = BresenhamLine(x0, y0, x1, y1);
        break;
    case DDA:
        line = DDALine(x0, y0, x1, y1);
        break;
    case BRESENHAM_CIRCLE:
        circle = BresenhamCircle(x0, y0, x1);
        break;
    }

    p.setPen(QPen(Qt::black,3));

    if (algorithm !=BRESENHAM_CIRCLE)
        for (int i = 1; i<line.size(); i++) {
            p.drawLine(pix.width()*(0.5+stp*line[i-1].x()),pix.height()*(0.5-stp*line[i-1].y()),pix.width()*(0.5+stp*line[i].x()),pix.height()*(0.5-stp*line[i].y()));
        }
    else {
        for (int i = 0; i < 4; i++) {
            for (int j = 1; j < circle[i].size(); j++) {
                p.drawLine(pix.width()*(0.5+stp*circle[i][j-1].x()),pix.height()*(0.5-stp*circle[i][j-1].y()),pix.width()*(0.5+stp*circle[i][j].x()),pix.height()*(0.5-stp*circle[i][j].y()));
            }
        }
    }

    ui->label->setPixmap(pix);

}

QVector<QPoint> MainWindow::StepByStepLine(double x0, double y0, double x1, double y1) {
    bool steep = abs(y1-y0)>abs(x1-x0);

    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    double k = (y1 - y0) / (x1 - x0);
    double b = (y0 - k * x0);

    QVector<QPoint> line;

    for (int x = x0; x<= x1;x++) {
        int y = qRound(k * x + b);
        line.push_back(steep ? QPoint(y,x) : QPoint(x,y));
    }
    return line;
}


QVector<QPoint> MainWindow::BresenhamLine(double x0, double y0, double x1, double y1) {
    bool steep = abs(y1-y0)>abs(x1-x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = abs(y1 - y0);
    int error = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;

    QVector<QPoint> line;
    for (int x = x0; x <= x1; x++) {
        line.push_back(steep ? QPoint(y,x) : QPoint(x,y));
        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
    return line;
}


QVector<QPoint> MainWindow::DDALine(double x0, double y0, double x1, double y1) {
    bool steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    double dx = x1 - x0;
    double dy = y1 - y0;
    double y = y0;

    QVector<QPoint> line;
    line.push_back(steep ? QPoint(y0, x0) : QPoint(x0, y0));

    for (int x = x0 + 1; x <= x1; x++) {
        y += dy/dx;
        line.push_back(steep ? QPoint(qRound(y), x) : QPoint(x, qRound(y)));
    }

    return line;
}

QVector<QVector<QPoint>> MainWindow::BresenhamCircle(double x0, double y0, double R) {
    QVector<QVector<QPoint>> points(4);

    int x = R;
    int y = 0;
    int decision = 3 - 2 * R;


    // Octant 1
    points[0].append(QPoint(x0 + x, y0 - y));
    // Octant 2
        points[0].append(QPoint(x0 + y, y0 - x));
        // Octant 3
        points[1].append(QPoint(x0 - y, y0 - x));
        // Octant 4
        points[1].append(QPoint(x0 - x, y0 - y));
        // Octant 5
        points[2].append(QPoint(x0 - x, y0 + y));
        // Octant 6
        points[2].append(QPoint(x0 - y, y0 + x));
        // Octant 7
        points[3].append(QPoint(x0 + y, y0 + x));
        // Octant 8
        points[3].append(QPoint(x0 + x, y0 + y));

        y++;

        if (decision > 0) {
            x--;
            decision = decision + 4 * (y - x) + 10;
        } else {
            decision = decision + 4 * y + 6;
        }


    return points;
}

void MainWindow::on_doubleSpinBox_editingFinished() {
    update();
}

void MainWindow::on_doubleSpinBox_2_editingFinished() {
    update();
}

void MainWindow::on_doubleSpinBox_3_editingFinished() {
    update();
}

void MainWindow::on_doubleSpinBox_4_editingFinished() {
    update();
}
