#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::crypt(QString data)
{
    unsigned char str_arr   [STR_SIZE];
    unsigned char key_arr   [KEY_SIZE];
    unsigned char vect_arr  [VECT_SIZE];

    std::string key_str = ui->keyEdit->text().toStdString();
    std::string vect_str = ui->vectorEdit->text().toStdString()+"0000000000000000";
    std::string data_str = ui->dataEdit->text().toStdString();

    int size = data_str.length();
    uint8_t out_buf[size];

    QString result;

    if(vect_str.length() != VECT_SIZE + 16)
    {
        return "Вектор должен быть 8 байт";
    }
    else if (key_str.length() != 64)
    {
        return "Ключ должен быть 32 байта";
    }
    else if (data_str.length() % 16)
    {
        return "Строка должна быть кратна 16";
    }
    else
    {
        key_str  = reverse_hex(key_str);
        vect_str = reverse_hex(vect_str);

        convert_hex(str_arr,  data_str.c_str(), size);
        convert_hex(key_arr,  key_str.c_str(),  KEY_SIZE);
        convert_hex(vect_arr, vect_str.c_str(), VECT_SIZE);

        Kuznechik kuz;

        kuz.CTR_Crypt(vect_arr, str_arr, out_buf, key_arr, size);

        result = QString::fromStdString(convert_to_string(out_buf, size/2));
    }
    return result;
}

void MainWindow::on_pushButton_clicked()
{
    ui->resultEdit->setText(crypt(ui->dataEdit->text()));
}

