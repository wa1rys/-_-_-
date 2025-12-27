#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <cmath>
#include <sstream>

using namespace std;

const double INDIVIDUAL_RATE = 0.13;      
const double ENTREPRENEUR_RATE = 0.06;    
const double COMPANY_RATE = 0.20;        
const double BENEFIT_DISCOUNT = 0.90;     
const double ALLOWED_DEVIATION = 1.0;     

bool read_double(double& value) {
    string input;
    getline(cin, input);
    
    for (char& c : input) {
        if (c == ',') c = '.';
    }
    
    stringstream ss(input);
    return (ss >> value) && (ss.eof());
}

double get_validated_input(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        
        if (!read_double(value)) {
            cout << "Ошибка: введено не число. Попробуйте снова.\n";
        }
        else if (value < 0) {
            cout << "Ошибка: значение не может быть отрицательным. Попробуйте снова.\n";
        }
        else if (value == 0) {
            cout << "Ошибка: значение должно быть положительным. Попробуйте снова.\n";
        }
        else {
            return value;
        }
    }
}

int get_category_input() {
    int category;
    while (true) {
        cout << "Категория налогоплательщика (1-ФЛ, 2-ИП, 3-ЮЛ): ";
        string input;
        getline(cin, input);
        stringstream ss(input);
        
        if (ss >> category && category >= 1 && category <= 3) {
            return category;
        } else {
            cout << "Ошибка: введите 1, 2 или 3.\n";
        }
    }
}

bool get_benefits_input() {
    char choice;
    while (true) {
        cout << "Наличие льгот (y/n): ";
        string input;
        getline(cin, input);
        
        if (!input.empty()) {
            choice = tolower(input[0]);
            if (choice == 'y' || choice == 'н') { 
                return true;
            } else if (choice == 'n' || choice == 'т') { 
                return false;
            }
        }
        cout << "Ошибка: введите y (да) или n (нет).\n";
    }
}

double calculate_expected_tax(double income, int category, bool has_benefits) {
    double rate;
    
    switch (category) {
        case 1: rate = INDIVIDUAL_RATE; break;
        case 2: rate = ENTREPRENEUR_RATE; break;
        case 3: rate = COMPANY_RATE; break;
        default: rate = 0.0;
    }
    
    if (has_benefits) {
        rate *= BENEFIT_DISCOUNT;
    }
    
    return income * rate;
}

double round_to_kopecks(double amount) {
    return round(amount * 100.0) / 100.0;
}

void run_declaration_check() {
    cout << "\n=== ПРОВЕРКА ДЕКЛАРАЦИИ О ДОХОДАХ ===\n";
    
    double income = get_validated_input("Введите общий доход (руб.): ");
    double tax_paid = get_validated_input("Введите уплаченный налог (руб.): ");
    int category = get_category_input();
    bool has_benefits = get_benefits_input();
    
    double expected_tax = calculate_expected_tax(income, category, has_benefits);
    double rounded_tax = round_to_kopecks(expected_tax);
    double deviation = (tax_paid - rounded_tax) / rounded_tax * 100.0;
    
    cout << fixed << setprecision(2);
    cout << "\n--- РЕЗУЛЬТАТЫ ПРОВЕРКИ ---\n";
    cout << "Введенные данные:\n";
    cout << "  Доход: " << income << " руб.\n";
    cout << "  Уплаченный налог: " << tax_paid << " руб.\n";
    cout << "  Категория: ";
    switch (category) {
        case 1: cout << "Физическое лицо\n"; break;
        case 2: cout << "Индивидуальный предприниматель\n"; break;
        case 3: cout << "Юридическое лицо\n"; break;
    }
    cout << "  Льготы: " << (has_benefits ? "есть" : "нет") << "\n";
    
    cout << "\nРасчетные значения:\n";
    cout << "  Ожидаемый налог: " << rounded_tax << " руб.\n";
    cout << "  Отклонение: " << deviation << "%\n";
    
    cout << "\nСтатус проверки: ";
    if (fabs(deviation) <= ALLOWED_DEVIATION) {
        cout << "ПРОВЕРКА ПРОЙДЕНА\n";
        cout << "Уплаченный налог соответствует расчетному с допустимым отклонением.\n";
    } else {
        cout << "ПРОВЕРКА НЕ ПРОЙДЕНА\n";
        if (deviation < 0) {
            cout << "Выявлено нарушение: НЕДОСТАТОЧНАЯ УПЛАТА налога.\n";
            cout << "Недостающая сумма: " << (rounded_tax - tax_paid) << " руб.\n";
        } else {
            cout << "Выявлено нарушение: ИЗЛИШНЯЯ УПЛАТА налога.\n";
            cout << "Излишне уплаченная сумма: " << (tax_paid - rounded_tax) << " руб.\n";
        }
    }
    cout << "=================================\n\n";
}

