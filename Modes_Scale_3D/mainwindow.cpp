#include "mainwindow.h"
#include "glwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QGroupBox>
#include <QFrame>
#include <QMessageBox>

// ---- Helper: make a styled button ----
static QPushButton* makeBtn(const QString& text, const QString& color)
{
    auto* btn = new QPushButton(text);
    btn->setFixedHeight(36);
    btn->setStyleSheet(QString(
        "QPushButton {"
        "  background: %1;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "  padding: 0 12px;"
        "}"
        "QPushButton:hover { opacity: 0.85; }"
        "QPushButton:pressed { padding-top: 2px; }"
    ).arg(color));
    return btn;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("3D Shape Explorer");
    resize(980, 680);

    // Dark background
    setStyleSheet("QMainWindow { background: #1a1a26; }");

    // Central widget
    auto* central = new QWidget(this);
    central->setStyleSheet("background: #1a1a26;");
    setCentralWidget(central);

    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // ---- LEFT PANEL (controls) ----
    auto* leftPanel = new QWidget;
    leftPanel->setFixedWidth(210);
    leftPanel->setStyleSheet("background: #22223a; border-radius: 10px;");

    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(12, 16, 12, 16);
    leftLayout->setSpacing(14);

    // App title
    auto* titleLbl = new QLabel("Shape Explorer");
    titleLbl->setStyleSheet("color: #e0e0ff; font-size: 16px; font-weight: bold;");
    titleLbl->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(titleLbl);

    // Separator
    auto* sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #44445a;");
    leftLayout->addWidget(sep);

    // Mode label
    auto* modeLbl = new QLabel("MODE");
    modeLbl->setStyleSheet("color: #8888aa; font-size: 11px; letter-spacing: 2px;");
    leftLayout->addWidget(modeLbl);

    // Mode buttons
    auto* btnSketch = makeBtn("✏  Sketch Mode", "#3a7bd5");
    auto* btnEdit = makeBtn("✦  Edit Mode", "#9b59b6");
    auto* btn3D = makeBtn("◈  3D Mode", "#27ae60");

    leftLayout->addWidget(btnSketch);
    leftLayout->addWidget(btnEdit);
    leftLayout->addWidget(btn3D);

    // Separator
    auto* sep2 = new QFrame;
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("color: #44445a;");
    leftLayout->addWidget(sep2);

    // ---- Sketch panel ----
    m_sketchPanel = new QWidget;
    auto* sketchLayout = new QVBoxLayout(m_sketchPanel);
    sketchLayout->setContentsMargins(0, 0, 0, 0);
    sketchLayout->setSpacing(8);

    auto* shapeLbl = new QLabel("CREATE SHAPE");
    shapeLbl->setStyleSheet("color: #8888aa; font-size: 11px; letter-spacing: 2px;");
    sketchLayout->addWidget(shapeLbl);

    auto* btnSquare = makeBtn("□  Square", "#e67e22");
    auto* btnRect = makeBtn("▭  Rectangle", "#e74c3c");
    auto* btnCircle = makeBtn("○  Circle", "#1abc9c");

    sketchLayout->addWidget(btnSquare);
    sketchLayout->addWidget(btnRect);
    sketchLayout->addWidget(btnCircle);
    leftLayout->addWidget(m_sketchPanel);

    // ---- Edit panel ----
    m_editPanel = new QWidget;
    auto* editLayout = new QVBoxLayout(m_editPanel);
    editLayout->setContentsMargins(0, 0, 0, 0);

    auto* editInfo = new QLabel("Drag a corner to\nresize the shape.");
    editInfo->setStyleSheet("color: #aaaacc; font-size: 12px;");
    editInfo->setWordWrap(true);
    editLayout->addWidget(editInfo);
    leftLayout->addWidget(m_editPanel);
    m_editPanel->hide();

    // ---- 3D panel ----
    m_3dPanel = new QWidget;
    auto* d3Layout = new QVBoxLayout(m_3dPanel);
    d3Layout->setContentsMargins(0, 0, 0, 0);

    auto* d3Info = new QLabel("Drag to rotate\nthe 3D shape.");
    d3Info->setStyleSheet("color: #aaaacc; font-size: 12px;");
    d3Info->setWordWrap(true);
    d3Layout->addWidget(d3Info);
    leftLayout->addWidget(m_3dPanel);
    m_3dPanel->hide();

    leftLayout->addStretch();

    // Status
    m_statusLabel = new QLabel("Select a shape to begin");
    m_statusLabel->setStyleSheet("color: #7777aa; font-size: 11px;");
    m_statusLabel->setWordWrap(true);
    leftLayout->addWidget(m_statusLabel);

    // ---- OpenGL widget ----
    m_gl = new GLWidget(central);
    m_gl->setMinimumSize(600, 500);
    m_gl->setStyleSheet("border-radius: 10px;");

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(m_gl, 1);

    // ---- Connections ----
    connect(btnSketch, &QPushButton::clicked, this, &MainWindow::onSketchMode);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onEditMode);
    connect(btn3D, &QPushButton::clicked, this, &MainWindow::on3DMode);

    connect(btnSquare, &QPushButton::clicked, this, &MainWindow::onCreateSquare);
    connect(btnRect, &QPushButton::clicked, this, &MainWindow::onCreateRectangle);
    connect(btnCircle, &QPushButton::clicked, this, &MainWindow::onCreateCircle);
}

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
    m_statusLabel->setText("Sketch Mode: Create a shape");
}

void MainWindow::onEditMode()
{
    m_gl->setMode(MODE_EDIT);
    updateModeUI(MODE_EDIT);
    m_statusLabel->setText("Edit Mode: Drag a corner to resize");
}

void MainWindow::on3DMode()
{
    m_gl->setMode(MODE_3D);
    updateModeUI(MODE_3D);
    m_statusLabel->setText("3D Mode: Drag to rotate");
}

void MainWindow::onCreateSquare()
{
    bool ok;
    int side = QInputDialog::getInt(this, "Square", "Enter side length (px):",
        100, 20, 500, 10, &ok);
    if (!ok) return;
    m_gl->createShape(SHAPE_SQUARE, side, side, 0);
    m_statusLabel->setText(QString("Square created — side: %1").arg(side));
}

void MainWindow::onCreateRectangle()
{
    bool ok;
    int w = QInputDialog::getInt(this, "Rectangle", "Enter width (px):",
        150, 20, 600, 10, &ok);
    if (!ok) return;
    int h = QInputDialog::getInt(this, "Rectangle", "Enter height (px):",
        100, 20, 600, 10, &ok);
    if (!ok) return;
    m_gl->createShape(SHAPE_RECTANGLE, w, h, 0);
    m_statusLabel->setText(QString("Rectangle: %1 × %2").arg(w).arg(h));
}

void MainWindow::onCreateCircle()
{
    bool ok;
    int r = QInputDialog::getInt(this, "Circle", "Enter radius (px):",
        80, 10, 300, 5, &ok);
    if (!ok) return;
    m_gl->createShape(SHAPE_CIRCLE, 0, 0, r);
    m_statusLabel->setText(QString("Circle created — radius: %1").arg(r));
}