/*----------------------------------------------------------------------------------------------------------------------------------------
Estudiantes:
  - Diego José Masís Quirós - 2020403956.
  - Kevin Jose Rodriguez Lanuza - 2016112117.
Asignación: Proyecto 2 - Emulación de un empaquetador TAR.

*** Esta tarea fue realizada en el IDE Visual Studio Code y compilada a traves del WSL de Windows 11. ***

INSTRUCCIONES:

El siguiente programa permite empaquetar archivos dentro de un archivo tar y realizar diferentes tipos de manipulaciones de los mismos.

Para utilizar el programa primero debe compilar el archivo mediante el siguiente comando:

    g++ Proyecto_2.c

Luego ejecute la busqueda de la siguiente manera:

    ./a.out star <comandos> <nombre del archivo tar a manejar> <nombres del (los) archivos(s) a mejar>

Notas:

Los comandos permitidos son los siguientes.

1- <-c> Crear un archivo tar.
2- <-x> Extraer un archivo del interior un archivo tar.
3- <-t> Listar el contenido de un archivo tar.
4- <-d> Borrar un archivo del interior de un archivo tar.
5- <-u> Sobreescribir un archivo del interior de un archivo tar.
6- <-v> Ver el reporte de acciones.
7- <-f> Empacar archivos al crear un archivo tar.
8- <-r> Agregar archivos adicionales al interior de un archivo tar.
9- <-p> Desfragmentar un archivo tar.

Algunos ejemplos:
                              ./a.out star -cfv paquete.tar mifamilia.jpg reporte.pdf apuntes.txt
                              ./a.out star -l empaquetados.tar
                              ./a.out star -dl mipaquete.tar proyecto4.docx 

----------------------------------------------------------------------------------------------------------------------------------------*/

// IMPORTACIÓN DE LIBRERÍAS.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include<unistd.h>// -SLEEP

// DEFINICIÓN DE "CONSTANTES".

#define OPTIONS \
"                       +--------------------------------------+\n" \
"                       |                                      |\n" \
"                       |         ¡¡¡¡¡¡¡ ERROR !!!!!!!        |\n" \
"                       |                                      |\n" \
"                       +--------------------------------------+\n\n" \
"En este momento usted tiene las siguientes opciones disponibles:\n\n" \
"1- <-c> Crear un archivo tar.\n" \
"2- <-x> Extraer un archivo del interior un archivo tar.\n" \
"3- <-t> Listar el contenido de un archivo tar.\n" \
"4- <-d> Borrar un archivo del interior de un archivo tar.\n" \
"5- <-u> Sobreescribir un archivo del interior de un archivo tar.\n" \
"6- <-v> Ver el reporte de acciones.\n" \
"7- <-f> Empacar archivos al crear un archivo tar.\n" \
"8- <-r> Agregar archivos adicionales al interior de un archivo tar.\n" \
"9- <-p> Desfragmentar un archivo tar.\n\n" \
"El formato general en que debe ser utilizado el programa debe ser el siguiente:\n\n" \
"    ./<nombre del ejecutable>.out star <comandos> <nombre del tar> <nombres de archivos>\n\n" \
"Usted puede combinar diferentes comandos en caso de desearlo. Por ejemplo:\n\n" \
"    ./a.out star -cfv paquete.tar archivo1.txt archivo2.jpg archivo3.pdf\n\n" \
"Que desea hacer a continuación???:\n\n"

// DEFINICIÓN DE ESTRUCTURAS.

struct Archivo{
    char nombre[256];
    long peso;
    long inicio;
    long fin;
};

struct Espacio{
    long inicio;
    long fin;
};

// DEFINICIÓN DE FUNCIONES.

// Funciones de segundo nivel para el funcionamiento de las funciones principales.

void actualizarRegistros(FILE* archivoTAR, const char* nombre, long peso, long inicio, long fin){
  
  // Se mueve el cursor al principio del archivo.

  fseek(archivoTAR, 0, SEEK_SET);

  // Se crea una arreglo para contener los datos entrantes.

  struct Archivo archivos[100];

  // Se lee los datos existentes del archivo.

  fread(archivos, sizeof(struct Archivo), 100, archivoTAR);

  // Se busca el primer índice disponible (con valores de inicialización).

  int indiceDisponible = -1;
  for(int i = 0; i < 100; i++){
    if(strcmp(archivos[i].nombre, "") == 0){
      indiceDisponible = i;
      break;
    }
  }

  // Si se encuentra un índice disponible actualiza el elemento en el índice encontrado.

  if(indiceDisponible != -1){
    strcpy(archivos[indiceDisponible].nombre, nombre);
    archivos[indiceDisponible].peso = peso;
    archivos[indiceDisponible].inicio = inicio;
    archivos[indiceDisponible].fin = fin;

    for(int i = 90; i < 100; i++){
      strcpy(archivos[i].nombre, "");
      archivos[i].peso = -1;
      archivos[i].inicio = -1;
      archivos[i].fin = -1;
    }

    // Se mueve el cursor al principio del archivo antes de escribir los datos actualizados en el archivo tar.

    fseek(archivoTAR, 0, SEEK_SET);

    // Se escriben los datos actualizados en el archivo.

    fwrite(archivos, sizeof(struct Archivo), 100, archivoTAR);

  }else{

    printf("No hay índices disponibles en el archivo TAR para actualizar.\n");
    
  }
}

void actualizarEspacios(FILE* archivoTAR, long inicioTemp, long finTemp){

  // Se mueve el cursor al principio del arreglo "Espacios".

  fseek(archivoTAR, sizeof(struct Archivo) * 100, SEEK_SET);

  // Se crea una arreglo para contener los datos entrantes.

  struct Espacio espacios[100];

  // Se lee los datos existentes del archivo.

  fread(espacios, sizeof(struct Espacio), 100, archivoTAR);

  // Se busca el primer índice disponible en "Espacios" (inicio y fin igual a -1).

  for (int i = 0; i < 100; i++) {

    // Si se encuentra un indice disponible se actualiza la información.

    if (espacios[i].inicio == -1 && espacios[i].fin == -1) {
      espacios[i].inicio = inicioTemp;
      espacios[i].fin = finTemp;

      // Se mueve el cursor al principio del arreglo "Espacios" antes de escribir los datos actualizados.

      fseek(archivoTAR, sizeof(struct Archivo) * 100, SEEK_SET);

      // Se escribe la información actualizada.

      fwrite(espacios, sizeof(struct Espacio), 100, archivoTAR);

      // Se finaliza el proceso.

      break;
    }
  }
}

