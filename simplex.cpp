#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cmath>

using namespace std;

class SimplexMaximization
{
private:
    vector<vector<double>> tabla;
    vector<string> variables_base;
    vector<string> variables_no_base;
    int num_variables;
    int num_restricciones;

public:
    SimplexMaximization(int vars, int restricciones)
        : num_variables(vars), num_restricciones(restricciones)
    {
        // Inicializar tabla simplex
        // Filas: restricciones + fila Z
        // Columnas: variables originales + variables de holgura + RHS
        tabla.resize(restricciones + 1, vector<double>(vars + restricciones + 1, 0));

        // Inicializar nombres de variables
        for (int i = 0; i < vars; i++)
        {
            variables_no_base.push_back("x" + to_string(i + 1));
        }
        for (int i = 0; i < restricciones; i++)
        {
            variables_base.push_back("s" + to_string(i + 1));
        }
    }

    void ingresarFuncionObjetivo()
    {
        cout << "\n=== INGRESO DE FUNCIÓN OBJETIVO ===" << endl;
        cout << "Ingrese los coeficientes de la función objetivo a MAXIMIZAR:" << endl;

        for (int j = 0; j < num_variables; j++)
        {
            cout << "Coeficiente de x" << (j + 1) << ": ";
            cin >> tabla[num_restricciones][j];
            // En simplex para maximización, los coeficientes van negativos en la fila Z
            tabla[num_restricciones][j] = -tabla[num_restricciones][j];
        }
    }

    void ingresarRestricciones()
    {
        cout << "\n=== INGRESO DE RESTRICCIONES ===" << endl;
        cout << "Ingrese las restricciones en la forma: ax1 + bx2 + ... <= c" << endl;

        for (int i = 0; i < num_restricciones; i++)
        {
            cout << "\nRestricción " << (i + 1) << ":" << endl;

            // Coeficientes de las variables originales
            for (int j = 0; j < num_variables; j++)
            {
                cout << "Coeficiente de x" << (j + 1) << ": ";
                cin >> tabla[i][j];
            }

            // Variable de holgura (coeficiente = 1 en su propia restricción)
            tabla[i][num_variables + i] = 1;

            // Lado derecho de la restricción
            cout << "Valor del lado derecho (<=): ";
            cin >> tabla[i][num_variables + num_restricciones];
        }
    }

    void mostrarTabla()
    {
        cout << "\n=== TABLA SIMPLEX ===" << endl;

        // Encabezados
        cout << setw(8) << "Base";
        for (int j = 0; j < num_variables; j++)
        {
            cout << setw(10) << ("x" + to_string(j + 1));
        }
        for (int j = 0; j < num_restricciones; j++)
        {
            cout << setw(10) << ("s" + to_string(j + 1));
        }
        cout << setw(10) << "RHS" << endl;

        cout << string(80, '-') << endl;

        // Filas de restricciones
        for (int i = 0; i < num_restricciones; i++)
        {
            cout << setw(8) << variables_base[i];
            for (int j = 0; j < num_variables + num_restricciones + 1; j++)
            {
                cout << setw(10) << fixed << setprecision(2) << tabla[i][j];
            }
            cout << endl;
        }

        // Fila Z
        cout << setw(8) << "Z";
        for (int j = 0; j < num_variables + num_restricciones + 1; j++)
        {
            cout << setw(10) << fixed << setprecision(2) << tabla[num_restricciones][j];
        }
        cout << endl
             << endl;
    }

    int encontrarColumnaEntrante()
    {
        int col = -1;
        double min_valor = 0;

        // Buscar el coeficiente más negativo en la fila Z (excluyendo RHS)
        for (int j = 0; j < num_variables + num_restricciones; j++)
        {
            if (tabla[num_restricciones][j] < min_valor)
            {
                min_valor = tabla[num_restricciones][j];
                col = j;
            }
        }

        return col;
    }

    int encontrarFilaSaliente(int col_entrante)
    {
        int fila = -1;
        double min_ratio = 1e9;

        for (int i = 0; i < num_restricciones; i++)
        {
            if (tabla[i][col_entrante] > 0)
            {
                double ratio = tabla[i][num_variables + num_restricciones] / tabla[i][col_entrante];
                if (ratio >= 0 && ratio < min_ratio)
                {
                    min_ratio = ratio;
                    fila = i;
                }
            }
        }

        return fila;
    }

