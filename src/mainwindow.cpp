#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Text edit
    textEdit = new QTextEdit(this);

    layout->addWidget(textEdit);

    // Create menu bar
    QMenu *fileMenu = menuBar()->addMenu("&File");
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    QMenu *findMenu = menuBar()->addMenu("&Find");
    QMenu *optionsMenu = menuBar()->addMenu("&Options");
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *openAction = fileMenu->addAction("&Open");
    QAction *saveAction = fileMenu->addAction("&Save");
    QAction *saveAsAction = fileMenu->addAction("&Save As");
    QAction *exitAction = fileMenu->addAction("&Exit");

    QAction *undoAction = editMenu->addAction("&Undo");
    QAction *redoAction = editMenu->addAction("&Redo");
    editMenu->addSeparator();
    QAction *cutAction = editMenu->addAction("&Cut");
    QAction *copyAction = editMenu->addAction("&Copy");
    QAction *pasteAction = editMenu->addAction("&Paste");
    QAction *deleteAction = editMenu->addAction("&Delete");
    QAction *selectAction = editMenu->addAction("&Select all");

    QAction *findAction = findMenu->addAction("&Find");
    QAction *replaceAction = findMenu->addAction("&Replace");

    QAction *fontAction = optionsMenu->addAction("&Font");
    QAction *wordWrapAction = optionsMenu->addAction("&Word Wrap");

    QAction *aboutAction = helpMenu->addAction("&About");

    connect(fontAction, &QAction::triggered, this, &MainWindow::changeFont);

    exitAction->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    openAction->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    saveAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);

    undoAction->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, textEdit, &QTextEdit::undo);

    redoAction->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, textEdit, &QTextEdit::redo);

    cutAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, textEdit, &QTextEdit::cut);

    copyAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, textEdit, &QTextEdit::copy);

    pasteAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, textEdit, &QTextEdit::paste);

    deleteAction->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, textEdit, [=]() {
        textEdit->textCursor().removeSelectedText();
    });

    editMenu->addSeparator();

    selectAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAction, &QAction::triggered, textEdit, &QTextEdit::selectAll);

    findAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    findAction->setShortcut(QKeySequence::Find);
    connect(findAction, &QAction::triggered, this, &MainWindow::find);

    replaceAction->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    replaceAction->setShortcut(QKeySequence::Replace);
    connect(replaceAction, &QAction::triggered, this, &MainWindow::replace);

    wordWrapAction->setCheckable(true);
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);

    connect(wordWrapAction, &QAction::triggered, this, &MainWindow::wordWrap);

    aboutAction->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

    resize(800, 500);

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}

void MainWindow::saveFile(){
    if(!path.isEmpty()){
        saveToThePath(path);
    } else {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Text File", "", "Text Files (*.txt);;All Files (*)");
        if(fileName.isEmpty()){
            return;
        }
        saveToThePath(fileName);
    }
}

void MainWindow::saveAsFile(){
    QString fileName = QFileDialog::getSaveFileName(this, "Save Text File As", "", "Text Files (*.txt);;All Files (*)");
    if(fileName.isEmpty()){
        return;
    }
    saveToThePath(fileName);
}

void MainWindow::openFile(){
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open Text File",
                                                    "",
                                                    "Text Files (*.txt);;All Files (*)");

    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(this, "Error", "The file could not be opened for reading.");
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    textEdit->setText(content);
    file.close();

    path = fileName;
    textEdit->document()->setModified(false);
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(textEdit->toPlainText().isEmpty() || !textEdit->document()->isModified()){
        event->accept();
        return;
    }

    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(this,
                                   "Exit",
                                   "Do you want to save the changes before closing?",
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if(answer == QMessageBox::Save){
        saveFile();
        if(textEdit->document()->isModified()){
            event->ignore();
        } else {
            event->accept();
        }
    } else if (answer == QMessageBox::Discard){
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::saveToThePath(const QString &path){
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, "Error", "The file could not be opened for saving.");
        return false;
    }

    QTextStream out(&file);
    out << textEdit->toPlainText();
    file.close();

    this->path = path;
    textEdit->document()->setModified(false);
    return true;
}

void MainWindow::changeFont(){
    bool ok;

    QFont selectedFont = QFontDialog::getFont(&ok, textEdit->currentFont(), this, "Choose Font");

    if(ok)
        textEdit->setFont(selectedFont);
}

void MainWindow::find(){
    bool ok;

    QString term = QInputDialog::getText(this, "Find", "text to be searched", QLineEdit::Normal, "", &ok);

    if(ok && !term.isEmpty()){
        bool find = textEdit->find(term);

        if(!find){
            textEdit->moveCursor(QTextCursor::Start);
            if(!textEdit->find(term)){
                QMessageBox::information(this, "Find", "The specified text was not found.");
            }
        }
    }
}

void MainWindow::replace(){
    bool ok1, ok2;

    QString target = QInputDialog::getText(this, "Replace", "Term to find:", QLineEdit::Normal, "", &ok1);
    if (!ok1 || target.isEmpty()){
        return;
    }

    QString replacement = QInputDialog::getText(this, "Replace", "Replace with:", QLineEdit::Normal, "", &ok2);
    if (!ok2){
        return;
    }

    QTextCursor cursor = textEdit->textCursor();
    if(cursor.hasSelection() && cursor.selectedText() == target){
        cursor.insertText(replacement);
    } else {
        if (textEdit->find(target)){
            textEdit->textCursor().insertText(replacement);
        } else {
            QMessageBox::information(this, "Replace", "The specified text was not found.");
        }
    }
}

void MainWindow::wordWrap(bool checked){
    if (checked){
        textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    } else {
        textEdit->setLineWrapMode(QTextEdit::NoWrap);
    }
}

void MainWindow::about(){
    QMessageBox::about(this,
                       "About NotepadQt",
                       "<h3>NotepadQt 1.0</h3>"
                       "<p>A simple text editor built with C++ and Qt.</p>"
                       "<p>Created as a lightweight notepad alternative.</p>"
                       "<b>GNU General Public License v3.0</b>.</p>");
}
