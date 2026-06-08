#ifndef PROFESOR_H
#define PROFESOR_H

#include "Persona.h"

/**
 * @class Profesor
 * @brief Especialización de Persona para usuarios de tipo Profesor/Investigador.
 *
 * Restricciones de negocio:
 *   - Máximo 10 préstamos simultáneos.
 *   - Periodo máximo de préstamo: 30 días.
 *   - Tarifa de mora diferenciada: $0.50 por día de retraso.
 */
class Profesor : public Persona {
private:
    std::string departamento;
    std::string categoria; // Titular, Asociado, Investigador, etc.

public:
    /**
     * @brief Constructor de Profesor.
     * @param _uid          Identificador único.
     * @param _nombre       Nombre completo.
     * @param _departamento Departamento académico al que pertenece.
     * @param _categoria    Categoría docente o investigadora.
     */
    Profesor(std::string _uid, std::string _nombre,
             std::string _departamento, std::string _categoria = "Titular")
        : Persona(_uid, _nombre, 10),   // Límite fijo: 10 libros
          departamento(_departamento),
          categoria(_categoria) {}

    // ── Getters de atributos propios ────────────────────────────────────────────
    std::string getDepartamento() const { return departamento; }
    std::string getCategoria()    const { return categoria; }

    /**
     * @brief Muestra el perfil completo del profesor en consola.
     */
    void mostrarPerfil() const override {
        std::cout << "[PROFESOR]   UID: "     << uid
                  << " | Nombre: "            << nombre
                  << " | Departamento: "      << departamento
                  << " | Categoría: "         << categoria
                  << " | Préstamos Activos: " << librosPrestadosActuales
                  << "/"                      << maxLibrosPermitidos
                  << std::endl;
    }

    /**
     * @brief Calcula la tarifa de mora para el profesor (tarifa reducida).
     * @param diasRetraso Número de días de retraso en la devolución.
     * @return Monto total de penalidad en USD.
     */
    double calcularTarifaMora(int diasRetraso) const override {
        return (diasRetraso > 0) ? (diasRetraso * 0.50) : 0.0;
    }
};

#endif // PROFESOR_H