int compararPorInicio(const void* a, const void* b){

  // Se convierten los punteros a structs Espacio.

  const struct Espacio* espacioA = (const struct Espacio*)a;
  const struct Espacio* espacioB = (const struct Espacio*)b;

  // Se comparan los campos 'inicio' de las estructuras.

  if(espacioA->inicio != -1 && espacioB->inicio != -1){

    // Si los dos espacios tienen un inicio válido, se comparan.

    if(espacioA->inicio < espacioB->inicio){

      return -1;

    }else if(espacioA->inicio > espacioB->inicio){

      return 1;

    }else{

      return 0;

    }
  }else if(espacioA->inicio != -1 && espacioB->inicio == -1){

    // Espacio A va antes que el espacio B.

    return -1;

  }else if(espacioA->inicio == -1 && espacioB->inicio != -1){

    // Espacio B va antes que el espacio A

    return 1;

  }else{

    return 0;
    
  }
}

void concatenarEspaciosContiguos(const char* nombreTAR){

  // Se abre el archivo TAR.

  FILE* archivoTAR = fopen(nombreTAR, "rb+");

  if(archivoTAR == NULL){
    printf("Error al abrir el archivo tar: %s\n", nombreTAR);
    return;
  }

  // Se mueve el cursor al principio del arreglo "Espacios".

  fseek(archivoTAR, sizeof(struct Archivo) * 100, SEEK_SET);

  // Se crea un arreglo para contener la información de los Espacios del archivo TAR.

  struct Espacio Espacios[100];

  // Se lee la información de los Espacios del archivo TAR.

  fread(Espacios, sizeof(struct Espacio), 100, archivoTAR);

  // Se ordena la matriz "Espacios" por el valor de inicio.

  qsort(Espacios, 100, sizeof(struct Espacio), compararPorInicio);

  // Se crean variables para almacenar la información de los espacios contiguos.

  int contiguos = 0;
  int indiceInicio = -1;
  int inicioPrimero = -1;
  int finUltimo = -1;

  // Se recorre la matriz "Espacios" para encontrar los espacios contiguos.

  for(int i = 0; i < 100; i++){

    // Si el espacio actual no está vacío, se comprueba si es contiguo al anterior.

    if(Espacios[i].inicio != -1 && Espacios[i].fin != -1){

      // Si el espacio anterior está vacío, se guarda el índice de inicio y el inicio del primer espacio.

      if(indiceInicio == -1){
        indiceInicio = i;
        inicioPrimero = Espacios[i].inicio;
      }

      // Se guarda el fin del último espacio.

      finUltimo = Espacios[i].fin;
      contiguos++;

    }else{

      // Si el espacio actual está vacío, se comprueba si los espacios anteriores eran contiguos.

      if(contiguos > 0){

        // Se eliminan los espacios antiguos en el rango.

        for(int j = indiceInicio; j <= i - 1; j++){
          Espacios[j].inicio = -1;
          Espacios[j].fin = -1;
        }

        // Se crea un nuevo espacio que abarque desde el inicio del primero al fin del último.

        Espacios[indiceInicio].inicio = inicioPrimero;
        Espacios[indiceInicio].fin = finUltimo;
      }

      // Se reinician las variables para almacenar la información de los espacios contiguos.

      contiguos = 0;
      indiceInicio = -1;
      inicioPrimero = -1;
      finUltimo = -1;
    }
  }

  // Se comprueba si los espacios contiguos llegan hasta el final.

  if (contiguos > 0) {
    
      // Se eliminan los espacios antiguos en el rango.

      for (int j = indiceInicio; j <= 99; j++) {
          Espacios[j].inicio = -1;
          Espacios[j].fin = -1;
      }

      // Se crea un nuevo espacio que abarque desde el inicio del primero al fin del último.

      Espacios[indiceInicio].inicio = inicioPrimero;
      Espacios[indiceInicio].fin = finUltimo;
  }

  // Se ordena nuevamente la matriz para mover los espacios con inicio igual a -1 al final.

  qsort(Espacios, 100, sizeof(struct Espacio), compararPorInicio);

  // Se mueve el cursor al principio del arreglo "Espacios" antes de escribir los datos actualizados.

  fseek(archivoTAR, sizeof(struct Archivo) * 100, SEEK_SET);

  // Se escriben los datos actualizados de los espacios en el archivo TAR.

  fwrite(Espacios, sizeof(struct Espacio), 100, archivoTAR);

  // Se cierra el archivo TAR cuando se haya terminado.
  
  fclose(archivoTAR);
}

// Funciones de primer nivel referentes a los comandos principales.

// Función referente al comando <-c>.
void crearTAR(char* nombreTAR, bool v_command){

  // Se crean e inicializan las estructuras de registros de información.

  if(v_command) printf("\nINICIA LA CREACIÓN DEL ARCHIVO TAR.\n\n");

  struct Archivo archivos[100];

  if(v_command) printf("- El arreglo de datos de archivos ha sido declarado.\n");

  struct Espacio espacios[100];

  if(v_command) printf("- El arreglo de datos de espacios ha sido declarado.\n");

  for (int i = 0; i < 100; i++) {
    snprintf(archivos[i].nombre, sizeof(archivos[i].nombre), "");
    archivos[i].peso = -1;
    archivos[i].inicio = -1;
    archivos[i].fin = -1;

    espacios[i].inicio = -1;
    espacios[i].fin = -1;
  }

  if(v_command) printf("- Los arreglos de datos de archivos y espacios han sido inicializados.\n");

  // Se crea el archivo tar solicirado.

  FILE* archivoTAR = fopen(nombreTAR, "wb");

  if(v_command) printf("- El archivo '%s' ha sido abierto.\n", nombreTAR);

  if (archivoTAR == NULL) {
    printf("\nError al abrir el archivo '%s'.\n", nombreTAR);
    exit(1);
  }

  // Se escriben las estructuras de registro inicializadas dentro del archivo tar.

  fwrite(archivos, sizeof(struct Archivo), 100, archivoTAR);
  if(v_command) printf("- Se ha escrito el arreglo de datos de archivos en el archivo '%s'.\n", nombreTAR);
  fwrite(espacios, sizeof(struct Espacio), 100, archivoTAR);
  if(v_command) printf("- Se ha escrito el arreglo de espacios de archivos en el archivo '%s'.\n", nombreTAR);

  // Se cierra el archivo.

  fclose(archivoTAR);

  // Final de proceso.

  if(v_command) printf("- El archivo '%s' ha sido cerrado.\n", nombreTAR);

  if(v_command) printf("\nFINALIZA LA CREACIÓN DEL ARCHIVO TAR.\n\n");
}

