#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tetris.h"

TetBlock tet_templates[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void printTetGame(TetGame* tetg) {
  TetField* tf = tetg->field;  // Получение указателя на игровое поле
  TetFigure* t = tetg->figure;  // Получение указателя на текущую фигуру
  TetFigure* next_t =
      tetg->next_figure;  // Получение указателя на следующую фигуру

  // Отображение игрового поля
  for (int i = 0; i < tf->height; i++) {  // Проход по строкам игрового поля
    for (int j = 0; j < tf->width; j++) {  // Проход по столбцам игрового поля
      int sym = 0;  // Переменная для хранения символа для отображения
      if (tf->blocks[i * tf->width + j].b != 0) {  // Если в поле есть блок
        sym = 2;  // Установка символа для отображения блока
      } else {
        int x = j - t->x;  // Вычисление относительной позиции фигуры по x
        int y = i - t->y;  // Вычисление относительной позиции фигуры по y
        if (x >= 0 && x < t->size && y >= 0 &&
            y < t->size)  // Проверка, находится ли блок фигуры в пределах поля
          if (t->blocks[y * t->size + x].b != 0)  // Если блок фигуры не пустой
            sym = 2;  // Установка символа для отображения блока
      }
      attron(COLOR_PAIR(sym));  // Установка цвета для символа
      mvaddch(i, j, ' ');  // Отображение символа на экране
      attroff(COLOR_PAIR(sym));  // Сброс цвета для символа
    }
  }

  // Отображение следующей фигуры

  // Отображение счета
  attron(COLOR_PAIR(1));  // Установка цветовой пары 1 для отображения счета
  mvprintw(0, 0, "Score: %d", tetg->score);  // Отображение счета на экране
  move(tetg->field->height, tetg->field->width);  // Перемещение курсора
  attroff(COLOR_PAIR(1));  // Сброс цветовой пары
}

int main(int argc, char* argv[]) {
  struct timespec sp_start, sp_end, ts1,
      ts2 = {0, 0};  // Структуры для работы с временем

  initscr();      // Инициализация ncurses
  start_color();  // Включение поддержки цвета
  init_pair(1, COLOR_WHITE, COLOR_MAGENTA);  // Определение цветовой пары 1
  init_pair(2, COLOR_WHITE, COLOR_GREEN);  // Определение цветовой пары 2
  init_pair(3, COLOR_WHITE, COLOR_BLUE);  // Определение цветовой пары 3
  init_pair(4, COLOR_WHITE, COLOR_CYAN);  // Определение цветовой пары 4
  cbreak();  // Включение режима "без буферизации ввода"
  noecho();  // Отключение отображения вводимых символов
  nodelay(stdscr, TRUE);  // Включение не блокирующего режима getch()
  scrollok(stdscr, TRUE);  // Разрешение прокрутки экрана

  TetGame* tetg =
      createTetGame(10, 20, 5, 7, tet_templates);  // Создание игры Тетрис

  TetPlayer player;  // Создание игрока
  player.action = TET_PLAYER_NOP;  // Установка начального действия игрока
                                   // (ничего не делать)
  tetg->player = &player;  // Установка игрока в игре
  dropNewFigure(tetg);     // Генерация новой фигуры

  int started = 0;  //
  int paused = 0;   // Флаг для состояния паузы

  clear();  // Очистка экрана
  mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Press 'Enter'");
  refresh();

  while (!started) {
    int ch = getch();
    if (ch == '\n') {  // Проверка на нажатие Enter
      started = 1;
      clear();  // Очистка экрана перед началом игры
      printTetGame(tetg);  // Отображение состояния игры
      refresh();
    }
  }

  while (tetg->playing != TET_GAMEOVER) {  // Пока игра не окончена
    clock_gettime(CLOCK_MONOTONIC,
                  &sp_start);  // Получение времени начала цикла

    int ch = getch();  // Получение символа ввода
    switch (ch) {
      case 'w':
        if (!paused) player.action = TET_PLAYER_UP;  // Действие вверх
        break;
      case 's':
        if (!paused) player.action = TET_PLAYER_DOWN;  // Действие вниз
        break;
      case 'a':
        if (!paused) player.action = TET_PLAYER_LEFT;  // Действие влево
        break;
      case 'd':
        if (!paused) player.action = TET_PLAYER_RIGHT;  // Действие вправо
        break;
      case 'p':
        paused = !paused;  // Переключение состояния паузы
        if (paused) {
          mvprintw(tetg->field->height / 2, tetg->field->width / 2 - 5,
                   "PAUSED");  // Отображение сообщения о паузе
          refresh();
        } else {
          clear();
          printTetGame(tetg);  // Перерисовка игры при снятии паузы
          refresh();
        }
        break;
      case 'q':
        tetg->playing = TET_GAMEOVER;  // Завершение игры
        break;
      default:
        if (!paused) player.action = TET_PLAYER_NOP;  // Нет действия
        break;
    }

    if (!paused) {
      calculateTet(tetg);  // Выполнение расчетов для обновления состояния игры
      printTetGame(tetg);  // Отображение состояния игры

      attron(COLOR_PAIR(1));  // Установка цветовой пары 1 для отображения счета
      mvprintw(0, 0, "Score: %d", tetg->score);  // Отображение счета на экране
      move(tetg->field->height, tetg->field->width);  // Перемещение курсора
      attroff(COLOR_PAIR(1));  // Сброс цветовой пары

      refresh();  // Обновление экрана
      clock_gettime(CLOCK_MONOTONIC,
                    &sp_end);  // Получение времени окончания цикла
      if (sp_end.tv_sec - sp_start.tv_sec <=
              0 &&  // Если время цикла меньше или равно 1 секунде
          (ts2.tv_nsec = 33000000 - (sp_end.tv_nsec - sp_start.tv_nsec)) >
              0)  // Рассчитать время ожидания
        nanosleep(&ts2, &ts1);  // Ожидание до следующего цикла
    }
  }

  freeTetGame(tetg);  // Освобождение памяти, занятой игрой
  endwin();           // Завершение работы с ncurses
  return 0;           // Завершение программы
}