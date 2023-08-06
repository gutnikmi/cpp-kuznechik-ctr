#include <QtWidgets>
#include "kuz_calc.h"

size_t convert_hex(uint8_t *dest, size_t count, const char *src) {
    char buf[3];
    size_t i;
    for (i = 0; i < count && *src; i++) {
        buf[0] = *src++;
        buf[1] = '\0';
        if (*src) {
            buf[1] = *src++;
            buf[2] = '\0';
        }
        if (sscanf(buf, "%hhx", &dest[i]) != 1)
            break;
    }
    return i;
}
std::string convert_to_string(const unsigned char* arr, size_t size) {

    std::string result;
    for (int i = 0; i < size; i++) {
        char hex[3];
        sprintf(hex, "%02x", arr[i]);
        result += hex;
    }
    return result;
}

int main(int argc, char *argv[])
{

    unsigned char
        encrypt_test_string[32];
    unsigned char
        test_key[KEY_SIZE];
    unsigned char
        init_vect_ctr_string[VECT_SIZE];

    unsigned char
        temp_vect[BLCK_SIZE] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Kuznechik CTR");

    // Создаем QLineEdit для ввода строк
    QLineEdit* lineEdit1 = new QLineEdit(&window);
    QLineEdit* lineEdit2 = new QLineEdit(&window);
    QLineEdit* lineEdit3 = new QLineEdit(&window);

    // Создаем QPushButton для сложения строк
    QPushButton* addButton = new QPushButton("Зашифровать/Расшифровать", &window);

    // Создаем QLabel для вывода результата
    QLabel* resultLabel = new QLabel(&window);
    QLabel* label1 = new QLabel(&window);
    QLabel* label2 = new QLabel(&window);
    QLabel* label3 = new QLabel(&window);
    QLabel* label4 = new QLabel(&window);
    label1->setText("Строка");
    label2->setText("Ключ");
    label3->setText("Вектор");
    label4->setText("Шифротекст");
    // Создаем горизонтальный лейаут
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(label1);
    layout->addWidget(lineEdit1);
    layout->addWidget(label2);
    layout->addWidget(lineEdit2);
    layout->addWidget(label3);
    layout->addWidget(lineEdit3);
    layout->addWidget(addButton);
    layout->addWidget(label4);
    layout->addWidget(resultLabel);

    // Устанавливаем лейаут для окна
    window.setLayout(layout);

    QObject::connect(addButton, &QPushButton::clicked, [&]() {
        // Получаем текст из QLineEdit и складываем их
        if((lineEdit3->text().toStdString()).length() != VECT_SIZE)
        {
            std::string res = "Вектор должен быть 8 байт";
            QString result = QString::fromStdString(res);
            resultLabel->setText(result);
            resultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        }
        else if((lineEdit2->text().toStdString()).length() != 64)
        {
            std::string res = "Ключ должен быть 32 байта";
            QString result = QString::fromStdString(res);
            resultLabel->setText(result);
            resultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        }
        else if(((lineEdit1->text().toStdString()).length() % 16))
        {
            std::string res = "Строка должна быть кратна 16";
            QString result = QString::fromStdString(res);
            resultLabel->setText(result);
            resultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        }
        else
        {
        int size = (lineEdit1->text().toStdString()).length();
        convert_hex(encrypt_test_string, size, (lineEdit1->text().toStdString()).c_str());
        convert_hex(test_key,KEY_SIZE, (lineEdit2->text().toStdString()).c_str());
        convert_hex(init_vect_ctr_string, VECT_SIZE, (lineEdit3->text().toStdString()+"0000000000000000").c_str());
        uint8_t out_buf[size];

        memcpy(temp_vect, init_vect_ctr_string, VECT_SIZE);
        CTR_Crypt(init_vect_ctr_string, encrypt_test_string, out_buf, test_key, size);
        std::string res = convert_to_string(out_buf, size/2);
        QString result = QString::fromStdString(res);

        // Устанавливаем результат в QLabel
        resultLabel->setText(result);
        resultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        }
    });
    window.resize(600, 250);
    window.show();

    return app.exec();
}