void run_tests() {
    cout << "\n=== АВТОМАТИЧЕСКОЕ ТЕСТИРОВАНИЕ ===\n";
    
    struct TestCase {
        double income;
        double tax_paid;
        int category;
        bool benefits;
        string description;
        bool expected_pass;
    };
    
    TestCase tests[] = {
        {500000.0, 65000.0, 1, false, "ФЛ без льгот, корректная уплата", true},
        {1000000.0, 117000.0, 1, true, "ФЛ с льготами, корректная уплата", true},
        {500000.0, 27000.0, 2, false, "ИП без льгот, корректная уплата", true},
        {1000000.0, 108000.0, 2, true, "ИП с льготами, корректная уплата", true},
        {2000000.0, 400000.0, 3, false, "ЮЛ без льгот, корректная уплата", true},
        {1000000.0, 180000.0, 3, true, "ЮЛ с льготами, корректная уплата", true},
        {1000000.0, 100000.0, 3, false, "ЮЛ, недостаточная уплата", false},
        {500000.0, 70000.0, 1, false, "ФЛ, излишняя уплата", false},
        {100000.0, 13130.0, 1, false, "Граничный случай (+1% отклонение)", true},
        {100000.0, 12870.0, 1, false, "Граничный случай (-1% отклонение)", true}
    };
    
    int total_tests = sizeof(tests) / sizeof(tests[0]);
    int passed_tests = 0;
    
    cout << fixed << setprecision(2);
    
    for (int i = 0; i < total_tests; i++) {
        cout << "\nТест " << (i + 1) << ": " << tests[i].description << "\n";
        
        double expected_tax = calculate_expected_tax(
            tests[i].income, 
            tests[i].category, 
            tests[i].benefits
        );
        double rounded_tax = round_to_kopecks(expected_tax);
        double deviation = (tests[i].tax_paid - rounded_tax) / rounded_tax * 100.0;
        
        bool actual_pass = (fabs(deviation) <= ALLOWED_DEVIATION);
        bool test_passed = (actual_pass == tests[i].expected_pass);
        
        cout << "  Вход: доход=" << tests[i].income 
             << ", налог=" << tests[i].tax_paid
             << ", кат." << tests[i].category
             << ", льготы=" << (tests[i].benefits ? "да" : "нет") << "\n";
        cout << "  Ожидаемый налог: " << rounded_tax << " руб.\n";
        cout << "  Отклонение: " << deviation << "%\n";
        cout << "  Ожидаемый результат: " 
             << (tests[i].expected_pass ? "ПРОЙДЕНА" : "НЕ ПРОЙДЕНА") << "\n";
        cout << "  Фактический результат: " 
             << (actual_pass ? "ПРОЙДЕНА" : "НЕ ПРОЙДЕНА") << "\n";
        cout << "  Статус теста: " 
             << (test_passed ? "[ПРОЙДЕН]" : "[ПРОВАЛЕН]") << "\n";
        
        if (test_passed) {
            passed_tests++;
        }
    }
    
    cout << "\n=== ИТОГ ТЕСТИРОВАНИЯ ===\n";
    cout << "Всего тестов: " << total_tests << "\n";
    cout << "Пройдено: " << passed_tests << "\n";
    cout << "Провалено: " << (total_tests - passed_tests) << "\n";
    cout << "Успешность: " 
         << fixed << setprecision(1) 
         << (passed_tests * 100.0 / total_tests) << "%\n\n";
}

void show_menu() {
    cout << "\n================================\n";
    cout << "  ПРОВЕРКА ДЕКЛАРАЦИЙ О ДОХОДАХ\n";
    cout << "================================\n";
    cout << "1. Проверить декларацию\n";
    cout << "2. Запустить тестирование\n";
    cout << "3. Выход\n";
    cout << "================================\n";
    cout << "Выберите пункт меню (1-3): ";
}

int main() {
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif
    
    setlocale(LC_ALL, "Russian");
    
    cout << "Добро пожаловать в программу проверки деклараций о доходах!\n";
    cout << "Программа проверяет соответствие уплаченных налогов расчетным суммам.\n";
    cout << "Используйте точку (.) как десятичный разделитель.\n\n";
    
    bool running = true;
    
    while (running) {
        show_menu();
        
        string input;
        getline(cin, input);
        stringstream ss(input);
        
        int choice;
        if (!(ss >> choice)) {
            cout << "\nОшибка: введите число от 1 до 3.\n\n";
            continue;
        }
        
        switch (choice) {
            case 1:
                run_declaration_check();
                break;
                
            case 2:
                run_tests();
                break;
                
            case 3:
                cout << "\nВыход из программы. До свидания!\n";
                running = false;
                break;
                
            default:
                cout << "\nОшибка: неверный выбор. Введите 1, 2 или 3.\n\n";
        }
    }
    
    return 0;
}