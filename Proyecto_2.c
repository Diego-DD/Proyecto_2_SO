// IMPORTACIÓN DE LIBRERÍAS.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

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
}

// PROGRAMA PRINCIPAL.

int main(int argc, char* argv[]) {

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

  // Ejecución de funciones solicitadas por comandos.

  if(c_command){
    char *nombreTAR = argv[3];
    crearTAR(nombreTAR, v_command);
  }
  if(x_command){
    // Función extraer archivos.
  }
  if(t_command){
    // Función listar tar.
  }
  if(d_command){
    // Función borrar archivos.
  }
  if(u_command){
    // Función reescribir archivo.
  }
  if(f_command){
    if(c_command == false){
      printf("\nEl comando <-f> debe ser usado en conjunto con el comando <-c>.\n\nPor favor vuelva a intentarlo.....\n\n");
      return 1;
    }
    // Función empacar archivos al crear tar.
  }
  if(r_command){
    // Función agregar archivos en una tar ya existente.
  }
  if(p_command){
    // Función desfragmentar tar.
  }

  return 0;
}




/* int main(int argc, char* argv[]) {

  if (argc < 3) {
        printf("Uso: %s <opción> <archivo_tar> [archivos...]\n", argv[0]);
        printf("Opciones:\n");
        printf("  -c: Crear archivo tar (empaquetar)\n");
        printf("  -x: Extraer archivo tar (desempaquetar)\n");
        printf("  -l: Listar contenido de archivo tar\n");
        printf("  -d: Eliminar archivo de archivo tar\n");
        printf("  -a: Agregar archivo a archivo tar\n");
        return 1;
    }

    const char* option = argv[1];
    const char* tar_file = argv[2];

    if (strcmp(option, "-c") == 0) {
        // Crear un archivo tar
        int num_files = argc - 3;
        const char** files_to_pack = (const char**)&argv[3];
        create_tar(tar_file, files_to_pack, num_files);
    } else if (strcmp(option, "-x") == 0) {
        // Extraer archivos del archivo tar
        extract_tar(tar_file);
    } else if (strcmp(option, "-l") == 0) {
        // Listar el contenido del archivo tar
        list_tar_contents(tar_file);
    } else if (strcmp(option, "-d") == 0) {
        if (argc < 4) {
            printf("Debe proporcionar el nombre del archivo a eliminar.\n");
            return 1;
        }

        // Eliminar el contenido del archivo tar
        const char* file_to_delete = argv[3];
        delete_file_from_tar(tar_file, file_to_delete);
    } else if (strcmp(option, "-a") == 0) {
        if (argc < 4) {
            printf("Debe proporcionar el nombre del archivo a agregar.\n");
            return 1;
        }

        // Agregar un nuevo archivo a archivo tar
        const char* file_to_append = argv[3];
        append_to_tar(tar_file, file_to_append);
    } else {
        printf("Opción no válida: %s\n", option);
        return 1;
    }
    return 0;
} */


