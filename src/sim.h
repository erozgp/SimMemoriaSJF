#ifndef _SIM_H
#define _SIM_H

#include <stdlib.h>

#define LIST	1 /*estado Listo*/
#define EJEC	2 /*estado Ejecución*/
#define SWAP	3 /*estado Swap*/
#define TERM	4 /*estado Terminado*/

#define NMPA	18 /*Número Máximo de Procesos Activos*/
#define NMPS	18 /*Número Máximo de Procesos en Swap*/

#define NMMM	32 /*Número Máximo de Marcos en Memoria*/
#define NMPP	32 /*Número Máximo de Páginas por Proceso*/

#define PAGCOD	0 /*Página de código*/
#define PAGDAT	1 /*Página de datos*/
#define PAGPIL	2 /*Página de pila*/

struct stMarco{
		int pid;	/*Identificador de procesos al que está asignado*/
		int tip; 	/*Tipo de página que almacena*/
};
typedef struct stMarco STMARCO;
	
struct stMemoria{
		STMARCO marcos[NMMM];	/*Marcos en memoria*/
		int nma; 	/*Número de marcos asignados*/
};
typedef struct stMemoria STMEMORIA;

struct stProceso{
		int idp;	/*Identificador de proceso*/
		int edo; 	/*Estado del proceso*/
		int cp; /*Contador de programa*/
		int maras; /*Marcos asignados*/
		int npc;
		int mapam[NMPP]; /*Mapa de memoria*/
		int tiempo; /*Tiempo de llegada*/
};
typedef struct stProceso STPROCESO;

struct stProcesoEspera{
		int idp;	/*Identificador de proceso*/
		int edo; 	/*Estado del proceso*/
		int cp; /*Contador de programa*/
		int maras; /*Marcos asignados*/
		int npc;
		int npd;
		int npp;
		int mapam[NMPP]; /*Mapa de memoria*/
		int tiempo; /*Tiempo de llegada*/
};
typedef struct stProcesoEspera STPROCESOESPERA;

STMEMORIA MemRam, MemSwap;
STPROCESO ProcesosAc[NMPA];
STPROCESO ProcesosSw[NMPS];
STPROCESOESPERA Espera[18];
int ipej = 0; /*Índice del proceso en ejecución*/

unsigned int idps = 0; /*Identificador de procesos,
											se incrementa en 1 cada nuevo proceso*/
int ticks; /*Lo uso para llevar el tiempo*/

int contpa=0; /*contador de procesos activos*/
int contps=0; /*contador de procesos en swap*/
int contesp=0;

int maxpa=NMPA; /*número máximo de procesos activos*/
int maxps=NMPS; /*número máximo de procesos en swap*/

int contadorswap = 0;
int bmem = 0;
/*Función para inicializar la memoria*/
void inicializarmem(STMEMORIA *mem){
	int i;
	
	for(i=0; i<NMMM; i++){
		mem->marcos[i].pid = -1;
	}
	mem->nma = 0;
}
/*Función para calcular el número de marcos libres*/
int marcoslibres(STMEMORIA mem){
	return NMMM - mem.nma;
}
/*Función para asignar marcos a un proceso*/
int asignarmarcos(STPROCESO proc){
	int i, nm, j=0;
	for(i = 0; i<proc.maras;i++, j++){
		nm = (int) ((1.0*NMMM)*rand()/(RAND_MAX+1.0));
		printf("Marco: %d\ttipo: %d\n", nm, proc.mapam[j]);
		if(MemRam.marcos[nm].pid == -1){
			MemRam.marcos[nm].pid = proc.idp;
			MemRam.marcos[nm].tip = proc.mapam[j];
		}else{
			for( ; nm < NMMM; nm++){
				if(MemRam.marcos[nm].pid == -1){
					MemRam.marcos[nm].pid = proc.idp;
					MemRam.marcos[nm].tip = proc.mapam[j];
					break;
				}
			}
			if(nm == NMMM){
				for(nm = 0; nm < NMMM; nm++){
					if(MemRam.marcos[nm].pid == -1){
						MemRam.marcos[nm].pid = proc.idp;
						MemRam.marcos[nm].tip = proc.mapam[j];
						break;
					}
				}
			}
		}
	} 
	MemRam.nma+=proc.maras;
}

