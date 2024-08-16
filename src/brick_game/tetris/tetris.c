#include "tetris.h"
#include <stdlib.h>  // Подключение стандартной библиотеки для работы с памятью
#ifndef TETRIS_C  // Условная компиляция для предотвращения повторного включения

#define TETRIS_C

// Функция для создания структуры хранения всех фигур
TetFiguresT* createTetFiguresT(int count, int figures_size,
                               TetBlock* figures_template) {
  TetFiguresT* tetft = (TetFiguresT*)malloc(
      sizeof(TetFiguresT));  // Выделение памяти для структуры
  tetft->count = count;  // Установка количества фигур
  tetft->size = figures_size;  // Установка размера фигур
  tetft->blocks = figures_template;  // Установка указателя на шаблон фигур
  return tetft;  // Возвращение указателя на созданную структуру
};

void freeTetFiguresT(TetFiguresT* tetft) {
  if (tetft)  // Проверка, что указатель не равен NULL
    free(tetft);  // Освобождение памяти, занятой структурой фигур
};

TetField* createTetField(int width, int height) {
  TetField* tetf = (TetField*)malloc(
      sizeof(TetField));  // Выделение памяти для структуры игрового поля
  tetf->width = width;    // Установка ширины поля
  tetf->height = height;  // Установка высоты поля
  tetf->blocks = (TetBlock*)malloc(sizeof(TetBlock) * width *
                                   height);  // Выделение памяти для блоков поля
  for (int i = 0; i < width * height; i++) {  // Инициализация всех блоков поля
    tetf->blocks[i].b = 0;  // Установка значения блока в 0 (пустой блок)
  }
  return tetf;  // Возвращение указателя на созданное поле
};

void freeTetField(TetField* tetf) {
  if (tetf) {  // Проверка, что указатель не равен NULL
    if (tetf->blocks)  // Проверка, что блоки не равны NULL
      free(tetf->blocks);  // Освобождение памяти, занятой блоками
    free(tetf);  // Освобождение памяти, занятой структурой поля
  };
};

TetGame* createTetGame(int field_width, int field_height, int figures_size,
                       int count, TetBlock* figures_template) {
  TetGame* tetg =
      (TetGame*)malloc(sizeof(TetGame));  // Выделение памяти для структуры игры
  tetg->field =
      createTetField(field_width, field_height);  // Создание игрового поля
  tetg->figurest = createTetFiguresT(count, figures_size,
                                     figures_template);  // Создание всех фигур

  tetg->ticks = TET_TICKS_START;  // Установка начального значения тиков
  tetg->ticks_left =
      TET_TICKS_START;  // Установка оставшихся тиков до следующего действия
  tetg->score = 0;      // Установка начального счета
  tetg->playing =
      TET_PLAYING;  // Установка начального состояния игры (в процессе)

  tetg->next_figure =
      createTetFigure(tetg);  // Создание фигуры, которая будет следующей

  return tetg;  // Возвращение указателя на созданную игру
};

void freeTetGame(TetGame* tetg) {
  if (tetg) {
    freeTetField(tetg->field);  // Освобождение памяти, занятой игровым полем
    freeTetFiguresT(
        tetg->figurest);  // Освобождение памяти, занятой всеми фигурами
    freeTetFigure(tetg->figure);  // Освобождение памяти текущей фигуры
    freeTetFigure(tetg->next_figure);  // Освобождение памяти следующей фигуры
    free(tetg);  // Освобождение памяти, занятой структурой игры
  }
};

void moveFigureDown(TetGame* tetg) {
  tetg->figure->y++;  // Перемещение фигуры вниз (увеличение координаты y)
};

void moveFigureUp(TetGame* tetg) {
  tetg->figure->y--;  // Перемещение фигуры вверх (уменьшение координаты y)
};

void moveFigureRight(TetGame* tetg) {
  tetg->figure->x++;  // Перемещение фигуры вправо (увеличение координаты x)
};

void moveFigureLeft(TetGame* tetg) {
  tetg->figure->x--;  // Перемещение фигуры влево (уменьшение координаты x)
};

