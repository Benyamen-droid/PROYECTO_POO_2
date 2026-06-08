#ifndef MATERIAL_BIBLIOTECARIO_H
#define MATERIAL_BIBLIOTECARIO_H

#include <string>
#include <iostream>

/**
 * @enum EstadoMaterial
 * @brief Estados transaccionales posibles para un material del catálogo.
 */
enum class EstadoMaterial {
    DISPONIBLE,
    PRESTADO,
    RESERVADO,
    EN_MANTENIMIENTO
};

/**
 * @brief Convierte el enum EstadoMaterial a su representación en cadena.
 */
inline std::string estadoToString(EstadoMaterial e) {
    switch (e) {
        case EstadoMaterial::DISPONIBLE:       return "Disponible";
        case EstadoMaterial::PRESTADO:         return "Prestado";
        case EstadoMaterial::RESERVADO:        return "Reservado";
        case EstadoMaterial::EN_MANTENIMIENTO: return "En Mantenimiento";
        default:                               return "Desconocido";
    }
}

/**
 * @class MaterialBibliotecario
 * @brief Clase abstracta pura que representa cualquier material del inventario.
 *
 * Define la interfaz común para LibroFisico y LibroDigital.
 */
class MaterialBibliotecario {
protected:
    std::string identificador;   // ISBN o UUID
    std::string titulo;
    std::string autor;
    int         anio;
    std::string categoria;
    EstadoMaterial estado;

public:
    /**
     * @brief Constructor parametrizado de MaterialBibliotecario.
     */
    MaterialBibliotecario(std::string _id, std::string _titulo,
                          std::string _autor, int _anio, std::string _categoria)
        : identificador(_id), titulo(_titulo), autor(_autor),
          anio(_anio), categoria(_categoria),
          estado(EstadoMaterial::DISPONIBLE) {}

    virtual ~MaterialBibliotecario() {}

    // ── Getters ─────────────────────────────────────────────────────────────────
    std::string    getIdentificador() const { return identificador; }
    std::string    getTitulo()        const { return titulo; }
    std::string    getAutor()         const { return autor; }
    int            getAnio()          const { return anio; }
    std::string    getCategoria()     const { return categoria; }
    EstadoMaterial getEstado()        const { return estado; }

    // ── Setters controlados ──────────────────────────────────────────────────────
    void setEstado(EstadoMaterial nuevoEstado) { estado = nuevoEstado; }

    bool estaDisponible() const { return estado == EstadoMaterial::DISPONIBLE; }

    // ── Métodos virtuales puros ──────────────────────────────────────────────────
    virtual void mostrarDetalle()     const = 0;
    virtual std::string getTipo()     const = 0;
};

#endif // MATERIAL_BIBLIOTECARIO_H