/*Función para adicionar procesos*/
int adicionar(int npc, int npd, int npp, int tiempo){
	int ret = 0, tamproc, i, j;
	tamproc = npc + npd + npp;
	if(contpa < maxpa && marcoslibres(MemRam) >= tamproc){
		ProcesosAc[contpa].idp = idps;
		ProcesosAc[contpa].edo = LIST;
		ProcesosAc[contpa].cp = 0;
		ProcesosAc[contpa].maras = tamproc;
		ProcesosAc[contpa].tiempo = tiempo; //tiempo de llegada
		
		for(j=0, i=0; j<npc; j++, i++){
			ProcesosAc[contpa].mapam[i] = PAGCOD;
		}
		for(j=0; j<npd; j++, i++){
			ProcesosAc[contpa].mapam[i] = PAGDAT;
		}
		for(j=0; j<npp; j++, i++){
			ProcesosAc[contpa].mapam[i] = PAGPIL;
		}
		asignarmarcos(ProcesosAc[contpa]);
		contpa++;
		ret = 1;
	}else if(contps < maxps && marcoslibres(MemSwap) >= tamproc){
		ProcesosSw[contps].idp = idps;
		ProcesosSw[contps].edo = SWAP;
		ProcesosSw[contps].cp = 0;
		ProcesosSw[contps].maras = tamproc;
		ProcesosSw[contps].tiempo = tiempo; //tiempo de llegada
		for(j=0, i=0; j<npc; j++, i++){
			ProcesosSw[contps].mapam[i] = PAGCOD;
		}
		for(j=0; j<npd; j++, i++){
			ProcesosSw[contps].mapam[i] = PAGDAT;
		}
		for(j=0; j<npd; j++, i++){
			ProcesosSw[contps].mapam[i] = PAGPIL;
		}
		MemSwap.nma+=tamproc;
		contps++;
		ret = 1;
	}
	if(ret){
		idps++;
	}
	
	return ret;
}

int esperaDeSw(int npc, int npd, int npp, int tiempo){
	int ret = 0, tamproc, i, j;
	tamproc = npc + npd + npp;
	
		Espera[contesp].idp = idps;
		Espera[contesp].edo = SWAP;
		Espera[contesp].cp = 0;
		Espera[contesp].maras = tamproc;
		Espera[contesp].npc = npc;
		Espera[contesp].npd = npd;
		Espera[contesp].npp = npp;
		Espera[contesp].tiempo = tiempo; //tiempo de llegada

		contesp++;
		ret = 1;
	
	if(ret){
		idps++;
	}
	
	return ret;

}

/*Funcion que adiciona a SWAP*/
int adicionarSW(int npc, int npd, int npp, int tiempo){
	int ret = 0, tamproc, i, j;
	tamproc = npc + npd + npp;
	
	if(contpa < maxps && marcoslibres(MemSwap) >= tamproc){
		ProcesosSw[contps].idp = idps;
		ProcesosSw[contps].edo = SWAP;
		ProcesosSw[contps].cp = 0;
		ProcesosSw[contps].maras = tamproc;
		ProcesosSw[contps].tiempo = tiempo; //tiempo de llegada
		
		for(j=0, i=0; j<npc; j++, i++){
			ProcesosSw[contps].mapam[i] = PAGCOD;
		}
		for(j=0; j<npd; j++, i++){
			ProcesosSw[contps].mapam[i] = PAGDAT;
		}
		for(j=0; j<npp; j++, i++){
			ProcesosSw[contps].mapam[i] = PAGPIL;
		}
		MemSwap.nma+=tamproc;
		contps++;
		ret = 1;
	}
	if(ret){
		idps++;
	}
	
	return ret;
}

/*Función pára buscar entre los procesos en Swap uno menor o igual a cierto tamaño*/
int buscarprocs(int tam){
	int i;
	for(i=0; i<contps; i++){
		printf("ProcesosSw[i].maras = %d\t<=\t tam = %d \n",ProcesosSw[i].maras, tam);
		fflush(stdout);
		if(ProcesosSw[i].maras <= tam){
			return i;
		}
	}
	return -1;
}

/*Función para liberar los marcos asociados a un proceso en ...*/
void liberarmarcos(int pid){
	int i;
	for(i=0; i <NMMM; i++){
		if(MemRam.marcos[i].pid == pid){
			MemRam.marcos[i].pid = -1;
			MemRam.nma--;
		}
	}
}

