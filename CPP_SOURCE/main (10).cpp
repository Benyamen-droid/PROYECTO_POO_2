/**
 * @file main.cpp
 * @brief Punto de entrada del sistema SmartLib v2.0 (C++17).
 *
 * Ejecuta un flujo completo de transacciones simuladas demostrando:
 *   - Instanciación polimórfica mediante colecciones base
 *   - Gestión de usuarios y catálogo con smart pointers
 *   - Ciclo préstamo → devolución → penalidad
 *   - Búsquedas multicitario e impresión de auditoría
 *
 * Compilación:
 *   g++ -std=c++17 -Wall -Wextra -o smartlib main.cpp
 */

#include <iostream>
#include <memory>
#include <vector>
#include "Biblioteca.h"
#include "Estudiante.h"
#include "Profesor.h"
#include "LibroFisico.h"
#include "LibroDigital.h"

// ── Utilidad: banner de sección ─────────────────────────────────────────────
void banner(const std::string& titulo) {
    std::cout << "\n╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << titulo;
    int pad = 52 - static_cast<int>(titulo.size());
    for (int i = 0; i < pad; i++) std::cout << ' ';
    std::cout << "║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║         SMARTLIB v2.0 — Sistema de Biblioteca        ║\n";
    std::cout << "║         Universidad Tecnológica  |  C++17            ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n";

    // ══════════════════════════════════════════════════════════════════════════
    //  1. Crear la instancia principal de la biblioteca
    // ══════════════════════════════════════════════════════════════════════════
    Biblioteca smartLib("Biblioteca Central de Ingeniería");

    // ══════════════════════════════════════════════════════════════════════════
    //  2. Instanciación polimórfica de usuarios via colección base Persona
    // ══════════════════════════════════════════════════════════════════════════
    banner("FASE 1: REGISTRO DE USUARIOS");

    // Vector de punteros base — despacho polimórfico en tiempo de ejecución
    std::vector<std::shared_ptr<Persona>> usuarios = {
        std::make_shared<Estudiante>("E001", "Carlos Mendoza",    "20241045", "Ingeniería de Software"),
        std::make_shared<Estudiante>("E002", "Ana Torres",        "20230876", "Sistemas de Información"),
        std::make_shared<Profesor> ("P001", "Dr. Arthur Dynamic", "Ciencias de la Computación", "Titular"),
        std::make_shared<Profesor> ("P002", "Dra. Laura Rivas",   "Ingeniería Electrónica",      "Asociado"),
    };

    for (auto& u : usuarios)
        smartLib.registrarUsuario(u);

    // Iteración polimórfica sobre la colección base
    smartLib.desplegarUsuarios();

    // ══════════════════════════════════════════════════════════════════════════
    //  3. Poblar el catálogo con smart pointers
    // ══════════════════════════════════════════════════════════════════════════
    banner("FASE 2: CARGA DEL CATÁLOGO");

    // Colección polimórfica de materiales
    std::vector<std::shared_ptr<MaterialBibliotecario>> materiales = {
        std::make_shared<LibroFisico> ("978-0201633610", "Design Patterns",
                                       "GoF", 1994, "Ingeniería de Software",
                                       "A3", "E12", 2),
        std::make_shared<LibroFisico> ("978-0132350884", "Clean Code",
                                       "Robert C. Martin", 2008, "Programación",
                                       "A3", "E15", 3),
        std::make_shared<LibroDigital>("UUID-CPP17-001", "C++ Primer (6th ed.)",
                                       "Stanley Lippman", 2023, "Programación C++",
                                       "https://lib.uni.edu/cpp-primer", 48.5, "PDF"),
        std::make_shared<LibroDigital>("UUID-PY-002",    "Python Object-Oriented Programming",
                                       "Dusty Phillips", 2018, "Python",
                                       "https://lib.uni.edu/py-oop", 32.0, "EPUB"),
        std::make_shared<LibroFisico> ("978-0134494166", "Clean Architecture",
                                       "Robert C. Martin", 2017, "Arquitectura Software",
                                       "B1", "E03", 1),
    };

    for (auto& m : materiales)
        smartLib.agregarMaterial(m);

    // Mostrar catálogo completo con despacho polimórfico
    smartLib.mostrarCatalogo();

    // ══════════════════════════════════════════════════════════════════════════
    //  4. Ciclo de préstamos (incluye verificación de disponibilidad y cuota)
    // ══════════════════════════════════════════════════════════════════════════
    banner("FASE 3: TRANSACCIONES DE PRÉSTAMO");

    smartLib.realizarPrestamo("E001", "978-0201633610", "2026-06-14");  // OK
    smartLib.realizarPrestamo("E001", "978-0132350884", "2026-06-14");  // OK
    smartLib.realizarPrestamo("E001", "978-0134494166", "2026-06-14");  // OK (límite 3)
    smartLib.realizarPrestamo("E001", "UUID-CPP17-001", "2026-06-14");  // FALLA: límite alcanzado
    smartLib.realizarPrestamo("P001", "UUID-CPP17-001", "2026-07-07");  // OK Profesor
    smartLib.realizarPrestamo("P001", "UUID-PY-002",    "2026-07-07");  // OK Profesor
    smartLib.realizarPrestamo("E002", "978-0201633610", "2026-06-14");  // FALLA: ya prestado

    // Mostrar estado actualizado de usuarios
    smartLib.desplegarUsuarios();

    // ══════════════════════════════════════════════════════════════════════════
    //  5. Devoluciones con cálculo polimórfico de penalidades
    // ══════════════════════════════════════════════════════════════════════════
    banner("FASE 4: DEVOLUCIONES Y PENALIDADES");

    smartLib.registrarDevolucion("PRE-1", 0);   // A tiempo: sin penalidad
    smartLib.registrarDevolucion("PRE-2", 5);   // 5 días tarde: Estudiante → $7.50
    smartLib.registrarDevolucion("PRE-5", 3);   // 3 días tarde: Profesor  → $1.50
    smartLib.registrarDevolucion("PRE-1", 0);   // Intento de devolución duplicada

    // ══════════════════════════════════════════════════════════════════════════
    //  6. Motor de consultas (búsquedas multicitario)
    // ══════════════════════════════════════════════════════════════════════════
    banner("FASE 5: BÚSQUEDAS EN CATÁLOGO");

    smartLib.buscarMaterial("Martin");       // Por autor
    smartLib.buscarMaterial("Python");       // Por título/categoría
    smartLib.buscarMaterial("Arquitectura"); // Por categoría

    // ══════════════════════════════════════════════════════════════════════════
    //  7. Reporte de auditoría (consola + archivo)
    // ══════════════════════════════════════════════════════════════════════════
    banner("FASE 6: REPORTE DE AUDITORÍA");

    smartLib.generarReporteAuditoria("auditoria_smartlib.txt");

    // ══════════════════════════════════════════════════════════════════════════
    //  8. Demostración explícita de polimorfismo con punteros base
    // ══════════════════════════════════════════════════════════════════════════
    banner("FASE 7: DEMOSTRACIÓN DE POLIMORFISMO DINÁMICO");

    std::cout << "\n-> Iterando colección base Persona* con despacho virtual:\n";
    for (const auto& u : usuarios) {
        // mostrarPerfil() y calcularTarifaMora() son despachados dinámicamente
        u->mostrarPerfil();
        std::cout << "   Mora (10 días): $"
                  << std::fixed << std::setprecision(2)
                  << u->calcularTarifaMora(10) << std::endl;
    }

    std::cout << "\n-> Iterando colección base MaterialBibliotecario* con despacho virtual:\n";
    for (const auto& m : materiales) {
        m->mostrarDetalle();   // LibroFisico::mostrarDetalle() o LibroDigital::mostrarDetalle()
    }

    std::cout << "\n✓ SmartLib v2.0 — Ejecución completada sin errores.\n" << std::endl;
    return 0;
}
