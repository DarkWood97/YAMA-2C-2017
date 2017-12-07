/*
 * FuncionesDirectorios.c
 *
 *  Created on: 2/12/2017
 *      Author: utnso
 */

#include "FuncionesDirectorios.h"

int cantParam(char ** com){

	int i=0;
	while(com[i] != NULL){
		i++;
	}
	return i;

}

bool chequearParamCom(char ** com, int cantMin, int cantMax){
	int x = cantParam(com);
	return cantMin <= x && x <= cantMax;
}

bool contieneYamafs(char * path){

	char ** pathDesc = string_split(path,"/");

	int i = 0;
	while(pathDesc[i] != NULL){
		if(strcmp(pathDesc[i],"yamafs:") == 0){
			free(pathDesc);
			return true;
		} else {
			i++;
		}
	}
	free(pathDesc);
	return false;
}

bool existePath(char * path){

	char ** pathDesc = string_split(path,"/");
	return recorrerPath(pathDesc,0,-1);

}

bool recorrerPath(char ** pathDesc,int indice,int idPadre){

	char * pathAct = pathDesc[indice];
	char * pathProx = pathDesc[indice+1];

	tablaArchivos * archivo = existeArchivoPath(pathAct,idPadre);

	if (archivo != NULL){

		if(pathProx == NULL){
			return true;
		} else {
			return false;
		}

	} else {

		t_directory * directorio = existeDirectorioPath(pathAct,idPadre);
		if (directorio != NULL){

			if(pathProx == NULL){
				return true;
			} else {
				return recorrerPath(pathDesc,indice+1,directorio->index);
			}

		} else {
			return false;
		}

	}

}

tablaArchivos* existeArchivoPath(char * nombreArchivo,int idPadre){
	bool buscaPorNombre(tablaArchivos * archivo){
		return strcmp(archivo->nombreArchivo,nombreArchivo) == 0 && archivo->directorioPadre == idPadre;
	}
	return list_find(tablaGlobalArchivos, (void *)buscaPorNombre);;
}

t_directory* existeDirectorioPath(char * nombreDirectorio, int idPadre){
	bool buscaPorNombre(t_directory * directorio){
		return strcmp(directorio->nombre,nombreDirectorio) == 0 && directorio->padre==idPadre;
	}
	return list_find(listaDirectorios, (void *)buscaPorNombre);
}


int obtenerIdPadreArchivo(char ** pathDesc,int indice,int idPadre){

	char * pathAct = pathDesc[indice];
	char * pathProx = pathDesc[indice+1];

	tablaArchivos * archivo = existeArchivoPath(pathAct,idPadre);

	if (archivo != NULL){

		if(pathProx == NULL){
			return archivo->directorioPadre;
		} else {
			// Ruta incorrecta
			return -2;
		}

	} else {

		t_directory * directorio = existeDirectorioPath(pathAct,idPadre);
		if (directorio != NULL){

			if(pathProx == NULL){
				// Es un directorio
				return -3;
			} else {
				return obtenerIdPadreArchivo(pathDesc,indice+1,directorio->index);
			}

		} else {
			// Ruta incorrecta
			return -2;
		}

	}

}


char* obtenerNombreUltimoPath(char** rutaDesmembrada){

	int posicion = 0;
	char* ultimoPath = string_new();
	while(1){

		if(rutaDesmembrada[posicion+1] == NULL){
			string_append(&ultimoPath, rutaDesmembrada[posicion]);
			break;
		}
		posicion++;

	}

	return ultimoPath;

}