    void pivotear(int fila_pivot, int col_pivot)
    {
        double pivot = tabla[fila_pivot][col_pivot];

        // Normalizar fila pivot
        for (int j = 0; j <= num_variables + num_restricciones; j++)
        {
            tabla[fila_pivot][j] /= pivot;
        }

        // Eliminar en otras filas
        for (int i = 0; i <= num_restricciones; i++)
        {
            if (i != fila_pivot)
            {
                double factor = tabla[i][col_pivot];
                for (int j = 0; j <= num_variables + num_restricciones; j++)
                {
                    tabla[i][j] -= factor * tabla[fila_pivot][j];
                }
            }
        }

        // Actualizar variable base
        if (col_pivot < num_variables)
        {
            variables_base[fila_pivot] = "x" + to_string(col_pivot + 1);
        }
        else
        {
            variables_base[fila_pivot] = "s" + to_string(col_pivot - num_variables + 1);
        }
    }

    bool esOptima()
    {
        // Verificar si todos los coeficientes en la fila Z son >= 0
        for (int j = 0; j < num_variables + num_restricciones; j++)
        {
            if (tabla[num_restricciones][j] < -1e-10)
            {
                return false;
            }
        }
        return true;
    }

    void resolver()
    {
        cout << "\n=== RESOLUCIÓN POR MÉTODO SIMPLEX ===" << endl;

        mostrarTabla();

        int iteracion = 1;

        while (!esOptima())
        {
            cout << "ITERACIÓN " << iteracion << ":" << endl;

            // Encontrar variable entrante
            int col_entrante = encontrarColumnaEntrante();
            if (col_entrante == -1)
            {
                cout << "Error: No se encontró variable entrante." << endl;
                return;
            }

            string var_entrante;
            if (col_entrante < num_variables)
            {
                var_entrante = "x" + to_string(col_entrante + 1);
            }
            else
            {
                var_entrante = "s" + to_string(col_entrante - num_variables + 1);
            }

            // Encontrar variable saliente
            int fila_saliente = encontrarFilaSaliente(col_entrante);
            if (fila_saliente == -1)
            {
                cout << "El problema es no acotado (solución infinita)." << endl;
                return;
            }

            string var_saliente = variables_base[fila_saliente];

            cout << "Variable entrante: " << var_entrante << " (columna " << col_entrante + 1 << ")" << endl;
            cout << "Variable saliente: " << var_saliente << " (fila " << fila_saliente + 1 << ")" << endl;
            cout << "Elemento pivot: " << tabla[fila_saliente][col_entrante] << endl;

            // Pivotear
            pivotear(fila_saliente, col_entrante);

            cout << "\nTabla después del pivoteo:" << endl;
            mostrarTabla();

            iteracion++;
        }

        cout << "¡SOLUCIÓN ÓPTIMA ENCONTRADA!" << endl;
        mostrarSolucion();
    }

    void mostrarSolucion()
    {
        cout << "\n=== SOLUCIÓN ÓPTIMA ===" << endl;

        // Valor óptimo de la función objetivo
        double valor_optimo = tabla[num_restricciones][num_variables + num_restricciones];
        cout << "Valor máximo de Z = " << valor_optimo << endl;

        cout << "\nValores de las variables:" << endl;

        // Variables básicas
        for (int i = 0; i < num_restricciones; i++)
        {
            cout << variables_base[i] << " = "
                 << tabla[i][num_variables + num_restricciones] << endl;
        }

        // Variables no básicas (valor = 0)
        vector<string> todas_variables;
        for (int i = 0; i < num_variables; i++)
        {
            todas_variables.push_back("x" + to_string(i + 1));
        }
        for (int i = 0; i < num_restricciones; i++)
        {
            todas_variables.push_back("s" + to_string(i + 1));
        }

        cout << "\nVariables no básicas (valor = 0):" << endl;
        for (const string &var : todas_variables)
        {
            bool es_basica = false;
            for (const string &var_base : variables_base)
            {
                if (var == var_base)
                {
                    es_basica = true;
                    break;
                }
            }
            if (!es_basica)
            {
                cout << var << " = 0" << endl;
            }
        }
    }
};

int main()
{
    cout << "=== MÉTODO SIMPLEX PARA MAXIMIZACIÓN ===" << endl;

    int num_variables, num_restricciones;

    cout << "\nIngrese el número de variables: ";
    cin >> num_variables;

    cout << "Ingrese el número de restricciones: ";
    cin >> num_restricciones;

    SimplexMaximization simplex(num_variables, num_restricciones);

    simplex.ingresarFuncionObjetivo();
    simplex.ingresarRestricciones();

    cout << "\n=== TABLA INICIAL ===" << endl;
    simplex.mostrarTabla();

    simplex.resolver();

    cout << "\n¡Programa finalizado!" << endl;

    return 0;
}