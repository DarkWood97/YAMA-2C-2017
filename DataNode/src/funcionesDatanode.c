
#include "funcionesDatanode.h"

void cargarDataNode(t_config* configuracionDN){

    if(!config_has_property(configuracionDN, "IP_FILESYSTEM")){
        log_error(loggerDatanode, "El archivo de configuracion no contiene IP_FILESYSTEM");
        exit(-1);
    }else{
        IP_FILESYSTEM = string_new();
        string_append(&IP_FILESYSTEM, config_get_string_value(configuracionDN, "IP_FILESYSTEM"));
    }
    if(!config_has_property(configuracionDN, "PUERTO_FILESYSTEM")){
    	log_error(loggerDatanode, "El archivo de configuracion no contiene PUERTO_FILESYSTEM");
    	exit(-1);
    }else{
        PUERTO_FILESYSTEM = config_get_int_value(configuracionDN, "PUERTO_FILESYSTEM");
    }
    if(!config_has_property(configuracionDN, "NOMBRE_NODO")){
    	log_error(loggerDatanode, "El archivo de configuracion no contiene NOMBRE_NODO");
    	exit(-1);
    }else{
        NOMBRE_NODO = string_new();
        string_append(&NOMBRE_NODO, config_get_string_value(configuracionDN, "NOMBRE_NODO"));
    }
    if(!config_has_property(configuracionDN, "PUERTO_DATANODE")){
    	log_error(loggerDatanode, "El archivo de configuracion no contiene PUERTO_DATANODE");
    	exit(-1);
    }else{
        PUERTO_DATANODE = config_get_int_value(configuracionDN, "PUERTO_DATANODE");
    }
    if(!config_has_property(configuracionDN, "RUTA_DATABIN")){
    	log_error(loggerDatanode, "El archivo de configuracion no contiene RUTA_DATABIN");
    	exit(-1);
    }else{
        RUTA_DATABIN = string_new();
        string_append(&RUTA_DATABIN, config_get_string_value(configuracionDN, "RUTA_DATABIN"));
    }
    config_destroy(configuracionDN);
}

void realizarHandshakeFS(int socketFS){
	sendDeNotificacion(socketFS, ES_DATANODE);
	int notificacion = recvDeNotificacion(socketFS);
	if(notificacion != ES_FS){
		log_error(loggerDatanode, "La conexion establecida es erronea.");
		exit(-1);
	}
	log_info(loggerDatanode, "Conexion con FileSystem existosa.");
}

void cargarBin(){

	if(stat(RUTA_DATABIN,&infoDatabin) < 0){
		// Error al abrir el archivo
		log_error(loggerDatanode,"Error al tratar de abrir el archivo.");
		log_destroy(loggerDatanode);
		exit(-1);

	} else {

		log_info(loggerDatanode,"Archivo binario de %d bytes encontrado", infoDatabin.st_size);

		// Abro el archivo
		int archivo = open(RUTA_DATABIN, O_RDWR);

		// Lo mapeo a memoria
		mapArchivo = mmap(0, infoDatabin.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, archivo, 0);

		// Creo el bitarray
		cantBloques = infoDatabin.st_size/SIZEBLOQUE;
		memBitarray = malloc(cantBloques);
		bitarray = bitarray_create_with_mode(memBitarray,cantBloques,MSB_FIRST);

		// Vacio bitarray
		int i = 0;
		while(i<cantBloques){
			bitarray_clean_bit(bitarray, i);
			i++;
		}

	}

}

int escribirBloque(int nroBloque, void * dataBloque){

	if(nroBloque >= cantBloques){

		return -1;

	} else {

		memcpy(mapArchivo+(nroBloque*SIZEBLOQUE), dataBloque, SIZEBLOQUE);
		msync(mapArchivo,nroBloque*SIZEBLOQUE,MS_SYNC);

		return 0;

	}

}

void * leerBloque(int nroBloque){

	if(nroBloque >= cantBloques){

		return NULL;

	} else {

		void * dataBloque = malloc(SIZEBLOQUE);
		memcpy(dataBloque,mapArchivo+(nroBloque*SIZEBLOQUE),SIZEBLOQUE);
		return dataBloque;

	}

}

void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    int i;
    for (i = 0; i < len; i++) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len-1] = '\0';
}
