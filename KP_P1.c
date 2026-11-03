/*
 * Курсовой проект «Разработка программы табулирования функции»
 * Подзадача 1. Вариант 1 (Бакатанова А. В., 6ИД-261)
 *
 * Y(x) = (a - e^(bx)) / ln|2x|
 *
 *        | 1 + sqrt(|cos(x)|),  x > 1
 * V(x) = | x + 1,              -0.5 <= x <= 1
 *        | 1 - x^2,             x < -0.5
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <float.h>

#define EPS         1e-9   /* точность сравнения вещественных чисел   */
#define EXP_LIMIT   700.0  /* больше - exp() выходит за пределы double */
#define GRAPH_WIDTH 60     /* ширина графика в символах                */
#define MAX_POINTS  500    /* максимальное число строк таблицы         */

double func_y(double x, double a, double b);
double func_v(double x);
int    y_is_defined(double x);
int    is_defined(int choice, double x);
int    y_in_range(double x, double a, double b);
int    v_in_range(double x);
int    in_range(int choice, double x, double a, double b);
int    can_compute(int choice, double x, double a, double b);
double func_value(int choice, double x, double a, double b);
int    count_points(double xmin, double xmax, double dx);
int    first_index(int type);
int    last_index(int type, double xmin, double xmax, double dx, int n);
double find_min(int choice, double xmin, double dx, int i_first, int i_last, double a, double b);
double find_max(int choice, double xmin, double dx, int i_first, int i_last, double a, double b);
int    to_column(double y, double ymin, double ymax, int width);
int    axis_column(double ymin, double ymax, int width);
char   plot_char(int col, int point_col, int axis_col);

