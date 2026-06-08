#ifndef PERSONA_H
#define PERSONA_H

#include <string>
#include <iostream>

/**
 * @class Persona
 * @brief Clase abstracta pura que define la interfaz común para todos los usuarios del sistema.
 *
 * Implementa encapsulación estricta con propiedades privadas/protegidas y
 * polimorfismo dinámico mediante métodos virtuales puros.
 */
class Persona {
protected:
    std::string uid;
    std::string nombre;
    int librosPrestadosActuales;
    int maxLibrosPermitidos;

public:
    /**
     * @brief Constructor parametrizado de Persona.
     * @param _uid Identificador único del usuario.
     * @param _nombre Nombre completo del usuario.
     * @param _maxLibros Límite máximo de préstamos simultáneos según rol.
     */
    Persona(std::string _uid, std::string _nombre, int _maxLibros)
        : uid(_uid), nombre(_nombre),
          librosPrestadosActuales(0), maxLibrosPermitidos(_maxLibros) {}

    virtual ~Persona() {} // Destructor virtual obligatorio para evitar memory leaks

    // ── Getters ────────────────────────────────────────────────────────────────
    std::string getUid()    const { return uid; }
    std::string getNombre() const { return nombre; }
    int getLibrosPrestados() const { return librosPrestadosActuales; }
    int getMaxLibros()       const { return maxLibrosPermitidos; }

    /**
     * @brief Intenta registrar un nuevo préstamo para este usuario.
     * @return true si hay cuota disponible, false si el límite fue alcanzado.
     */
    bool registrarNuevoPrestamo() {
        if (librosPrestadosActuales < maxLibrosPermitidos) {
            librosPrestadosActuales++;
            return true;
        }
        return false;
    }

    /**
     * @brief Decrementa el contador de préstamos activos al devolver un material.
     */
    void registrarDevolucion() {
        if (librosPrestadosActuales > 0) librosPrestadosActuales--;
    }

    // ── Métodos virtuales puros (polimorfismo dinámico) ─────────────────────────
    virtual void   mostrarPerfil()                        const = 0;
    virtual double calcularTarifaMora(int diasRetraso)    const = 0;
};

#endif // PERSONA_H
