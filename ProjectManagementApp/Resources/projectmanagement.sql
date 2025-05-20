-- SQLite3 dump - Sistema de Gestión de Proyectos

PRAGMA foreign_keys = ON;

--
-- Tabla Usuarios
--
DROP TABLE IF EXISTS Usuarios;
CREATE TABLE Usuarios (
    usuario_id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT(100) NOT NULL,
    apellido TEXT(100) NOT NULL,
    email TEXT(100) NOT NULL UNIQUE,
    telefono TEXT(20),
    departamento TEXT(100),
    fecha_registro DATETIME DEFAULT CURRENT_TIMESTAMP,
    url_imagen_perfil TEXT
);

--
-- Datos para la tabla Usuarios
--
INSERT INTO Usuarios (usuario_id, nombre, apellido, email, telefono, departamento, fecha_registro, url_imagen_perfil) VALUES
(1,'Juan','Pérez','jperez@ejemplo.com','555-1234','Desarrollo','2025-04-23 10:48:57','https://randomuser.me/api/portraits/men/1.jpg'),
(2,'María','López','mlopez@ejemplo.com','555-2345','Diseño','2025-04-23 10:48:57','https://randomuser.me/api/portraits/women/2.jpg'),
(3,'Carlos','González','cgonzalez@ejemplo.com','555-3456','Marketing','2025-04-23 10:48:57','https://randomuser.me/api/portraits/men/3.jpg'),
(4,'Ana','Martínez','amartinez@ejemplo.com','555-4567','Ventas','2025-04-23 10:48:57','https://randomuser.me/api/portraits/women/4.jpg'),
(5,'Pedro','Rodríguez','prodriguez@ejemplo.com','555-5678','Desarrollo','2025-04-23 10:48:57','https://randomuser.me/api/portraits/men/5.jpg'),
(6,'Laura','Sánchez','lsanchez@ejemplo.com','555-6789','Recursos Humanos','2025-04-23 10:48:57','https://randomuser.me/api/portraits/women/6.jpg'),
(7,'Miguel','Fernández','mfernandez@ejemplo.com','555-7890','Finanzas','2025-04-23 10:48:57','https://randomuser.me/api/portraits/men/7.jpg'),
(8,'Sofía','Díaz','sdiaz@ejemplo.com','555-8901','Atención al Cliente','2025-04-23 10:48:57','https://randomuser.me/api/portraits/women/8.jpg'),
(9,'Javier','Torres','jtorres@ejemplo.com','555-9012','Desarrollo','2025-04-23 10:48:57','https://randomuser.me/api/portraits/men/9.jpg'),
(10,'Carmen','Ruiz','cruiz@ejemplo.com','555-0123','Diseño','2025-04-23 10:48:57','https://randomuser.me/api/portraits/women/10.jpg'),
(11,'Alejandro','Vargas','avargas@ejemplo.com','555-1235','Marketing','2025-04-23 10:48:57','https://randomuser.me/api/portraits/men/11.jpg'),
(12,'Isabel','Castro','icastro@ejemplo.com','555-2346','Ventas','2025-04-23 10:48:57','https://randomuser.me/api/portraits/women/12.jpg'),
(13,'Fernando','Ortega','fortega@ejemplo.com','555-3457','Desarrollo','2025-04-23 10:48:57','https://randomuser.me/api/portraits/men/13.jpg'),
(14,'Lucía','Morales','lmorales@ejemplo.com','555-4568','Recursos Humanos','2025-04-23 10:48:57','https://randomuser.me/api/portraits/women/14.jpg'),
(15,'Roberto','Herrera','rherrera@ejemplo.com','555-5679','Finanzas','2025-04-23 10:48:57','https://randomuser.me/api/portraits/men/15.jpg');

--
-- Tabla Proyectos
--
DROP TABLE IF EXISTS Proyectos;
CREATE TABLE Proyectos (
    proyecto_id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT(100) NOT NULL,
    descripcion TEXT,
    fecha_inicio DATE NOT NULL,
    fecha_fin_prevista DATE,
    estado TEXT CHECK (estado IN ('Pendiente','En Progreso','Finalizado')) DEFAULT 'Pendiente',
    prioridad TEXT CHECK (prioridad IN ('Baja','Media','Alta','Urgente')) DEFAULT 'Media',
    responsable_id INTEGER,
    FOREIGN KEY (responsable_id) REFERENCES Usuarios (usuario_id)
);

