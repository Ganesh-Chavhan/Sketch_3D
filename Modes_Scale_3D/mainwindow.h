#pragma once
#include <QMainWindow>

class GLWidget;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onSketchMode();
    void onEditMode();
    void on3DMode();

    void onCreateSquare();
    void onCreateRectangle();
    void onCreateCircle();

private:
    GLWidget* m_gl;
    QLabel* m_statusLabel;

    // Panels
    QWidget* m_sketchPanel;
    QWidget* m_editPanel;
    QWidget* m_3dPanel;

    void updateModeUI(int mode);
};
