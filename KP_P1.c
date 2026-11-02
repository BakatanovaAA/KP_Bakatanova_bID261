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

#define EPS         1e-9   
#define EXP_LIMIT   700.0  
#define GRAPH_WIDTH 60     
#define MAX_POINTS  500   

double func_y(double x, double a, double b);
double func_v(double x);
int    y_is_defined(double x, double b);
int    is_defined(int choice, double x, double b);
double func_value(int choice, double x, double a, double b);
int    count_points(double xmin, double xmax, double dx);
double find_min(int choice, double xmin, double dx, int n, double a, double b);
double find_max(int choice, double xmin, double dx, int n, double a, double b);
int    to_column(double y, double ymin, double ymax, int width);
int    axis_column(double ymin, double ymax, int width);
char   plot_char(int col, int point_col, int axis_col);

int main(void)
{
    int choice;
    double a = 0.0, b = 0.0;
    double xmin, xmax, dx;
    double x, y, ymin, ymax;
    int n, i, col, point_col, axis_col;

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
            while (getchar() != '\n');
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
                while (getchar() != '\n');
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

        printf("Введите начало и конец интервала (xmin xmax): ");
        if (scanf("%lf %lf", &xmin, &xmax) != 2)
        {
            printf("Ошибка: нужно ввести два числа.\n");
            while (getchar() != '\n');
            continue;
        }
        printf("Введите шаг табуляции dx: ");
        if (scanf("%lf", &dx) != 1)
        {
            printf("Ошибка: нужно ввести число.\n");
            while (getchar() != '\n');
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
            printf("Слишком много точек (%d). Увеличьте шаг.\n", n);
            continue;
        }

        printf("\n  Таблица значений функции %s\n", (choice == 1) ? "Y(x)" : "V(x)");
        printf("+------------+-----------------+\n");
        printf("|      x     |      %s       |\n", (choice == 1) ? "Y(x)" : "V(x)");
        printf("+------------+-----------------+\n");
        for (i = 0; i < n; i++)
        {
            x = xmin + i * dx;
            if (is_defined(choice, x, b))
            {
                y = func_value(choice, x, a, b);
                printf("| %10.4f | %15.6f |\n", x, y);
            }
            else
            {
                printf("| %10.4f | %15s |\n", x, "не определена");
            }
        }
        printf("+------------+-----------------+\n");

        ymin = find_min(choice, xmin, dx, n, a, b);
        ymax = find_max(choice, xmin, dx, n, a, b);
        if (ymin > ymax)
        {
            printf("Функция не определена ни в одной точке интервала.\n");
            continue;
        }

        axis_col = axis_column(ymin, ymax, GRAPH_WIDTH);

        printf("\n  График %s (ось x - вниз, ось y - вправо)\n",
            (choice == 1) ? "Y(x)" : "V(x)");
        printf("  y от %.4f до %.4f, '|' - линия y = 0\n\n", ymin, ymax);

        for (i = 0; i < n; i++)
        {
            x = xmin + i * dx;
            if (is_defined(choice, x, b))
                point_col = to_column(func_value(choice, x, a, b), ymin, ymax, GRAPH_WIDTH);
            else
                point_col = -1;

            printf("%10.4f :", x);
            for (col = 0; col < GRAPH_WIDTH; col++)
                putchar(plot_char(col, point_col, axis_col));
            printf("\n");
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

/* проверка обл.доп.зн.Y(x): 1 - определена, 0 - нет */
int y_is_defined(double x, double b)
{
    int ok = 1;
    if (fabs(x) < EPS)                         /* ln(0) не существует */
        ok = 0;
    else if (fabs(fabs(2.0 * x) - 1.0) < EPS)  /* ln(1) = 0 деление на 0 */
        ok = 0;
    else if (b * x > EXP_LIMIT)                /* переполнение exp() */
        ok = 0;
    return ok;
}

int is_defined(int choice, double x, double b)
{
    int ok;
    ok = (choice == 1) ? y_is_defined(x, b) : 1;
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
    int n;
    n = (int)((xmax - xmin) / dx + EPS) + 1;
    return n;
}

double find_min(int choice, double xmin, double dx, int n, double a, double b)
{
    double min = DBL_MAX;
    double x, y;
    int i;
    for (i = 0; i < n; i++)
    {
        x = xmin + i * dx;
        if (is_defined(choice, x, b))
        {
            y = func_value(choice, x, a, b);
            if (y < min)
                min = y;
        }
    }
    return min;
}

double find_max(int choice, double xmin, double dx, int n, double a, double b)
{
    double max = -DBL_MAX;
    double x, y;
    int i;
    for (i = 0; i < n; i++)
    {
        x = xmin + i * dx;
        if (is_defined(choice, x, b))
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
    if (ymax - ymin < EPS)
        col = width / 2;
    else
        col = (int)((y - ymin) / (ymax - ymin) * (width - 1) + 0.5);
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