int collisionTet(TetGame* tetg) {
  TetFigure* t = tetg->figure;  // Получение текущей фигуры
  TetField* tf = tetg->field;   // Получение игрового поля
  for (int i = 0; i < t->size; i++) {  // Проход по строкам фигуры
    for (int j = 0; j < t->size; j++) {  // Проход по столбцам фигуры
      if (t->blocks[i * t->size + j].b != 0) {  // Если блок фигуры не пустой
        int fx = t->x + j;  // Вычисление координаты x блока на поле
        int fy = t->y + i;  // Вычисление координаты y блока на поле
        if (tf->blocks[fy * tf->width + fx].b !=
            0) {  // Проверка на столкновение с уже установленным блоком на поле
          return 1;  // Обнаружено столкновение
        };
        if (fx < 0 || fx >= tf->width || fy < 0 ||
            fy >= tf->height) {  // Проверка выхода блока за границы поля
          return 1;  // Обнаружено столкновение с границей поля
        };
      };
    };
  };
  return 0;  // Нет столкновений
};

void plantFigure(TetGame* tetg) {
  TetFigure* t = tetg->figure;  // Получение текущей фигуры
  for (int i = 0; i < t->size; i++)  // Проход по строкам фигуры
    for (int j = 0; j < t->size; j++)  // Проход по столбцам фигуры
      if (t->blocks[i * t->size + j].b != 0) {  // Если блок фигуры не пустой
        int fx = t->x + j;  // Вычисление координаты x блока на поле
        int fy = t->y + i;  // Вычисление координаты y блока на поле
        tetg->field->blocks[fy * tetg->field->width + fx].b =
            t->blocks[i * t->size + j].b;  // Установка блока на поле
      };
};

int lineFilledTet(int i, TetField* tfl) {
  for (int j = 0; j < tfl->width; j++)  // Проход по всем столбцам строки
    if (tfl->blocks[i * tfl->width + j].b ==
        0)       // Если хотя бы один блок пустой
      return 0;  // Линия не заполнена
  return 1;      // Линия заполнена
};

void dropLineTet(int i, TetField* tfl) {
  if (i == 0) {  // Если удаляемая линия первая (вверхнее положение)
    for (int j = 0; j < tfl->width;
         j++)  // Проход по всем столбцам первой строки
      tfl->blocks[j].b = 0;  // Обнуление всех блоков первой строки
  } else {  // Если удаляемая линия не первая
    for (int k = i; k > 0; k--)  // Проход с удаляемой строки вверх
      for (int j = 0; j < tfl->width; j++)  // Проход по всем столбцам строки
        tfl->blocks[k * tfl->width + j].b =
            tfl->blocks[(k - 1) * tfl->width + j]
                .b;  // Перемещение блока на одну строку вниз
  }
};

int eraseLinesTet(TetGame* tetg) {
  TetField* tfl = tetg->field;  // Получение игрового поля
  int count = 0;  // Количество удаленных линий
  for (int i = tfl->height - 1; i >= 0;
       i--) {  // Проход по всем строкам поля с конца
    while (lineFilledTet(i, tfl)) {  // Пока линия заполнена
      dropLineTet(i, tfl);           // Удаление линии
      count++;  // Увеличение счетчика удаленных линий
    };
  };
  return count;  // Возвращение количества удаленных линий
};

TetFigure* createTetFigure(TetGame* tetg) {
  TetFigure* t = (TetFigure*)malloc(
      sizeof(TetFigure));  // Выделение памяти для новой фигуры
  t->x = 0;  // Установка начальной координаты x фигуры
  t->y = 0;  // Установка начальной координаты y фигуры
  t->size = tetg->figurest->size;  // Установка размера фигуры
  t->blocks = (TetBlock*)malloc(sizeof(TetBlock) * t->size *
                                t->size);  // Выделение памяти для блоков фигуры
  return t;  // Возвращение указателя на созданную фигуру
};

void freeTetFigure(TetFigure* tf) {
  if (tf) {  // Проверка, что указатель не равен NULL
    if (tf->blocks)  // Проверка, что блоки не равны NULL
      free(tf->blocks);  // Освобождение памяти, занятой блоками фигуры
    free(tf);  // Освобождение памяти, занятой структурой фигуры
  };
};