// Función referente al comando <-f>.
void empacarArchivos(char* nombreTAR, const char* archivosEntrantes[], int cantidadArchivos, bool v_command){

  // Se crea un archivo tar para contener los archivos.

  crearTAR(nombreTAR, v_command);

  if(v_command) printf("\nINICIA LA EMPAQUETACIÓN DE ARCHIVOS.\n\n");
  
  FILE* archivoTAR = fopen(nombreTAR, "rb+");
  if(!archivoTAR){
    printf("Error al crear el archivo tar.\n");
    return;
  }

  if(v_command) printf("\n- El archivo %s ha sido abierto.\n", nombreTAR);

  // Se escribie la información de cada archivo por empaquetar en el archivo tar.

  for(int i = 0; i < cantidadArchivos; i++){

    // Se abre el archivo entrante para leer su información.

    FILE* archivoEntrante = fopen(archivosEntrantes[i], "rb");

    if(!archivoEntrante){
      printf("Error al abrir el archivo: '%s'\n", archivosEntrantes[i]);
      continue;
    }

    if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n", archivosEntrantes[i]);

    // Se obtiene el tamaño del archivo entrante.

    fseek(archivoEntrante, 0, SEEK_END);
    long tamanoArchivo = ftell(archivoEntrante);
    fseek(archivoEntrante, 0, SEEK_SET);

    if(v_command) printf("\n- El peso '%ld' del archivo '%s' ha sido obtenido.\n", tamanoArchivo, archivosEntrantes[i]);

    // Se obtiene el byte de inicio de escritura.

    fseek(archivoTAR, 0, SEEK_END);
    long byteInicio = ftell(archivoTAR);

    if(v_command) printf("\n- El byte de inicio '%ld' del archivo '%s' ha sido obtenido.\n", byteInicio, archivosEntrantes[i]);

    // Se obtiene el byte de final de escritura.

    long byteFinal = byteInicio + tamanoArchivo;

    if(v_command) printf("\n- El byte final '%ld' del archivo '%s' ha sido obtenido.\n", byteFinal, archivosEntrantes[i]);

    // Se procede a escribir el contenido del archivo entrante en el archivo TAR.

    char buffer[1024];
    size_t bytesLeidos;
    while((bytesLeidos = fread(buffer, sizeof(char), sizeof(buffer), archivoEntrante)) > 0){
      fwrite(buffer, sizeof(char), bytesLeidos, archivoTAR);
    }

    if(v_command) printf("\n- El comtenido del archivo '%s' ha sido escrito en el archivo %s.\n", archivosEntrantes[i], nombreTAR);

    // Se agrega un byte vacío antes de escribir el siguiente archivo.

    char byteVacio = 0;
    fwrite(&byteVacio, sizeof(char), 1, archivoTAR);

    // Se cierra el archivo entrante.

    fclose(archivoEntrante);

    if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", archivosEntrantes[i]);

    // Se actualizan los datos en los registros de archivos empaquetados.

    actualizarRegistros(archivoTAR, archivosEntrantes[i], tamanoArchivo, byteInicio, byteFinal);

    if(v_command) printf("\n- Los datos han sido actualizados en los registros de archivos de '%s'.\n", nombreTAR);

  }

  // Se cierra el archivo TAR.

  fclose(archivoTAR);

  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", nombreTAR);

  // Fin del proceso.

  if(v_command) printf("\nFINALIZA LA EMPAQUETACIÓN DE ARCHIVOS.\n\n");
}

// Función referente al comando <-t>.
void listarTAR(const char* nombreTAR, bool v_command){

  if(v_command) printf("\nINICIA EL LISTADO DEL CONTENIDO DEL ARCHIVO TAR.\n\n");

  // Se abre el archivo TAR que se debe revisar.

  FILE* archivoTAR = fopen(nombreTAR, "rb");

  if(archivoTAR == NULL){
    printf("Error al abrir el archivo '%s'.\n", nombreTAR);
    return;
  }

  if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n\n", nombreTAR);

  // Se leen los datos de cada registro.

  struct Archivo archivo;
  bool control = false; // Variable de control para verificar si se encontró al menos un archivo

  for (int i = 0; i < 100; i++) {
    if (fread(&archivo, sizeof(struct Archivo), 1, archivoTAR) != 1) {
      break;  // Salir del bucle si no se pudo leer un registro.
    }

    if (strcmp(archivo.nombre, "") != 0) {
      printf("-----------------------------------------------------\n");
      //printf("POSICIÓN '%d'.\n", i);
      printf("Nombre del archivo:\n\t\t\t%s\n", archivo.nombre);
      printf("Peso correspondiente:\n\t\t\t%ld bytes.\n", archivo.peso);
      printf("Byte de inicio:\n\t\t\t%ld bytes.\n", archivo.inicio);
      printf("Byte final:\n\t\t\t%ld bytes.\n", archivo.fin);

      control = true; // Indicar que se encontró al menos un archivo.
    }
  }

  if (!control) {
    printf("\nEl archivo '%s' no contiene archivos registrados en este momento.\n", nombreTAR);
  }

  // Se cierra el archivo TAR.

  fclose(archivoTAR);

  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", nombreTAR);

  // Fin del proceso.

  if(v_command) printf("\nFINALIZA EL LISTADO DEL CONTENIDO DEL ARCHIVO TAR.\n\n");
}

// Función referente al comando <-x>.
void extraerArchivos(const char* nombreTAR, bool v_command){

  if(v_command) printf("\nINICIA LA EXTRACCIÓN DEL CONTENIDO DEL ARCHIVO TAR.\n\n");

  // Se abre el archivo TAR.

  FILE* archivoTAR = fopen(nombreTAR, "rb");

  if (!archivoTAR){
    printf("Error al abrir el archivo tar.\n");
    return;
  }

  if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n", nombreTAR);

  // Se crea un directorio para los archivos extraídos.

  char directorioExtraccion[100];
  snprintf(directorioExtraccion, sizeof(directorioExtraccion), "%s_desempacado", nombreTAR);
  mkdir(directorioExtraccion, 0777);

  if(v_command) printf("\n- El directorio '%s_desempacado' ha sido creado.\n", nombreTAR);

  // Se crea una arreglo para contener la información de los registros de archivos.

  struct Archivo archivos[100];

  // Se lee la información de los registros de archivos en el TAR.

  fread(archivos, sizeof(struct Archivo), 100, archivoTAR);

  if(v_command) printf("\n- Se inicia la lectura de datos del archivio '%s'.\n", nombreTAR);

  // Se inicia la extracción de todos los archivos contenidos.

  for(int i = 0; i < 100; i++){

    // Se omiten espacios vacíos en la revisión.

    if(archivos[i].nombre[0] == '\0'){
      continue;
    }

    // Se construye la ruta del archivo extraído.

    char rutaExtraccion[200];
    snprintf(rutaExtraccion, sizeof(rutaExtraccion), "%s/%s", directorioExtraccion, archivos[i].nombre);

    if(v_command) printf("\n- Se ha construido la ruta para el archivo a extraer '%s'.\n", archivos[i].nombre);

    // Se crea un archivo en blanco para contener la información del archivo por extraer..

    FILE* archivoExtraido = fopen(rutaExtraccion, "wb");

    if(!archivoExtraido){
      printf("Error al crear el archivo extraído: %s\n", rutaExtraccion);
      continue;
    }

    if(v_command) printf("\n- Se ha abierto un archivo en blanco para alvergar los datos copiados del archivo a extraer '%s'.\n", archivos[i].nombre);

    // Se cobtiene el peso del archivo por extraer.

    long tamanoArchivo = archivos[i].peso;

    // Se mueve el puntero de lectura.

    fseek(archivoTAR, archivos[i].inicio, SEEK_SET);

    // Se leen los datos del archivo por extraer.

    char buffer[1024];
    size_t bytesLeidos;
    
    if(v_command) printf("\n- Se han preparado las estructuras para copiar los datos del archivo a extraer '%s'.\n", archivos[i].nombre);
    if(v_command) printf("\n- Se inicia la copia de datos del archivo '%s' en el archivo en blanco.\n", archivos[i].nombre);

    while(tamanoArchivo > 0 && (bytesLeidos = fread(buffer, sizeof(char), sizeof(buffer), archivoTAR)) > 0){
      size_t bytesPorLeer = bytesLeidos;
      if(bytesPorLeer > tamanoArchivo){
        bytesPorLeer = tamanoArchivo;
      }

      // Se escriben los datos leídos en el archivo en blanco creado previamente.

      fwrite(buffer, sizeof(char), bytesPorLeer, archivoExtraido);
      tamanoArchivo -= bytesPorLeer;
    }

    if(v_command) printf("\n- Se han copiado los datos del archivo '%s' en el archivo en blanco.\n", archivos[i].nombre);
    if(v_command) printf("\n- Se han terminado de extraer el archivo '%s'.\n", archivos[i].nombre);

    // Se cierra el archivo por extraer..

    fclose(archivoExtraido);

    if(v_command) printf("\n- El archivo extraido ha sido cerrado.\n");
  }

  // Se cierra el archivo TAR.

  fclose(archivoTAR);

  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", nombreTAR);

  // Fin del proceso.

  if(v_command) printf("\nFINALIZA LA EXTRACCIÓN DEL CONTENIDO DEL ARCHIVO TAR.\n\n");
}

