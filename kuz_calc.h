#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#define BLCK_SIZE 16 /// Размер блока
#define KEY_SIZE  32 /// Размер ключа
#define VECT_SIZE 16 /// Размер вектора инициализации
#define STR_SIZE  32 /// Максимальный размер строки

/*
* значения для нелинейного преобразования
* множества двоичных векторов (преобразование S)
*/
static const unsigned char Pi[256] = {
    0xFC, 0xEE, 0xDD, 0x11, 0xCF, 0x6E, 0x31, 0x16,
    0xFB, 0xC4, 0xFA, 0xDA, 0x23, 0xC5, 0x04, 0x4D,
    0xE9, 0x77, 0xF0, 0xDB, 0x93, 0x2E, 0x99, 0xBA,
    0x17, 0x36, 0xF1, 0xBB, 0x14, 0xCD, 0x5F, 0xC1,
    0xF9, 0x18, 0x65, 0x5A, 0xE2, 0x5C, 0xEF, 0x21,
    0x81, 0x1C, 0x3C, 0x42, 0x8B, 0x01, 0x8E, 0x4F,
    0x05, 0x84, 0x02, 0xAE, 0xE3, 0x6A, 0x8F, 0xA0,
    0x06, 0x0B, 0xED, 0x98, 0x7F, 0xD4, 0xD3, 0x1F,
    0xEB, 0x34, 0x2C, 0x51, 0xEA, 0xC8, 0x48, 0xAB,
    0xF2, 0x2A, 0x68, 0xA2, 0xFD, 0x3A, 0xCE, 0xCC,
    0xB5, 0x70, 0x0E, 0x56, 0x08, 0x0C, 0x76, 0x12,
    0xBF, 0x72, 0x13, 0x47, 0x9C, 0xB7, 0x5D, 0x87,
    0x15, 0xA1, 0x96, 0x29, 0x10, 0x7B, 0x9A, 0xC7,
    0xF3, 0x91, 0x78, 0x6F, 0x9D, 0x9E, 0xB2, 0xB1,
    0x32, 0x75, 0x19, 0x3D, 0xFF, 0x35, 0x8A, 0x7E,
    0x6D, 0x54, 0xC6, 0x80, 0xC3, 0xBD, 0x0D, 0x57,
    0xDF, 0xF5, 0x24, 0xA9, 0x3E, 0xA8, 0x43, 0xC9,
    0xD7, 0x79, 0xD6, 0xF6, 0x7C, 0x22, 0xB9, 0x03,
    0xE0, 0x0F, 0xEC, 0xDE, 0x7A, 0x94, 0xB0, 0xBC,
    0xDC, 0xE8, 0x28, 0x50, 0x4E, 0x33, 0x0A, 0x4A,
    0xA7, 0x97, 0x60, 0x73, 0x1E, 0x00, 0x62, 0x44,
    0x1A, 0xB8, 0x38, 0x82, 0x64, 0x9F, 0x26, 0x41,
    0xAD, 0x45, 0x46, 0x92, 0x27, 0x5E, 0x55, 0x2F,
    0x8C, 0xA3, 0xA5, 0x7D, 0x69, 0xD5, 0x95, 0x3B,
    0x07, 0x58, 0xB3, 0x40, 0x86, 0xAC, 0x1D, 0xF7,
    0x30, 0x37, 0x6B, 0xE4, 0x88, 0xD9, 0xE7, 0x89,
    0xE1, 0x1B, 0x83, 0x49, 0x4C, 0x3F, 0xF8, 0xFE,
    0x8D, 0x53, 0xAA, 0x90, 0xCA, 0xD8, 0x85, 0x61,
    0x20, 0x71, 0x67, 0xA4, 0x2D, 0x2B, 0x09, 0x5B,
    0xCB, 0x9B, 0x25, 0xD0, 0xBE, 0xE5, 0x6C, 0x52,
    0x59, 0xA6, 0x74, 0xD2, 0xE6, 0xF4, 0xB4, 0xC0,
    0xD1, 0x66, 0xAF, 0xC2, 0x39, 0x4B, 0x63, 0xB6
};
/// массив коэффицентов для R-преобразования
static const unsigned char l_vec[16] = {
    1  , 148, 32 , 133,
    16 , 194, 192, 1  ,
    251, 1  , 192, 194,
    16 , 133, 32 , 148
};

typedef uint8_t vect[BLCK_SIZE]; /// Определяем тип vect как 16-байтовый массив

