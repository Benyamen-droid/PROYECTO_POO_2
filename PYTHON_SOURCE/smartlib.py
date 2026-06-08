"""
smartlib.py
===========
SmartLib v2.0 - Sistema de Gestion de Biblioteca Inteligente
Implementacion en Python 3.11+ con POO avanzada.

Caracteristicas:
    - Clases abstractas puras via modulo `abc`.
    - Encapsulacion estricta con propiedades (@property).
    - Type Hinting completo (PEP 484 / PEP 526).
    - Persistencia bidireccional en JSON (Opcion A del desafio avanzado).
    - Busquedas avanzadas multi-criterio.
    - Calculo automatico de penalidades por mora.

Autor: SmartLib Development Team
Ciclo: 2026 - POO II
"""

# ─────────────────────────────────────────────────────────────────────────────
# Importaciones estandar
# ─────────────────────────────────────────────────────────────────────────────
import json
import os
from abc import ABC, abstractmethod
from enum import Enum
from typing import List, Optional


# ─────────────────────────────────────────────────────────────────────────────
# ENUMERACION: Estado transaccional del material
# ─────────────────────────────────────────────────────────────────────────────
class EstadoMaterial(Enum):
    """Estados transaccionales posibles de un material bibliotecario."""
    DISPONIBLE       = "Disponible"
    PRESTADO         = "Prestado"
    RESERVADO        = "Reservado"
    EN_MANTENIMIENTO = "En Mantenimiento"


# =============================================================================
# JERARQUIA DE USUARIOS
# =============================================================================

class Persona(ABC):
    """
    Clase abstracta pura que define la interfaz comun para todas las entidades humanas.

    Encapsulacion estricta:
        - __uid y __nombre son atributos fuertemente privados (name-mangling).
        - _libros_prestados_actuales y _max_libros_permitidos son protegidos.
        - Acceso controlado exclusivamente via @property.
    """

    def __init__(self, uid: str, nombre: str, max_libros: int) -> None:
        self.__uid: str = uid
        self.__nombre: str = nombre
        self._libros_prestados_actuales: int = 0
        self._max_libros_permitidos: int = max_libros

    # ── Propiedades de solo lectura (encapsulacion) ──────────────────────────

    @property
    def uid(self) -> str:
        """Identificador unico del usuario (solo lectura)."""
        return self.__uid

    @property
    def nombre(self) -> str:
        """Nombre completo del usuario (solo lectura)."""
        return self.__nombre

    @property
    def libros_prestados(self) -> int:
        """Numero de prestamos activos actuales."""
        return self._libros_prestados_actuales

    @property
    def max_libros(self) -> int:
        """Limite maximo de prestamos simultaneos permitidos."""
        return self._max_libros_permitidos

    # ── Metodos de negocio ───────────────────────────────────────────────────

    def registrar_nuevo_prestamo(self) -> bool:
        """
        Intenta registrar un nuevo prestamo si no se ha alcanzado la cuota.

        Returns:
            True si el prestamo fue autorizado, False si se supero el limite.
        """
        if self._libros_prestados_actuales < self._max_libros_permitidos:
            self._libros_prestados_actuales += 1
            return True
        return False

    def registrar_devolucion(self) -> None:
        """Decrementa el contador de prestamos activos al procesar una devolucion."""
        if self._libros_prestados_actuales > 0:
            self._libros_prestados_actuales -= 1

    # ── Interfaz abstracta (polimorfismo obligatorio) ────────────────────────

    @abstractmethod
    def mostrar_perfil(self) -> None:
        """Muestra el perfil completo del usuario en consola."""
        pass

    @abstractmethod
    def calcular_tarifa_mora(self, dias_retraso: int) -> float:
        """
        Calcula el cobro por mora segun el rol institucional del usuario.

        Args:
            dias_retraso: Dias transcurridos despues de la fecha de devolucion pactada.

        Returns:
            Monto total de la penalidad en dolares (0.0 si no hay mora).
        """
        pass

    def to_dict(self) -> dict:
        """Serializa los atributos comunes de Persona a diccionario para persistencia JSON."""
        return {
            "uid": self.uid,
            "nombre": self.nombre,
            "libros_prestados": self._libros_prestados_actuales,
        }


# ─────────────────────────────────────────────────────────────────────────────