// Función referente al comando <-d>.
void borrarArchivo(char* nombreTAR, char* archivoPorBorrar, bool v_command){

  if(v_command) printf("\nINICIA EL BORRADO DEL ARCHIVO.\n\n");

  // Se abre el archivo TAR.
  
  FILE* archivoTAR = fopen(nombreTAR, "rb+");

  if(archivoTAR == NULL){
    printf("Error al abrir el archivo tar: %s\n", nombreTAR);
    return;
  }

  if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n", nombreTAR);

  // Se crea un arreglo para contener la información del registro de archivos.

  struct Archivo archivos[100];

  // Se lee la información del registro de archivos en el TAR.

  fread(archivos, sizeof(struct Archivo), 100, archivoTAR);

  if(v_command) printf("\n- Los registros de archivos han sido preparados.\n");

  // Se crean estructuras para guardar información del archivo buscado.

  int indiceEncontrado = -1;
  long inicioTemp = -1;
  long finTemp = -1;

  // Se inicia la busqueda del archivo por borrar.

  if(v_command) printf("\n- Se inicia la busqueda del archivo '%s' en los registros.\n", archivoPorBorrar);

  for(int i = 0; i < 100; i++){

    // Si se encuentra el archivo, se inicia el proceso de "borrado".

    if(strcmp(archivos[i].nombre, archivoPorBorrar) == 0){

      if(v_command) printf("\n- Se ha encontrado el archivo '%s' en los registros.\n", archivoPorBorrar);

      // Se guardan los valores necesarios del archivo encontrado para un posterior uso.

      indiceEncontrado = i;
      inicioTemp = archivos[i].inicio;
      finTemp = archivos[i].fin;

      // Se cambia el espacio de nombre del archivo encontrado al valor por defecto "".

      strcpy(archivos[i].nombre, "");

      // Termina la búsqueda.

      break;
    }
  }

  // Finaliza el proceso de "borrado".

  if(v_command) printf("\n- Se ha borrado el archivo '%s' de los registros.\n", archivoPorBorrar);

  // Se actualiza la información de los registros de archivos dentro del TAR.

  if(indiceEncontrado != -1){

    // Se mueve el cursor al principio del arreglo "Archivos" antes de escribir los datos actualizados.

    fseek(archivoTAR, 0, SEEK_SET);

    // Se escribe la información actualizada de los registros dentro del TAR.

    fwrite(archivos, sizeof(struct Archivo), 100, archivoTAR);

    // Se actualiza la información de los espacios en blanco en el registro "Espacios" dentro del TAR.

    if (inicioTemp != -1 && finTemp != -1) {
        actualizarEspacios(archivoTAR, inicioTemp, finTemp);
    }

    // Se asegura que los cambios se guarden en el archivo TAR.

    fflush(archivoTAR);

    if(v_command) printf("\n- Se han actualizado los registros de espacios en blanco.\n");

  }else{

    printf("No se encontró el archivo %s en el archivo tar.\n", archivoPorBorrar);

  }

  // Se cierra el archivo TAR.

  fclose(archivoTAR);

  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", nombreTAR);

  // Finaliza el proceso completo.

  if(v_command) printf("\nFINALIZA EL BORRADO DEL ARCHIVO.\n\n");
}