/*Función para reorganizar los procesos*/
void reorganizarproc(STPROCESO procesos[], int ipt, int contp){
	int i, j;
	printf("reorganizarproc = %d\t contp = %d \n", ipt, contp);
	fflush(stdout);
	
	for(i = ipt; i < contp-1; i++){
		procesos[i].idp = procesos[i+1].idp;
		procesos[i].edo = procesos[i+1].edo;
		procesos[i].cp = procesos[i+1].cp;
		procesos[i].maras = procesos[i+1].maras;
		procesos[i].npc = procesos[i+1].npc;
		procesos[i].tiempo = procesos[i+1].tiempo; //Tiempo de llegada
		
		for(j = 0; j < procesos[i].maras; j++){
			procesos[i].mapam[j] = procesos[i+1].mapam[j];
		}
	}
}
void reorganizarproc2(STPROCESOESPERA procesos[], int ipt, int contesp){
	int l, k;
	printf("reorganizarproc2 = %d\t contp = %d \n", ipt, contesp);
	fflush(stdout);
	
	for(l = ipt; l < contesp-1; l++){
		procesos[l].idp = procesos[l+1].idp;
		procesos[l].edo = procesos[l+1].edo;
		procesos[l].cp = procesos[l+1].cp;
		procesos[l].maras = procesos[l+1].maras;
		procesos[l].npc = procesos[l+1].npc;
		procesos[l].npd = procesos[l+1].npd;
		procesos[l].npp = procesos[l+1].npp;
		procesos[l].tiempo = procesos[l+1].tiempo; //Tiempo de llegada
		
		for(k = 0; k < procesos[l].maras; k++){
			procesos[l].mapam[k] = procesos[l+1].mapam[k];
		}
	}
}

void intercambia(STPROCESO *f,STPROCESO *g){
	STPROCESO tmp;
	tmp = *f;
	*f = *g;
	*g = tmp;
}


void intercambiaEnEspera(STPROCESOESPERA *f,STPROCESOESPERA *g){
	STPROCESOESPERA tmp;
	tmp = *f;
	*f = *g;
	*g = tmp;
}

void ordSJFconCero(void){
	int i,j;
	for (i=contps-1;i>1;i--){
		for (j=1;j<i;j++){
			if (ProcesosSw[j].maras>ProcesosSw[j+1].maras){
				intercambia(&ProcesosSw[j],&ProcesosSw[j+1]);
			}
		}
	}
}

void ordSJFsinCero(void){
	int i,j;
	for (i=contps-1;i>1;i--){
		for (j=0;j<i;j++){
			if (ProcesosSw[j].maras>ProcesosSw[j+1].maras){
				intercambia(&ProcesosSw[j],&ProcesosSw[j+1]);
			}
		}
	}
}

void ordenar(void){
	int i,j;
	for (i=contps-1;i>1;i--){
		for (j=0;j<i;j++){
			if (ProcesosSw[j].npc>ProcesosSw[j+1].npc){
				intercambia(&ProcesosSw[j],&ProcesosSw[j+1]);
			}
		}
	}
}

void odenarTabla(void){
	int i,j;
	for (i=contesp-1;i>1;i--){
		for (j=0;j<i;j++){
			if (Espera[j].tiempo>Espera[j+1].tiempo){
				intercambiaEnEspera(&Espera[j],&Espera[j+1]);
			}
		}
	}
}


int buscarMenor(void){
	int i, pid;
	int menor;
	menor = ProcesosSw[0].tiempo;
	for (i=1;i<contps;i++){
		if(ProcesosSw[i].tiempo < menor ){
			menor=ProcesosSw[i].tiempo;
			pid = i;
		}
	}
	
	return pid;

}

int buscarCero(void){
	int i;
	int encontrado;
	encontrado=-1;
	for (i=0;i<contps;i++){
		if ( ProcesosSw[i].tiempo==0 ){    
			encontrado=i;
			return encontrado;
		}
	}
	return encontrado;
}