class Estudiante(Persona):
    """
    Representacion de un usuario de tipo Estudiante con politicas restrictivas estandar.

    Reglas de negocio:
        - Maximo 3 prestamos simultaneos.
        - Tarifa de mora: $1.50 por dia de retraso.
        - Periodo maximo de prestamo: 7 dias.
    """

    def __init__(self, uid: str, nombre: str,
                 codigo_estudiante: str, carrera: str) -> None:
        super().__init__(uid, nombre, max_libros=3)
        self.codigo_estudiante: str = codigo_estudiante
        self.carrera: str = carrera

    def mostrar_perfil(self) -> None:
        print(
            f"[ESTUDIANTE] UID: {self.uid} | Nombre: {self.nombre} "
            f"| Codigo: {self.codigo_estudiante} | Carrera: {self.carrera} "
            f"| Prestamos: {self.libros_prestados}/{self.max_libros}"
        )

    def calcular_tarifa_mora(self, dias_retraso: int) -> float:
        """Mora fija de $1.50 por dia para estudiantes."""
        return max(0.0, dias_retraso * 1.50)

    def to_dict(self) -> dict:
        datos = super().to_dict()
        datos.update({
            "tipo": "Estudiante",
            "codigo_estudiante": self.codigo_estudiante,
            "carrera": self.carrera,
        })
        return datos


# ─────────────────────────────────────────────────────────────────────────────

class Profesor(Persona):
    """
    Representacion de un usuario de tipo Profesor con privilegios ampliados de investigacion.

    Reglas de negocio:
        - Maximo 10 prestamos simultaneos.
        - Tarifa de mora diferenciada: $0.50 por dia de retraso.
        - Periodo maximo de prestamo: 30 dias.
    """

    def __init__(self, uid: str, nombre: str, departamento: str) -> None:
        super().__init__(uid, nombre, max_libros=10)
        self.departamento: str = departamento

    def mostrar_perfil(self) -> None:
        print(
            f"[PROFESOR] UID: {self.uid} | Nombre: {self.nombre} "
            f"| Departamento: {self.departamento} "
            f"| Prestamos: {self.libros_prestados}/{self.max_libros}"
        )

    def calcular_tarifa_mora(self, dias_retraso: int) -> float:
        """Mora diferenciada de $0.50 por dia para profesores e investigadores."""
        return max(0.0, dias_retraso * 0.50)

    def to_dict(self) -> dict:
        datos = super().to_dict()
        datos.update({
            "tipo": "Profesor",
            "departamento": self.departamento,
        })
        return datos


# =============================================================================
# JERARQUIA DE MATERIALES BIBLIOTECARIOS
# =============================================================================

class MaterialBibliotecario(ABC):
    """
    Clase abstracta pura base de la jerarquia de materiales del inventario.

    Define la interfaz comun para LibroFisico y LibroDigital.
    """

    def __init__(self, identificador: str, titulo: str,
                 autor: str, anio: int) -> None:
        self.__identificador: str = identificador
        self.__titulo: str = titulo
        self.__autor: str = autor
        self.__anio: int = anio
        self.__estado: EstadoMaterial = EstadoMaterial.DISPONIBLE

    # ── Propiedades ──────────────────────────────────────────────────────────

    @property
    def identificador(self) -> str:
        return self.__identificador

    @property
    def titulo(self) -> str:
        return self.__titulo

    @property
    def autor(self) -> str:
        return self.__autor

    @property
    def anio(self) -> int:
        return self.__anio

    @property
    def estado(self) -> EstadoMaterial:
        return self.__estado

    @estado.setter
    def estado(self, nuevo_estado: EstadoMaterial) -> None:
        """Setter controlado para cambiar el estado transaccional del material."""
        self.__estado = nuevo_estado

    # ── Interfaz abstracta ───────────────────────────────────────────────────

    @abstractmethod
    def mostrar_info(self) -> None:
        """Muestra la informacion completa del material en consola."""
        pass

    @abstractmethod
    def to_dict(self) -> dict:
        """Serializa el material a diccionario para persistencia JSON."""
        pass

    def _base_dict(self) -> dict:
        """Retorna atributos comunes serializados (uso interno)."""
        return {
            "identificador": self.identificador,
            "titulo": self.titulo,
            "autor": self.autor,
            "anio": self.anio,
            "estado": self.estado.value,
        }