// Función referente al comando <-p>.
void desfragmentar(char* nombreTAR, bool v_command){

  if(v_command) printf("\nINICIA EL DESFRAGMENTADO DEL ARCHIVO.\n\n");

  // Se abre el archivo TAR.

  FILE* archivoTAR = fopen(nombreTAR, "rb");

  if(!archivoTAR){
    printf("Error al abrir el archivo tar: %s\n", nombreTAR);
    return;
  }

  if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n", nombreTAR);


  // Se crea un arreglo para contener la información de los registros de archivos.

  struct Archivo archivos[100];

  // Se lee la inforamción de los registros de archivos contenida en el TAR.

  fread(archivos, sizeof(struct Archivo), 100, archivoTAR);

  if(v_command) printf("\n- Se han leido los datos de los registros de archivos.\n");

  // Se crea un arreglo para contener los nombres de los archivos extraidos temporalmente.

  char* archivosExtraidos[100];

  // Se lleva un conteo de los archivos extraídos temporalmente.

  int cantidadArchivosExtraidos = 0;

  // Se inicia el proceso de extracción temporal de archivos.
  
  if(v_command) printf("\n- Se inicia la extracción 'ficticio/temporal' de archivos.\n");

  for(int i = 0; i < 100; i++){

    // Se omiten los espacios vacíos en la revisión.

    if(archivos[i].nombre[0] == '\0'){
      continue;
    }

    // Se construye el nombre del archivo a extraer temporalmente.

    char nombreArchivoExtraido[256];
    snprintf(nombreArchivoExtraido, sizeof(nombreArchivoExtraido), "./%s", archivos[i].nombre);

    if(v_command) printf("\n- Se ha construido el nombre del archivo temporal extraido.\n");

    // Se crea un archivo en blanco para contener la información del archivo a extraer temporalmente.

    FILE* archivoPorExtraer = fopen(nombreArchivoExtraido, "wb");

    if(!archivoPorExtraer){
      printf("Error al crear el archivo extraído: %s\n", nombreArchivoExtraido);
      continue;
    }

    if(v_command) printf("\n- El archivo temporal '%s' ha sido creado.\n", nombreArchivoExtraido);

    // Se inicia la copia del contenido del archivo por extraer temporalmente desde el archivo TAR.

    if(v_command) printf("\n- Se inicia la copia del contenido original al archivo temporal.\n");

    // Se obtiene el peso del archivo por extraer temporalmente.

    long tamanoArchivo = archivos[i].peso;

    // Se mueve el puntero de lectura.

    fseek(archivoTAR, archivos[i].inicio, SEEK_SET);

    // Se leen los bytes del archivo por extraer temporalmente.

    char buffer[1024];
    size_t bytesLeidos;

    while(tamanoArchivo > 0 && (bytesLeidos = fread(buffer, sizeof(char), sizeof(buffer), archivoTAR)) > 0){
      size_t bytesPorLeer = bytesLeidos;
      if(bytesPorLeer > tamanoArchivo){
        bytesPorLeer = tamanoArchivo;
      }

      // Se escriben los datos leídos en el archivo en blanco creado previamente.

      fwrite(buffer, sizeof(char), bytesPorLeer, archivoPorExtraer);
      tamanoArchivo -= bytesPorLeer;
    }

    // Se guardar el nombre del archivo extraído en el arreglo creado previamente.

    archivosExtraidos[cantidadArchivosExtraidos] = strdup(archivos[i].nombre);

    // Se actualiza el contador de archivos extraidos temporalmente.

    cantidadArchivosExtraidos++;

    if(v_command) printf("\n- Se registra el nombre del archivo temporal extraido.\n");

    // Se cierra el archivo por extraer.

    fclose(archivoPorExtraer);

    if(v_command) printf("\n- El archivo temporal '%s' ha sido cerrado.\n", nombreArchivoExtraido);
  }

  // Se cierra el archivo TAR del que se extrajeron los archivos temporalmente.

  fclose(archivoTAR);

  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", nombreTAR);

  // Se empacan los archivos extraidos temporalmente en un archivo TAR nuevo llamado "copia.tar".

  empacarArchivos((char*)"copia.tar", (const char**)archivosExtraidos, cantidadArchivosExtraidos, false);

  if(v_command) printf("\n- Se han empacado en los archivos temporales en un nuevo archivo tar.\n");

  // Se borran los archivos extraidos temporalmente.

  for(int i = 0; i < cantidadArchivosExtraidos; i++){
    remove(archivosExtraidos[i]);
    free(archivosExtraidos[i]);

    if(v_command) printf("\n- Se ha eliminado el archivo temporal '%s'.\n", archivosExtraidos[i]);
  }

  // Se borra el archivo TAR original.

  remove(nombreTAR);

  if(v_command) printf("\n- Se ha eliminado el archivo tar original '%s'.\n", nombreTAR);

  // Se renombra el archivo "copia.tar" con el nombre del archivo TAR original.

  rename("copia.tar", nombreTAR);

  if(v_command) printf("\n- Se ha renombrado el archivo tar copia a '%s'.\n", nombreTAR);

  // Fin del proceso.

  if(v_command) printf("\nFINALIZA EL DESFRAGMENTADO DEL ARCHIVO.\n\n");
}

