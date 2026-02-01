# MatCom Invasion 🚀👾

Un juego de invasión espacial tipo **Space Invaders** desarrollado en C usando la biblioteca ncurses para interfaz de terminal.

## 📋 Descripción

Los cielos oscuros sobre la Tierra anuncian una amenaza inminente: una invasión alienígena está en marcha. Los enemigos del espacio exterior han lanzado una ofensiva contra la Facultad de Matemática y Computación de la Universidad de la Habana, y la humanidad depende de tus habilidades para repeler la invasión.

MatCom Invasion es un juego arcade clásico donde controlas una nave espacial para defender la tierra de invasores alienígenas. Dispara a los enemigos, esquiva colisiones y alcanza el puntaje más alto.

### Características

- ✨ Interfaz de terminal con colores usando ncurses
- 🎮 Sistema de jugador con 3 vidas
- 👾 Múltiples tipos de enemigos alienígenas
- 💥 Sistema de colisiones y disparos
- 🏆 Tabla de puntajes altos (Top 10)
- 💾 Sistema de guardado y carga de partidas
- ⏸️ Menú de pausa con opciones de guardado
- 🎯 Algoritmo LRU para gestión de frames de enemigos
- 🧵 Arquitectura multihilo (concurrencia con pthread)

## 🛠️ Requisitos

### Dependencias

- GCC (compilador de C)
- Make
- libncurses5-dev (biblioteca ncurses)
- pthread (para multithreading)

### Instalación de Dependencias

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y libncurses5-dev libncursesw5-dev gcc make
```

O simplemente usa:
```bash
make install-deps
```

#### Arch Linux
```bash
sudo pacman -S ncurses gcc make
```

#### Fedora
```bash
sudo dnf install ncurses-devel gcc make
```

## 🚀 Compilación y Ejecución

### Compilar el juego
```bash
make
```

### Compilar y ejecutar directamente
```bash
make run
```

### Compilar versión de debug
```bash
make debug
```

### Compilar versión optimizada (release)
```bash
make release
```

## 🎮 Controles

### Menú Principal
- **↑/↓**: Navegar por las opciones
- **Espacio**: Seleccionar opción
- **Letras/Números**: Ingresar nombre de jugador

### Durante el Juego
- **A / ←**: Mover izquierda
- **D / →**: Mover derecha
- **W / ↑**: Mover arriba
- **S / ↓**: Mover abajo
- **Espacio**: Disparar
- **Q**: Pausar juego

### Menús (Pausa/Game Over)
- **↑/↓**: Navegar opciones
- **Espacio**: Seleccionar
- **S**: Volver al menú principal (en pantalla de scores)

## 📁 Estructura del Proyecto

```
MatCom-Invasion/
├── src/               # Código fuente (.c)
│   ├── main.c        # Punto de entrada
│   ├── game.c        # Lógica del juego
│   ├── draw.c        # Funciones de dibujo
│   ├── input.c       # Manejo de entrada
│   └── score.c       # Sistema de puntajes
├── include/          # Archivos de cabecera (.h)
│   ├── types.h       # Definiciones de tipos
│   ├── config.h      # Configuraciones
│   ├── game.h
│   ├── draw.h
│   ├── input.h
│   └── score.h
├── data/             # Datos del juego
│   ├── high_scores.txt   # Puntajes altos
│   └── saved_game.dat    # Partida guardada
├── build/            # Archivos objeto (.o)
├── bin/              # Ejecutable compilado
├── Makefile          # Sistema de compilación
└── README.md         # Este archivo
```

## 🎯 Opciones del Menú Principal

1. **START NEW GAME**: Iniciar nueva partida
2. **START SAVED GAME**: Cargar partida guardada
3. **BEST SCORES**: Ver tabla de mejores puntajes
4. **EXIT**: Salir del juego

## 💾 Sistema de Guardado

El juego guarda automáticamente:
- **Puntajes altos**: Se guardan automáticamente al terminar cada partida
- **Partida**: Puedes guardar tu progreso desde el menú de pausa

Ubicaciones:
- Puntajes: `data/high_scores.txt`
- Partida guardada: `data/saved_game.dat`

## 🏗️ Desarrollo

### Limpiar archivos de compilación
```bash
make clean
```

### Limpieza completa
```bash
make distclean
```

### Ver ayuda del Makefile
```bash
make help
```

## 🔧 Configuración

Puedes modificar la configuración del juego editando `include/config.h`:

- `DELAY`: Velocidad del juego (microsegundos entre frames)
- `NUMBER_BULLETS`: Máximo de balas simultáneas
- `NUMBER_ALIENS`: Máximo de aliens simultáneos
- `MAX_HIGH_SCORES`: Cantidad de puntajes a guardar

## 📝 Conceptos Implementados

### Programación Orientada a Eventos
El juego utiliza programación orientada a eventos para manejar las interacciones del jugador con el entorno. Detecta eventos como entrada del teclado y responde apropiadamente (mover nave, disparar, etc.).

### Concurrencia Mediante Hilos
Se utilizan dos hilos principales para ejecutar tareas en paralelo:
- **Hilo de juego**: Actualiza estado del juego y renderiza gráficos
- **Hilo de entrada**: Maneja input del usuario en tiempo real
- **Sincronización**: Usa mutex (`pthread_mutex`) para evitar condiciones de carrera

### Algoritmo de Reemplazo de Páginas (LRU)
Implementa el algoritmo **Least Recently Used (LRU)** para determinar dónde aparecerán los enemigos en el campo de juego. Gestiona frames de memoria para los aliens de manera eficiente.

### Algoritmo Next-Fit
Usado para asignación eficiente de recursos:
- Gestión de bullets (disparos)
- Gestión de aliens activos
- Optimiza el uso de memoria del juego

## 🐛 Solución de Problemas

### El juego no compila
- Verifica que tengas instaladas todas las dependencias
- Asegúrate de tener permisos de escritura en el directorio

### Errores de ncurses
```bash
# Reinstalar ncurses
sudo apt-get install --reinstall libncurses5-dev
```

### La terminal se ve mal después de salir
```bash
# Resetear la terminal
reset
```

### Error: "No such file or directory" al ejecutar
```bash
# Asegúrate de que la carpeta data existe
mkdir -p data
```

## 👥 Autor

Proyecto desarrollado como parte del curso de Sistemas Operativos en la Facultad de Matemática y Computación, Universidad de La Habana.

## 📜 Licencia

Este proyecto es de código abierto para fines educativos.

---

**¡Disfruta del juego y defiende MatCom! 🌍🚀**
