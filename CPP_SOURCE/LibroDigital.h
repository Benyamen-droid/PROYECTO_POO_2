#ifndef LIBRO_DIGITAL_H
#define LIBRO_DIGITAL_H

#include "MaterialBibliotecario.h"

/**
 * @class LibroDigital
 * @brief Representación de un libro digital (e-book) en el inventario.
 *
 * Extiende MaterialBibliotecario con atributos propios de recursos en línea:
 * URL de acceso, tamaño en MB y formato de archivo.
 */
class LibroDigital : public MaterialBibliotecario {
private:
    std::string urlAcceso;
    double      tamanioMB;
    std::string formato;   // PDF, EPUB, MOBI, etc.

public:
    /**
     * @brief Constructor de LibroDigital.
     * @param _id        UUID del recurso digital.
     * @param _titulo    Título del recurso.
     * @param _autor     Autor principal.
     * @param _anio      Año de publicación digital.
     * @param _categoria Categoría temática.
     * @param _url       URL de acceso al recurso.
     * @param _mb        Tamaño del archivo en megabytes.
     * @param _formato   Formato del archivo (PDF, EPUB, etc.).
     */
    LibroDigital(std::string _id, std::string _titulo, std::string _autor,
                 int _anio, std::string _categoria,
                 std::string _url, double _mb, std::string _formato = "PDF")
        : MaterialBibliotecario(_id, _titulo, _autor, _anio, _categoria),
          urlAcceso(_url), tamanioMB(_mb), formato(_formato) {}

    // ── Getters propios ─────────────────────────────────────────────────────────
    std::string getUrlAcceso() const { return urlAcceso; }
    double      getTamanioMB() const { return tamanioMB; }
    std::string getFormato()   const { return formato; }

    /**
     * @brief Implementación polimórfica: muestra el detalle del libro digital.
     */
    void mostrarDetalle() const override {
        std::cout << "[DIGITAL] UUID: "     << identificador
                  << " | Título: "          << titulo
                  << " | Autor: "           << autor
                  << " | Año: "             << anio
                  << " | URL: "             << urlAcceso
                  << " | Tamaño: "          << tamanioMB << " MB"
                  << " | Formato: "         << formato
                  << " | Estado: "          << estadoToString(estado)
                  << std::endl;
    }

    std::string getTipo() const override { return "Libro Digital"; }
};

#endif // LIBRO_DIGITAL_H
