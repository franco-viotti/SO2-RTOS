# Trabajo Práctico Nº 4 - Sistemas Operativos II

## Ingeniería en Computación
## Facultad de Ciencias Exactas, Físicas y Naturales
## Universidad Nacional de Córdoba

### Descripción
Este repositorio contiene el código y la documentación correspondiente al Trabajo Práctico Nº 4 de la materia Sistemas Operativos II. El objetivo de este trabajo es poder diseñar, crear, comprobar y validar una aplicación de tiempo real sobre un RTOS, en este caso FreeRTOS.
### Requisitos
- [QEMU](https://www.qemu.org/)
- [Make](https://www.gnu.org/software/make/)

### Instalación
1. Clonar el repositorio:
  ```bash
  git clone git@github.com:franco-viotti/SO2-RTOS.git # Por SSH

  # ó

  git clone https://github.com/franco-viotti/SO2-RTOS.git # Por HTTPS
  ```

### Uso
1. Para compilar y correr el proyecto, simplemente debe correrse el script `build-and-run.sh`:
  ```bash
  # Una vez clonado el repositorio, ejecutar:
  cd SO2-RTOS
  
  chmod +x build-and-run.sh # En caso de que no tenga permisos de ejecución

  ./build-and-run.sh
  ```

### Autor
- Franco Viotti

### Licencia
Este proyecto está bajo la Licencia Apache 2.0. Consulte el archivo [LICENSE](./LICENSE) para más detalles.

### Informe
El informe del trabajo práctico se encuentra en el archivo [report](./report.pdf).