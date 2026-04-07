#pragma once
#include <QMainWindow>

class GLWidget;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    // Mode buttons
    void onSketchMode();
    void onEditMode();
    void on3DMode();

    // Shape buttons
    void onCreateSquare();
    void onCreateRectangle();
    void onCreateCircle();

    // Export/Load buttons
    void onExportSTL();
    void onLoadSTL();

    void onClear();

private:
    GLWidget* m_gl;
    QLabel* m_statusLabel;

    // Side panels
    QWidget* m_sketchPanel;
    QWidget* m_editPanel;
    QWidget* m_3dPanel;

    void updateModeUI(int mode);
};
