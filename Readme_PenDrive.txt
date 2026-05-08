┌─────────────────────────┐
│ Pen Drive — Guía de uso │
└─────────────────────────┘

* Conector y hardware
  ───────────────────

┌───────────────────┬─────────────────┬────────────────────────────┐
│ Señal             │ Pin             │ Nota                       │
├───────────────────┼─────────────────┼────────────────────────────┤
│ D− / D+           │ PA11 / PA12     │ USB OTG HS, PHY integrado  │
│ VBUS sense        │ PA9             │ Entrada — detecta tensión  │
│ ID                │ PA10            │ OTG ID                     │
│ Alimentación VBUS │ PI15 (FS_PW_SW) │ Activo-bajo: LOW = VBUS ON │
└───────────────────┴─────────────────┴────────────────────────────┘
El pen drive debe conectarse al conector CN3 (USB-C).
El equipo actúa siempre como Host (suministra VBUS al pen drive).
Formato admitido: FAT32. Los nombres de directorio son insensibles a mayúsculas.

* Secuencia de arranque
  ─────────────────────
Encendido
  │
  ├─ 0   – 500 ms   Espera estabilización
  ├─ 500 – 1500 ms  Ventana de detección USB
  │                 Si no se detecta pen drive → arranque normal
  │
  └─ USB detectado, se ejecutan los bloques siguientes en orden:
       1. GEN_UPDATE      (actualización firmware generador)
       2. Service         (importar config de servicio, solo si también hay SD)
       3. Exportar Log    (exportar config a pen drive)
       4. Arranque normal (carga config de SD)

* Estructura de directorios en el pen drive
  ─────────────────────────────────────────

PENDRIVE (1:/)
│
├── GEN_UPDATE/   *.hex - (Intel HEX). Actualización firmware del generador (PSoC) 
│
├── Service/      Importar configuración de servicio al equipo (su cintenido se copia en la SD del equipo)
│   ├── Hw/       *.hwr - Configuración hardware del equipo              
│   ├── Menu/     *.mnu - Configuración menú (terapias disponibles)
│   ├── Modes/    *.mod - Configuración de cada terapia                 
│   ├── Params/   *.prm - Parámetros de calibración del equipo
│   ├── Syringe/  *.syr - Caracterización de las jeringas reconocidas por el equipo
│   └── User/     *.usr - Configuración realizadad por el usuario (generación basada en sensor de O3 o no)
│
└── Log/          Directorio creado por el equipo al exportar. Copia de SD (0:/Config/*) generada automáticamente
    └── (copia de 0:/Config/* generada automáticamente)

Solo se activan los bloques cuyos directorios existen en el pen drive.
Los subdirectorios de `Service/` son independientes — pueden estar presentes solo algunos.

Notas:
 - En cada directorio de Service/ se carga el primer archivo con la extensión correspondiente que encuentre (FindFirstFileWithExt), salvo Params/ que espera el nombre fijo params.prm.
 - Log/ no tiene formato propio — es una copia de respaldo de la SD; el equipo la crea al exportar.
 - Todos los formatos (.hwr, .mnu, .mod, .prm, .syr, .usr) son ficheros texto tipo INI con secciones [section] y pares clave=valor.
  