# ─────────────────────────────────────────────────────────────────────────────

class LibroFisico(MaterialBibliotecario):
    """
    Subclase concreta de MaterialBibliotecario para libros fisicos.

    Atributos adicionales: pasillo y estante de ubicacion.
    """

    def __init__(self, isbn: str, titulo: str, autor: str, anio: int,
                 pasillo: str, estante: str) -> None:
        super().__init__(isbn, titulo, autor, anio)
        self.pasillo: str = pasillo
        self.estante: str = estante

    def mostrar_info(self) -> None:
        print(
            f"[FISICO] ISBN: {self.identificador} | Titulo: {self.titulo} "
            f"| Autor: {self.autor} | Anio: {self.anio} "
            f"| Ubicacion: Pasillo {self.pasillo} - {self.estante} "
            f"| Estado: {self.estado.value}"
        )

    def to_dict(self) -> dict:
        datos = self._base_dict()
        datos.update({"tipo": "LibroFisico", "pasillo": self.pasillo, "estante": self.estante})
        return datos


# ─────────────────────────────────────────────────────────────────────────────

class LibroDigital(MaterialBibliotecario):
    """
    Subclase concreta de MaterialBibliotecario para recursos digitales.

    Atributos adicionales: URL de acceso y tamano en megabytes.
    """

    def __init__(self, uuid: str, titulo: str, autor: str, anio: int,
                 url: str, tamano_mb: float) -> None:
        super().__init__(uuid, titulo, autor, anio)
        self.url: str = url
        self.tamano_mb: float = tamano_mb

    def mostrar_info(self) -> None:
        print(
            f"[DIGITAL] UUID: {self.identificador} | Titulo: {self.titulo} "
            f"| Autor: {self.autor} | Anio: {self.anio} "
            f"| URL: {self.url} | Tamano: {self.tamano_mb} MB "
            f"| Estado: {self.estado.value}"
        )

    def to_dict(self) -> dict:
        datos = self._base_dict()
        datos.update({"tipo": "LibroDigital", "url": self.url, "tamano_mb": self.tamano_mb})
        return datos


# =============================================================================
# ENTIDAD DE ASOCIACION: Prestamo
# =============================================================================

class Prestamo:
    """
    Entidad de asociacion que vincula un Persona con un MaterialBibliotecario.

    Registra la transaccion completa: usuario, material, fechas y estado.
    """

    def __init__(self, id_prestamo: str, usuario: Persona,
                 material: MaterialBibliotecario,
                 fecha_salida: str, fecha_devolucion_pactada: str) -> None:
        self.__id_prestamo: str = id_prestamo
        self.__usuario: Persona = usuario
        self.__material: MaterialBibliotecario = material
        self.__fecha_salida: str = fecha_salida
        self.__fecha_devolucion_pactada: str = fecha_devolucion_pactada
        self.__devuelto: bool = False

    # ── Propiedades ──────────────────────────────────────────────────────────

    @property
    def id_prestamo(self) -> str:
        return self.__id_prestamo

    @property
    def usuario(self) -> Persona:
        return self.__usuario

    @property
    def material(self) -> MaterialBibliotecario:
        return self.__material

    @property
    def fecha_salida(self) -> str:
        return self.__fecha_salida

    @property
    def fecha_devolucion_pactada(self) -> str:
        return self.__fecha_devolucion_pactada

    @property
    def devuelto(self) -> bool:
        return self.__devuelto

    # ── Logica de negocio ────────────────────────────────────────────────────

    def procesar_devolucion(self, dias_retraso: int = 0) -> None:
        """
        Cierra el prestamo, restaura disponibilidad del material y calcula multa.

        Args:
            dias_retraso: Dias de mora (0 si la devolucion fue puntual).
        """
        if self.__devuelto:
            print(f"  [AVISO] El prestamo {self.__id_prestamo} ya fue cerrado.")
            return

        self.__devuelto = True
        self.__material.estado = EstadoMaterial.DISPONIBLE
        self.__usuario.registrar_devolucion()

        multa = self.__usuario.calcular_tarifa_mora(dias_retraso)
        print(f"\n--- DEVOLUCION PROCESADA ---")
        print(f"  Prestamo ID : {self.__id_prestamo}")
        print(f"  Usuario     : {self.__usuario.nombre}")
        print(f"  Material    : {self.__material.titulo}")
        print(f"  Dias retraso: {dias_retraso}")
        if multa > 0:
            print(f"  ** PENALIDAD APLICADA: ${multa:.2f} **")
        else:
            print("  Sin penalidad (devolucion a tiempo).")

    def mostrar_resumen(self) -> None:
        """Muestra el resumen del prestamo en consola."""
        estado_str = "CERRADO" if self.__devuelto else "ACTIVO"
        print(
            f"  Prestamo [{self.__id_prestamo}] "
            f"Usuario: {self.__usuario.nombre} "
            f"| Material: {self.__material.titulo} "
            f"| Salida: {self.__fecha_salida} "
            f"| Vence: {self.__fecha_devolucion_pactada} "
            f"| Estado: {estado_str}"
        )

    def to_dict(self) -> dict:
        """Serializa el prestamo para persistencia JSON."""
        return {
            "id_prestamo": self.__id_prestamo,
            "uid_usuario": self.__usuario.uid,
            "id_material": self.__material.identificador,
            "fecha_salida": self.__fecha_salida,
            "fecha_devolucion_pactada": self.__fecha_devolucion_pactada,
            "devuelto": self.__devuelto,
        }