int main(void)
{
    int choice, action, type;
    double a = 0.0, b = 0.0;
    double xmin, xmax, dx;
    double x, y, ymin, ymax;
    int n, i, i_first, i_last, col, point_col, axis_col;
    int ch;

    setlocale(LC_CTYPE, "Russian");

    do
    {
        printf("\n Табулирование функции \n");
        printf("1 - Y(x) = (a - e^(bx)) / ln|2x|\n");
        printf("2 - V(x) - кусочная функция\n");
        printf("0 - выход\n");
        printf("Ваш выбор: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Ошибка: нужно ввести число.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF)
                return 0;
            choice = -1;
            continue;
        }
        switch (choice)
        {
        case 0:
            printf("Работа программы завершена.\n");
            break;
        case 1:
            printf("Введите параметры a и b: ");
            if (scanf("%lf %lf", &a, &b) != 2)
            {
                printf("Ошибка: нужно ввести два числа.\n");
                while ((ch = getchar()) != '\n' && ch != EOF);
                if (ch == EOF)
                    return 0;
                continue;
            }
            if (!isfinite(a) || !isfinite(b))
            {
                printf("Ошибка: a и b должны быть конечными числами.\n");
                continue;
            }
            break;
        case 2:
            break;
        default:
            printf("Нет такого пункта меню.\n");
            break;
        }

        if (choice != 1 && choice != 2)
            continue;
        printf("\nЧто сделать с функцией %s?\n", (choice == 1) ? "Y(x)" : "V(x)");
        printf("1 - вычислить значение в точке\n");
        printf("2 - таблица значений\n");
        printf("3 - график\n");
        printf("Ваш выбор: ");
        if (scanf("%d", &action) != 1)
        {
            printf("Ошибка: нужно ввести число.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF)
                return 0;
            continue;
        }
        if (action < 1 || action > 3)
        {
            printf("Нет такого пункта меню.\n");
            continue;
        }
        if (action == 1)
{
    printf("Введите x: ");
    if (scanf("%lf", &x) != 1)
    {
        printf("Ошибка: нужно ввести число.\n");
        while ((ch = getchar()) != '\n' && ch != EOF);
        if (ch == EOF)
            return 0;
        continue;
    }
    if (!isfinite(x))
    {
        printf("Ошибка: x должен быть конечным числом.\n");
        continue;
    }

    if (!is_defined(choice, x))
        printf("%s не определена в точке x = %g\n",
            (choice == 1) ? "Y(x)" : "V(x)", x);
    else if (!in_range(choice, x, a, b))
        printf("Значение %s в точке x = %g не помещается в double (переполнение)\n",
            (choice == 1) ? "Y(x)" : "V(x)", x);
    else
        printf("%s = %.6f при x = %g\n",
            (choice == 1) ? "Y(x)" : "V(x)", func_value(choice, x, a, b), x);
    continue;
        }
        printf("Тип промежутка:\n");
        printf("1 - отрезок      [xmin; xmax]\n");
        printf("2 - интервал     (xmin; xmax)\n");
        printf("3 - полуинтервал [xmin; xmax)\n");
        printf("4 - полуинтервал (xmin; xmax]\n");
        printf("Ваш выбор: ");
        if (scanf("%d", &type) != 1)
        {
            printf("Ошибка: нужно ввести число.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF)
                return 0;
            continue;
        }
        if (type < 1 || type > 4)
        {
            printf("Нет такого типа промежутка.\n");
            continue;
        }
        printf("Введите начало и конец промежутка (xmin xmax): ");
        if (scanf("%lf %lf", &xmin, &xmax) != 2)
        {
            printf("Ошибка: нужно ввести два числа.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF)
                return 0;
            continue;
        }
        printf("Введите шаг табуляции dx: ");
        if (scanf("%lf", &dx) != 1)
        {
            printf("Ошибка: нужно ввести число.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF)
                return 0;
            continue;
        }

        if (!isfinite(xmin) || !isfinite(xmax) || !isfinite(dx))
        {
            printf("Ошибка: xmin, xmax и dx должны быть конечными числами.\n");
            continue;
        }
        if (xmin >= xmax)
        {
            printf("Ошибка: xmin должен быть меньше xmax.\n");
            continue;
        }
        if (dx <= 0.0)
        {
            printf("Ошибка: шаг должен быть больше нуля.\n");
            continue;
        }

        n = count_points(xmin, xmax, dx);
        if (n > MAX_POINTS)
        {
            printf("Слишком много точек (больше %d). Увеличьте шаг.\n", MAX_POINTS);
            continue;
        }

        i_first = first_index(type);
        i_last = last_index(type, xmin, xmax, dx, n);
        if (i_first > i_last)
        {
            printf("На этом промежутке с таким шагом нет ни одной точки. Уменьшите шаг.\n");
            continue;
        }

        if (action == 2)
        {
            printf("\n  Таблица значений функции %s\n", (choice == 1) ? "Y(x)" : "V(x)");
            printf("+------------+-----------------+\n");
            printf("|      x     |      %s       |\n", (choice == 1) ? "Y(x)" : "V(x)");
            printf("+------------+-----------------+\n");
            for (i = i_first; i <= i_last; i++)
            {
                x = xmin + i * dx;
                if (!is_defined(choice, x))
                {
                    printf("| %10.4f | %15s |\n", x, "не определена");
                }
                else if (!in_range(choice, x, a, b))
                {
                    printf("| %10.4f | %15s |\n", x, "переполнение");
                }
                else
                {
                    y = func_value(choice, x, a, b);
                    printf("| %10.4f | %15.6f |\n", x, y);
                }
            }
            printf("+------------+-----------------+\n");
        }
        else
        {
            ymin = find_min(choice, xmin, dx, i_first, i_last, a, b);
            ymax = find_max(choice, xmin, dx, i_first, i_last, a, b);
            if (ymin > ymax)
            {
                printf("Функцию нельзя вычислить ни в одной точке промежутка.\n");
                continue;
            }

            axis_col = axis_column(ymin, ymax, GRAPH_WIDTH);

            printf("\n  График %s (ось x - вниз, ось y - вправо)\n",
                (choice == 1) ? "Y(x)" : "V(x)");
            printf("  y от %.4f до %.4f, '|' - линия y = 0\n", ymin, ymax);
            printf("  пустая строка - функция в точке не определена или переполнение\n\n");

            for (i = i_first; i <= i_last; i++)
            {
                x = xmin + i * dx;
                if (can_compute(choice, x, a, b))
                    point_col = to_column(func_value(choice, x, a, b), ymin, ymax, GRAPH_WIDTH);
                else
                    point_col = -1;

                printf("%10.4f :", x);
                for (col = 0; col < GRAPH_WIDTH; col++)
                    putchar(plot_char(col, point_col, axis_col));
                printf("\n");
            }
        }

    } while (choice != 0);

    return 0;
}

double func_y(double x, double a, double b)
{
    double result;
    result = (a - exp(b * x)) / log(fabs(2.0 * x));
    return result;
}

double func_v(double x)
{
    double result;
    if (x > 1.0)
        result = 1.0 + sqrt(fabs(cos(x)));
    else if (x >= -0.5)
        result = x + 1.0;
    else
        result = 1.0 - x * x;
    return result;
}

int y_is_defined(double x)
{
    int ok = 1;
    if (fabs(x) < EPS)                         /* ln(0) не существует */
        ok = 0;
    else if (fabs(fabs(2.0 * x) - 1.0) < EPS)  /* ln(1) = 0, деление на 0 */
        ok = 0;
    return ok;
}

int is_defined(int choice, double x)
{
    int ok;
    ok = (choice == 1) ? y_is_defined(x) : 1;
    return ok;
}

int y_in_range(double x, double a, double b)
{
    int ok = 1;
    double num, den;
    if (b * x > EXP_LIMIT)                     /* e^(bx) не помещается     */
        ok = 0;
    else
    {
        num = a - exp(b * x);                  /* числ */
        den = log(fabs(2.0 * x));              /* знам */
        if (!isfinite(num))                    /* переполнение a - e^(bx)  */
            ok = 0;
        else if (fabs(den) < 1.0 && fabs(num) > DBL_MAX * fabs(den))
            ok = 0;                            /* переполнение при делении на малое ln */
    }
    return ok;
}

int v_in_range(double x)
{
    int ok = 1;
    if (x < -0.5 && fabs(x) > sqrt(DBL_MAX))
        ok = 0;
    return ok;
}

int in_range(int choice, double x, double a, double b)
{
    int ok;
    ok = (choice == 1) ? y_in_range(x, a, b) : v_in_range(x);
    return ok;
}

int can_compute(int choice, double x, double a, double b)
{
    int ok = 0;
    if (is_defined(choice, x))
        ok = in_range(choice, x, a, b);
    return ok;
}

double func_value(int choice, double x, double a, double b)
{
    double result;
    if (choice == 1)
        result = func_y(x, a, b);
    else
        result = func_v(x);
    return result;
}

int count_points(double xmin, double xmax, double dx)
{
    double steps;
    int n;
    steps = (xmax - xmin) / dx + EPS;          /* число шагов считаем в double */
    if (!isfinite(steps) || steps >= MAX_POINTS)
        n = MAX_POINTS + 1;                    /* слишком много, в int не переводим */
    else
        n = (int)steps + 1;
    return n;
}

int first_index(int type)
{
    int first;
    first = (type == 2 || type == 4) ? 1 : 0;
    return first;
}

int last_index(int type, double xmin, double xmax, double dx, int n)
{
    int last = n - 1;
    if ((type == 2 || type == 3) && fabs(xmin + (n - 1) * dx - xmax) < dx * 1e-6)
        last = n - 2;
    return last;
}

double find_min(int choice, double xmin, double dx, int i_first, int i_last, double a, double b)
{
    double min = DBL_MAX;
    double x, y;
    int i;
    for (i = i_first; i <= i_last; i++)
    {
        x = xmin + i * dx;
        if (can_compute(choice, x, a, b))
        {
            y = func_value(choice, x, a, b);
            if (y < min)
                min = y;
        }
    }
    return min;
}

double find_max(int choice, double xmin, double dx, int i_first, int i_last, double a, double b)
{
    double max = -DBL_MAX;
    double x, y;
    int i;
    for (i = i_first; i <= i_last; i++)
    {
        x = xmin + i * dx;
        if (can_compute(choice, x, a, b))
        {
            y = func_value(choice, x, a, b);
            if (y > max)
                max = y;
        }
    }
    return max;
}

int to_column(double y, double ymin, double ymax, int width)
{
    int col;
    if (ymax * 0.5 - ymin * 0.5 < EPS)
        col = width / 2;
    else
        col = (int)((y * 0.5 - ymin * 0.5) / (ymax * 0.5 - ymin * 0.5) * (width - 1) + 0.5);
    return col;
}

int axis_column(double ymin, double ymax, int width)
{
    int col;
    if (ymin > 0.0 || ymax < 0.0)
        col = -1;
    else
        col = to_column(0.0, ymin, ymax, width);
    return col;
}

char plot_char(int col, int point_col, int axis_col)
{
    char c;
    if (col == point_col)
        c = '*';
    else if (col == axis_col)
        c = '|';
    else
        c = ' ';
    return c;
}