// Función referente al comando <-r>.
void agregarArchivo(const char* nombreTAR, const char* archivoPorAgregar, bool v_command){

  if(v_command) printf("\nINICIA LA AGREGACIÓN DE UN ARCHIVO DENTRO DEL TAR.\n\n");

  // Se abre el archivo por agregar para obtener su tamaño.

  FILE *archivoEnt = fopen(archivoPorAgregar, "rb");

  if(archivoEnt == NULL){
    printf("No se pudo abrir el archivo: %s\n", archivoPorAgregar);
    return;
  }

  if(v_command) printf("- El archivo '%s' ha sido abierto.\n", archivoPorAgregar);

  // Se mueve el puntero de lectura al final del archivo para obtener su tamaño.

  fseek(archivoEnt, 0, SEEK_END);

  // Se obtiene la posición actual, que es el tamaño del archivo.

  long tamanoArchivo = ftell(archivoEnt);

  if(v_command) printf("- El tamaño del archivo '%s' es de %ld bytes.\n", archivoPorAgregar, tamanoArchivo);

  // Se cierra el archivo por agregar.

  fclose(archivoEnt);

  if(v_command) printf("- El archivo '%s' ha sido cerrado.\n", archivoPorAgregar);

  // Se abre el archivo TAR.

  FILE* archivoTAR = fopen(nombreTAR, "rb+");

  if(archivoTAR == NULL){
    printf("Error al abrir el archivo tar: %s\n", nombreTAR);
    return;
  }

  if(v_command) printf("- El archivo '%s' ha sido abierto.\n", nombreTAR);

  // Se crea un arreglo para contener los datos del registro de archivos dentro del TAR.

  struct Archivo archivos[100];

  // Se lee la información de los registros de archivos dentro del TAR.

  fread(archivos, sizeof(struct Archivo), 100, archivoTAR);

  if(v_command) printf("- Se ha recuperado la información de los registros de archivos dentro del TAR.\n");

  // Se crea un arreglo para contener los datos del registro de espacios dentro del TAR.

  struct Espacio espacios[100];

  // Se mueve el puntero de lectura al inicio del registro de espacios dentro del TAR.

  fseek(archivoTAR, sizeof(struct Archivo) * 100, SEEK_SET);

  // Se lee la información de los registros de espacios dentro del TAR.

  fread(espacios, sizeof(struct Espacio), 100, archivoTAR);

  if(v_command) printf("- Se ha recuperado la información de los registros de espacios dentro del TAR.\n");

  // Se comprueba si el archivo por agregar ya existe dentro del archivo TAR.

  if(v_command) printf("- Se inicia la comprobación de existencia del archivo por agregar dentro del archivo TAR.\n");

  for(int i = 0; i < 100; i++){

    if(archivos[i].peso != -1 && strcmp(archivos[i].nombre, archivoPorAgregar) == 0){

      printf("El archivo %s ya existe en el archivo TAR. Actualiza en lugar de agregar.\n", archivoPorAgregar);

      if(v_command) printf("- El archivo '%s' ha sido cerrado.\n", nombreTAR);

      fclose(archivoTAR);

      if(v_command) printf("\nFINALIZA LA AGREGACIÓN DE UN ARCHIVO DENTRO DEL TAR.\n\n");

      return;
    }
  }

  // Se crean variables para la busqueda de un espacio adecuado para el archivo por agregar.

  int espacioEncontrado = -1;
  long inicioArchivo = -1;
  long finArchivo = -1;
  long pesoArchivo = -1;

  // Se busca un espacio adecuado para el archivo por agregar.

  if(v_command) printf("- Se inicia la búsqueda de un espacio adecuado para el archivo por agregar.\n");

  for(int i = 0; i < 100; i++){

    // Se verifica si el espacio está disponible.

    if(espacios[i].inicio != -1 && espacios[i].fin != -1){

      // Se calcula el tamaño del espacio.

      long tamanoEspacio = espacios[i].fin - espacios[i].inicio;

      // Si el tamaño del espacio es mayor o igual al tamaño del archivo por agregar se actualizan las variables.

      if(espacios[i].inicio + tamanoEspacio >= inicioArchivo + pesoArchivo){

        espacioEncontrado = i;
        inicioArchivo = espacios[i].inicio;
        finArchivo = espacios[i].fin;
        pesoArchivo = archivos[i].peso;

        if(v_command) printf("- Se ha encontrado un espacio adecuado para el archivo por agregar.\n");

        // Se termina la busqueda.

        break;
      }
    }
  }

  // Se procede a agregar el archivo por en el espacio encontrado.

  if(v_command) printf("- Se inicia la agregación del archivo por agregar en el espacio encontrado.\n");

  if(espacioEncontrado != -1){

    // Se comprueba si el tamaño del archivo por agregar es igual al espacio disponible.

    if(tamanoArchivo == finArchivo - inicioArchivo+1){
      
      // Se actualizan los registros de archivos con los datos del archivo por agregar.

      actualizarRegistros(archivoTAR, archivoPorAgregar, tamanoArchivo, inicioArchivo, tamanoArchivo + inicioArchivo);

      // Se reemplaza el espacio existente en el índice encontrado con el nuevo espacio no disponible.

      espacios[espacioEncontrado].inicio = -1;
      espacios[espacioEncontrado].fin = -1;

    // Se comprueba si el tamaño del archivo por agregar es menor al espacio disponible.

    }else if(tamanoArchivo < finArchivo - inicioArchivo+1){

      // Se actualizan los registros de archivos con los datos del archivo por agregar.
      
      actualizarRegistros(archivoTAR, archivoPorAgregar, tamanoArchivo, inicioArchivo, tamanoArchivo + inicioArchivo);

      // Se crea un nuevo archivo con el espacio restante.

      long espacioRestante = finArchivo - (inicioArchivo + tamanoArchivo);

      // Se comprueba si el espacio restante es mayor a 0.

      if(espacioRestante > 0){

        // Se crea el nuevo archivo con el espacio restante.

        FILE* nuevoArchivo = fopen("loco", "wb");

        // Se comprueba si el nuevo archivo se ha creado correctamente.

        if(nuevoArchivo != NULL){

          // Se asigna el peso del nuevo archivo.

          fseek(nuevoArchivo, espacioRestante - 1, SEEK_SET);
          fputc(0, nuevoArchivo);
          fclose(nuevoArchivo);

          // Se actualizan los registros de archivos con los datos del nuevo archivo.

          actualizarRegistros(archivoTAR, "", espacioRestante,  tamanoArchivo + inicioArchivo + 1, tamanoArchivo + inicioArchivo + espacioRestante );

          // Se reemplaza el espacio existente en el índice encontrado con el nuevo espacio.

          espacios[espacioEncontrado].inicio = inicioArchivo + tamanoArchivo + 1;
          espacios[espacioEncontrado].fin = finArchivo;

          // Se mueve el puntero de escritura al inicio del registro de espacios dentro del TAR.

          fseek(archivoTAR, sizeof(struct Archivo) * 100, SEEK_SET);

          // Se escriben los registros de espacios dentro del TAR.

          fwrite(espacios, sizeof(struct Espacio), 100, archivoTAR);
        }else{
          printf("No se pudo crear el nuevo archivo para el espacio restante.\n");
        }
      }
    }else{

      // Se agregar el archivo al final del arreglo de archivos si es mayor que el espacio.

      int espacioDisponible = -1;

      for(int i = 0; i < 100; i++){

        // Se verifica si el espacio está disponible.

        if(archivos[i].peso == -1){

          espacioDisponible = i;

          // Se ha encontrado un espacio vacío en el arreglo de archivos.

          break;
        }
      }

      // Se comprueba si se ha encontrado un espacio disponible.

      if(espacioDisponible != -1 && espacioDisponible < 100){

        // Se copia el nombre y demás datos del archivo a agregar en la estructura de archivos.

        strncpy(archivos[espacioDisponible].nombre, archivoPorAgregar, sizeof(archivos[espacioDisponible].nombre));
        archivos[espacioDisponible].peso = tamanoArchivo;
        archivos[espacioDisponible].inicio = archivos[espacioDisponible-1].fin + 1;
        archivos[espacioDisponible].fin = archivos[espacioDisponible-1].fin + tamanoArchivo;

        // Se mueve el puntero de escritura al inicio del registro de archivos dentro del TAR.

        fseek(archivoTAR, espacioDisponible * sizeof(struct Archivo), SEEK_SET);

        // Se escriben los registros de archivos dentro del TAR.

        fwrite(&archivos[espacioDisponible], sizeof(struct Archivo), 1, archivoTAR);

        if(v_command) printf("- Se ha agregado el archivo '%s' con éxito.\n", archivoPorAgregar);
      }else{
        printf("No hay espacio en el arreglo de archivos para agregar %s.\n", archivoPorAgregar);
      }
    }
  }else{

    // Se agrega el archivo al final del arreglo de archivos si es mayor que el espacio.

    int espacioDisponible = -1;

    for(int i = 0; i < 100; i++){

      // Se verifica si el espacio está disponible.

      if(archivos[i].peso == -1){

        espacioDisponible = i;

        // Se ha encontrado un espacio vacío en el arreglo de archivos.

        break;
      }
    }

    // Se comprueba si se ha encontrado un espacio disponible.

    if(espacioDisponible != -1 && espacioDisponible < 100){

      // Se copia el nombre y demás datos del archivo a agregar en la estructura de archivos.
      strncpy(archivos[espacioDisponible].nombre, archivoPorAgregar, sizeof(archivos[espacioDisponible].nombre));
      archivos[espacioDisponible].peso = tamanoArchivo;
      archivos[espacioDisponible].inicio = archivos[espacioDisponible-1].fin + 1;
      archivos[espacioDisponible].fin = archivos[espacioDisponible-1].fin + tamanoArchivo;

      // Se mueve el puntero de escritura al inicio del registro de archivos dentro del TAR.

      fseek(archivoTAR, espacioDisponible * sizeof(struct Archivo), SEEK_SET);

      // Se escriben los registros de archivos dentro del TAR.

      fwrite(&archivos[espacioDisponible], sizeof(struct Archivo), 1, archivoTAR);

      if(v_command) printf("- Se ha agregado el archivo '%s' con éxito.\n", archivoPorAgregar);
    }
  }

  // Se cierra el archivo TAR.

  fclose(archivoTAR);

  if(v_command) printf("- El archivo '%s' ha sido cerrado.\n", nombreTAR);

  if(v_command) printf("\nFINALIZA LA AGREGACIÓN DE UN ARCHIVO DENTRO DEL TAR.\n\n");
}

