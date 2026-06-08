#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "Persona.h"
#include "MaterialBibliotecario.h"
#include "Prestamo.h"

/**
 * @class Biblioteca
 * @brief Clase contenedora principal del sistema SmartLib v2.0.
 *
 * Relaciones UML:
 *   - Composición (◆) con MaterialBibliotecario (1..*) y Persona (1..*)
 *   - Agregación  (◇) con Prestamo (1..*)
 *
 * Utiliza std::shared_ptr y std::vector para gestión segura de memoria (RAII).
 */
class Biblioteca {
private:
    std::string nombreInstitucion;
    std::vector<std::shared_ptr<Persona>>               listaUsuarios;
    std::vector<std::shared_ptr<MaterialBibliotecario>> catalogo;
    std::vector<std::shared_ptr<Prestamo>>              historialPrestamos;
    int contadorPrestamos;

public:
    // ── Constructor ─────────────────────────────────────────────────────────────
    explicit Biblioteca(std::string _nombre)
        : nombreInstitucion(std::move(_nombre)), contadorPrestamos(0) {}

    // ══════════════════════════════════════════════════════════════════════════
    //  GESTIÓN DE USUARIOS
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * @brief Registra un nuevo usuario verificando que el UID no esté duplicado.
     * @param usuario  Smart pointer al usuario a registrar.
     */
    void registrarUsuario(std::shared_ptr<Persona> usuario) {
        auto it = std::find_if(listaUsuarios.begin(), listaUsuarios.end(),
            [&](const std::shared_ptr<Persona>& u){ return u->getUid() == usuario->getUid(); });
        if (it != listaUsuarios.end()) {
            std::cout << "[ERROR] UID " << usuario->getUid() << " ya registrado." << std::endl;
            return;
        }
        listaUsuarios.push_back(usuario);
        std::cout << "-> Usuario '" << usuario->getNombre()
                  << "' registrado en " << nombreInstitucion << "." << std::endl;
    }

    /**
     * @brief Busca un usuario por su UID.
     * @return Puntero al usuario o nullptr si no existe.
     */
    std::shared_ptr<Persona> buscarUsuarioPorUid(const std::string& uid) const {
        auto it = std::find_if(listaUsuarios.begin(), listaUsuarios.end(),
            [&](const std::shared_ptr<Persona>& u){ return u->getUid() == uid; });
        return (it != listaUsuarios.end()) ? *it : nullptr;
    }

    /**
     * @brief Despliega el reporte de todos los usuarios registrados (polimorfismo dinámico).
     */
    void desplegarUsuarios() const {
        std::cout << "\n=== REPORTE DE USUARIOS - " << nombreInstitucion << " ===" << std::endl;
        for (const auto& usr : listaUsuarios) {
            usr->mostrarPerfil();  // Despacho polimórfico en tiempo de ejecución
        }
    }

    // ══════════════════════════════════════════════════════════════════════════
    //  GESTIÓN DE CATÁLOGO
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * @brief Agrega un material al catálogo verificando ISBN/UUID único.
     */
    void agregarMaterial(std::shared_ptr<MaterialBibliotecario> material) {
        auto it = std::find_if(catalogo.begin(), catalogo.end(),
            [&](const std::shared_ptr<MaterialBibliotecario>& m){
                return m->getIdentificador() == material->getIdentificador(); });
        if (it != catalogo.end()) {
            std::cout << "[ERROR] Identificador " << material->getIdentificador()
                      << " ya existe en el catálogo." << std::endl;
            return;
        }
        catalogo.push_back(material);
        std::cout << "-> Material '" << material->getTitulo()
                  << "' agregado al catálogo." << std::endl;
    }

    /**
     * @brief Busca materiales por título, autor o categoría (criterios múltiples).
     * @param criterio Cadena de búsqueda (case-insensitive parcial).
     */
    void buscarMaterial(const std::string& criterio) const {
        std::cout << "\n=== BÚSQUEDA: \"" << criterio << "\" ===" << std::endl;
        bool encontrado = false;
        std::string critLower = criterio;
        std::transform(critLower.begin(), critLower.end(), critLower.begin(), ::tolower);

        for (const auto& mat : catalogo) {
            std::string titulo    = mat->getTitulo();
            std::string autor     = mat->getAutor();
            std::string categoria = mat->getCategoria();

            std::transform(titulo.begin(),    titulo.end(),    titulo.begin(),    ::tolower);
            std::transform(autor.begin(),     autor.end(),     autor.begin(),     ::tolower);
            std::transform(categoria.begin(), categoria.end(), categoria.begin(), ::tolower);

            if (titulo.find(critLower) != std::string::npos ||
                autor.find(critLower)  != std::string::npos ||
                categoria.find(critLower) != std::string::npos) {
                mat->mostrarDetalle();
                encontrado = true;
            }
        }
        if (!encontrado)
            std::cout << "No se encontraron resultados para \"" << criterio << "\"." << std::endl;
    }

    /**
     * @brief Lista todos los materiales del catálogo con despacho polimórfico.
     */
    void mostrarCatalogo() const {
        std::cout << "\n=== CATÁLOGO COMPLETO - " << nombreInstitucion << " ===" << std::endl;
        for (const auto& mat : catalogo) {
            mat->mostrarDetalle();   // Polimorfismo: LibroFisico o LibroDigital
        }
    }

