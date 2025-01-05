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
    unsigned char
        str_arr[32];
    unsigned char
        key_arr[KEY_SIZE];
    unsigned char
        init_vect_ctr_string[VECT_SIZE];

    unsigned char
        temp_vect[BLCK_SIZE] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };
    bool checked = true;
    QString result;

    if((ui->vectorEdit->text().toStdString()).length() != VECT_SIZE)
    {
        std::string res = "Вектор должен быть 8 байт";
        result = QString::fromStdString(res);
        ui->resultEdit->setText(result);
    }
    else if((ui->keyEdit->text().toStdString()).length() != 64)
    {
        std::string res = "Ключ должен быть 32 байта";
        result = QString::fromStdString(res);
        ui->resultEdit->setText(result);
    }
    else if(((ui->dataEdit->text().toStdString()).length() % 16))
    {
        std::string res = "Строка должна быть кратна 16";
        result = QString::fromStdString(res);
        ui->resultEdit->setText(result);
    }
    else
    {
        std::string key_str = ui->keyEdit->text().toStdString();
        std::string vect_str = ui->vectorEdit->text().toStdString()+"0000000000000000";
        if(checked)
        {
            key_str = reverse_hex(key_str);
            vect_str = reverse_hex(vect_str);
        }
        int size = (ui->dataEdit->text().toStdString()).length();
        convert_hex(str_arr, size, (ui->dataEdit->text().toStdString()).c_str());
        convert_hex(key_arr,KEY_SIZE, (key_str.c_str()));
        convert_hex(init_vect_ctr_string, VECT_SIZE, vect_str.c_str());
        uint8_t out_buf[size];
        //reverse_array(str_arr, size);

        memcpy(temp_vect, init_vect_ctr_string, VECT_SIZE);
        CTR_Crypt(init_vect_ctr_string, str_arr, out_buf, key_arr, size, checked);
        std::string res = convert_to_string(out_buf, size/2);
        result = QString::fromStdString(res);

        // Устанавливаем результат в QLabel
         ui->resultEdit->setText(result);
    }
    return result;
}

void MainWindow::on_pushButton_clicked()
{
    crypt(ui->dataEdit->text());
}

