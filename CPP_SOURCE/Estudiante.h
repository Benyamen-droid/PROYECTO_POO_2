#ifndef ESTUDIANTE_H
#define ESTUDIANTE_H

#include "Persona.h"

/**
 * @class Estudiante
 * @brief Especialización de Persona para usuarios de tipo Estudiante.
 *
 * Restricciones de negocio:
 *   - Máximo 3 préstamos simultáneos.
 *   - Periodo máximo de préstamo: 7 días.
 *   - Tarifa de mora: $1.50 por día de retraso.
 */
class Estudiante : public Persona {
private:
    std::string codigoEstudiante;
    std::string carrera;

public:
    /**
     * @brief Constructor de Estudiante.
     * @param _uid             Identificador único.
     * @param _nombre          Nombre completo.
     * @param _codigoEstudiante Código de matrícula institucional.
     * @param _carrera         Programa académico al que pertenece.
     */
    Estudiante(std::string _uid, std::string _nombre,
               std::string _codigoEstudiante, std::string _carrera)
        : Persona(_uid, _nombre, 3),          // Límite fijo: 3 libros
          codigoEstudiante(_codigoEstudiante),
          carrera(_carrera) {}

    // ── Getters de atributos propios ────────────────────────────────────────────
    std::string getCodigoEstudiante() const { return codigoEstudiante; }
    std::string getCarrera()          const { return carrera; }

    /**
     * @brief Muestra el perfil completo del estudiante en consola.
     */
    void mostrarPerfil() const override {
        std::cout << "[ESTUDIANTE] UID: "     << uid
                  << " | Nombre: "            << nombre
                  << " | Código: "            << codigoEstudiante
                  << " | Carrera: "           << carrera
                  << " | Préstamos Activos: " << librosPrestadosActuales
                  << "/"                      << maxLibrosPermitidos
                  << std::endl;
    }

    /**
     * @brief Calcula la tarifa de mora para el estudiante.
     * @param diasRetraso Número de días de retraso en la devolución.
     * @return Monto total de penalidad en USD.
     */
    double calcularTarifaMora(int diasRetraso) const override {
        return (diasRetraso > 0) ? (diasRetraso * 1.50) : 0.0;
    }
};

#endif // ESTUDIANTE_H
