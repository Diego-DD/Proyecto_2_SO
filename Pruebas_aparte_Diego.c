#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>



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

void crearTAR(const char* output_filename){

  struct Archivo archivos[100];

  struct Espacio espacios[100];

  for (int i = 0; i < 100; i++) {
    snprintf(archivos[i].nombre, sizeof(archivos[i].nombre), "");
    archivos[i].peso = -1;
    archivos[i].inicio = -1;
    archivos[i].fin = -1;

    espacios[i].inicio = -1;
    espacios[i].fin = -1;
  }

  FILE* archivoTAR = fopen(output_filename, "wb");

  if (archivoTAR == NULL) {
    printf("\nError al abrir el archivo '%s'.\n", output_filename);
    exit(1);
  }

  fwrite(archivos, sizeof(struct Archivo), 100, archivoTAR);
  fwrite(espacios, sizeof(struct Espacio), 100, archivoTAR);

  fflush(archivoTAR);
  fclose(archivoTAR);
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

    for(int i = 90; i < 100; i++){
      strcpy(archivos[i].nombre, "");
      archivos[i].peso = -1;
      archivos[i].inicio = -1;
      archivos[i].fin = -1;
    }

    // Mueve el cursor al principio del archivo antes de escribir los datos actualizados
    fseek(archivoTAR, 0, SEEK_SET);

    // Escribe los datos actualizados en el archivo
    fwrite(archivos, sizeof(struct Archivo), 100, archivoTAR);
  }else{
    //printf("No hay índices disponibles en el archivo TAR para actualizar.\n");
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

void empacarArchivos(const char* output_filename, const char* input_files[], int num_files){

  crearTAR(output_filename);

  //------------------------------------------------------------------------------------------------------------------------
  
  FILE* tar_file = fopen(output_filename, "rb+");
  if(!tar_file){
    printf("Error al crear el archivo tar.\n");
    return;
  }

  // Escribir la información de cada archivo en el archivo tar
  for(int i = 0; i < num_files; i++){
    FILE* input_file = fopen(input_files[i], "rb");
    if(!input_file){
      printf("Error al abrir el archivo: '%s'\n", input_files[i]);
      continue;
    }

    // Obtener el tamaño del archivo
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    // Obtener el byte de inicio de escritura
    fseek(tar_file, 0, SEEK_END);
    long byte_inicio = ftell(tar_file);

    long byte_final = byte_inicio + file_size;

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

    // Agregar un byte vacío
    char byte_vacio = 0; // Puedes usar cualquier valor que desees como byte vacío
    fwrite(&byte_vacio, sizeof(char), 1, tar_file);

    fclose(input_file);

    actualizarRegistros(tar_file, input_files[i], file_size, byte_inicio, byte_final);
  }

  fclose(tar_file);
}

void listarTAR(const char* tar_filename){
  FILE* archivoTAR = fopen(tar_filename, "rb");

  if (archivoTAR == NULL) {
    printf("Error al abrir el archivo '%s'.\n", tar_filename);
    return;
  }

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
    printf("\nEl archivo '%s' no contiene archivos registrados en este momento.\n", tar_filename);
  }

  fclose(archivoTAR);
}


void extraerArchivos(const char* input_filename){

  FILE* tar_file = fopen(input_filename, "rb");
  if (!tar_file){
    printf("Error al abrir el archivo tar.\n");
    return;
  }

  // Crear un directorio para los archivos extraídos
  char extract_dir[100];
  snprintf(extract_dir, sizeof(extract_dir), "%s_desempacado", input_filename);
  mkdir(extract_dir, 0777);

  // Leer la información de los archivos desde el archivo tar
  struct Archivo archivos[100];
  fread(archivos, sizeof(struct Archivo), 100, tar_file);

  for(int i = 0; i < 100; i++){
    if(archivos[i].nombre[0] == '\0'){
      continue;
    }

    // Construir la ruta del archivo extraído
    char extract_path[200];
    snprintf(extract_path, sizeof(extract_path), "%s/%s", extract_dir, archivos[i].nombre);

    // Crear y escribir el contenido del archivo extraído
    FILE* extracted_file = fopen(extract_path, "wb");
    if(!extracted_file){
      printf("Error al crear el archivo extraído: %s\n", extract_path);
      continue;
    }

    // Copiar el contenido del archivo desde el archivo tar
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

    fclose(extracted_file);
  }

  fclose(tar_file);
}

