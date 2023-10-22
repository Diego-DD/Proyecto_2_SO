#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void create_tar(const char* output_filename, const char* input_files[], int num_files) {
    FILE* tar_file = fopen(output_filename, "wb");
    if (!tar_file) {
        printf("Error al crear el archivo tar.\n");
        return;
    }

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

    fclose(tar_file);
}

void extract_tar(const char* input_filename) {
    FILE* tar_file = fopen(input_filename, "rb");
    if (!tar_file) {
        printf("Error al abrir el archivo tar.\n");
        return;
    }

    // Crear un directorio para los archivos extraídos
    char extract_dir[100];
    snprintf(extract_dir, sizeof(extract_dir), "%s_extracted", input_filename);
    mkdir(extract_dir, 0777);

    while (!feof(tar_file)) {
        char file_name[100];
        long file_size;

        // Leer el nombre del archivo
        fread(file_name, sizeof(char), 100, tar_file);
        if (feof(tar_file)) {
            break;
        }

        // Leer el tamaño del archivo
        fread(&file_size, sizeof(long), 1, tar_file);

        // Construir la ruta del archivo extraído
        char extract_path[200];
        snprintf(extract_path, sizeof(extract_path), "%s/%s", extract_dir, file_name);

        // Crear los directorios necesarios para la ruta del archivo
        char* last_slash = strrchr(extract_path, '/');
        if (last_slash != NULL) {
            *last_slash = '\0';
            mkdir(extract_path, 0777);
            *last_slash = '/';
        }

        // Crear y escribir el contenido del archivo extraído
        FILE* extracted_file = fopen(extract_path, "wb");
        if (!extracted_file) {
            printf("Error al crear el archivo extraído: %s\n", extract_path);
            continue;
        }

        char buffer[file_size];
        size_t bytes_read;
        while (file_size > 0 && (bytes_read = fread(buffer, sizeof(char), sizeof(buffer), tar_file)) > 0) {
            fwrite(buffer, sizeof(char), bytes_read, extracted_file);
            file_size -= bytes_read;
        }
        fclose(extracted_file);
    }

    fclose(tar_file);
   
}


void list_tar_contents(const char* input_filename) {
    FILE* tar_file = fopen(input_filename, "rb");
    if (!tar_file) {
        printf("Error al abrir el archivo tar.\n");
        return;
    }

    char file_name[100];
    long file_size;

    while (!feof(tar_file)) {
        // Leer el nombre del archivo
        fread(file_name, sizeof(char), 100, tar_file);
        if (feof(tar_file)) {
            break;
        }

        // Leer el tamaño del archivo
        fread(&file_size, sizeof(long), 1, tar_file);

        // Imprimir el nombre del archivo
        printf("%s\n", file_name);

        // Saltar el contenido del archivo
        fseek(tar_file, file_size, SEEK_CUR);
    }

    fclose(tar_file);
}

void delete_file_from_tar(const char* input_filename, const char* file_to_delete) {
    FILE* tar_file = fopen(input_filename, "rb");
    if (!tar_file) {
        printf("Error al abrir el archivo tar.\n");
        return;
    }

    // Crear un archivo temporal para guardar los datos modificados
    FILE* temp_file = fopen("temp.tar", "wb");
    if (!temp_file) {
        printf("Error al crear el archivo temporal.\n");
        fclose(tar_file);
        return;
    }

    while (1) {
        char file_name[100];
        long file_size;

        // Leer el nombre del archivo
        size_t name_bytes_read = fread(file_name, sizeof(char), 100, tar_file);
        if (name_bytes_read < 100) {
            break;  // Fin del archivo tar
        }

        // Leer el tamaño del archivo
        fread(&file_size, sizeof(long), 1, tar_file);

        if (strcmp(file_name, file_to_delete) == 0) {
            // Saltar el contenido del archivo que se quiere eliminar
            fseek(tar_file, file_size, SEEK_CUR);
        } else {
            // Escribir el nombre del archivo
            fwrite(file_name, sizeof(char), 100, temp_file);

            // Escribir el tamaño del archivo
            fwrite(&file_size, sizeof(long), 1, temp_file);

            // Copiar el contenido del archivo
            char buffer[1024];
            size_t bytes_read;
            while (file_size > 0) {
                bytes_read = fread(buffer, sizeof(char), (file_size > sizeof(buffer) ? sizeof(buffer) : file_size), tar_file);
                fwrite(buffer, sizeof(char), bytes_read, temp_file);
                file_size -= bytes_read;
            }
        }
    }

    fclose(tar_file);
    fclose(temp_file);

    // Reemplazar el archivo original con el archivo temporal
    remove(input_filename);
    rename("temp.tar", input_filename);
}

void append_to_tar(const char* input_filename, const char* file_to_append) {
    FILE* tar_file = fopen(input_filename, "ab");
    if (!tar_file) {
        printf("Error al abrir el archivo tar.\n");
        return;
    }

    FILE* input_file = fopen(file_to_append, "rb");
    if (!input_file) {
        printf("Error al abrir el archivo: %s\n", file_to_append);
        fclose(tar_file);
        return;
    }

    // Obtener el tamaño del archivo
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    // Escribir el nombre del
        fwrite(file_to_append, sizeof(char), 100, tar_file);

    // Escribir el tamaño del archivo
    fwrite(&file_size, sizeof(long), 1, tar_file);

    // Escribir el contenido del archivo
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), input_file)) > 0) {
        fwrite(buffer, sizeof(char), bytes_read, tar_file);
    }

    fclose(input_file);
    fclose(tar_file);
}

int main(int argc, char* argv[]) {
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
        printf("NUMEROS = %d\n", num_files);
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
}