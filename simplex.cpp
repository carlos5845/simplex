#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;

const int MAX = 10;

int numVariables = 2;     // Número de variables reales
int numRestricciones = 2; // Número de restricciones

// Tabla del método simplex
double tabla[MAX][MAX];
int variablesBasicas[MAX]; // Índices de variables básicas

void mostrarTabla()
{
    cout << fixed << setprecision(2);
    cout << "\nTabla Simplex:\n";
    cout << "VB\t";
    for (int j = 0; j < numVariables + numRestricciones; j++)
        cout << "x" << j + 1 << "\t";
    cout << "LD\n";

    for (int i = 0; i <= numRestricciones; i++)
    {
        if (i < numRestricciones)
            cout << "x" << variablesBasicas[i] + 1 << "\t";
        else
            cout << "Z\t";
        for (int j = 0; j <= numVariables + numRestricciones; j++)
            cout << tabla[i][j] << "\t";
        cout << endl;
    }
}

int encontrarColumnaPivote()
{
    int columna = -1;
    double minimo = 0;
    for (int j = 0; j < numVariables + numRestricciones; j++)
    {
        if (tabla[numRestricciones][j] < minimo)
        {
            minimo = tabla[numRestricciones][j];
            columna = j;
        }
    }
    return columna;
}

int encontrarFilaPivote(int columna)
{
    int fila = -1;
    double menorRazon = 1e9;
    for (int i = 0; i < numRestricciones; i++)
    {
        if (tabla[i][columna] > 0)
        {
            double razon = tabla[i][numVariables + numRestricciones] / tabla[i][columna];
            if (razon < menorRazon)
            {
                menorRazon = razon;
                fila = i;
            }
        }
    }
    return fila;
}

void realizarPivoteo(int fila, int columna)
{
    double elementoPivote = tabla[fila][columna];
    for (int j = 0; j <= numVariables + numRestricciones; j++)
        tabla[fila][j] /= elementoPivote;

    for (int i = 0; i <= numRestricciones; i++)
    {
        if (i != fila)
        {
            double factor = tabla[i][columna];
            for (int j = 0; j <= numVariables + numRestricciones; j++)
                tabla[i][j] -= factor * tabla[fila][j];
        }
    }

    variablesBasicas[fila] = columna;
}

void metodoSimplex()
{
    int iteracion = 1;
    while (true)
    {
        cout << "\n--- Iteración " << iteracion++ << " ---";
        mostrarTabla();

        int columnaPivote = encontrarColumnaPivote();
        if (columnaPivote == -1)
        {
            cout << "\nSe ha alcanzado la solución óptima.\n";
            break;
        }

        int filaPivote = encontrarFilaPivote(columnaPivote);
        if (filaPivote == -1)
        {
            cout << "La solución es ilimitada.\n";
            return;
        }

        cout << "Variable entrante: x" << columnaPivote + 1
             << ", Variable saliente: x" << variablesBasicas[filaPivote] + 1 << endl;

        realizarPivoteo(filaPivote, columnaPivote);
    }

    cout << "\nSolución óptima encontrada:\n";
    for (int i = 0; i < numVariables; i++)
    {
        bool encontrada = false;
        for (int j = 0; j < numRestricciones; j++)
        {
            if (variablesBasicas[j] == i)
            {
                cout << "x" << i + 1 << " = " << tabla[j][numVariables + numRestricciones] << endl;
                encontrada = true;
                break;
            }
        }
        if (!encontrada)
            cout << "x" << i + 1 << " = 0" << endl;
    }
    cout << "Valor óptimo de Z = " << tabla[numRestricciones][numVariables + numRestricciones] << endl;
}

int main()
{
    // Problema de ejemplo:
    // Max Z = 3x1 + 2x2
    // sujeto a:
    // x1 + x2       ≤ 4
    // 2x1 + x2      ≤ 5

    // Llenar la tabla con restricciones + variables de holgura
    tabla[0][0] = 1;
    tabla[0][1] = 1;
    tabla[0][2] = 1;
    tabla[0][3] = 0;
    tabla[0][4] = 4;
    tabla[1][0] = 2;
    tabla[1][1] = 1;
    tabla[1][2] = 0;
    tabla[1][3] = 1;
    tabla[1][4] = 5;

    // Función objetivo (Z = -3x1 -2x2 + 0x3 + 0x4)
    tabla[2][0] = -3;
    tabla[2][1] = -2;
    tabla[2][2] = 0;
    tabla[2][3] = 0;
    tabla[2][4] = 0;

    // Variables básicas iniciales (x3 y x4)
    variablesBasicas[0] = 2;
    variablesBasicas[1] = 3;

    metodoSimplex();

    return 0;
}