void borrarArchivo(const char* tar_filename, const char* file_to_erase) {
    FILE* tar_file = fopen(tar_filename, "rb+");

    if (tar_file == NULL) {
        printf("Error al abrir el archivo tar: %s\n", tar_filename);
        return;
    }

    // Leer el contenido del archivo tar.
    struct Archivo archivos[100];
    fread(archivos, sizeof(struct Archivo), 100, tar_file);

    int indiceEncontrado = -1;
    long inicioTemp = -1;
    long finTemp = -1;

    for (int i = 0; i < 100; i++) {
        if (strcmp(archivos[i].nombre, file_to_erase) == 0) {
            // Encontrado el archivo con el nombre deseado.
            indiceEncontrado = i;
            inicioTemp = archivos[i].inicio;
            finTemp = archivos[i].fin;

            // Cambiar el valor del espacio "nombre" por "".
            strcpy(archivos[i].nombre, "");

            break;
        }
    }

    if (indiceEncontrado != -1) {
        // Mueve el cursor al principio del arreglo "Archivos" antes de escribir los datos actualizados.
        fseek(tar_file, 0, SEEK_SET);
        fwrite(archivos, sizeof(struct Archivo), 100, tar_file);

        // Llama a la función para actualizar "Espacios".
        if (inicioTemp != -1 && finTemp != -1) {
            actualizarEspacios(tar_file, inicioTemp, finTemp);
        }

        // Asegurarse de que los cambios se guarden en el archivo tar.
        fflush(tar_file);
    } else {
        printf("No se encontró el archivo %s en el archivo tar.\n", file_to_erase);
    }

    // Cerrar el archivo tar cuando hayas terminado.
    fclose(tar_file);
}



void archivos_pruebas(const char* tar_filename) {

    FILE* archivoTAR = fopen(tar_filename, "rb");
    if (!archivoTAR) {
      printf("Error al crear el archivo tar.\n");
      return;
    }

    // Mueve el cursor al principio del archivo
    fseek(archivoTAR, 0, SEEK_SET);

    struct Archivo archivos[100];

    // Lee los datos existentes del archivo
    fread(archivos, sizeof(struct Archivo), 100, archivoTAR);

    printf("----------------------------------------------------------------------------------\n");

    for (int i = 0; i < 100; i++) {
      printf("POSICIÓN %d...\n", i);
      printf("Nombre del archivo:\n\t\t\t%s\n", archivos[i].nombre);
      printf("Peso correspondiente:\n\t\t\t%ld bytes.\n", archivos[i].peso);
      printf("Byte de inicio:\n\t\t\t%ld bytes.\n", archivos[i].inicio);
      printf("Byte final:\n\t\t\t%ld bytes.\n", archivos[i].fin);
      printf("\n");
    }
  

    printf("----------------------------------------------------------------------------------\n");

}

void listarEspacios(const char* tar_filename) {
    // Abrir el archivo tar en modo lectura.
    FILE* tar_file = fopen(tar_filename, "rb");

    if (tar_file == NULL) {
        printf("Error al abrir el archivo tar: %s\n", tar_filename);
        return;
    }

    // Mueve el cursor al principio del arreglo "Espacios".
    fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);

    struct Espacio espacios[100];
    fread(espacios, sizeof(struct Espacio), 100, tar_file);

    for (int i = 0; i < 100; i++) {
        printf("Índice %d - Inicio: %ld, Fin: %ld\n", i, espacios[i].inicio, espacios[i].fin);
    }

    // Cerrar el archivo tar cuando hayas terminado.
    fclose(tar_file);
}

void extraerEnMismoLugar(const char* tar_filename) {
    FILE* tar_file = fopen(tar_filename, "rb");
    if (!tar_file) {
        printf("Error al abrir el archivo tar: %s\n", tar_filename);
        return;
    }

    // Leer la información de los archivos desde el archivo tar.
    struct Archivo archivos[100];
    fread(archivos, sizeof(struct Archivo), 100, tar_file);

    // Extraer los archivos en el mismo lugar y guardar sus nombres.
    char* extracted_files[100];  // Arreglo para almacenar los nombres de los archivos extraídos.
    int num_extracted_files = 0; // Inicializar el número de archivos extraídos.
    for (int i = 0; i < 100; i++) {
        if (archivos[i].nombre[0] == '\0') {
            continue;
        }

        // Construir el nombre del archivo a extraer.
        char extract_filename[256];
        snprintf(extract_filename, sizeof(extract_filename), "./%s", archivos[i].nombre);

        FILE* extracted_file = fopen(extract_filename, "wb");
        if (!extracted_file) {
            printf("Error al crear el archivo extraído: %s\n", extract_filename);
            continue;
        }

        // Copiar el contenido del archivo desde el archivo tar.
        long file_size = archivos[i].peso;
        fseek(tar_file, archivos[i].inicio, SEEK_SET);
        char buffer[1024];
        size_t bytes_read;

        while (file_size > 0 && (bytes_read = fread(buffer, sizeof(char), sizeof(buffer), tar_file)) > 0) {
            size_t bytes_to_write = bytes_read;
            if (bytes_to_write > file_size) {
                bytes_to_write = file_size;
            }
            fwrite(buffer, sizeof(char), bytes_to_write, extracted_file);
            file_size -= bytes_to_write;
        }

        // Guardar el nombre del archivo extraído en el arreglo.
        extracted_files[num_extracted_files] = strdup(archivos[i].nombre);
        num_extracted_files++;

        fclose(extracted_file);
    }

    fclose(tar_file);
    printf("Archivos extraídos en el mismo lugar donde se encuentra '%s'.\n", tar_filename);

    // Llamar a empacarArchivos después de extraer todos los archivos.
    empacarArchivos("copia.tar", (const char**)extracted_files, num_extracted_files);

    // Liberar la memoria de los nombres de los archivos extraídos.
    for (int i = 0; i < num_extracted_files; i++) {
      remove(extracted_files[i]);
        free(extracted_files[i]);
    }
    remove(tar_filename);
    rename("copia.tar", tar_filename);
}






