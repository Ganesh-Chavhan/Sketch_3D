#include "mainwindow.h"
#include "glwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QFrame>

// simple button (no styling)
static QPushButton* makeBtn(const QString& text)
{
    return new QPushButton(text);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Sketch_3D_Application");
    resize(900, 600);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    // ---- LEFT PANEL ----
    QWidget* leftPanel = new QWidget;
    leftPanel->setFixedWidth(200);

    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    QLabel* titleLbl = new QLabel("Sketch_3D");
    titleLbl->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(titleLbl);

    QFrame* sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    leftLayout->addWidget(sep);

    // MODE BUTTONS
    QPushButton* btnSketch = makeBtn("Sketch Mode");
    QPushButton* btnEdit = makeBtn("Edit Mode");
    QPushButton* btn3D = makeBtn("3D Mode");

    leftLayout->addWidget(btnSketch);
    leftLayout->addWidget(btnEdit);
    leftLayout->addWidget(btn3D);

    // ---- SKETCH PANEL ----
    m_sketchPanel = new QWidget;
    QVBoxLayout* sketchLayout = new QVBoxLayout(m_sketchPanel);

    QLabel* shapeLbl = new QLabel("Create Shape");
    sketchLayout->addWidget(shapeLbl);

    QPushButton* btnSquare = makeBtn("Square");
    QPushButton* btnRect = makeBtn("Rectangle");
    QPushButton* btnCircle = makeBtn("Circle");

    sketchLayout->addWidget(btnSquare);
    sketchLayout->addWidget(btnRect);
    sketchLayout->addWidget(btnCircle);

    leftLayout->addWidget(m_sketchPanel);

    // ---- EDIT PANEL ----
    m_editPanel = new QWidget;
    QVBoxLayout* editLayout = new QVBoxLayout(m_editPanel);

    QLabel* editInfo = new QLabel("Drag to resize shape");
    editLayout->addWidget(editInfo);

    leftLayout->addWidget(m_editPanel);
    m_editPanel->hide();

    // ---- 3D PANEL ----
    m_3dPanel = new QWidget;
    QVBoxLayout* d3Layout = new QVBoxLayout(m_3dPanel);

    QLabel* d3Info = new QLabel("Drag to rotate");
    d3Layout->addWidget(d3Info);

    leftLayout->addWidget(m_3dPanel);
    m_3dPanel->hide();

    leftLayout->addStretch();

    m_statusLabel = new QLabel("Select mode");
    leftLayout->addWidget(m_statusLabel);

    // ---- OPENGL WIDGET ----
    m_gl = new GLWidget(central);
    m_gl->setMinimumSize(600, 500);

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(m_gl, 1);

    // ---- CONNECTIONS ----
    connect(btnSketch, &QPushButton::clicked, this, &MainWindow::onSketchMode);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onEditMode);
    connect(btn3D, &QPushButton::clicked, this, &MainWindow::on3DMode);

    connect(btnSquare, &QPushButton::clicked, this, &MainWindow::onCreateSquare);
    connect(btnRect, &QPushButton::clicked, this, &MainWindow::onCreateRectangle);
    connect(btnCircle, &QPushButton::clicked, this, &MainWindow::onCreateCircle);
}


// ---- MODE SWITCH ----
void MainWindow::updateModeUI(int mode)
{
    m_sketchPanel->setVisible(mode == MODE_SKETCH);
    m_editPanel->setVisible(mode == MODE_EDIT);
    m_3dPanel->setVisible(mode == MODE_3D);
}

void MainWindow::onSketchMode()
{
    m_gl->setMode(MODE_SKETCH);
    updateModeUI(MODE_SKETCH);
    m_statusLabel->setText("Sketch Mode");
}

void MainWindow::onEditMode()
{
    m_gl->setMode(MODE_EDIT);
    updateModeUI(MODE_EDIT);
    m_statusLabel->setText("Edit Mode");
}

void MainWindow::on3DMode()
{
    m_gl->setMode(MODE_3D);
    updateModeUI(MODE_3D);
    m_statusLabel->setText("3D Mode");
}


// ---- SHAPE CREATION ----
void MainWindow::onCreateSquare()
{
    bool ok;
    int side = QInputDialog::getInt(this, "Square", "Side:", 100, 20, 500, 10, &ok);
    if (!ok) return;

    m_gl->createShape(SHAPE_SQUARE, side, side, 0);
    m_statusLabel->setText("Square created");
}

void MainWindow::onCreateRectangle()
{
    bool ok;
    int w = QInputDialog::getInt(this, "Rectangle", "Width:", 150, 20, 600, 10, &ok);
    if (!ok) return;

    int h = QInputDialog::getInt(this, "Rectangle", "Height:", 100, 20, 600, 10, &ok);
    if (!ok) return;

    m_gl->createShape(SHAPE_RECTANGLE, w, h, 0);
    m_statusLabel->setText("Rectangle created");
}

void MainWindow::onCreateCircle()
{
    bool ok;
    int r = QInputDialog::getInt(this, "Circle", "Radius:", 80, 10, 300, 5, &ok);
    if (!ok) return;

    m_gl->createShape(SHAPE_CIRCLE, 0, 0, r);
    m_statusLabel->setText("Circle created");
}