// Función referente al comando <-u>.
void actualizarArchivo(const char* nombreTAR, const char* archivoPorActualizar, bool v_command){

  if(v_command) printf("\nINICIA LA ACTUALIZACIÓN DEL ARCHIVO '%s'.\n\n", archivoPorActualizar);

  // Se crea un arreglo para almacenar la información de los registros de archivos.

  struct Archivo archivos[100];

  // Se crea un arreglo para almacenar la información de los espacios disponibles.

  struct Espacio espacios[100];

  if(v_command) printf("- Se crean las estructuras para almacenar la información del archivo '%s'.\n", nombreTAR);

  // Se abre el archivo TAR.

  FILE* archivoTAR = fopen(nombreTAR, "rb+");
  
  if(archivoTAR == NULL){
    printf("Error al abrir el archivo tar: %s\n", nombreTAR);
    return;
  }

  if(v_command) printf("- El archivo '%s' ha sido abierto.\n", nombreTAR);

  // Se leer el contenido del archivo TAR.
  
  fread(archivos, sizeof(struct Archivo), 100, archivoTAR);

  if(v_command) printf("- Se ha leído el contenido del archivo '%s'.\n", nombreTAR);

  // Se crean variables necesarias para la busqueda del archivo a actualizar.

  bool encontrado = false;
  int posicion = 0;

  // Se busca el archivo a actualizar en el arreglo de archivos.

  for(int i = 0; i < 100; i++){

    // Si se encuentra el archivo, se actualizan las variables de control.

    if(strcmp(archivos[i].nombre, archivoPorActualizar) == 0){
      
      encontrado = true;
      posicion = i;

      // Finaliza la búsqueda.

      break;
    }
  }
  
  if(encontrado){

    if(v_command) printf("- El archivo '%s' ha sido encontrado.\n", archivoPorActualizar);

    // Se guardan los valores de inicio y fin del archivo antes de borrarlo.

    long inicioViejo = archivos[posicion].inicio;
    long finViejo = archivos[posicion].fin;

    if(v_command) printf("- Inicia el proceso de actualización del archivo '%s'.\n", archivoPorActualizar);

    // Se abre el archivo a actualizar.

    FILE* archivoPorActualizarFP = fopen(archivoPorActualizar, "rb");

    if(archivoPorActualizarFP == NULL){
      printf("Error al abrir el archivo %s.\n", archivoPorActualizar);
      return;
    }

    if(v_command) printf("- El archivo '%s' ha sido abierto.\n", archivoPorActualizar);

    // Se mueve el cursor de lectura al final del archivo.

    fseek(archivoPorActualizarFP, 0, SEEK_END);

    // Se obtiene el peso del archivo a actualizar.

    long pesoPorActualizar = ftell(archivoPorActualizarFP);

    if(v_command) printf("- El peso del nuevo archivo '%s' es de %ld bytes.\n", archivoPorActualizar, pesoPorActualizar);

    // Se cierra el archivo a actualizar.

    fclose(archivoPorActualizarFP);

    if(v_command) printf("- El archivo '%s' ha sido cerrado.\n", archivoPorActualizar);

    // Se borra el archivo a actualizar del TAR.

    borrarArchivo((char*)nombreTAR, (char*)archivoPorActualizar, false);

    // Se concatenan los espacios contiguos.

    concatenarEspaciosContiguos(nombreTAR);

    // Se mueve el cursor al principio del arreglo "Espacios".

    fseek(archivoTAR, sizeof(struct Archivo) * 100, SEEK_SET);

    // Se lee el contenido del arreglo "Espacios".

    fread(espacios, sizeof(struct Espacio), 100, archivoTAR);

    // Se crean variables para almacenar los índices del espacio encontrado.

    long inicioNuevo;
    long finNuevo;

    // Se busca el espacio disponible más cercano al inicio del archivo a actualizar.

    int espacioEncontrado;

    for(int i = 0; i < 100; i++){

      // Si se encuentra un espacio disponible, se actualizan las variables de control.

      if((espacios[i].inicio != -1 && espacios[i].fin != -1) && (espacios[i].inicio == inicioViejo || espacios[i].fin == finViejo || (espacios[i].inicio < inicioViejo && espacios[i].fin > finViejo))){

        inicioNuevo = espacios[i].inicio;
        finNuevo = espacios[i].fin;
        espacioEncontrado = i;
      }
    }

    // Si el espacio encontrado es exactamente del mismo tamaño que el archivo a actualizar.

    if(finNuevo - inicioNuevo  == pesoPorActualizar){

      // Se actualizan los registros del archivo TAR.

      actualizarRegistros(archivoTAR, archivoPorActualizar, pesoPorActualizar, inicioNuevo, pesoPorActualizar + inicioNuevo);

      // Se reemplaza el espacio existente en el índice encontrado con el nuevo espacio no disponible.

      espacios[espacioEncontrado].inicio = -1;
      espacios[espacioEncontrado].fin = -1;

      // Se mueve el cursor al principio del arreglo "Espacios".

      fseek(archivoTAR, sizeof(struct Archivo) * 100, SEEK_SET);

      // Se escriben los cambios en el arreglo "Espacios".

      fwrite(espacios, sizeof(struct Espacio), 100, archivoTAR);

      // Si el espacio encontrado es mayor que el archivo a actualizar.
        
    }else if(finNuevo - inicioNuevo > pesoPorActualizar){

        // Se actualizan los registros del archivo TAR.

        actualizarRegistros(archivoTAR, archivoPorActualizar, pesoPorActualizar, inicioNuevo, pesoPorActualizar + inicioNuevo);

        // Se crea un nuevo archivo con el espacio restante.

        long espacioRestante = finNuevo - (inicioNuevo + pesoPorActualizar);

        // Si el espacio restante es mayor a 0, se crea un nuevo archivo.

        if(espacioRestante > 0){
          
          // Se crea un nuevo archivo con el espacio restante.

          FILE* nuevoArchivo = fopen("loco", "wb");

          // Si el archivo se crea correctamente, se actualizan los registros del archivo TAR.

          if(nuevoArchivo == NULL){
            printf("Error al abrir el archivo %s.\n", nuevoArchivo);
            return;
          }

          // Se mueve el cursor al final del archivo.

          fseek(nuevoArchivo, espacioRestante - 1, SEEK_SET);

          // Se escribe un caracter nulo al final del archivo.

          fputc(0, nuevoArchivo);

          // Se cierra el archivo.

          fclose(nuevoArchivo);

          // Se actualizan los registros del archivo TAR.

          actualizarRegistros(archivoTAR, "", espacioRestante,  pesoPorActualizar + inicioNuevo + 1, pesoPorActualizar + inicioNuevo + espacioRestante );

          // Se reemplaza el espacio existente en el índice encontrado con el nuevo espacio.

          espacios[espacioEncontrado].inicio = inicioNuevo + pesoPorActualizar + 1;
          espacios[espacioEncontrado].fin = finNuevo;

          // Se mueve el cursor al principio del arreglo "Espacios".

          fseek(archivoTAR, sizeof(struct Archivo) * 100, SEEK_SET);

          // Se escriben los cambios en el arreglo "Espacios".

          fwrite(espacios, sizeof(struct Espacio), 100, archivoTAR);
        }

    // Si el espacio encontrado es menor que el archivo a actualizar.

    }else{

      // Se busca un espacio disponible en el arreglo "Espacios".

      int espacioDisponible = -1;
      for(int j = 0; j < 100; j++){

        // Si se encuentra un espacio disponible, se actualizan la variable 'espacioDisponible'.
        if(archivos[j].peso == -1){

          espacioDisponible = j;

          // Finaliza la búsqueda.

          break;
        }
      }

      // Si se encontró un espacio disponible, se actualizan los registros del archivo TAR.

      if(espacioDisponible != -1 && espacioDisponible < 100){

        // Se actualizan los registros del archivo TAR.

        strncpy(archivos[espacioDisponible].nombre, archivoPorActualizar, sizeof(archivos[espacioDisponible].nombre));
        archivos[espacioDisponible].peso = pesoPorActualizar;
        archivos[espacioDisponible].inicio = archivos[espacioDisponible - 1].fin + 1;
        archivos[espacioDisponible].fin = archivos[espacioDisponible - 1].fin + pesoPorActualizar;

        // Se mueve el cursor al principio del arreglo "Archivos".

        fseek(archivoTAR, espacioDisponible * sizeof(struct Archivo), SEEK_SET);

        // Se escriben los cambios en el arreglo "Archivos".

        fwrite(&archivos[espacioDisponible], sizeof(struct Archivo), 1, archivoTAR);
      }
    }

  if(v_command) printf("- Se actualiza el contenido del archivo '%s'.\n", archivoPorActualizar);

  if(v_command) printf("- Se han actualizado los registros de archivos y espacios.\n");

  }else{
    printf("No se puede actualizar un archivo que no está en el TAR.\n");
  }
    
  // Se cierra el archivo TAR.
  
  fclose(archivoTAR);

  if(v_command) printf("- El archivo '%s' ha sido cerrado.\n", nombreTAR);

  if(v_command) printf("\nFINALIZA LA ACTUALIZACIÓN DEL ARCHIVO '%s'.\n\n", archivoPorActualizar);
}