void listarArchivosTAR(const char* tar_filename){
  FILE* archivoTAR = fopen(tar_filename, "rb");

  if (archivoTAR == NULL) {
    printf("Error al abrir el archivo '%s'.\n", tar_filename);
    return;
  }

  struct Archivo archivo;
  bool control = false; // Variable de control para verificar si se encontró al menos un archivo

  for (int i = 0; i < 100; i++) {
    if (fread(&archivo, sizeof(struct Archivo), 1, archivoTAR) != 1) {
      break;  // Salir del bucle si no se pudo leer un registro.
    }

    // Verificar si el peso del archivo es -1 (o cualquier otro valor que quieras omitir)
    if (archivo.peso != -1) {
      printf("-----------------------------------------------------\n");
      printf("Nombre del archivo:\n\t\t\t%s\n", archivo.nombre);
      printf("Peso correspondiente:\n\t\t\t%ld bytes.\n", archivo.peso);
      printf("Byte de inicio:\n\t\t\t%ld bytes.\n", archivo.inicio);
      printf("Byte final:\n\t\t\t%ld bytes.\n", archivo.fin);

      control = true; // Indicar que se encontró al menos un archivo.
    }
  }

  if (!control) {
    printf("\nEl archivo '%s' no contiene archivos registrados en este momento.\n", tar_filename);
  }

  fclose(archivoTAR);
}


