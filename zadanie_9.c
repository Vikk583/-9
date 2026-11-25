#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <queue>
#include <chrono>

using namespace std;

volatile int result_sink = 0;

struct Node {
    int vertex;
    Node* next;
};

int** createMatrix(int n) {
    int** a = (int**)malloc(n * sizeof(int*));

    if (!a) {
        printf("Ошибка выделения памяти\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        a[i] = (int*)calloc(n, sizeof(int));

        if (!a[i]) {
            printf("Ошибка выделения памяти\n");

            for (int j = 0; j < i; j++) {
                free(a[j]);
            }

            free(a);
            exit(1);
        }
    }

    return a;
}

void freeMatrix(int** a, int n) {
    if (!a) return;

    for (int i = 0; i < n; i++) {
        free(a[i]);
    }

    free(a);
}

void generateMatrix(int** a, int n, int p) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            a[i][j] = 0;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if ((rand() % 100) < p) {
                a[i][j] = 1;
                a[j][i] = 1;
            }
        }
    }
}

void printMatrix(int** a, int n) {
    printf("\nМатрица смежности графа G:\n");

    printf("    ");

    for (int j = 0; j < n; j++)
        printf("%3d", j + 1);

    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%3d|", i + 1);

        for (int j = 0; j < n; j++)
            printf("%3d", a[i][j]);

        printf("\n");
    }
}

