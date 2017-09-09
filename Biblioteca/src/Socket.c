
#include "Socket.h"


int verificarErrorSocket(int socket) {
	if (socket == -1) {
		perror("Error de socket");
		exit(-1);
	} else {
		return 0;
	}
}
int verificarErrorSetsockopt(int socket) {
	int yes = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("Error de setsockopt");
		exit(-1);
	} else {
		return 0;
	}
}
int verificarErrorBind(int socket, struct sockaddr_in mySocket) {
	if (bind(socket, (struct sockaddr *) &mySocket, sizeof(mySocket)) == -1) {
		perror("Error de bind");
		exit(-1);
	} else {
		return 0;
	}
}
int verificarErrorListen(int socket) {
	if (listen(socket, BACKLOG) == -1) {
		perror("Error de listen");
		exit(-1);
	} else {
		return 0;
	}
}


int ponerseAEscucharClientes(int puerto, int protocolo) {
	struct sockaddr_in mySocket;
	int socketListener = socket(AF_INET, SOCK_STREAM, protocolo);
	verificarErrorSocket(socketListener);
	verificarErrorSetsockopt(socketListener);
	mySocket.sin_family = AF_INET;
	mySocket.sin_port = htons(puerto);
	mySocket.sin_addr.s_addr = INADDR_ANY;
	memset(&(mySocket.sin_zero), '\0', 8);
	verificarErrorBind(socketListener, mySocket);
	verificarErrorListen(socketListener);
	return socketListener;
}

int aceptarConexionDeCliente(int socketListener) {
	int socketAceptador;
	struct sockaddr_in su_addr;
	socklen_t sin_size;
	sin_size = sizeof(struct sockaddr_in); //VER COMO IMPLEMENTAR SELECT!!

	//while (1) {
	if ((socketAceptador = accept(socketListener, (struct sockaddr *) &su_addr,
			&sin_size)) == -1) {
		perror("Error al aceptar conexion");
		exit(-1);
	} else {
		printf("Se ha conectado a: %s\n", inet_ntoa(su_addr.sin_addr));
	}
	//}

	return socketAceptador;
}

int conectarAServer(char *ip, int puerto) { //Recibe ip y puerto, devuelve socket que se conecto

	int socket_server = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent *infoDelServer;
	struct sockaddr_in direccion_server; // información del server

	//Obtengo info del server
	if ((infoDelServer = gethostbyname(ip)) == NULL) {
		perror("Error al obtener datos del server.");
		exit(-1);
	}

	verificarErrorSetsockopt(socket_server);

	//Guardo datos del server
	direccion_server.sin_family = AF_INET;
	direccion_server.sin_port = htons(puerto);
	direccion_server.sin_addr = *(struct in_addr *) infoDelServer->h_addr; //h_addr apunta al primer elemento h_addr_lista
	memset(&(direccion_server.sin_zero), 0, 8);

	//Conecto con servidor, si hay error finalizo
	if (connect(socket_server, (struct sockaddr *) &direccion_server,
			sizeof(struct sockaddr)) == -1) {
		perror("Error al conectar con el servidor.");
		close(socket_server);
		exit(-1);
	}

	return socket_server;

}


int calcularSocketMaximo(int socketNuevo, int socketMaximoPrevio){
	if(socketNuevo>socketMaximoPrevio){
		return socketNuevo;
	}
	else{
		return socketMaximoPrevio;
	}
}

int calcularTamanioTotalPaquete(int tamanioMensaje){
  int tamanio = sizeof(int)*2 + tamanioMensaje;
  return tamanio;
}

void sendRemasterizado(int aQuien, int tipo, int tamanio, void* que){
  void *bufferAEnviar;
  int tamanioDeMensaje = calcularTamanioTotalPaquete(tamanio);
  bufferAEnviar = malloc(tamanioDeMensaje);
  memcpy(bufferAEnviar, &tipo, sizeof(int));
  memcpy(bufferAEnviar+sizeof(int), &tamanio, sizeof(int));
  memcpy(bufferAEnviar+sizeof(int)*2, que, tamanio);
  if(send(aQuien, bufferAEnviar, tamanioDeMensaje, 0)==-1){
    perror("Error al enviar mensaje");
    exit(-1);
  }
  free(bufferAEnviar);
}

paquete *recvRemasterizado(int deQuien){
  paquete* paqueteConMensaje;
  paqueteConMensaje = malloc(sizeof(paquete));
  if(recv(deQuien, &paqueteConMensaje->tipoMsj, sizeof(int), 0)==-1){
    perror("Error al recibir mensaje");
    exit(-1);
  }
  if(recv(deQuien, &paqueteConMensaje->tamMsj, sizeof(int),0)==-1){
    perror("Error al recibir mensaje");
    exit(-1);
  }
  paqueteConMensaje->mensaje = malloc(paqueteConMensaje->tamMsj);
  if(recv(deQuien, paqueteConMensaje->mensaje,paqueteConMensaje->tamMsj, 0)==-1){
    perror("Error al recibir mensaje");
    exit(-1);
  }
  return paqueteConMensaje;
}

void sendDeNotificacion(int aQuien, int notificacion){
	if(send(aQuien, &notificacion, sizeof(int),0)==-1){
		perror("Error al enviar notificacion.");
		exit(-1);
	}
}

int recvDeNotificacion(int deQuien){
	int notificacion;
	if(recv(deQuien, &notificacion, sizeof(int), 0)==-1){
		perror("Error al recibir la notificacion.");
		exit(-1);
	}
	return notificacion;
}

void destruirPaquete(paquete* paqueteADestruir){
    free(paqueteADestruir->mensaje);
    free(paqueteADestruir);
}