void agregarArchivo(const char* tar_filename, const char* archivo_a_agregar) {
    // Abre el archivo a agregar para obtener su tamaño.
    FILE *file;
    long file_size;
    file = fopen(archivo_a_agregar, "rb");

    if (file == NULL) {
        printf("No se pudo abrir el archivo: %s\n", archivo_a_agregar);
        return;
    }

    // Mueve el puntero de lectura al final del archivo para obtener su tamaño.
    fseek(file, 0, SEEK_END);

    // Obtiene la posición actual, que es el tamaño del archivo.
    file_size = ftell(file);

    fclose(file);

    // Abre el archivo TAR.
    FILE* tar_file = fopen(tar_filename, "rb+");

    if (tar_file == NULL) {
        printf("Error al abrir el archivo tar: %s\n", tar_filename);
        return;
    }

    struct Archivo archivos[100];
    fread(archivos, sizeof(struct Archivo), 100, tar_file);

    struct Espacio espacios[100];
    fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);
    fread(espacios, sizeof(struct Espacio), 100, tar_file);

    // Comprueba si el archivo a agregar ya existe en el archivo TAR.
    for (int i = 0; i < 100; i++) {
        if (archivos[i].peso != -1 && strcmp(archivos[i].nombre, archivo_a_agregar) == 0) {
            printf("El archivo %s ya existe en el archivo TAR. Actualiza en lugar de agregar.\n", archivo_a_agregar);
            fclose(tar_file);
            return;
        }
    }

    int espacio_encontrado = -1;
    long inicio_archivo = -1;
    long fin_archivo = -1;
    long peso_archivo = -1;

    for (int i = 0; i < 100; i++) {
        if (espacios[i].inicio != -1 && espacios[i].fin != -1) {
            long espacio_size = espacios[i].fin - espacios[i].inicio;

            if (espacios[i].inicio + espacio_size >= inicio_archivo + peso_archivo) {
                espacio_encontrado = i;
                inicio_archivo = espacios[i].inicio;
                fin_archivo = espacios[i].fin;
                peso_archivo = archivos[i].peso;

                break;
            }
        }
    }

    if (espacio_encontrado != -1) {
        printf("Espacio adecuado encontrado en el índice %d:\n", espacio_encontrado);
        printf("Inicio del espacio: %ld\n", inicio_archivo);
        printf("Fin del espacio: %ld\n", fin_archivo);
        printf("Peso del archivo a agregar: %ld\n", file_size);
        printf("Inicio del espacio + Tamaño del archivo: %ld\n", inicio_archivo + peso_archivo);

        // Verifica si el tamaño del archivo a agregar es igual al espacio disponible.
        if (file_size == fin_archivo - inicio_archivo+1) {
            printf("El tamaño del archivo es igual al espacio disponible.\n");
            // Llama a la función para actualizar registros.
            actualizarRegistros(tar_file, archivo_a_agregar, file_size, inicio_archivo, file_size + inicio_archivo);

            // Reemplaza el espacio existente en el índice encontrado con el nuevo espacio no disponible.
            espacios[espacio_encontrado].inicio = -1;
            espacios[espacio_encontrado].fin = -1;
/*
            // Cierra el archivo después de hacer las modificaciones.
            fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);
            fwrite(espacios, sizeof(struct Espacio), 100, tar_file);
            fclose(tar_file);
*/
        } else if (file_size < fin_archivo - inicio_archivo+1) {
            printf("El tamaño del archivo es menor que el espacio disponible.\n");

            // Llama a la función para actualizar registros.
            actualizarRegistros(tar_file, archivo_a_agregar, file_size, inicio_archivo, file_size + inicio_archivo);

            // Crea un nuevo archivo con el espacio restante.
            long espacio_restante = fin_archivo - (inicio_archivo + file_size);
            if (espacio_restante > 0) {
                char* name = "loco"; // Nombre del nuevo archivo necesario
                FILE* nuevo_archivo = fopen(name, "wb");
                if (nuevo_archivo != NULL) {
                    // Asigna el peso del nuevo archivo.
                    fseek(nuevo_archivo, espacio_restante - 1, SEEK_SET);
                    fputc(0, nuevo_archivo);
                    fclose(nuevo_archivo);

                    // Llama a la función para actualizar registros del nuevo archivo.
                    long nuevo_inicio = fin_archivo + 1;
                    long nuevo_fin = nuevo_inicio + espacio_restante - 1;
                    printf("Nuevo archivo creado con éxito: %s\n", name);
                    actualizarRegistros(tar_file, "", espacio_restante,  file_size + inicio_archivo + 1, file_size + inicio_archivo + espacio_restante );

                    // Reemplaza el espacio existente en el índice encontrado con el nuevo espacio.
                    espacios[espacio_encontrado].inicio = inicio_archivo + file_size + 1;
                    espacios[espacio_encontrado].fin = fin_archivo;

                    // Cierra el archivo después de hacer las modificaciones.
                    fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);
                    fwrite(espacios, sizeof(struct Espacio), 100, tar_file);
                    
                } else {
                    printf("No se pudo crear el nuevo archivo para el espacio restante.\n");
                }
            }
        } else {
            printf("El tamaño del archivo es mayor que el espacio disponible.\n");

            

            // Agregar el archivo al final del arreglo de archivos si es mayor que el espacio.
            int espacio_disponible = -1;
            for (int i = 0; i < 100; i++) {
                if (archivos[i].peso == -1) {
                    espacio_disponible = i;
                    // Se ha encontrado un espacio vacío en el arreglo de archivos.
                    break;
                }
            }

            if (espacio_disponible != -1 && espacio_disponible < 100) {
                // Copia el nombre del archivo a agregar en la estructura de archivos.
                strncpy(archivos[espacio_disponible].nombre, archivo_a_agregar, sizeof(archivos[espacio_disponible].nombre));
                archivos[espacio_disponible].peso = file_size;
                archivos[espacio_disponible].inicio = archivos[espacio_disponible-1].fin + 1;
                archivos[espacio_disponible].fin = archivos[espacio_disponible-1].fin + file_size;

                fseek(tar_file, espacio_disponible * sizeof(struct Archivo), SEEK_SET);
                fwrite(&archivos[espacio_disponible], sizeof(struct Archivo), 1, tar_file);

                

                printf("Archivo %s agregado en la posición %d.\n", archivo_a_agregar, espacio_disponible);
            } else {
                printf("No hay espacio en el arreglo de archivos para agregar %s.\n", archivo_a_agregar);
            }
        }
    } else {
        
        printf("No se encontró un espacio adecuado para el archivo %s en el archivo tar.\n", archivo_a_agregar);
                    // Agregar el archivo al final del arreglo de archivos si es mayor que el espacio.
            int espacio_disponible = -1;
            for (int i = 0; i < 100; i++) {
                if (archivos[i].peso == -1) {
                    espacio_disponible = i;
                    // Se ha encontrado un espacio vacío en el arreglo de archivos.
                    break;
                }
            }

            if (espacio_disponible != -1 && espacio_disponible < 100) {
                // Copia el nombre del archivo a agregar en la estructura de archivos.
                strncpy(archivos[espacio_disponible].nombre, archivo_a_agregar, sizeof(archivos[espacio_disponible].nombre));
                archivos[espacio_disponible].peso = file_size;
                archivos[espacio_disponible].inicio = archivos[espacio_disponible-1].fin + 1;
                archivos[espacio_disponible].fin = archivos[espacio_disponible-1].fin + file_size;

                fseek(tar_file, espacio_disponible * sizeof(struct Archivo), SEEK_SET);
                fwrite(&archivos[espacio_disponible], sizeof(struct Archivo), 1, tar_file);
        
    }
    }

    fclose(tar_file);
}

