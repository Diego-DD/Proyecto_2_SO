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

void crearTAR(char* output_filename, bool v_command){

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

  FILE* archivoTAR = fopen(output_filename, "wb");

  if(v_command) printf("- El archivo '%s' ha sido abierto.\n", output_filename);

  if (archivoTAR == NULL) {
    printf("\nError al abrir el archivo '%s'.\n", output_filename);
    exit(1);
  }

  fwrite(archivos, sizeof(struct Archivo), 100, archivoTAR);
  if(v_command) printf("- Se ha escrito el arreglo de datos de archivos en el archivo '%s'.\n", output_filename);
  fwrite(espacios, sizeof(struct Espacio), 100, archivoTAR);
  if(v_command) printf("- Se ha escrito el arreglo de espacios de archivos en el archivo '%s'.\n", output_filename);

  fclose(archivoTAR);
  if(v_command) printf("- El archivo '%s' ha sido cerrado.\n", output_filename);

  if(v_command) printf("\nFINALIZA LA CREACIÓN DEL ARCHIVO TAR.\n\n");
}

void actualizarRegistros(FILE* archivoTAR, const char* nombre, long peso, long inicio, long fin){
  // Mueve el cursor al principio del archivo
  fseek(archivoTAR, 0, SEEK_SET);

  struct Archivo archivos[100];

  // Lee los datos existentes del archivo
  fread(archivos, sizeof(struct Archivo), 100, archivoTAR);

  int indiceDisponible = -1;

  // Busca el primer índice disponible (con valores de inicialización)
  for(int i = 0; i < 100; i++){
    if(strcmp(archivos[i].nombre, "") == 0){
      indiceDisponible = i;
      break;
    }
  }

  if(indiceDisponible != -1){
    // Actualiza el elemento en el índice encontrado
    strcpy(archivos[indiceDisponible].nombre, nombre);
    archivos[indiceDisponible].peso = peso;
    archivos[indiceDisponible].inicio = inicio;
    archivos[indiceDisponible].fin = fin;

    // Mueve el cursor al principio del archivo antes de escribir los datos actualizados
    fseek(archivoTAR, 0, SEEK_SET);

    // Escribe los datos actualizados en el archivo
    fwrite(archivos, sizeof(struct Archivo), 100, archivoTAR);
  }else{
    printf("No hay índices disponibles en el archivo TAR para actualizar.\n");
  }
}

void actualizarEspacios(FILE* tar_file, long inicioTemp, long finTemp) {
    // Mueve el cursor al principio del arreglo "Espacios".
    fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);

    struct Espacio espacios[100];
    fread(espacios, sizeof(struct Espacio), 100, tar_file);

    // Busca el primer índice disponible en "Espacios" (inicio y fin igual a -1).
    for (int i = 0; i < 100; i++) {
        if (espacios[i].inicio == -1 && espacios[i].fin == -1) {
            // Encontrado un índice disponible en "Espacios".
            espacios[i].inicio = inicioTemp;
            espacios[i].fin = finTemp;

            // Mueve el cursor al principio del arreglo "Espacios" antes de escribir los datos actualizados.
            fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);
            fwrite(espacios, sizeof(struct Espacio), 100, tar_file);
            break;
        }
    }
}