int pasarAsw(void)
{
	int i,j, k, l;
	int pid=-1;
	int tamproc;
	
	
	if(contesp == 0){
		return 0;
	}
	
	for(i=0;i < contesp;i++){
		if( ticks >= Espera[i].tiempo){	
			tamproc = Espera[i].npc + Espera[i].npd + Espera[i].npp;					
			if(contpa < maxps && marcoslibres(MemSwap) >= tamproc){		
						//MemSwap.nma-=Espera[i].maras;
						ProcesosSw[contps].idp = Espera[i].idp;
						ProcesosSw[contps].edo = Espera[i].edo;
						ProcesosSw[contps].cp = Espera[i].cp;
						ProcesosSw[contps].maras = Espera[i].maras;
						ProcesosSw[contps].npc = Espera[i].npc;
						ProcesosSw[contps].tiempo = Espera[i].tiempo;		
						for(k=0, l=0; k<Espera[i].npc; k++, l++){
							ProcesosSw[contps].mapam[l] = PAGCOD;
						}
						for(k=0; k<Espera[i].npd; k++, l++){
							ProcesosSw[contps].mapam[l] = PAGDAT;
						}
						for(k=0; k<Espera[i].npp; k++, l++){
							ProcesosSw[contps].mapam[l] = PAGPIL;
						}	
						/*for(j=0; j<Espera[i].maras; j++){
							ProcesosSw[contps].mapam[j] = Espera[i].mapam[j];
						}*/				
						MemSwap.nma+=tamproc;
						reorganizarproc2(Espera, i, contesp);		
						contesp--;
						contps++;
					}
				}
			//ordenar();
			/*if(ProcesosSw[0].tiempo == 0){
				ordSJFconCero();
				printf("Ordena de acuerdo al tamaño\n, pero ignora el indice 0\n");
				
			}else if(ProcesosSw[0].tiempo != 0){
				ordSJFsinCero();
				printf("Ordena TODOS de acuerdo al tamaño\n");
			}
			*/ 
		}
		return 1;
	}	
	
	



/*Función para intentar sacar un proceso de swap y colocarlo en ram*/
int activarswapin(void){
	int np, j;
	printf("swapin activado...\n");
	fflush(stdout);
	
	/*
	if((np = buscarprocs(NMMM-MemRam.nma)) == -1){
		return 0;
	}
	printf("Antes -> contpa = %d\t contps = %d\n", contpa, contps);
	fflush(stdout);
	*/
	//pasarAsw();
	if(contps == 0){
		return 0;
	}
	
	np = 0;
	
	MemSwap.nma-=ProcesosSw[np].maras;
	ProcesosAc[contpa].idp = ProcesosSw[np].idp;
	ProcesosAc[contpa].edo = ProcesosSw[np].edo;
	ProcesosAc[contpa].cp = ProcesosSw[np].cp;
	ProcesosAc[contpa].maras = ProcesosSw[np].maras;
	ProcesosAc[contpa].npc = ProcesosSw[np].npc;
	ProcesosAc[contpa].tiempo = ProcesosSw[np].tiempo;
	for(j=0; j<ProcesosSw[np].maras; j++){
		ProcesosAc[contpa].mapam[j] = ProcesosSw[np].mapam[j];
	}
	reorganizarproc(ProcesosSw, np, contps);
	ordenar();
	contps--;
	contpa++;
	
	printf("Después -> contpa = %d\t contps = %d\n", contpa, contps);
	fflush(stdout);
	
	return 1;
}

/*Función para ejecutar procesos*/
int ejecutar(int i){
	/*if(contpa + contps < 1){
		return 0;
	}*/

	switch(ProcesosAc[i].edo){
		case EJEC:
				if(contpa == 0){
					
					if(!activarswapin()){
						return 0;
					}
				
				}else if(ProcesosAc[i].mapam[ProcesosAc[i].cp] == PAGCOD){
						printf("PAGCOD %d\t cp = %d\n", ProcesosAc[i].idp, ProcesosAc[i].cp);
						fflush(stdout);
						ProcesosAc[i].cp++;
						ticks++;
						
						printf("NPC DEL PROCESO %d = %d\nY CP LLEVA ESTO = %d\n", ProcesosAc[i].idp, ProcesosAc[i].npc, ProcesosAc[i].cp );
						if(ProcesosAc[i].mapam[ProcesosAc[i].cp] != PAGCOD || ProcesosAc[i].cp >= ProcesosAc[i].npc ){
							printf("No PAGCOD %d\t cp = %d\n", ProcesosAc[i].idp, ProcesosAc[i].cp);
							fflush(stdout);
							liberarmarcos(ProcesosAc[i].idp);
							reorganizarproc(ProcesosAc, i, contpa);
							//reorganizarproc(ProcesosSw, np, contps);
							ordenar();
							contpa--;
							
							if(contps > 0){
								activarswapin();
							}
							
						}
						
					}
			
		break;
		case SWAP:
			ProcesosAc[i].edo = EJEC;
			asignarmarcos(ProcesosAc[i]);
		break;
	}
	return 1;
}
#endif