int compararPorInicio(const void* a, const void* b) {
    const struct Espacio* espacioA = (const struct Espacio*)a;
    const struct Espacio* espacioB = (const struct Espacio*)b;

    if (espacioA->inicio != -1 && espacioB->inicio != -1) {
        if (espacioA->inicio < espacioB->inicio) {
            return -1;
        } else if (espacioA->inicio > espacioB->inicio) {
            return 1;
        } else {
            return 0;
        }
    } else if (espacioA->inicio != -1 && espacioB->inicio == -1) {
        return -1; // Espacio A va antes que el espacio B
    } else if (espacioA->inicio == -1 && espacioB->inicio != -1) {
        return 1; // Espacio B va antes que el espacio A
    } else {
        return 0;
    }
}

void concatenarEspaciosContiguos(const char* tar_filename) {
    // Abrir el archivo TAR en modo lectura y escritura.
    FILE* tar_file = fopen(tar_filename, "rb+");

    if (tar_file == NULL) {
        printf("Error al abrir el archivo tar: %s\n", tar_filename);
        return;
    }

    // Mueve el cursor al principio del arreglo "Espacios".
    fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);

    struct Espacio espacios[100];
    fread(espacios, sizeof(struct Espacio), 100, tar_file);

    // Ordena la matriz espacios por el valor de inicio.
    qsort(espacios, 100, sizeof(struct Espacio), compararPorInicio);

    int contiguos = 0;
    int indice_inicio = -1;
    int inicio_primero = -1;
    int fin_ultimo = -1;

    for (int i = 0; i < 100; i++) {
        if (espacios[i].inicio != -1 && espacios[i].fin != -1) {
            if (indice_inicio == -1) {
                indice_inicio = i;
                inicio_primero = espacios[i].inicio;
            }
            fin_ultimo = espacios[i].fin;
            contiguos++;
        } else {
            if (contiguos > 0) {
                printf("Espacios contiguos: %d, Índice de inicio: %d, Índice de fin: %d, Inicio del primero: %ld, Fin del último: %ld\n", contiguos, indice_inicio, i - 1, inicio_primero, fin_ultimo);

                // Eliminar los espacios antiguos en el rango.
                for (int j = indice_inicio; j <= i - 1; j++) {
                    espacios[j].inicio = -1;
                    espacios[j].fin = -1;
                }

                // Crear un nuevo espacio que abarque desde el inicio del primero al fin del último.
                espacios[indice_inicio].inicio = inicio_primero;
                espacios[indice_inicio].fin = fin_ultimo;
            }
            contiguos = 0;
            indice_inicio = -1;
            inicio_primero = -1;
            fin_ultimo = -1;
        }
    }

    // Comprobar si los espacios contiguos llegan hasta el final.
    if (contiguos > 0) {
        printf("Espacios contiguos: %d, Índice de inicio: %d, Índice de fin: %d, Inicio del primero: %ld, Fin del último: %ld\n", contiguos, indice_inicio, 99, inicio_primero, fin_ultimo);
        // Eliminar los espacios antiguos en el rango.
        for (int j = indice_inicio; j <= 99; j++) {
            espacios[j].inicio = -1;
            espacios[j].fin = -1;
        }
        // Crear un nuevo espacio que abarque desde el inicio del primero al fin del último.
        espacios[indice_inicio].inicio = inicio_primero;
        espacios[indice_inicio].fin = fin_ultimo;
    }

    // Ordena nuevamente la matriz para mover los espacios con inicio igual a -1 al final.
    qsort(espacios, 100, sizeof(struct Espacio), compararPorInicio);

    // Mueve el cursor al principio del arreglo "Espacios" antes de escribir los datos actualizados.
    fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);
    fwrite(espacios, sizeof(struct Espacio), 100, tar_file);

    // Cerrar el archivo tar cuando hayas terminado.
    fclose(tar_file);
}


// Función de comparación para qsort
int compararArchivos(const void *a, const void *b) {
    const struct Archivo *archivoA = (const struct Archivo *)a;
    const struct Archivo *archivoB = (const struct Archivo *)b;
    return archivoA->inicio - archivoB->inicio;
}

void ordenarArchivosPorInicio(const char* tar_filename) {
    FILE* tar_file = fopen(tar_filename, "rb+");

    if (tar_file == NULL) {
        printf("Error al abrir el archivo tar: %s\n", tar_filename);
        return;
    }

    // Leer todos los registros de archivo en el archivo TAR
    struct Archivo archivos[100];
    int numArchivos = 0;
    while (numArchivos < 100 && fread(&archivos[numArchivos], sizeof(struct Archivo), 1, tar_file) == 1) {
        if (archivos[numArchivos].inicio > -1) {
            numArchivos++;
        }
    }

    // Ordenar los archivos en función de su valor de inicio
    qsort(archivos, numArchivos, sizeof(struct Archivo), compararArchivos);

    // Sobrescribir el archivo TAR con los archivos ordenados
    fseek(tar_file, 0, SEEK_SET);
    for (int i = 0; i < numArchivos; i++) {
        fwrite(&archivos[i], sizeof(struct Archivo), 1, tar_file);
    }

    fclose(tar_file);
}



