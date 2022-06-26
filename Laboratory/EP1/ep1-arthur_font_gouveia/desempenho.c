#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include<signal.h>

int main (int argc, char **argv) {
	if (argc != 2) {
        fprintf(stderr,"Falta o número de clientes que deseja conectar ao servidor.\n");
        exit(1);
    }
	int n = atoi(argv[1])/2;
	for (int c = 0; c<n; c++) {
		int ret = fork();
		if (ret == 0) {
			int mqtt_sub = system("mosquitto_sub -t teste");
			//kill(getpid(), SIGKILL);
		}
		else {
			int mqtt_pub = system("mosquitto_pub -t teste -m Palmeiras");
			//kill(getppid(), SIGKILL);
		}
	}
	sleep(5);
	kill(getppid(), SIGKILL);
	exit(0);
}