// PROGRAMA PRINCIPAL.

int main(int argc, char* argv[]){

  // Análisis de errores en los comandos.
  
  if(argc < 4 || (strcmp(argv[1], "star") != 0) || argv[2][0] != '-'){
    printf("%s", OPTIONS);
    return 1;
  }

  // Análisis de los comandos a utilizar en el argumento de comandos.

  bool c_command = false;
  bool x_command = false;
  bool t_command = false;
  bool d_command = false;
  bool u_command = false;
  bool v_command = false;
  bool f_command = false;
  bool r_command = false;
  bool p_command = false;

  char *comandos = argv[2];
  for(int i = 0; comandos[i] != '\0'; i++){
    if(comandos[i] == 'c'){
      c_command = true;
    } else if(comandos[i] == 'x'){
      x_command = true;
    } else if(comandos[i] == 't'){
      t_command = true;
    } else if(comandos[i] == 'd'){
      d_command = true;
    } else if(comandos[i] == 'u'){
      u_command = true;
    } else if(comandos[i] == 'v'){
      v_command = true;
    } else if(comandos[i] == 'f'){
      f_command = true;
    } else if(comandos[i] == 'r'){
      r_command = true;
    } else if(comandos[i] == 'p'){
      p_command = true;
    }
  }

  if(f_command) c_command == false;

  if(d_command & r_command){
    printf("\nLos comandos <-d> y <-r> no pueden ser usados en conjunto.\n\nPor favor vuelva a intentarlo.....\n\n");
    return 1;
  }
  if(d_command & u_command){
    printf("\nLos comandos <-d> y <-u> no pueden ser usados en conjunto.\n\nPor favor vuelva a intentarlo.....\n\n");
    return 1;
  }
  if(r_command & u_command){
    printf("\nLos comandos <-r> y <-u> no pueden ser usados en conjunto.\n\nPor favor vuelva a intentarlo.....\n\n");
    return 1;
  }
  if((x_command & c_command) | (x_command & t_command) | (x_command & d_command) | (x_command & u_command) | (x_command & r_command) | (x_command & p_command) | (x_command & f_command)){
    printf("\nEl comando <-x> no puede ser usado en conjunto con los comandos <-c>, <-t>, <-d>, <-u>, <-r>, <-p> o <-f>.\n\nPor favor vuelva a intentarlo.....\n\n");
    return 1;
  }
  if((p_command & c_command) | (p_command & t_command) | (p_command & d_command) | (p_command & u_command) | (p_command & r_command) | (p_command & x_command) | (p_command & f_command)){
    printf("\nEl comando <-p> no puede ser usado en conjunto con los comandos <-c>, <-t>, <-d>, <-u>, <-r>, <-x> o <-f>.\n\nPor favor vuelva a intentarlo.....\n\n");
    return 1;
  }

  // Ejecución de funciones solicitadas por comandos.

  char *nombreTAR = argv[3];

  // CREAR ARCHIVO TAR.

  if(c_command){
    crearTAR(nombreTAR, v_command);
  }

  // EMPAQUETAR ARCHIVOS.

  if(f_command){
    if(c_command == false){
      printf("\nEl comando <-f> debe ser usado en conjunto con el comando <-c>.\n\nPor favor vuelva a intentarlo.....\n\n");
      return 1;
    }
    int numeroArchivos = argc - 4;
    const char** archivosPorEmpacar = (const char**)&argv[4];
    empacarArchivos(nombreTAR, archivosPorEmpacar, numeroArchivos, v_command);
  }

  // EXTRAER ARCHIVOS.

  if(x_command){
    extraerArchivos(nombreTAR, v_command);
  }

  // BORRAR UN ARCHIVO.

  if(d_command){
    char* archivoPorBorrar = argv[4];
    borrarArchivo(nombreTAR, archivoPorBorrar, v_command);
  }

  // AGREGAR UN ARCHIVO.

  if(r_command){
    char* archivoPorAgregar = argv[4];
    agregarArchivo(nombreTAR, archivoPorAgregar, v_command);
  }

  // REESCRIBIR UN ARCHIVO.

  if(u_command){
    char* archivoPorActualizar = argv[4];
    actualizarArchivo(nombreTAR, archivoPorActualizar, v_command);
  }

  // LISTAR ARCHIVO TAR.

  if(t_command){
    listarTAR(nombreTAR, v_command);
  }

  // DESFRAGMENTAR ARCHIVO TAR.

  if(p_command){
    desfragmentar(nombreTAR, v_command);
  }

  return 0;
}