void empacarArchivos(char* output_filename, const char* input_files[], int num_files, bool v_command){

  crearTAR(output_filename, v_command);

  if(v_command) printf("\nINICIA LA EMPAQUETACIÓN DE ARCHIVOS.\n\n");
  
  FILE* tar_file = fopen(output_filename, "wb");
  if(!tar_file){
    printf("Error al crear el archivo tar.\n");
    return;
  }

  if(v_command) printf("\n- El archivo %s ha sido abierto.\n", output_filename);

  // Escribir la información de cada archivo en el archivo tar
  for(int i = 0; i < num_files; i++){
    FILE* input_file = fopen(input_files[i], "rb");
    if(!input_file){
      printf("Error al abrir el archivo: '%s'\n", input_files[i]);
      continue;
    }

    if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n", input_files[i]);

    // Obtener el tamaño del archivo
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    if(v_command) printf("\n- El peso '%ld' del archivo '%s' ha sido obtenido.\n", file_size, input_files[i]);

    // Obtener el byte de inicio de escritura
    fseek(tar_file, 0, SEEK_END);
    long byte_inicio = ftell(tar_file);
    if(v_command) printf("\n- El byte de inicio '%ld' del archivo '%s' ha sido obtenido.\n", byte_inicio, input_files[i]);

    long byte_final = byte_inicio + file_size;
    if(v_command) printf("\n- El byte final '%ld' del archivo '%s' ha sido obtenido.\n", byte_final, input_files[i]);

    /* // Escribir el nombre del archivo
    fwrite(input_files[i], sizeof(char), 100, tar_file);

    // Escribir el tamaño del archivo
    fwrite(&file_size, sizeof(long), 1, tar_file); */

    // Escribir el contenido del archivo
    char buffer[1024];
    size_t bytes_read;
    while((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), input_file)) > 0){
      fwrite(buffer, sizeof(char), bytes_read, tar_file);
    }
    if(v_command) printf("\n- El comtenido del archivo '%s' ha sido escrito en el archivo %s.\n", input_files[i], output_filename);

    // Agregar un byte vacío
    char byte_vacio = 0; // Puedes usar cualquier valor que desees como byte vacío
    fwrite(&byte_vacio, sizeof(char), 1, tar_file);

    fclose(input_file);
    if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", input_files[i]);

    actualizarRegistros(tar_file, input_files[i], file_size, byte_inicio, byte_final);
    if(v_command) printf("\n- Los datos han sido actualizados en los registros de archivos de '%s'.\n", output_filename);

  }

  fclose(tar_file);
  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", output_filename);

  if(v_command) printf("\nFINALIZA LA EMPAQUETACIÓN DE ARCHIVOS.\n\n");
}

void listarTAR(const char* tar_filename, bool v_command){

  if(v_command) printf("\nINICIA EL LISTADO DEL CONTENIDO DEL ARCHIVO TAR.\n\n");

  FILE* archivoTAR = fopen(tar_filename, "rb");

  if(archivoTAR == NULL){
    printf("Error al abrir el archivo '%s'.\n", tar_filename);
    return;
  }

  if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n\n", tar_filename);

  struct Archivo archivo;
  int index = 0;
  bool control = false;
  while(fread(&archivo, sizeof(struct Archivo), 1, archivoTAR) == 1){
    if(strcmp(archivo.nombre, "") == 0){
      break;  // Salir del bucle si se encuentra una estructura vacía.
    }

    printf("-----------------------------------------------------\n");
    printf("Nombre del archivo:\n\t\t\t%s\n", archivo.nombre);
    printf("Peso correspondiente:\n\t\t\t%ld bytes.\n", archivo.peso);
    printf("Byte de inicio:\n\t\t\t%ld bytes.\n", archivo.inicio);
    printf("Byte final:\n\t\t\t%ld bytes.\n", archivo.fin);

    index++;
    control = true;
  }
  if(control){
    printf("-----------------------------------------------------\n");
  }
  else{
    printf("\nEl archivo '%s' no contiene archivos registrados en este momento.\n", tar_filename);
  }

  fclose(archivoTAR);

  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", tar_filename);

  if(v_command) printf("\nFINALIZA EL LISTADO DEL CONTENIDO DEL ARCHIVO TAR.\n\n");
}