--
-- Datos para la tabla Proyectos
--
INSERT INTO Proyectos (proyecto_id, nombre, descripcion, fecha_inicio, fecha_fin_prevista, estado, prioridad, responsable_id) VALUES
(1,'Rediseño Web Corporativa','Actualización completa del sitio web de la empresa','2025-01-15','2025-04-30','Finalizado','Alta',1),
(2,'App Móvil Clientes','Desarrollo de aplicación móvil para clientes','2025-02-01','2025-07-31','En Progreso','Alta',5),
(3,'Campaña Marketing Q2','Campaña de marketing para el segundo trimestre','2025-03-15','2025-06-15','Finalizado','Media',3),
(4,'Sistema CRM Interno','Implementación de nuevo sistema CRM','2025-04-01','2025-10-31','En Progreso','Alta',9),
(5,'Plan Estratégico 2026','Desarrollo del plan estratégico para el próximo año','2025-05-15','2025-08-31','En Progreso','Media',7),
(6,'Automatización Procesos','Automatización de procesos internos','2025-06-01','2025-11-30','En Progreso','Alta',13),
(7,'Expansión Internacional','Proyecto de expansión a nuevos mercados','2025-07-15','2026-01-31','Pendiente','Urgente',4),
(8,'Actualización Infraestructura IT','Modernización de la infraestructura tecnológica','2025-08-01','2025-12-15','En Progreso','Alta',1),
(9,'Programa Capacitación','Programa de capacitación para empleados','2025-09-15','2025-12-31','Pendiente','Media',6),
(10,'Optimización Cadena Suministro','Mejora de la cadena de suministro','2025-10-01','2026-03-31','Pendiente','Alta',12);

--
-- Tabla Actividades
--
DROP TABLE IF EXISTS Actividades;
CREATE TABLE Actividades (
    actividad_id INTEGER PRIMARY KEY AUTOINCREMENT,
    proyecto_id INTEGER NOT NULL,
    nombre TEXT(100) NOT NULL,
    descripcion TEXT,
    fecha_inicio DATE,
    fecha_fin_prevista DATE,
    estado TEXT CHECK (estado IN ('Pendiente','En Progreso','Finalizado')) DEFAULT 'Pendiente',
    prioridad TEXT CHECK (prioridad IN ('Baja','Media','Alta','Urgente')) DEFAULT 'Media',
    FOREIGN KEY (proyecto_id) REFERENCES Proyectos (proyecto_id) ON DELETE CASCADE
);

-- Índice para mejorar rendimiento
CREATE INDEX idx_actividades_proyecto ON Actividades (proyecto_id);

--
-- Datos para la tabla Actividades
--
INSERT INTO Actividades (actividad_id, proyecto_id, nombre, descripcion, fecha_inicio, fecha_fin_prevista, estado, prioridad) VALUES
(1,1,'Análisis de Requisitos','Definición de requisitos del nuevo sitio web','2025-01-15','2025-01-31','Finalizado','Alta'),
(2,1,'Diseño UI/UX','Creación de wireframes y diseños','2025-02-01','2025-02-28','Finalizado','Alta'),
(3,1,'Desarrollo Frontend','Implementación del diseño en HTML/CSS/JS','2025-03-01','2025-03-31','Finalizado','Alta'),
(4,1,'Pruebas y Correcciones','Pruebas de usabilidad y correcciones','2025-04-01','2025-04-30','Finalizado','Alta'),
(5,2,'Análisis de Requisitos App','Definición de funcionalidades de la app','2025-02-01','2025-02-28','Finalizado','Alta'),
(6,2,'Diseño de Interfaz','Diseño de interfaces para la app','2025-03-01','2025-03-31','Finalizado','Alta'),
(7,2,'Desarrollo Backend','Implementación de APIs y servicios','2025-04-01','2025-05-31','Finalizado','Alta'),
(8,2,'Desarrollo Frontend','Desarrollo de interfaces de usuario','2025-05-01','2025-06-30','En Progreso','Alta'),
(9,2,'Pruebas Integración','Pruebas de integración entre módulos','2025-07-01','2025-07-31','Pendiente','Alta'),
(10,3,'Definición de Estrategia','Definir estrategia de marketing','2025-03-15','2025-03-31','Finalizado','Alta'),
(11,3,'Creación de Contenidos','Desarrollo de contenidos para campaña','2025-04-01','2025-04-30','Finalizado','Media'),
(12,3,'Lanzamiento Campaña','Lanzamiento oficial de la campaña','2025-05-01','2025-05-15','Finalizado','Alta'),
(13,3,'Seguimiento y Análisis','Monitoreo y análisis de resultados','2025-05-16','2025-06-15','Finalizado','Media'),
(14,4,'Evaluación de Soluciones','Análisis de diferentes soluciones CRM','2025-04-01','2025-04-30','Finalizado','Alta'),
(15,4,'Implementación Sistema','Instalación y configuración del sistema','2025-05-01','2025-06-30','Finalizado','Alta'),
(16,4,'Migración de Datos','Migración de datos al nuevo sistema','2025-07-01','2025-08-31','En Progreso','Alta'),
(17,4,'Capacitación Usuarios','Capacitación a usuarios finales','2025-09-01','2025-10-31','Pendiente','Media'),
(18,5,'Análisis Situación Actual','Diagnóstico de la situación actual','2025-05-15','2025-06-15','Finalizado','Alta'),
(19,5,'Definición de Objetivos','Establecimiento de objetivos estratégicos','2025-06-16','2025-07-15','Finalizado','Alta'),
(20,5,'Desarrollo de Estrategias','Elaboración de estrategias','2025-07-16','2025-08-31','En Progreso','Alta'),
(21,6,'Mapeo de Procesos','Identificación y mapeo de procesos actuales','2025-06-01','2025-07-15','Finalizado','Alta'),
(22,6,'Selección de Herramientas','Evaluación y selección de herramientas','2025-07-16','2025-08-31','Finalizado','Media'),
(23,6,'Implementación Fase 1','Primera fase de implementación','2025-09-01','2025-10-31','En Progreso','Alta'),
(24,6,'Implementación Fase 2','Segunda fase de implementación','2025-11-01','2025-11-30','Pendiente','Alta'),
(25,6,'Capacitación','Capacitación a usuarios en nuevos procesos','2025-11-01','2025-11-30','Pendiente','Media');