char * obtenerArchivo(char * path, int idPadre){

	char * buffer = string_new();
	char ** pathDesc = string_split(path,"/");
	char * nombreArchivo = obtenerNombreUltimoPath(pathDesc);

	tablaArchivos * archivo = existeArchivoPath(nombreArchivo,idPadre);

	t_list * bloques = archivo->bloques;

	void funcionMagicaDeAplicacion(copiasXBloque * bloque){

		int bytes = bloque->bytes;
		copia * copia1 = bloque->copia1;
		copia * copia2 = bloque->copia2;
		contenidoNodo * nodo;

		bool funcionMagicaDeBusqueda(contenidoNodo * nodo){
			return strcmp(nodo->nodo,copia2->nodo);
		}

		bool funcionMagicaDeBusqueda2(contenidoNodo * nodo){
			return strcmp(nodo->nodo,copia1->nodo);
		}

		nodo = list_find(tablaGlobalNodos,(void *) funcionMagicaDeBusqueda);

		if(nodo == NULL){
			nodo = list_find(tablaGlobalNodos,(void *) funcionMagicaDeBusqueda2);
		}

		nodo->socket;

	}

	list_iterate(bloques,(void *) funcionMagicaDeAplicacion);

	return buffer;

}

/*
int buscarYamafs(char ** pathDesc){

	int i=0;

	while(pathDesc[i] != NULL){
		if(strcmp(pathDesc[i],"yamafs:") == 0){
			return i;
		} else {
			i++;
		}
	}

	if(pathDesc[i] == NULL){
		return -1;
	} else {
		return i;
	}

}

tablaArchivos* esArchivoPath(char * nombreArchivo,int idPadre){
	bool buscaPorNombre(tablaArchivos * archivo){
		return strcmp(archivo->nombreArchivo,nombreArchivo) == 0 && archivo->directorioPadre == idPadre;
	}
	return list_find(tablaGlobalArchivos, (void *)buscaPorNombre);;
}

t_directory* esDirectorioPath(char * nombreDirectorio, int idPadre){
	bool buscaPorNombre(t_directory * directorio){
		return strcmp(directorio->nombre,nombreDirectorio) == 0 && directorio->padre==idPadre;
	}
	return list_find(listaDirectorios, (void *)buscaPorNombre);
}

bool recorrerPath(char ** pathDesc,int indice,int idPadre){

	char * pathAct = pathDesc[indice];
	char * pathProx = pathDesc[indice+1];

	tablaArchivos * archivo = esArchivoPath(pathAct,idPadre);

	if (archivo != NULL){

		if(pathProx == NULL){
			return true;
		} else {
			return false;
		}

	} else {

		t_directory * directorio = esDirectorioPath(pathAct,idPadre);
		if (directorio != NULL){

			if(pathProx == NULL){
				return true;
			} else {
				return recorrerPath(pathDesc,indice+1,directorio->index);
			}

		} else {
			return false;
		}

	}

}

bool existePath(char * pathDirectorio){

	char ** pathDesc = string_split(pathDirectorio,"/");
	int indice = buscarYamafs(pathDesc);

	if(indice == -1){
		log_warning(loggerFileSystem,"El directorio no corresponde a yamafs");
		return false;
	} else {
		return recorrerPath(pathDesc,indice,-1);
	}

	free(pathDesc);

}

//-------------------------------- NEW DIRECTORIOS

//BORRAR DIRECTORIO

char* obtenerNombreUltimoPath(char** rutaDesmembrada){

	int posicion = 0;
	char* ultimoPath = string_new();
	while(1){

		if(rutaDesmembrada[posicion+1] == NULL){
			string_append(&ultimoPath, rutaDesmembrada[posicion]);
			break;
		}
		posicion++;

	}

	return ultimoPath;

}



int obtenerIdPadreArchivo(char ** pathDesc,int indice,int idPadre){

	char * pathAct = pathDesc[indice];
	char * pathProx = pathDesc[indice+1];

	tablaArchivos * archivo = esArchivoPath(pathAct,idPadre);

	if (archivo != NULL){

		if(pathProx == NULL){
			return archivo->directorioPadre;
		} else {
			// Ruta incorrecta
			return -2;
		}

	} else {

		t_directory * directorio = esDirectorioPath(pathAct,idPadre);
		if (directorio != NULL){

			if(pathProx == NULL){
				// No se puede hacer un rm de un directorio
				return -3;
			} else {
				return obtenerIdPadreArchivo(pathDesc,indice+1,directorio->index);
			}

		} else {
			// Ruta incorrecta
			return -2;
		}

	}

}



void obtenerIdPadreDirectorio(char ** pathDesc,int indice,int idPadre, int * idPadreDir, int * idDir){

	char * pathAct = pathDesc[indice];
	char * pathProx = pathDesc[indice+1];

	tablaArchivos * archivo = esArchivoPath(pathAct,idPadre);

	if (archivo != NULL){

		if(pathProx == NULL){
			// No se puede hacer un rm -d de un archivo
			(*idPadreDir) = -3;
		} else {
			// Ruta incorrecta
			(*idPadreDir) = -2;
		}

	} else {

		t_directory * directorio = esDirectorioPath(pathAct,idPadre);
		if (directorio != NULL){

			if(pathProx == NULL){
				(*idPadreDir) = directorio->padre;
				(*idDir) = directorio->index;
			} else {
				obtenerIdPadreDirectorio(pathDesc,indice+1,directorio->index, idPadreDir, idDir);
			}

		} else {
			// Ruta incorrecta
			(*idPadreDir) = -2;
		}

	}

}



void borrarArchivo(char * path){

	char ** pathDesc = string_split(path,"/");
	int indice = buscarYamafs(pathDesc);

	if(indice == -1){
		log_warning(loggerFileSystem,"El directorio no corresponde a yamafs");
	} else {

		int idPadre = obtenerIdPadreArchivo(pathDesc,indice,-1);

		if(idPadre == -2){
			log_error(loggerFileSystem,"El path es inexistente");
		} else if (idPadre == -3){
			log_error(loggerFileSystem,"No se puede hacer un rm de un directorio");
		} else {

			char * nombreArchivo = obtenerNombreUltimoPath(pathDesc);

			bool eliminarArchivo(tablaArchivos* archivo){
				return (strcmp(archivo->nombreArchivo,nombreArchivo)==0 && archivo->directorioPadre==idPadre);
			}

			tablaArchivos* archivo=list_remove_by_condition(tablaGlobalArchivos,(void*)eliminarArchivo);

			if(archivo!=NULL){

				free(archivo->nombreArchivo);
				free(archivo->tipo);
				free(archivo);

			}

			log_debug(loggerFileSystem,"Se ha removido el archivo correctamente");

			//tieneQueModificarBitArrays

		}

	}

}


void borrarDirectorio(char * path){

	char ** pathDesc = string_split(path,"/");
	int indice = buscarYamafs(pathDesc);

	if(indice == -1){
		log_warning(loggerFileSystem,"El directorio no corresponde a yamafs");
	} else {

		int idPadreDir, idDir;
		obtenerIdPadreDirectorio(pathDesc,indice,-1, &idPadreDir, &idDir);

		if(idPadreDir == -2){
			log_error(loggerFileSystem,"El path es inexistente");
		} else if (idPadreDir == -3){
			log_error(loggerFileSystem,"No se puede hacer un rm -d de un archivo");
		} else {

			bool existenArchivosEnDir(tablaArchivos * archivo){
				return archivo->directorioPadre==idDir;
			}

			if(list_any_satisfy(tablaGlobalArchivos,(void *) existenArchivosEnDir)){

				log_error(loggerFileSystem, "No puedo borrar porque existen archivos :V :v :V");

			} else {

				char * nombreDirectorio = obtenerNombreUltimoPath(pathDesc);

				bool eliminarDirectorio(t_directory * directorio){
					return (strcmp(directorio->nombre,nombreDirectorio)==0 && directorio->padre==idPadreDir);
				}

				t_directory* directorio=list_remove_by_condition(listaDirectorios,(void*)eliminarDirectorio);

				if(directorio!=NULL){

					free(directorio->nombre);
					free(directorio);

				}

				log_debug(loggerFileSystem, "Pude borrar porque no existen archivo xdxdxdf");

			}

		}

	}

}

*/