# =============================================================================
# ORQUESTADOR CENTRAL: Biblioteca
# =============================================================================

class Biblioteca:
    """
    Orquestador central del ecosistema SmartLib v2.0.

    Administra las colecciones de usuarios, materiales y prestamos.
    Implementa persistencia bidireccional en JSON (Opcion A del desafio avanzado).

    Relaciones UML:
        - Composicion con MaterialBibliotecario (1 a 0..*).
        - Composicion con Persona              (1 a 0..*).
        - Agregacion  con Prestamo             (1 a *).
    """

    def __init__(self, nombre_institucion: str) -> None:
        self.nombre_institucion: str = nombre_institucion
        self.__lista_usuarios: List[Persona] = []
        self.__catalogo: List[MaterialBibliotecario] = []
        self.__historial_prestamos: List[Prestamo] = []
        self.__contador_prestamos: int = 0

    # ── Gestion de Usuarios ──────────────────────────────────────────────────

    def registrar_usuario(self, usuario: Persona) -> None:
        """
        Registra un nuevo usuario validando UID unico.

        Args:
            usuario: Instancia de Estudiante o Profesor.
        """
        if any(u.uid == usuario.uid for u in self.__lista_usuarios):
            print(f"  [ERROR] El UID '{usuario.uid}' ya se encuentra registrado.")
            return
        self.__lista_usuarios.append(usuario)
        print(f"-> Usuario '{usuario.nombre}' indexado correctamente en Python.")

    def desplegar_usuarios(self) -> None:
        """Despliega el reporte de usuarios usando polimorfismo dinamico."""
        print(f"\n=== REPORTE DE USUARIOS - {self.nombre_institucion.upper()} ===")
        for usuario in self.__lista_usuarios:
            usuario.mostrar_perfil()  # Polimorfismo: Duck Typing en Python

    def buscar_usuario_por_uid(self, uid: str) -> Optional[Persona]:
        """Busca un usuario por su UID. Retorna None si no existe."""
        for usuario in self.__lista_usuarios:
            if usuario.uid == uid:
                return usuario
        return None

    # ── Gestion del Catalogo ─────────────────────────────────────────────────

    def registrar_material(self, material: MaterialBibliotecario) -> None:
        """Agrega un material al catalogo de la biblioteca."""
        self.__catalogo.append(material)
        print(f"-> Material '{material.titulo}' indexado en el catalogo.")

    def desplegar_catalogo(self) -> None:
        """Despliega el catalogo completo usando polimorfismo dinamico."""
        print(f"\n=== CATALOGO DE MATERIALES - {self.nombre_institucion.upper()} ===")
        for material in self.__catalogo:
            material.mostrar_info()

    def buscar_material(self, criterio: str) -> None:
        """
        Busqueda multi-criterio insensible a mayusculas (titulo o autor).

        Args:
            criterio: Termino de busqueda libre.
        """
        criterio_lower = criterio.lower()
        print(f"\n=== BUSQUEDA: '{criterio}' ===")
        resultados = [
            m for m in self.__catalogo
            if criterio_lower in m.titulo.lower() or criterio_lower in m.autor.lower()
        ]
        if resultados:
            for m in resultados:
                m.mostrar_info()
        else:
            print("  No se encontraron materiales con ese criterio.")

    def buscar_material_por_id(self, identificador: str) -> Optional[MaterialBibliotecario]:
        """Busca un material por ISBN/UUID exacto."""
        for material in self.__catalogo:
            if material.identificador == identificador:
                return material
        return None

    # ── Gestion de Prestamos ─────────────────────────────────────────────────

    def realizar_prestamo(self, uid_usuario: str, id_material: str,
                          fecha_salida: str, fecha_devolucion: str) -> None:
        """
        Ejecuta un prestamo con validacion de disponibilidad y cuota de usuario.

        Flujo (segun diagrama de casos de uso):
            1. Consultar disponibilidad del material (<<include>>).
            2. Verificar cuota del usuario (<<extend>> si supero limite).
            3. Registrar la transaccion.

        Args:
            uid_usuario:     UID del usuario solicitante.
            id_material:     ISBN/UUID del material solicitado.
            fecha_salida:    Fecha de inicio del prestamo (YYYY-MM-DD).
            fecha_devolucion: Fecha limite de devolucion pactada.
        """
        usuario  = self.buscar_usuario_por_uid(uid_usuario)
        material = self.buscar_material_por_id(id_material)

        if not usuario:
            print(f"  [ERROR] Usuario con UID '{uid_usuario}' no encontrado.")
            return
        if not material:
            print(f"  [ERROR] Material con ID '{id_material}' no encontrado.")
            return
        if material.estado != EstadoMaterial.DISPONIBLE:
            print(f"  [RESTRICCION] El material '{material.titulo}' no esta disponible "
                  f"(estado actual: {material.estado.value}).")
            return
        if not usuario.registrar_nuevo_prestamo():
            print(f"  [RESTRICCION] '{usuario.nombre}' ha alcanzado su limite de "
                  f"{usuario.max_libros} prestamos simultaneos.")
            return

        material.estado = EstadoMaterial.PRESTADO
        self.__contador_prestamos += 1
        id_prestamo = f"PRE-{self.__contador_prestamos:03d}"
        prestamo = Prestamo(id_prestamo, usuario, material, fecha_salida, fecha_devolucion)
        self.__historial_prestamos.append(prestamo)

        print(f"\n  [PRESTAMO APROBADO] ID: {id_prestamo} "
              f"| {usuario.nombre} <=> {material.titulo} "
              f"| Vence: {fecha_devolucion}")

    def procesar_devolucion(self, id_prestamo: str, dias_retraso: int = 0) -> None:
        """
        Localiza y cierra un prestamo por su ID, aplicando mora si corresponde.

        Args:
            id_prestamo:  ID del prestamo a cerrar.
            dias_retraso: Dias de mora (0 si fue puntual).
        """
        for prestamo in self.__historial_prestamos:
            if prestamo.id_prestamo == id_prestamo:
                prestamo.procesar_devolucion(dias_retraso)
                return
        print(f"  [ERROR] Prestamo '{id_prestamo}' no encontrado en el historial.")

    def reporte_auditoria(self) -> None:
        """Genera el reporte de auditoria de todas las transacciones registradas."""
        print(f"\n=== REPORTE DE AUDITORIA DE PRESTAMOS ===")
        for prestamo in self.__historial_prestamos:
            prestamo.mostrar_resumen()

    # ── Persistencia JSON (Desafio Avanzado - Opcion A) ──────────────────────

    def exportar_a_json(self, ruta_usuarios: str = "usuarios.json",
                        ruta_catalogo: str = "catalogo.json",
                        ruta_prestamos: str = "prestamos.json") -> None:
        """
        Vuelca el estado completo del sistema en archivos JSON (serializacion).

        Args:
            ruta_usuarios:  Archivo de destino para la lista de usuarios.
            ruta_catalogo:  Archivo de destino para el catalogo de materiales.
            ruta_prestamos: Archivo de destino para el historial de prestamos.
        """
        try:
            with open(ruta_usuarios, "w", encoding="utf-8") as f:
                json.dump([u.to_dict() for u in self.__lista_usuarios], f,
                          ensure_ascii=False, indent=4)

            with open(ruta_catalogo, "w", encoding="utf-8") as f:
                json.dump([m.to_dict() for m in self.__catalogo], f,
                          ensure_ascii=False, indent=4)

            with open(ruta_prestamos, "w", encoding="utf-8") as f:
                json.dump([p.to_dict() for p in self.__historial_prestamos], f,
                          ensure_ascii=False, indent=4)

            print(f"\n  [PERSISTENCIA] Estado del sistema exportado:")
            print(f"    -> {ruta_usuarios}")
            print(f"    -> {ruta_catalogo}")
            print(f"    -> {ruta_prestamos}")

        except OSError as e:
            print(f"  [ERROR de E/S] No se pudo escribir el archivo: {e}")

    @staticmethod
    def importar_usuarios_desde_json(ruta: str) -> List[Persona]:
        """
        Hidrata objetos Persona desde un archivo JSON (deserializacion).

        Control de excepciones para archivos corruptos o ausentes.

        Args:
            ruta: Ruta al archivo JSON de usuarios.

        Returns:
            Lista de instancias Persona reconstruidas.
        """
        usuarios: List[Persona] = []
        try:
            with open(ruta, "r", encoding="utf-8") as f:
                datos = json.load(f)
            for item in datos:
                tipo = item.get("tipo")
                if tipo == "Estudiante":
                    u = Estudiante(item["uid"], item["nombre"],
                                   item["codigo_estudiante"], item["carrera"])
                elif tipo == "Profesor":
                    u = Profesor(item["uid"], item["nombre"], item["departamento"])
                else:
                    print(f"  [AVISO] Tipo de usuario desconocido: '{tipo}' — omitido.")
                    continue
                # Restaurar estado de prestamos activos
                u._libros_prestados_actuales = item.get("libros_prestados", 0)
                usuarios.append(u)
            print(f"  [PERSISTENCIA] {len(usuarios)} usuarios hidratados desde '{ruta}'.")
        except FileNotFoundError:
            print(f"  [ERROR] Archivo '{ruta}' no encontrado. Se omite la carga.")
        except json.JSONDecodeError as e:
            print(f"  [ERROR] Archivo '{ruta}' corrupto o con formato invalido: {e}")
        return usuarios


