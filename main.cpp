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
        test_key[32];
    unsigned char
        init_vect_ctr_string[64];

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
        convert_hex(encrypt_test_string, 32, (lineEdit1->text().toStdString()).c_str());
        convert_hex(test_key,32, (lineEdit2->text().toStdString()).c_str());
        convert_hex(init_vect_ctr_string, 16, (lineEdit3->text().toStdString()+"0000000000000000").c_str());
        uint8_t out_buf[64];
        uint8_t out2[64];

        memcpy(temp_vect, init_vect_ctr_string, 16);
        CTR_Crypt(init_vect_ctr_string, encrypt_test_string, out_buf, test_key, 32);
        std::string res = convert_to_string(out_buf, 32);
        QString result = QString::fromStdString(res);

        // Устанавливаем результат в QLabel
        resultLabel->setText(result);
        resultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    });
    window.setFixedSize(600, 250);
    window.show();

    return app.exec();
}
