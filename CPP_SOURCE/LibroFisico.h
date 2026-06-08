#ifndef LIBRO_FISICO_H
#define LIBRO_FISICO_H

#include "MaterialBibliotecario.h"

/**
 * @class LibroFisico
 * @brief Representación de un libro físico en el inventario de la biblioteca.
 *
 * Extiende MaterialBibliotecario con atributos de ubicación física
 * (pasillo y estante) propios del inventario tangible.
 */
class LibroFisico : public MaterialBibliotecario {
private:
    std::string pasillo;
    std::string estante;
    int         numeroCopias;

public:
    /**
     * @brief Constructor de LibroFisico.
     * @param _id           ISBN del libro.
     * @param _titulo       Título del libro.
     * @param _autor        Autor principal.
     * @param _anio         Año de publicación.
     * @param _categoria    Categoría temática.
     * @param _pasillo      Pasillo de ubicación en la biblioteca.
     * @param _estante      Estante dentro del pasillo.
     * @param _copias       Número de copias físicas disponibles.
     */
    LibroFisico(std::string _id, std::string _titulo, std::string _autor,
                int _anio, std::string _categoria,
                std::string _pasillo, std::string _estante, int _copias = 1)
        : MaterialBibliotecario(_id, _titulo, _autor, _anio, _categoria),
          pasillo(_pasillo), estante(_estante), numeroCopias(_copias) {}

    // ── Getters propios ─────────────────────────────────────────────────────────
    std::string getPasillo()     const { return pasillo; }
    std::string getEstante()     const { return estante; }
    int         getNumeroCopias()const { return numeroCopias; }

    /**
     * @brief Implementación polimórfica: muestra el detalle del libro físico.
     */
    void mostrarDetalle() const override {
        std::cout << "[FÍSICO]  ISBN: "     << identificador
                  << " | Título: "          << titulo
                  << " | Autor: "           << autor
                  << " | Año: "             << anio
                  << " | Pasillo: "         << pasillo
                  << " | Estante: "         << estante
                  << " | Copias: "          << numeroCopias
                  << " | Estado: "          << estadoToString(estado)
                  << std::endl;
    }

    std::string getTipo() const override { return "Libro Físico"; }
};

#endif // LIBRO_FISICO_H
