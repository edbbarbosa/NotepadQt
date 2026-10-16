#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QTextEdit>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QCloseEvent>
#include <QFontDialog>
#include <QInputDialog>
#include <QStyle>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QTextEdit *textEdit;
    void closeEvent(QCloseEvent *event) override;
private:
    bool saveToThePath(const QString &path);
    QString path;
private slots:
    void saveFile();
    void saveAsFile();
    void openFile();
    void changeFont();
    void find();
    void replace();
    void wordWrap(bool);
    void about();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