void extraerArchivos(const char* input_filename, bool v_command){

  if(v_command) printf("\nINICIA LA EXTRACCIÓN DEL CONTENIDO DEL ARCHIVO TAR.\n\n");

  FILE* tar_file = fopen(input_filename, "rb");
  if (!tar_file){
    printf("Error al abrir el archivo tar.\n");
    return;
  }

  if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n", input_filename);

  // Crear un directorio para los archivos extraídos
  char extract_dir[100];
  snprintf(extract_dir, sizeof(extract_dir), "%s_desempacado", input_filename);
  mkdir(extract_dir, 0777);
  if(v_command) printf("\n- El directorio '%s_desempacado' ha sido creado.\n", input_filename);

  // Leer la información de los archivos desde el archivo tar
  struct Archivo archivos[100];
  fread(archivos, sizeof(struct Archivo), 100, tar_file);
  if(v_command) printf("\n- Se inicia la lectura de datos del archivio '%s'.\n", input_filename);

  for(int i = 0; i < 100; i++){
    if(archivos[i].nombre[0] == '\0'){
      continue;
    }

    // Construir la ruta del archivo extraído
    char extract_path[200];
    snprintf(extract_path, sizeof(extract_path), "%s/%s", extract_dir, archivos[i].nombre);
    if(v_command) printf("\n- Se ha construido la ruta para el archivo a extraer '%s'.\n", archivos[i].nombre);

    // Crear y escribir el contenido del archivo extraído
    FILE* extracted_file = fopen(extract_path, "wb");
    if(!extracted_file){
      printf("Error al crear el archivo extraído: %s\n", extract_path);
      continue;
    }
    if(v_command) printf("\n- Se ha abierto un archivo en blanco para alvergar los datos copiados del archivo a extraer '%s'.\n", archivos[i].nombre);

    // Copiar el contenido del archivo desde el archivo tar
    long file_size = archivos[i].peso;
    fseek(tar_file, archivos[i].inicio, SEEK_SET);
    char buffer[1024];
    size_t bytes_read;
    if(v_command) printf("\n- Se han preparado las estructuras para copiar los datos del archivo a extraer '%s'.\n", archivos[i].nombre);
    if(v_command) printf("\n- Se inicia la copia de datos del archivo '%s' en el archivo en blanco.\n", archivos[i].nombre);

    while(file_size > 0 && (bytes_read = fread(buffer, sizeof(char), sizeof(buffer), tar_file)) > 0){
      size_t bytes_to_write = bytes_read;
      if(bytes_to_write > file_size){
        bytes_to_write = file_size;
      }
      fwrite(buffer, sizeof(char), bytes_to_write, extracted_file);
      file_size -= bytes_to_write;
    }
    if(v_command) printf("\n- Se han copiado los datos del archivo '%s' en el archivo en blanco.\n", archivos[i].nombre);
    if(v_command) printf("\n- Se han terminado de extraer el archivo '%s'.\n", archivos[i].nombre);

    fclose(extracted_file);
    if(v_command) printf("\n- El archivo extraido ha sido cerrado.\n");
  }

  fclose(tar_file);
  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", input_filename);

  if(v_command) printf("\nFINALIZA LA EXTRACCIÓN DEL CONTENIDO DEL ARCHIVO TAR.\n\n");
}

void borrarArchivo(char* tar_filename, char* file_to_erase, bool v_command){

  if(v_command) printf("\nINICIA EL BORRADO DEL ARCHIVO.\n\n");
  
  FILE* tar_file = fopen(tar_filename, "rb+");

  if(tar_file == NULL){
    printf("Error al abrir el archivo tar: %s\n", tar_filename);
    return;
  }

  if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n", tar_filename);

  // Leer el contenido del archivo tar.
  struct Archivo archivos[100];
  fread(archivos, sizeof(struct Archivo), 100, tar_file);

  if(v_command) printf("\n- Los registros de archivos han sido preparados.\n");

  int indiceEncontrado = -1;
  long inicioTemp = -1;
  long finTemp = -1;

  if(v_command) printf("\n- Se inicia la busqueda del archivo '%s' en los registros.\n", file_to_erase);

  for(int i = 0; i < 100; i++){
    if(strcmp(archivos[i].nombre, file_to_erase) == 0){
      if(v_command) printf("\n- Se ha encontrado el archivo '%s' en los registros.\n", file_to_erase);
      // Encontrado el archivo con el nombre deseado.
      indiceEncontrado = i;
      inicioTemp = archivos[i].inicio;
      finTemp = archivos[i].fin;

      // Cambiar el valor del espacio "nombre" por "".
      strcpy(archivos[i].nombre, "");

      break;
    }
  }

  if(v_command) printf("\n- Se ha borrado el archivo '%s' de los registros.\n", file_to_erase);

  if(indiceEncontrado != -1){
    // Mueve el cursor al principio del arreglo "Archivos" antes de escribir los datos actualizados.
    fseek(tar_file, 0, SEEK_SET);
    fwrite(archivos, sizeof(struct Archivo), 100, tar_file);

    // Llama a la función para actualizar "Espacios".
    if (inicioTemp != -1 && finTemp != -1) {
        actualizarEspacios(tar_file, inicioTemp, finTemp);
    }

    // Asegurarse de que los cambios se guarden en el archivo tar.
    fflush(tar_file);

    if(v_command) printf("\n- Se han actualizado los registros de espacios en blanco.\n");
  }else{
    printf("No se encontró el archivo %s en el archivo tar.\n", file_to_erase);
  }

  // Cerrar el archivo tar cuando hayas terminado.
  fclose(tar_file);
  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", tar_filename);

  if(v_command) printf("\nFINALIZA EL BORRADO DEL ARCHIVO.\n\n");
}