class Kuznechik
{
public:
    /*!
     * \brief CTR_Crypt - шифрование в режиме гаммирования без усечения гаммы
     * \param init_vec - вектор инициализации
     * \param in_buf - входные данные
     * \param out_buf - буфер для результата
     * \param key - ключ шифрования
     * \param size - размер данных
     */
    void CTR_Crypt(uint8_t *init_vec, uint8_t *in_buf, uint8_t *out_buf, uint8_t *key, uint64_t size);

private:

    /*!
     * \brief GOST_Kuz_Expand_Key - Разворачивания ключа
     * \param key - ключ
     */
    void GOST_Kuz_Expand_Key(const uint8_t *key);

    /*!
     * \brief GOST_Kuz_Destroy_Key - Уничтожения ключа
     */
    void GOST_Kuz_Destroy_Key();

    /*!
     * \brief GOST_Kuz_Encrypt - шифрование 1 блока данных
     * \param blk - незашифрованный блок
     * \param out_blk - блок после раунда шифрования
     */
    void GOST_Kuz_Encrypt(const uint8_t *blk, uint8_t *out_blk);

    /*!
     * \brief GOST_Kuz_S - функция S преобразования
     * \param in_data - данные на вход
     * \param out_data - данные на выход
     */
    void GOST_Kuz_S(const uint8_t *in_data, uint8_t *out_data);

    /*!
     * \brief GOST_Kuz_X - Сложение двух двоичных векторов по модулю 2
     * \param a - первый вектор
     * \param b - второй вектор
     * \param c - третий вектор
     */
    void GOST_Kuz_X(const uint8_t *a, const uint8_t *b, uint8_t *c);

    /*!
     * \brief GOST_Kuz_GF_mul - умножения чисел в конечном поле Галуа
     * \param a - число
     * \param b - множитель
     * \return произведение
     */
    uint8_t GOST_Kuz_GF_mul(uint8_t a, uint8_t b);

    /*!
     * \brief GOST_Kuz_R - функция R-преобразования
     * \param state - состояние
     */
    void GOST_Kuz_R(uint8_t *state);

    /*!
     * \brief GOST_Kuz_L - Линейное преобразование L
     * \param in_data - данные на вход
     * \param out_data - данные на выход
     */
    void GOST_Kuz_L(const uint8_t *in_data, uint8_t *out_data);

    /*!
     * \brief GOST_Kuz_Get_C - вычисление итерационных констант
     */
    void GOST_Kuz_Get_C();

    /*!
     * \brief GOST_Kuz_F - итерация развертывания ключа
     * \param in_key_1 - ключ 1 до преобразования
     * \param in_key_2 - ключ 2 до преобразования
     * \param out_key_1 - ключ 1 после преобразования
     * \param out_key_2 - ключ 1 после преобразования
     * \param iter_const - итерационная константа
     */
    void GOST_Kuz_F(const uint8_t *in_key_1, const uint8_t *in_key_2,uint8_t *out_key_1, uint8_t *out_key_2, uint8_t *iter_const);

    /*!
     * \brief inc_ctr - увеличение счетчика
     * \param ctr - указатель на счетчик
     */
    void inc_ctr(uint8_t *ctr);

    /*!
     * \brief add_xor - сложение исключающим или
     * \param a - слогаемое 1
     * \param b - слогаемое 2
     * \param c - результат сложения
     */
    void add_xor(const uint8_t *a, const uint8_t *b, uint8_t *c);

private:

    vect iter_C[32]; /// итерационные константы C

    vect iter_key[10]; /// итерационные ключи шифрования
};

/*!
 * \brief reverse_array - переворот массива
 * \param array - массив
 * \param size - размер массива
 */
void reverse_array(unsigned char *array, int size);

/*!
 * \brief convert_hex - конвертировать массива char в uint8_t
 * \param dest - назначение
 * \param src - источник
 * \param count - размер
 * \return сколько элементов конвертировано
 */
size_t convert_hex(uint8_t *dest, const char *src, size_t count);

/*!
 * \brief convert_to_string - конвертировать массива char в string
 * \param arr - массив char
 * \param size - размер массива
 * \return строка
 */
std::string convert_to_string(const unsigned char* arr, size_t size);

/*!
 * \brief reverse_hex - побайтовый переворот строки
 * \param str - строка
 * \return перевернутая строка
 */
std::string reverse_hex(std::string str);