void actualizarArchivo(const char* tar_filename, const char* archivo_a_actualizar) {
    // Abrir el archivo TAR en modo lectura.
    struct Espacio espacios[100];
    struct Archivo archivos[100];
    FILE* tar_file = fopen(tar_filename, "rb+");
    
    if (tar_file == NULL) {
        printf("Error al abrir el archivo tar: %s\n", tar_filename);
        return;
    }
  /*
    // Buscar el archivo en el TAR.
    ////////////////////////////////////////////////////struct Archivo archivos[100];
    int encontrado = 0;
    int i;
    // Leer todos los registros de archivo en el archivo TAR
    for (i = 0; i < 100; i++) {
        if (fread(&archivos[i], sizeof(struct Archivo), 1, tar_file) != 1) {
            // Si no se pudo leer un registro, salir del bucle
            break;
        }

        if (strcmp(archivos[i].nombre, archivo_a_actualizar) == 0) {
            // El archivo buscado se encontró en el arreglo de estructuras
            encontrado = 1;
            break;
            
        }
    }
    */



    int encontrado = 0;  // Variable para indicar si el archivo se encontró

    // Leer el contenido del archivo tar.
    
    fread(archivos, sizeof(struct Archivo), 100, tar_file);
    int posicion = 0;
    for (int i = 0; i < 100; i++) {
        if (strcmp(archivos[i].nombre, archivo_a_actualizar) == 0) {
            // Encontrado el archivo con el nombre deseado.
            encontrado = 1;
            posicion = i;
            break;  // No es necesario seguir buscando, ya lo encontraste.
        }
    }
    ///////
    
    if (encontrado) {
        printf("Archivo encontrado en el TAR:\n");
        printf("Nombre: %s\n", archivos[posicion].nombre);
        printf("Tamaño en el TAR: %ld bytes\n", archivos[posicion].peso);
        printf("Inicio: %ld\n", archivos[posicion].inicio);
        printf("Fin: %ld\n", archivos[posicion].fin);
        // Guardar los valores de inicio y fin del archivo antes de borrarlo.
        long inicio_viejo = archivos[posicion].inicio;
        long fin_viejo = archivos[posicion].fin;

        
        // Obtener el peso del archivo a actualizar.
        FILE* archivo_a_actualizar_fp = fopen(archivo_a_actualizar, "rb");
        if (archivo_a_actualizar_fp) {
            fseek(archivo_a_actualizar_fp, 0, SEEK_END);
            long peso_a_actualizar = ftell(archivo_a_actualizar_fp);
            fclose(archivo_a_actualizar_fp);
            printf("Peso del archivo a actualizar: %ld bytes\n", peso_a_actualizar);

            
            
            // Borrar el archivo.
            borrarArchivo(tar_filename, archivo_a_actualizar);

            // Concatenar los espacios contiguos.
            concatenarEspaciosContiguos(tar_filename);
            // Mueve el cursor al principio del arreglo "Espacios".
            fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);
            //////////////////////////////////////////////////////////////struct Espacio espacios[100];
            fread(espacios, sizeof(struct Espacio), 100, tar_file);
            long inicio_nuevo;
            long fin_nuevo;
            int espacio_encontrado;
            for (int i = 0; i < 100; i++) {
                if ((espacios[i].inicio != -1 && espacios[i].fin != -1) &&
                    (espacios[i].inicio == inicio_viejo ||
                     espacios[i].fin == fin_viejo ||
                     (espacios[i].inicio < inicio_viejo && espacios[i].fin > fin_viejo))) {
                    printf("+Inicio: %ld, Fin: %ld\n", espacios[i].inicio, espacios[i].fin);
                    printf("-Inicio viejo: %ld, Fin viejo: %ld\n", inicio_viejo, fin_viejo);
                    
                    inicio_nuevo = espacios[i].inicio;
                    fin_nuevo = espacios[i].fin;
                    espacio_encontrado = i;
                }
            }
            printf("----- %ld, ---- %ld\n",  fin_nuevo-inicio_nuevo, peso_a_actualizar);
            if (fin_nuevo - inicio_nuevo  == peso_a_actualizar) {
                printf("El espacio disponible es exactamente del mismo tamaño que el archivo a actualizar.\n");
                // Llama a la función para actualizar registros.
                actualizarRegistros(tar_file, archivo_a_actualizar, peso_a_actualizar, inicio_nuevo, peso_a_actualizar + inicio_nuevo);

                // Reemplaza el espacio existente en el índice encontrado con el nuevo espacio no disponible.
                espacios[espacio_encontrado].inicio = -1;
                espacios[espacio_encontrado].fin = -1;

                // Cierra el archivo después de hacer las modificaciones.
                fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);
                fwrite(espacios, sizeof(struct Espacio), 100, tar_file);
                
            } else if (fin_nuevo - inicio_nuevo > peso_a_actualizar) {
                printf("El espacio disponible es mayor que el tamaño del archivo a actualizar.\n");
                // Llama a la función para actualizar registros.
                actualizarRegistros(tar_file, archivo_a_actualizar, peso_a_actualizar, inicio_nuevo, peso_a_actualizar + inicio_nuevo);
    
                // Crea un nuevo archivo con el espacio restante.
                long espacio_restante = fin_nuevo - (inicio_nuevo + peso_a_actualizar);
                if (espacio_restante > 0) {
                    char* name = "loco"; // Nombre del nuevo archivo necesario
                    FILE* nuevo_archivo = fopen(name, "wb");
                    if (nuevo_archivo != NULL) {
                        // Asigna el peso del nuevo archivo.
                        fseek(nuevo_archivo, espacio_restante - 1, SEEK_SET);
                        fputc(0, nuevo_archivo);
                        fclose(nuevo_archivo);
    
                        // Llama a la función para actualizar registros del nuevo archivo.
                        long nuevo_inicio = fin_nuevo + 1;
                        long nuevo_fin = nuevo_inicio + espacio_restante - 1;
                        printf("Nuevo archivo creado con éxito: %s\n", name);
                        actualizarRegistros(tar_file, "", espacio_restante,  peso_a_actualizar + inicio_nuevo + 1, peso_a_actualizar + inicio_nuevo + espacio_restante );

                        // Reemplaza el espacio existente en el índice encontrado con el nuevo espacio.
                        espacios[espacio_encontrado].inicio = inicio_nuevo + peso_a_actualizar + 1;
                        espacios[espacio_encontrado].fin = fin_nuevo;

                        // Cierra el archivo después de hacer las modificaciones.
                        fseek(tar_file, sizeof(struct Archivo) * 100, SEEK_SET);
                        fwrite(espacios, sizeof(struct Espacio), 100, tar_file);
                        //ordenarArchivosPorInicio(tar_file);
                      } else {
                    printf("No se pudo crear el nuevo archivo para el espacio restante.\n");
                      }
                  }
                        
            } else {
                printf("El espacio disponible es insuficiente para el archivo a actualizar.\n");
                printf("La bandera A está 1%ld \n",archivos[4].peso);
                // Agregar el archivo al final del arreglo de archivos si es mayor que el espacio.
                
                int espacio_disponible = -1;
                for (int j = 0; j < 100; j++) {
                  printf("dale carajo\n");
                    if (archivos[j].peso == -1) {
                      printf("La bandera A está activada %ld \n", archivos[j].peso);
                        espacio_disponible = j;
                        // Se ha encontrado un espacio vacío en el arreglo de archivos.
                        break;
                    }
                }
                printf("La bandera A está 2\n");
                if (espacio_disponible != -1 && espacio_disponible < 100) {
                    // Copia el nombre del archivo a agregar en la estructura de archivos.
                    strncpy(archivos[espacio_disponible].nombre, archivo_a_actualizar, sizeof(archivos[espacio_disponible].nombre));
                    archivos[espacio_disponible].peso = peso_a_actualizar;
                    archivos[espacio_disponible].inicio = archivos[espacio_disponible-1].fin + 1;
                    archivos[espacio_disponible].fin = archivos[espacio_disponible-1].fin + peso_a_actualizar;

                    fseek(tar_file, espacio_disponible * sizeof(struct Archivo), SEEK_SET);
                    fwrite(&archivos[espacio_disponible], sizeof(struct Archivo), 1, tar_file);



                    printf("Archivo %s agregado en la posición %d.\n", archivo_a_actualizar, espacio_disponible);
                  }
                  }
        } else {
            printf("Error al abrir el archivo a actualizar: %s\n", archivo_a_actualizar);
        }
    } else {
        printf("No se puede actualizar un archivo que no está en el TAR.\n");
    }
    

    // Cerrar el archivo TAR cuando hayas terminado.
    fclose(tar_file);
}



