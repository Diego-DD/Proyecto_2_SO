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
  
  fclose(archivoTAR);
}

void actualizarArchivos(FILE* archivoTAR, const char* nombre, long peso, long inicio, long fin) {
    // Mueve el cursor al principio del archivo
    fseek(archivoTAR, 0, SEEK_SET);

    struct Archivo archivos[100];

    // Lee los datos existentes del archivo
    fread(archivos, sizeof(struct Archivo), 100, archivoTAR);

    int indiceDisponible = -1;

    // Busca el primer índice disponible (con valores de inicialización)
    for (int i = 0; i < 100; i++) {
        if (strcmp(archivos[i].nombre, "") == 0) {
            indiceDisponible = i;
            break;
        }
    }

    if (indiceDisponible != -1) {
        // Actualiza el elemento en el índice encontrado
        strcpy(archivos[indiceDisponible].nombre, nombre);
        archivos[indiceDisponible].peso = peso;
        archivos[indiceDisponible].inicio = inicio;
        archivos[indiceDisponible].fin = fin;

        // Mueve el cursor al principio del archivo antes de escribir los datos actualizados
        fseek(archivoTAR, 0, SEEK_SET);

        // Escribe los datos actualizados en el archivo
        fwrite(archivos, sizeof(struct Archivo), 100, archivoTAR);
    } else {
        printf("No hay índices disponibles en el archivo TAR para actualizar.\n");
    }
}

void mostrarArchivosEnTAR(const char* tar_filename) {
    FILE* archivoTAR = fopen(tar_filename, "rb");

    if (archivoTAR == NULL) {
        printf("Error al abrir el archivo TAR '%s'.\n", tar_filename);
        return;
    }

    struct Archivo archivo;
    int index = 0;

    for (int i = 0; i < 100; i++) {
        if (fread(&archivo, sizeof(struct Archivo), 1, archivoTAR) != 1) {
            break;  // Salir del bucle si no se pueden leer más elementos
        }

        if (strcmp(archivo.nombre, "") != 0) { //  || archivo.inicio != -1 || archivo.fin != -1
            printf("Elemento en la posición %d:\n", index);
            printf("Nombre: %s\n", archivo.nombre);
            printf("Peso: %d\n", archivo.peso);
            printf("Inicio: %d\n", archivo.inicio);
            printf("Fin: %d\n", archivo.fin);
        }
        index++;
    }

    fclose(archivoTAR);
}

void empacarTAR(const char* output_filename, const char* input_files[], int num_files) {
    FILE* tar_file = fopen(output_filename, "wb");
    if (!tar_file) {
        printf("Error al crear el archivo tar.\n");
        return;
    }

    fseek(tar_file, 0, SEEK_END);
    long file_size = ftell(tar_file);
    fseek(tar_file, 0, SEEK_SET);
    printf("PESO INICIAL = %d\n", file_size);

    // Escribir la información de cada archivo en el archivo tar
    for (int i = 0; i < num_files; i++) {
        FILE* input_file = fopen(input_files[i], "rb");
        if (!input_file) {
            printf("Error al abrir el archivo: %s\n", input_files[i]);
            continue;
        }

        // Obtener el tamaño del archivo
        fseek(input_file, 0, SEEK_END);
        long file_size = ftell(input_file);
        fseek(input_file, 0, SEEK_SET);

        // Escribir el nombre del archivo
        fwrite(input_files[i], sizeof(char), 100, tar_file);

        // Escribir el tamaño del archivo
        fwrite(&file_size, sizeof(long), 1, tar_file);

        // Escribir el contenido del archivo
        char buffer[1024];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), input_file)) > 0) {
            fwrite(buffer, sizeof(char), bytes_read, tar_file);
        }

        fclose(input_file);
    }

    fseek(tar_file, 0, SEEK_END);
    file_size = ftell(tar_file);
    fseek(tar_file, 0, SEEK_SET);
    printf("PESO FINAL = %d\n", file_size);

    fclose(tar_file);
}


int main(int argc, char* argv[]) {

  //const char* tar_file = argv[2];
  //int num_files = argc - 3;
  //printf("num_files = %d\n", num_files);
  //const char** files_to_pack = (const char**)&argv[3];
  /* for (int i = 0; files_to_pack[i] != NULL; i++) {
    printf("Elemento %d: %s\n", i+1, files_to_pack[i]);
  } */

  const char* tar_file = "probanding.tar";
  int num_files = 7;
  const char* files_to_pack[] = {"archivo1.pdf","archivo2.pdf","archivo3.pdf","archivo4.pdf","archivo5.pdf","archivo6.pdf","archivo7.pdf", NULL};
  empacarTAR(tar_file, files_to_pack, num_files);
  
  return 0;
}

  /* crearTAR("pruebas.tar");
  FILE* archivoTAR = fopen("pruebas.tar", "wb");
  actualizarArchivos(archivoTAR, "Pablo", 5, 10, 20);
  actualizarArchivos(archivoTAR, "Jesus", 7, 30, 40);
  actualizarArchivos(archivoTAR, "Alberto", 9, 50, 60);
  actualizarArchivos(archivoTAR, "Juan", 11, 70, 80);
  actualizarArchivos(archivoTAR, "Cristian", 13, 90, 100);
  actualizarArchivos(archivoTAR, "Rubén", 15, 110, 120);
  actualizarArchivos(archivoTAR, "María", 17, 130, 140);
  actualizarArchivos(archivoTAR, "Cuarentayocho", 19, 150, 160);
  fclose(archivoTAR);
  mostrarArchivosEnTAR("pruebas.tar"); */