    // ══════════════════════════════════════════════════════════════════════════
    //  GESTIÓN DE PRÉSTAMOS
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * @brief Realiza un préstamo verificando cuota y disponibilidad del material.
     * @param uidUsuario       UID del usuario solicitante.
     * @param idMaterial       ISBN/UUID del material solicitado.
     * @param fechaDevolucion  Fecha pactada de devolución (YYYY-MM-DD).
     * @return true si el préstamo fue registrado exitosamente.
     */
    bool realizarPrestamo(const std::string& uidUsuario,
                          const std::string& idMaterial,
                          const std::string& fechaDevolucion) {
        // Consultar disponibilidad (<<include>> en diagrama de casos de uso)
        auto usuario  = buscarUsuarioPorUid(uidUsuario);
        if (!usuario) {
            std::cout << "[ERROR] Usuario UID=" << uidUsuario << " no encontrado." << std::endl;
            return false;
        }

        auto itMat = std::find_if(catalogo.begin(), catalogo.end(),
            [&](const std::shared_ptr<MaterialBibliotecario>& m){
                return m->getIdentificador() == idMaterial; });
        if (itMat == catalogo.end()) {
            std::cout << "[ERROR] Material ID=" << idMaterial << " no está en el catálogo." << std::endl;
            return false;
        }

        auto material = *itMat;

        // Restricción por límite de libros (<<extend>> en casos de uso)
        if (usuario->getLibrosPrestados() >= usuario->getMaxLibros()) {
            std::cout << "[RESTRICCIÓN] " << usuario->getNombre()
                      << " ha alcanzado el límite de " << usuario->getMaxLibros()
                      << " préstamos simultáneos." << std::endl;
            return false;
        }

        if (!material->estaDisponible()) {
            std::cout << "[NO DISPONIBLE] '" << material->getTitulo()
                      << "' no está disponible actualmente." << std::endl;
            return false;
        }

        // Generar ID único del préstamo
        std::string idPrestamo = "PRE-" + std::to_string(++contadorPrestamos);
        auto prestamo = std::make_shared<Prestamo>(idPrestamo, usuario, material, fechaDevolucion);
        historialPrestamos.push_back(prestamo);

        std::cout << "✓ Préstamo " << idPrestamo << " registrado: '"
                  << material->getTitulo() << "' → " << usuario->getNombre()
                  << " (vence: " << fechaDevolucion << ")" << std::endl;
        return true;
    }

    /**
     * @brief Registra la devolución de un préstamo y calcula penalidad si aplica.
     * @param idPrestamo  Identificador del préstamo a devolver.
     * @param diasRetraso Días de retraso (0 si es a tiempo).
     */
    void registrarDevolucion(const std::string& idPrestamo, int diasRetraso = 0) {
        auto it = std::find_if(historialPrestamos.begin(), historialPrestamos.end(),
            [&](const std::shared_ptr<Prestamo>& p){ return p->getIdPrestamo() == idPrestamo; });

        if (it == historialPrestamos.end()) {
            std::cout << "[ERROR] Préstamo " << idPrestamo << " no encontrado." << std::endl;
            return;
        }

        auto& prestamo = *it;
        if (prestamo->estaDevuelto()) {
            std::cout << "[ADVERTENCIA] El préstamo " << idPrestamo << " ya fue devuelto." << std::endl;
            return;
        }

        // Calcular y registrar penalidad (<<extend>> Devolver Libro → Calcular Penalidad)
        prestamo->registrarDevolucion(diasRetraso);
        if (diasRetraso > 0) {
            std::cout << "⚠ MORA APLICADA: $" << std::fixed << std::setprecision(2)
                      << prestamo->getPenalidad() << " (" << diasRetraso << " días de retraso)." << std::endl;
        } else {
            std::cout << "✓ Devolución registrada a tiempo. Sin penalidad." << std::endl;
        }
    }

    // ══════════════════════════════════════════════════════════════════════════
    //  REPORTES Y AUDITORÍA
    // ══════════════════════════════════════════════════════════════════════════

    /**
     * @brief Imprime el historial completo de préstamos en consola y lo persiste en disco.
     * @param archivoSalida Ruta del archivo de auditoría.
     */
    void generarReporteAuditoria(const std::string& archivoSalida = "auditoria.txt") const {
        std::ostringstream reporte;
        reporte << "════════════════════════════════════════════════════════\n";
        reporte << " REPORTE DE AUDITORÍA - " << nombreInstitucion << "\n";
        reporte << "════════════════════════════════════════════════════════\n";
        reporte << "Total de transacciones registradas: " << historialPrestamos.size() << "\n\n";

        for (const auto& p : historialPrestamos) {
            reporte << "ID Préstamo : " << p->getIdPrestamo()         << "\n";
            reporte << "Usuario     : " << p->getUsuario()->getNombre()
                    << " (UID: "        << p->getUsuario()->getUid()   << ")\n";
            reporte << "Material    : " << p->getMaterial()->getTitulo()
                    << " ["             << p->getMaterial()->getTipo() << "]\n";
            reporte << "Salida      : " << p->getFechaSalida()         << "\n";
            reporte << "Estado      : " << (p->estaDevuelto() ? "DEVUELTO" : "ACTIVO") << "\n";
            if (p->estaDevuelto() && p->getPenalidad() > 0.0)
                reporte << "Penalidad   : $" << std::fixed << std::setprecision(2)
                        << p->getPenalidad() << "\n";
            reporte << "────────────────────────────────────────────────────\n";
        }

        // Imprimir en consola
        std::cout << reporte.str();

        // Persistir en disco
        std::ofstream ofs(archivoSalida);
        if (ofs.is_open()) {
            ofs << reporte.str();
            std::cout << "\n[INFO] Reporte persistido en: " << archivoSalida << std::endl;
        } else {
            std::cout << "[ERROR] No se pudo escribir el archivo de auditoría." << std::endl;
        }
    }
};

#endif // BIBLIOTECA_H
