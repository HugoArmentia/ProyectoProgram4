BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "Usuario" (
	"id"	INTEGER,
	"nombre"	TEXT NOT NULL,
	"tipo"	TEXT NOT NULL CHECK("tipo" IN ('Paciente', 'Medico', 'Admin')),
	"password"	TEXT NOT NULL,
	"email"	TEXT,
	"telefono"	TEXT,
	"direccion"	TEXT,
	"fecha_registro"	TEXT DEFAULT (datetime('now')),
	PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "citas" (
	"id"	INTEGER,
	"paciente_id"	INTEGER NOT NULL,
	"medico_id"	INTEGER NOT NULL,
	"fecha"	TEXT NOT NULL,
	"estado"	TEXT CHECK("estado" IN ('Programada', 'Completada', 'Cancelada')),
	"motivo"	TEXT,
	"fecha_modificacion"	TEXT DEFAULT (datetime('now')),
	PRIMARY KEY("id" AUTOINCREMENT),
	FOREIGN KEY("medico_id") REFERENCES "Usuario"("id"),
	FOREIGN KEY("paciente_id") REFERENCES "Usuario"("id")
);
CREATE TABLE IF NOT EXISTS "historial_citas" (
	"id"	INTEGER,
	"paciente_id"	INTEGER NOT NULL,
	"medico_id"	INTEGER NOT NULL,
	"fecha"	TEXT NOT NULL,
	"estado"	TEXT CHECK("estado" IN ('Programada', 'Completada', 'Cancelada')),
	"motivo"	TEXT,
	"fecha_modificacion"	TEXT DEFAULT 'datetime(''now'')',
	PRIMARY KEY("id" AUTOINCREMENT),
	FOREIGN KEY("medico_id") REFERENCES "Usuario"("id"),
	FOREIGN KEY("paciente_id") REFERENCES "Usuario"("id")
);
CREATE TABLE IF NOT EXISTS "logs" (
	"id"	INTEGER,
	"tipo_evento"	TEXT NOT NULL,
	"descripcion"	TEXT,
	"usuario_id"	INTEGER,
	"fecha_evento"	TEXT DEFAULT (datetime('now')),
	PRIMARY KEY("id" AUTOINCREMENT),
	FOREIGN KEY("usuario_id") REFERENCES "Usuario"("id")
);
COMMIT;