void dropNewFigure(TetGame* tetg) {
  TetFigure* t = tetg->next_figure;  // Получение следующей фигуры
  t->x = tetg->field->width / 2 -
         t->size / 2;  // Установка начальной координаты x фигуры
  t->y = 0;  // Установка начальной координаты y фигуры
  int fnum = rand() % tetg->figurest->count;  // Выбор случайного номера фигуры
  for (int i = 0; i < t->size; i++)  // Проход по строкам фигуры
    for (int j = 0; j < t->size; j++) {  // Проход по столбцам фигуры
      t->blocks[i * t->size + j].b =
          tetg->figurest->blocks[fnum * t->size * t->size + i * t->size + j]
              .b;  // Копирование блока из шаблона фигур
    };
  freeTetFigure(tetg->figure);  // Освобождение памяти предыдущей фигуры
  tetg->figure = tetg->next_figure;  // Установка новой фигуры
  tetg->next_figure = createTetFigure(tetg);  // Создание новой следующей фигуры
};

TetFigure* rotTetFigure(TetGame* tetg) {
  TetFigure* t = createTetFigure(
      tetg);  // Создание новой фигуры для хранения повернутой версии
  TetFigure* told = tetg->figure;  // Получение текущей фигуры
  t->x = told->x;  // Установка координаты x для повернутой фигуры
  t->y = told->y;  // Установка координаты y для повернутой фигуры
  for (int i = 0; i < t->size; i++)  // Проход по строкам фигуры
    for (int j = 0; j < t->size; j++)  // Проход по столбцам фигуры
      t->blocks[i * t->size + j].b = told->blocks[j * t->size + t->size - 1 - i]
                                         .b;  // Выполнение поворота блоков
  return t;  // Возвращение указателя на повернутую фигуру
};

void calculateTet(TetGame* tetg) {
  if (tetg->ticks_left <= 0) {  // Если время на перемещение фигуры истекло
    tetg->ticks_left = tetg->ticks;  // Сброс таймера
    moveFigureDown(tetg);      // Перемещение фигуры вниз
    if (collisionTet(tetg)) {  // Проверка на столкновение после перемещения
      moveFigureUp(tetg);      // Отмена перемещения
      plantFigure(tetg);       // Установка фигуры на поле
      tetg->score +=
          eraseLinesTet(tetg);  // Удаление заполненных линий и обновление счета
      dropNewFigure(tetg);  // Выбрасывание новой фигуры
      if (collisionTet(
              tetg)) {  // Проверка на столкновение после установки новой фигуры
        tetg->playing = TET_GAMEOVER;  // Завершение игры
        return;                        // Выход из функции
      };
    };
  };
  switch (tetg->player->action) {  // Обработка действия игрока
    case TET_PLAYER_RIGHT:  // Перемещение фигуры вправо
      moveFigureRight(tetg);
      if (collisionTet(tetg))  // Проверка на столкновение после перемещения
        moveFigureLeft(tetg);  // Отмена перемещения
      break;
    case TET_PLAYER_LEFT:  // Перемещение фигуры влево
      moveFigureLeft(tetg);
      if (collisionTet(tetg))  // Проверка на столкновение после перемещения
        moveFigureRight(tetg);  // Отмена перемещения
      break;
    case TET_PLAYER_DOWN:  // Перемещение фигуры вниз
      moveFigureDown(tetg);
      if (collisionTet(tetg))  // Проверка на столкновение после перемещения
        moveFigureUp(tetg);    // Отмена перемещения
      break;
    case TET_PLAYER_UP: {                 // Поворот фигуры
      TetFigure* t = rotTetFigure(tetg);  // Создание повернутой фигуры
      TetFigure* told = tetg->figure;  // Получение текущей фигуры
      tetg->figure = t;  // Установка повернутой фигуры
      if (collisionTet(tetg)) {  // Проверка на столкновение после поворота
        tetg->figure = told;  // Возвращение старой фигуры
        freeTetFigure(t);  // Освобождение памяти, занятой повернутой фигурой
      } else {
        freeTetFigure(told);  // Освобождение памяти старой фигуры
      };
    }; break;
    case TET_PLAYER_NOP:  // Нет действия
    default:
      break;
  };
  tetg->ticks_left--;  // Уменьшение оставшихся тиков
};

#endif  // Конец условной компиляции для предотвращения повторного включения
