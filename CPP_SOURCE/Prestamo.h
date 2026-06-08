#ifndef PRESTAMO_H
#define PRESTAMO_H

#include <string>
#include <iostream>
#include <memory>
#include <ctime>
#include <iomanip>
#include "Persona.h"
#include "MaterialBibliotecario.h"

/**
 * @class Prestamo
 * @brief Entidad de asociación que vincula un Persona con un MaterialBibliotecario.
 *
 * Registra la transacción completa: usuario, material, fechas y penalidades.
 * Agrega a Biblioteca (relación agregación, rombo blanco en UML).
 */
class Prestamo {
private:
    std::string                            idPrestamo;
    std::shared_ptr<Persona>               usuario;
    std::shared_ptr<MaterialBibliotecario> material;
    std::string                            fechaSalida;
    std::string                            fechaDevolucionPactada;
    std::string                            fechaDevolucionReal;
    bool                                   devuelto;
    double                                 penalidad;

    /**
     * @brief Genera una cadena con la fecha actual en formato YYYY-MM-DD.
     */
    static std::string fechaActual() {
        std::time_t t = std::time(nullptr);
        std::tm*    tm_info = std::localtime(&t);
        char buf[11];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_info);
        return std::string(buf);
    }

public:
    /**
     * @brief Constructor de Prestamo.
     * @param _id                    Identificador único del préstamo.
     * @param _usuario               Puntero compartido al usuario.
     * @param _material              Puntero compartido al material prestado.
     * @param _fechaDevolucionPactada Fecha límite de devolución (YYYY-MM-DD).
     */
    Prestamo(std::string _id,
             std::shared_ptr<Persona> _usuario,
             std::shared_ptr<MaterialBibliotecario> _material,
             std::string _fechaDevolucionPactada)
        : idPrestamo(_id), usuario(_usuario), material(_material),
          fechaSalida(fechaActual()),
          fechaDevolucionPactada(_fechaDevolucionPactada),
          fechaDevolucionReal(""),
          devuelto(false), penalidad(0.0)
    {
        // Actualizar estado del material y contador del usuario
        material->setEstado(EstadoMaterial::PRESTADO);
        usuario->registrarNuevoPrestamo();
    }

    // ── Getters ─────────────────────────────────────────────────────────────────
    std::string getIdPrestamo()   const { return idPrestamo; }
    bool        estaDevuelto()    const { return devuelto; }
    double      getPenalidad()    const { return penalidad; }
    std::string getFechaSalida()  const { return fechaSalida; }

    std::shared_ptr<Persona>               getUsuario()  const { return usuario; }
    std::shared_ptr<MaterialBibliotecario> getMaterial() const { return material; }

    /**
     * @brief Registra la devolución del material y calcula penalidad si hay retraso.
     * @param diasRetraso Número de días de retraso (0 si es a tiempo o anticipado).
     */
    void registrarDevolucion(int diasRetraso = 0) {
        if (devuelto) {
            std::cout << "[ADVERTENCIA] El préstamo " << idPrestamo
                      << " ya fue devuelto." << std::endl;
            return;
        }
        devuelto             = true;
        fechaDevolucionReal  = fechaActual();
        penalidad            = usuario->calcularTarifaMora(diasRetraso);
        material->setEstado(EstadoMaterial::DISPONIBLE);
        usuario->registrarDevolucion();
    }

    /**
     * @brief Imprime el resumen completo del préstamo en consola.
     */
    void mostrarResumen() const {
        std::cout << "┌─ PRÉSTAMO ID: " << idPrestamo << " ─────────────────────────────" << std::endl;
        std::cout << "│  Usuario   : " << usuario->getNombre()
                  << " (UID: " << usuario->getUid() << ")" << std::endl;
        std::cout << "│  Material  : " << material->getTitulo()
                  << " [" << material->getTipo() << "]" << std::endl;
        std::cout << "│  Salida    : " << fechaSalida << std::endl;
        std::cout << "│  Vence     : " << fechaDevolucionPactada << std::endl;
        std::cout << "│  Estado    : " << (devuelto ? "DEVUELTO" : "ACTIVO") << std::endl;
        if (devuelto && penalidad > 0.0) {
            std::cout << "│  Penalidad : $" << std::fixed << std::setprecision(2)
                      << penalidad << std::endl;
        }
        std::cout << "└─────────────────────────────────────────────────────" << std::endl;
    }
};

#endif // PRESTAMO_H