Node** createLists(int** a, int n) {
    Node** list = (Node**)malloc(n * sizeof(Node*));

    if (!list) {
        printf("Ошибка выделения памяти\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        list[i] = NULL;

        for (int j = n - 1; j >= 0; j--) {
            if (a[i][j] == 1) {
                Node* temp = new Node;

                temp->vertex = j;
                temp->next = list[i];

                list[i] = temp;
            }
        }
    }

    return list;
}

bool isIsolated(int** a, int n, int v) { //изолированная
    for (int i = 0; i < n; i++) {
        if (a[v][i] == 1) {
            return false;
        }
    }

    return true;
}

void printLists(Node** list, int n) {
    printf("\nСписки смежности:\n");

    for (int i = 0; i < n; i++) {
        printf("%3d -> ", i + 1);

        Node* temp = list[i];

        while (temp != NULL) {
            printf("%d ", temp->vertex + 1);
            temp = temp->next;
        }

        printf("\n");
    }
}

void deleteLists(Node** list, int n) {
    if (!list) return;

    for (int i = 0; i < n; i++) {
        Node* temp = list[i];

        while (temp != NULL) {
            Node* next = temp->next;

            delete temp;

            temp = next;
        }
    }

    free(list);
}

void reset_dist(int* DIST, int n, int start_v) {
    for (int i = 0; i < n; i++)
        DIST[i] = -1;

    DIST[start_v] = 0;
}

void print_dist(int* DIST, int n, int start) {
    printf("Кратчайшие расстояния от вершины %d:\n", start + 1);

    for (int i = 0; i < n; i++) {
        printf("  до вершины %3d: ", i + 1);

        if (DIST[i] == -1)
            printf("недостижима\n");
        else
            printf("%d\n", DIST[i]);
    }
}

void along_the_width(int start, int n, int** a, int* DIST) { //bfs матрица
    queue<int> q;

    q.push(start);

    while (!q.empty()) {
        int v = q.front();

        q.pop();

        for (int i = 0; i < n; i++) {
            if (a[v][i] == 1 && DIST[i] == -1) {
                DIST[i] = DIST[v] + 1;

                q.push(i);
            }
        }
    }
}

void along_the_width_list(int start, Node** list, int* DIST) {   //bfs списки
    queue<int> q;

    q.push(start);

    while (!q.empty()) {
        int v = q.front();

        q.pop();

        Node* temp = list[v];

        while (temp != NULL) {
            int u = temp->vertex;

            if (DIST[u] == -1) {
                DIST[u] = DIST[v] + 1;

                q.push(u);
            }

            temp = temp->next;
        }
    }
}

void Go_deep_matrix(int v, int n, int** a, int* DIST) {  //dfs матрица
    for (int i = 0; i < n; i++) {
        if (a[v][i] == 1) {
            if (DIST[i] == -1 || DIST[i] > DIST[v] + 1) {
                DIST[i] = DIST[v] + 1;

                Go_deep_matrix(i, n, a, DIST);
            }
        }
    }
}

void Go_deep_list(int v, Node** list, int* DIST) {   //dfs списки
    Node* temp = list[v];

    while (temp != NULL) {
        int u = temp->vertex;

        if (DIST[u] == -1 || DIST[u] > DIST[v] + 1) {
            DIST[u] = DIST[v] + 1;

            Go_deep_list(u, list, DIST);
        }

        temp = temp->next;
    }
}

void measure_time(int n, int start, int** a, Node** list, int* DIST) {
    chrono::steady_clock::time_point t1, t2;

    double t_bfs_m = 0;
    double t_bfs_l = 0;
    double t_dfs_m = 0;
    double t_dfs_l = 0;

    reset_dist(DIST, n, start);

    t1 = chrono::steady_clock::now();

    along_the_width(start, n, a, DIST);

    t2 = chrono::steady_clock::now();

    t_bfs_m = chrono::duration<double, milli>(t2 - t1).count();

    result_sink += DIST[n - 1];

    reset_dist(DIST, n, start);

    t1 = chrono::steady_clock::now();

    along_the_width_list(start, list, DIST);

    t2 = chrono::steady_clock::now();

    t_bfs_l = chrono::duration<double, milli>(t2 - t1).count();

    result_sink += DIST[n - 1];

    reset_dist(DIST, n, start);

    t1 = chrono::steady_clock::now();

    Go_deep_matrix(start, n, a, DIST);

    t2 = chrono::steady_clock::now();

    t_dfs_m = chrono::duration<double, milli>(t2 - t1).count();

    result_sink += DIST[n - 1];

    reset_dist(DIST, n, start);

    t1 = chrono::steady_clock::now();

    Go_deep_list(start, list, DIST);

    t2 = chrono::steady_clock::now();

    t_dfs_l = chrono::duration<double, milli>(t2 - t1).count();

    result_sink += DIST[n - 1];

    printf("\nвремя выполнения\n");

    printf("%-30s %.6f мс\n", "BFS по матрице:", t_bfs_m);
    printf("%-30s %.6f мс\n", "BFS по спискам:", t_bfs_l);
    printf("%-30s %.6f мс\n", "DFS по матрице:", t_dfs_m);
    printf("%-30s %.6f мс\n", "DFS по спискам:", t_dfs_l);
}

int main() {
    setlocale(LC_ALL, "Russian");

    srand((unsigned)time(NULL));

    int n, p, start;

    printf("\nВведите количество вершин: ");

    if (scanf("%d", &n) != 1 || n < 1) {
        printf("Некорректное количество вершин!\n");
        return 1;
    }

    printf("Введите вероятность появления ребра (0..100): ");

    if (scanf("%d", &p) != 1 || p < 0 || p > 100) {
        printf("Некорректная вероятность!\n");
        return 1;
    }

    int** a = createMatrix(n);

    generateMatrix(a, n, p);

    printMatrix(a, n);

    Node** list = createLists(a, n);

    printLists(list, n);

    while (1) {
        printf("\nВведите начальную вершину (1..%d): ", n);

        if (scanf("%d", &start) != 1 || start < 1 || start > n) {
            printf("Некорректная начальная вершина!\n");

            freeMatrix(a, n);
            deleteLists(list, n);

            return 1;
        }

        start--;

        if (isIsolated(a, n, start)) {
            printf("Эта вершина изолированная! "
                "С нее нельзя начинать поиск.\n");

            start++;
        }
        else {
            break;
        }
    }

    int* DIST = (int*)malloc(n * sizeof(int));

    if (!DIST) {
        printf("Ошибка выделения памяти\n");

        freeMatrix(a, n);
        deleteLists(list, n);

        exit(1);
    }

    printf("\nBFS по матрице (std::queue)\n");

    reset_dist(DIST, n, start);

    along_the_width(start, n, a, DIST);

    print_dist(DIST, n, start);

    printf("\nBFS по спискам смежности (std::queue)\n");

    reset_dist(DIST, n, start);

    along_the_width_list(start, list, DIST);

    print_dist(DIST, n, start);

    printf("\nDFS по матрице\n");

    reset_dist(DIST, n, start);

    Go_deep_matrix(start, n, a, DIST);

    print_dist(DIST, n, start);

    printf("\nDFS по спискам смежности\n");

    reset_dist(DIST, n, start);

    Go_deep_list(start, list, DIST);

    print_dist(DIST, n, start);

    measure_time(n, start, a, list, DIST);

    free(DIST);

    deleteLists(list, n);

    freeMatrix(a, n);

    return 0;
}