void desfragmentar(char* tar_filename, bool v_command){

  if(v_command) printf("\nINICIA EL DESFRAGMENTADO DEL ARCHIVO.\n\n");

  FILE* tar_file = fopen(tar_filename, "rb");
  if(!tar_file){
    printf("Error al abrir el archivo tar: %s\n", tar_filename);
    return;
  }

  if(v_command) printf("\n- El archivo '%s' ha sido abierto.\n", tar_filename);

  // Leer la información de los archivos desde el archivo tar.
  struct Archivo archivos[100];
  fread(archivos, sizeof(struct Archivo), 100, tar_file);

  if(v_command) printf("\n- Se han leido los datos de los registros de archivos.\n");

  // Extraer los archivos en el mismo lugar y guardar sus nombres.
  char* extracted_files[100];  // Arreglo para almacenar los nombres de los archivos extraídos.
  int num_extracted_files = 0; // Inicializar el número de archivos extraídos.
  if(v_command) printf("\n- Se inicia la extracción 'ficticio/temporal' de archivos.\n");
  for(int i = 0; i < 100; i++){
    if(archivos[i].nombre[0] == '\0'){
      continue;
    }

    // Construir el nombre del archivo a extraer.
    char extract_filename[256];
    snprintf(extract_filename, sizeof(extract_filename), "./%s", archivos[i].nombre);
    if(v_command) printf("\n- Se ha construido el nombre del archivo temporal extraido.\n");

    FILE* extracted_file = fopen(extract_filename, "wb");
    if(!extracted_file){
      printf("Error al crear el archivo extraído: %s\n", extract_filename);
      continue;
    }

    if(v_command) printf("\n- El archivo temporal '%s' ha sido creado.\n", extract_filename);

    // Copiar el contenido del archivo desde el archivo tar.
    if(v_command) printf("\n- Se inicia la copia del contenido original al archivo temporal.\n");
    long file_size = archivos[i].peso;
    fseek(tar_file, archivos[i].inicio, SEEK_SET);
    char buffer[1024];
    size_t bytes_read;

    while(file_size > 0 && (bytes_read = fread(buffer, sizeof(char), sizeof(buffer), tar_file)) > 0){
      size_t bytes_to_write = bytes_read;
      if(bytes_to_write > file_size){
        bytes_to_write = file_size;
      }
      fwrite(buffer, sizeof(char), bytes_to_write, extracted_file);
      file_size -= bytes_to_write;
    }

    // Guardar el nombre del archivo extraído en el arreglo.
    extracted_files[num_extracted_files] = strdup(archivos[i].nombre);
    num_extracted_files++;
    if(v_command) printf("\n- Se registra el nombre del archivo temporal extraido.\n");

    fclose(extracted_file);
    if(v_command) printf("\n- El archivo temporal '%s' ha sido cerrado.\n", extract_filename);
  }

  fclose(tar_file);
  if(v_command) printf("\n- El archivo '%s' ha sido cerrado.\n", tar_filename);
  //printf("Archivos extraídos en el mismo lugar donde se encuentra '%s'.\n", tar_filename);

  // Llamar a empacarArchivos después de extraer todos los archivos.
  empacarArchivos("copia.tar", (const char**)extracted_files, num_extracted_files);
  if(v_command) printf("\n- Se han empacado en los archivos temporales en un nuevo archivo tar.\n");

  // Liberar la memoria de los nombres de los archivos extraídos.
  for(int i = 0; i < num_extracted_files; i++){
    remove(extracted_files[i]);
    free(extracted_files[i]);
    if(v_command) printf("\n- Se ha eliminado el archivo temporal '%s'.\n", extracted_files[i]);
  }
  remove(tar_filename);
  if(v_command) printf("\n- Se ha eliminado el archivo tar original '%s'.\n", tar_filename);
  rename("copia.tar", tar_filename);
  if(v_command) printf("\n- Se ha renombrado el archivo tar copia a '%s'.\n", tar_filename);

  if(v_command) printf("\nFINALIZA EL DESFRAGMENTADO DEL ARCHIVO.\n\n");
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
    int num_files = argc - 4;
    const char** files_to_pack = (const char**)&argv[4];
    empacarArchivos(nombreTAR, files_to_pack, num_files, v_command);
  }

  // EXTRAER ARCHIVOS.

  if(x_command){
    extraerArchivos(nombreTAR, v_command);
  }

  // BORRAR UN ARCHIVO.

  if(d_command){
    char* file_to_erase = argv[4];
    borrarArchivo(nombreTAR, file_to_erase, v_command);
  }

  // AGREGAR UN ARCHIVO.

  if(r_command){
    // Función agregar archivos en una tar ya existente.
  }

  // REESCRIBIR UN ARCHIVO.

  if(u_command){
    // Función reescribir archivo.
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