# =============================================================================
# BLOQUE DE EJECUCION PRINCIPAL
# =============================================================================

def separador(titulo: str) -> None:
    """Imprime una cabecera de seccion para la consola."""
    print("\n" + "=" * 60)
    print(f"  {titulo}")
    print("=" * 60)


if __name__ == "__main__":

    # ──────────────────────────────────────────────────────────
    # 1. INICIALIZACION
    # ──────────────────────────────────────────────────────────
    separador("SMARTLIB v2.0 - Implementacion Python 3.11+")
    smart_lib = Biblioteca("Biblioteca Central de Ingenieria - UNAP")

    # ──────────────────────────────────────────────────────────
    # 2. REGISTRO DE USUARIOS - Instanciacion polimorfica
    # ──────────────────────────────────────────────────────────
    separador("FASE 1: Registro de Usuarios")

    u1 = Estudiante("E001", "Carlos Mendoza", "20241045", "Ingenieria de Sistemas")
    u2 = Estudiante("E002", "Lucia Flores",   "20241102", "Ingenieria Civil")
    u3 = Profesor("P001", "Dr. Arthur Dynamic", "Ciencias de la Computacion")
    u4 = Profesor("P002", "Mg. Aldo Zanabria",  "Informatica y Sistemas")

    smart_lib.registrar_usuario(u1)
    smart_lib.registrar_usuario(u2)
    smart_lib.registrar_usuario(u3)
    smart_lib.registrar_usuario(u4)

    # Intento de registro duplicado
    print("\n[Escenario duplicado]:")
    smart_lib.registrar_usuario(Estudiante("E001", "Duplicado", "99999", "Test"))

    smart_lib.desplegar_usuarios()

    # ──────────────────────────────────────────────────────────
    # 3. REGISTRO DE MATERIALES
    # ──────────────────────────────────────────────────────────
    separador("FASE 2: Carga del Catalogo")

    m1 = LibroFisico("978-0-13-235088-4", "The Pragmatic Programmer",
                     "Hunt & Thomas", 2019, "A3", "Estante-07")
    m2 = LibroFisico("978-0-13-468599-1", "Clean Code",
                     "Robert C. Martin", 2008, "B1", "Estante-02")
    m3 = LibroFisico("978-0-201-63361-0", "Design Patterns",
                     "Gamma, Helm, Johnson, Vlissides", 1994, "C2", "Estante-11")
    m4 = LibroDigital("UUID-7F3A-9D21", "Python Object-Oriented Programming",
                      "Dusty Phillips", 2018,
                      "https://smartlib.unap.edu.pe/digital/python-oop-phillips.pdf", 12.4)
    m5 = LibroDigital("UUID-4B1C-2E78", "C++ Primer 6th Edition",
                      "Lippman, Lajoie & Moo", 2023,
                      "https://smartlib.unap.edu.pe/digital/cpp-primer-6ed.pdf", 45.7)

    smart_lib.registrar_material(m1)
    smart_lib.registrar_material(m2)
    smart_lib.registrar_material(m3)
    smart_lib.registrar_material(m4)
    smart_lib.registrar_material(m5)

    smart_lib.desplegar_catalogo()

    # ──────────────────────────────────────────────────────────
    # 4. MOTOR DE CONSULTAS
    # ──────────────────────────────────────────────────────────
    separador("FASE 3: Motor de Consultas")
    smart_lib.buscar_material("Clean")
    smart_lib.buscar_material("Dusty")
    smart_lib.buscar_material("Inexistente")

    # ──────────────────────────────────────────────────────────
    # 5. PRESTAMOS
    # ──────────────────────────────────────────────────────────
    separador("FASE 4: Registro de Prestamos")

    smart_lib.realizar_prestamo("E001", "978-0-13-235088-4",
                                "2026-06-01", "2026-06-08")
    smart_lib.realizar_prestamo("E001", "978-0-13-468599-1",
                                "2026-06-01", "2026-06-08")
    smart_lib.realizar_prestamo("P001", "978-0-201-63361-0",
                                "2026-06-01", "2026-07-01")
    smart_lib.realizar_prestamo("P002", "UUID-7F3A-9D21",
                                "2026-06-01", "2026-07-01")

    # Escenarios de restriccion
    print("\n[Escenario: material no disponible]:")
    smart_lib.realizar_prestamo("E002", "978-0-13-235088-4",
                                "2026-06-02", "2026-06-09")

    # ──────────────────────────────────────────────────────────
    # 6. DEVOLUCIONES
    # ──────────────────────────────────────────────────────────
    separador("FASE 5: Devoluciones y Penalidades")

    smart_lib.procesar_devolucion("PRE-003", 0)    # Profesor, sin mora
    smart_lib.procesar_devolucion("PRE-001", 5)    # Estudiante: 5 dias * $1.50 = $7.50
    smart_lib.procesar_devolucion("PRE-004", 10)   # Profesor:   10 dias * $0.50 = $5.00

    # ──────────────────────────────────────────────────────────
    # 7. POLIMORFISMO EXPLICITO
    # ──────────────────────────────────────────────────────────
    separador("FASE 6: Demostracion de Polimorfismo Dinamico (Duck Typing)")

    todos_los_usuarios: List[Persona] = [u1, u2, u3, u4]
    print("\nCalculo de mora para 7 dias sobre cada tipo de usuario:")
    for usr in todos_los_usuarios:
        usr.mostrar_perfil()
        print(f"  -> Mora (7 dias): ${usr.calcular_tarifa_mora(7):.2f}\n")

    # ──────────────────────────────────────────────────────────
    # 8. REPORTE DE AUDITORIA
    # ──────────────────────────────────────────────────────────
    separador("FASE 7: Reporte Final de Auditoria")
    smart_lib.reporte_auditoria()
    smart_lib.desplegar_catalogo()

    # ──────────────────────────────────────────────────────────
    # 9. PERSISTENCIA JSON (Desafio Avanzado - Opcion A)
    # ──────────────────────────────────────────────────────────
    separador("FASE 8: Persistencia Bidireccional JSON (Desafio Avanzado)")

    smart_lib.exportar_a_json("usuarios.json", "catalogo.json", "prestamos.json")

    print("\n  [HIDRATACION] Reconstruyendo usuarios desde disco:")
    usuarios_hidratados = Biblioteca.importar_usuarios_desde_json("usuarios.json")
    print(f"  -> {len(usuarios_hidratados)} usuario(s) reconstruidos exitosamente.")

    separador("SmartLib v2.0 - Ejecucion Python finalizada correctamente")
