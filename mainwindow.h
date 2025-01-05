#pragma once

#include <QMainWindow>
#include "kuz_calc.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    /*!
     * \brief crypt - шифрует или расшифровывает данные
     * \param data - данные
     * \return шифротекст / расшифрованный текст
     */
    QString crypt(QString data);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

