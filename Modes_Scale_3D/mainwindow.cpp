#include "mainwindow.h"
#include "glwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QFrame>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("Sketch 3D");
    resize(900, 600);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    // LEFT PANEL
    QWidget* leftPanel = new QWidget;
    leftPanel->setFixedWidth(200);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    // Title
    QLabel* title = new QLabel("Sketch 3D");
    title->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(title);

    // Separator
    QFrame* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    leftLayout->addWidget(line);

    // Mode buttons
    QPushButton* btnSketch = new QPushButton("Sketch Mode");
    QPushButton* btnEdit = new QPushButton("Edit Mode");
    QPushButton* btn3D = new QPushButton("3D Mode");
    leftLayout->addWidget(btnSketch);
    leftLayout->addWidget(btnEdit);
    leftLayout->addWidget(btn3D);

    // SKETCH PANEL
    m_sketchPanel = new QWidget;
    QVBoxLayout* sketchLayout = new QVBoxLayout(m_sketchPanel);
    sketchLayout->addWidget(new QLabel("Create Shape:"));

    QPushButton* btnSquare = new QPushButton("Square");
    QPushButton* btnRect = new QPushButton("Rectangle");
    QPushButton* btnCircle = new QPushButton("Circle");
    sketchLayout->addWidget(btnSquare);
    sketchLayout->addWidget(btnRect);
    sketchLayout->addWidget(btnCircle);

    leftLayout->addWidget(m_sketchPanel);

    // EDIT PANEL
    m_editPanel = new QWidget;
    QVBoxLayout* editLayout = new QVBoxLayout(m_editPanel);
    editLayout->addWidget(new QLabel("Drag corners to resize"));
    leftLayout->addWidget(m_editPanel);
    m_editPanel->hide();

    // 3D PANEL
    m_3dPanel = new QWidget;
    QVBoxLayout* d3Layout = new QVBoxLayout(m_3dPanel);
    d3Layout->addWidget(new QLabel("Drag to rotate view"));

    QPushButton* btnExport = new QPushButton("Export STL");
    d3Layout->addWidget(btnExport);

    leftLayout->addWidget(m_3dPanel);
    m_3dPanel->hide();

    // Separator before Load STL
    QFrame* line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    leftLayout->addWidget(line2);

    // Load STL button
    QPushButton* btnLoadSTL = new QPushButton("Load STL");
    leftLayout->addWidget(btnLoadSTL);

    QPushButton* btnClear = new QPushButton("Clear Scrren");
    leftLayout->addWidget(btnClear);

    // Spacer + Status
    leftLayout->addStretch();
    m_statusLabel = new QLabel("Select a mode");
    leftLayout->addWidget(m_statusLabel);

    // OPENGL WIDGET
    m_gl = new GLWidget(central);
    m_gl->setMinimumSize(600, 500);

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(m_gl, 1);

    connect(btnSketch, &QPushButton::clicked, this, &MainWindow::onSketchMode);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onEditMode);
    connect(btn3D, &QPushButton::clicked, this, &MainWindow::on3DMode);

    connect(btnSquare, &QPushButton::clicked, this, &MainWindow::onCreateSquare);
    connect(btnRect, &QPushButton::clicked, this, &MainWindow::onCreateRectangle);
    connect(btnCircle, &QPushButton::clicked, this, &MainWindow::onCreateCircle);

    connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExportSTL);
    connect(btnLoadSTL, &QPushButton::clicked, this, &MainWindow::onLoadSTL);

    connect(btnClear, &QPushButton::clicked, this, &MainWindow::onClear);
}

// Mode Switching
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

// Shape Creation
void MainWindow::onCreateSquare()
{
    m_gl->setPendingShape(SHAPE_SQUARE);
    m_statusLabel->setText("Click and drag to draw a square");
}

void MainWindow::onCreateRectangle()
{
    m_gl->setPendingShape(SHAPE_RECTANGLE);
    m_statusLabel->setText("Click and drag to draw a rectangle");
}

void MainWindow::onCreateCircle()
{
    m_gl->setPendingShape(SHAPE_CIRCLE);
    m_statusLabel->setText("Click and drag to draw a circle");
}

// STL Export
void MainWindow::onExportSTL()
{
    if (m_gl->getMode() != MODE_3D) {
        QMessageBox::warning(this, "Error", "Switch to 3D Mode first!");
        return;
    }

    if (!m_gl->hasShape()) {
        QMessageBox::warning(this, "Error", "Create a shape first!");
        return;
    }

    QString path = QFileDialog::getSaveFileName(this, "Save STL", "model.stl", "STL Files (*.stl)");
    if (path.isEmpty()) return;

    if (!path.endsWith(".stl", Qt::CaseInsensitive)) {
        path += ".stl";
    }

    if (m_gl->exportSTL(path)) {
        m_statusLabel->setText("Exported: " + QFileInfo(path).fileName());
        QMessageBox::information(this, "Success", "STL file saved!");
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to save STL file");
    }
}

// STL Load
void MainWindow::onLoadSTL()
{
    QString path = QFileDialog::getOpenFileName(this, "Open STL", "", "STL Files (*.stl)");
    if (path.isEmpty()) return;

    if (m_gl->loadSTL(path)) {
        // Switch UI to 3D mode
        updateModeUI(MODE_3D);
        m_statusLabel->setText("Loaded: " + QFileInfo(path).fileName());
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to load STL file.\nMake sure it's an ASCII STL file.");
    }
}

void MainWindow::onClear()
{
    m_gl->clearAll();
    m_statusLabel->setText("Screen cleared");
}