int main(int argc, char* argv[]) {

  const char* tar_file = "probanding.tar";
  int num_files = 7;
  const char* files_to_pack[] = {"archivo1.pdf","archivo2.pdf","archivo3.pdf","archivo4.pdf","archivo5.pdf","archivo6.pdf","archivo7.pdf", NULL};
/*

  // Nombre del archivo PDF
    const char* filename = "archivo2.pdf";
    
    // Tamaño deseado en bytes
    long desiredSize = 8813684;

    // Abre el archivo en modo escritura binaria
    FILE* file = fopen(filename, "wb");

    if (file != NULL) {
        // Llena el archivo con ceros hasta alcanzar el tamaño deseado
        long currentSize = 0;
        while (currentSize < desiredSize) {
            // Calcula la cantidad de ceros a escribir en esta iteración
            long remainingSize = desiredSize - currentSize;
            int blockSize = remainingSize < 1024 ? remainingSize : 1024;

            // Escribe ceros en bloques de 1024 bytes
            char buffer[1024] = {0};
            fwrite(buffer, 1, blockSize, file);

            currentSize += blockSize;
        }
      }
        // Cierra el archivo
        fclose(file);
  */
  //crearTAR(tar_file);
  //archivos_pruebas(tar_file);

  //empacarArchivos(tar_file, files_to_pack, num_files);
  //listarTAR("probanding.tar");
  //archivos_pruebas(tar_file);

  //extraerArchivos("probanding.tar");

  //borrarArchivo("ArchivoTAR_Lleno.tar", "archivo4.pdf");
  //listarTAR("probanding.tar");
  //listarEspacios("ArchivoTAR_SinArchivos-2-4-6.tar");
  //concatenarEspaciosContiguos("ArchivoTAR_Lleno.tar");

  

  /**
  listarTAR("ArchivoTAR_Lleno.tar");
  listarEspacios("ArchivoTAR_Lleno.tar");
  agregarArchivo("ArchivoTAR_Lleno.tar","archivo1.pdf");
  listarTAR("ArchivoTAR_Lleno.tar");
  listarEspacios("ArchivoTAR_Lleno.tar");
  
  
**/
  //agregarArchivo("aq.tar","pufi.pdf");
  //listarArchivosTAR("aq.tar");
  //listarEspacios("aq.tar");
  //concatenarEspaciosContiguos("simulit.tar");
  //agregarArchivo("simulit.tar","archivo2.pdf");
  //listarTAR("simulit.tar");
  //istarEspacios("simulit.tar");
  //agregarArchivo("ArchivoTAR_Lleno.tar","archivo4.pdf");
  //crearTAR("test42.tar");
  //empacarArchivos("test.tar", "archivo1.pdf"  "archivo3.pdf" "archivo4.pdf" "archivo5.pdf",4);
  //concatenarEspaciosContiguos("simul.tar");


  //borrarArchivo("simuli.tar", "archivo2.pdf");
  //borrarArchivo("simuli.tar", "archivo3.pdf");
  //borrarArchivo("simulit.tar", "archivo2.pdf");
  //borrarArchivo("simulit.tar", "archivo3.pdf");
  /**
  
  agregarArchivo("simuli.tar","archivo2.pdf");
  listarTAR("simuli.tar");
  listarEspacios("simuli.tar");
  */
  
  //concatenarEspaciosContiguos("simuli.tar");


  //concatenarEspaciosContiguos("ArchivoTAR_SinArchivos-2-4-6.tar");
  //agregarArchivo("ArchivoTAR_SinArchivos-2-4-6.tar","archivo1.pdf");

  //listarTAR("ArchivoTAR_SinArchivos-2-4-6.tar");
  //listarEspacios("ArchivoTAR_SinArchivos-2-4-6.tar");
  //crearTAR("simu.tar");
  //empacarArchivos(tar_file, files_to_pack, num_files);
  //empacarArchivos(tar_file, files_to_pack, num_files);
  //borrarArchivo(tar_file, "archivo3.pdf");
  //extraerEnMismoLugar(tar_file);
  //listarTAR("copia.tar");
  //archivos_pruebas("copia.tar");
/*
  crearTAR("fff.tar");
  const char* archivos_entrada[] = {"archivo1.pdf", "archivo2.pdf", "archivo3.pdf", "archivo4.pdf", "archivo5.pdf"};
  int num_archivos = 5;
  const char* archivo_salida = "fff.tar";

  // Llamar a la función con valores predeterminados
  empacarArchivos(archivo_salida, archivos_entrada, num_archivos);
*/
  //borrarArchivo("test36.tar", "archivo88.pdf");
  //borrarArchivo("f.tar", "archivo1.pdf");
  //concatenarEspaciosContiguos("bh.tar");
  //listarTAR("actualizar.tar");
  actualizarArchivo("ffff.tar", "archivo2.pdf");
  //crearTAR("cartago");
  //agregarArchivo("test.tar","archivo1.pdf");
  //agregarArchivo("f.tar","archivo1.pdf");
  //agregarArchivo("cartago.tar","archivo3.pdf");
  //agregarArchivo("cartago.tar","archivo4.pdf");
  //agregarArchivo("cartago.tar","archivo5.pdf");
  listarArchivosTAR("ffff.tar");
  listarEspacios("ffff.tar");

  return 0;
}