--
-- Tabla Asignaciones
--
DROP TABLE IF EXISTS Asignaciones;
CREATE TABLE Asignaciones (
    asignacion_id INTEGER PRIMARY KEY AUTOINCREMENT,
    actividad_id INTEGER NOT NULL,
    usuario_id INTEGER NOT NULL,
    fecha_asignacion DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (actividad_id) REFERENCES Actividades (actividad_id) ON DELETE CASCADE,
    FOREIGN KEY (usuario_id) REFERENCES Usuarios (usuario_id)
);

-- Índices para mejorar rendimiento
CREATE INDEX idx_asignaciones_actividad ON Asignaciones (actividad_id);
CREATE INDEX idx_asignaciones_usuario ON Asignaciones (usuario_id);

--
-- Datos para la tabla Asignaciones
--
INSERT INTO Asignaciones VALUES
(1,1,1,'2025-01-15 00:00:00'),
(2,1,5,'2025-01-15 00:00:00'),
(3,2,2,'2025-02-01 00:00:00'),
(4,2,10,'2025-02-01 00:00:00'),
(5,3,9,'2025-03-01 00:00:00'),
(6,3,5,'2025-03-01 00:00:00'),
(7,4,1,'2025-04-01 00:00:00'),
(8,4,9,'2025-04-01 00:00:00'),
(9,5,5,'2025-02-01 00:00:00'),
(10,6,2,'2025-03-01 00:00:00'),
(11,7,9,'2025-04-01 00:00:00'),
(12,8,1,'2025-05-01 00:00:00'),
(13,8,9,'2025-05-01 00:00:00'),
(14,10,3,'2025-03-15 00:00:00'),
(15,11,3,'2025-04-01 00:00:00');

-- Resumen general del dashboard
DROP VIEW IF EXISTS vista_dashboard_resumen;
CREATE VIEW vista_dashboard_resumen AS
SELECT
    -- Proyectos activos (En Progreso)
    (SELECT COUNT(*) FROM Proyectos WHERE estado = 'En Progreso') AS proyectos_activos,

    -- Total de actividades
    (SELECT COUNT(*) FROM Actividades) AS total_tareas,

    -- Actividades completadas
    (SELECT COUNT(*) FROM Actividades WHERE estado = 'Finalizado') AS tareas_completadas,

    -- Actividades próximas a vencer (próximos 7 días y no finalizadas)
    (SELECT COUNT(*)
     FROM Actividades
     WHERE estado != 'Finalizado'
       AND fecha_fin_prevista IS NOT NULL
       AND DATE(fecha_fin_prevista) BETWEEN DATE('now') AND DATE('now', '+7 days')
    ) AS tareas_proximas_vencer;

select *
from Usuarios;