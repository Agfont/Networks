# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
default:
	gcc ep1-mqtt-server.c -Wall -o mqtt-server
	
# To start over from scratch, type 'make clean'. This
# removes the executable file, as well as old .o object
# files and *~ backup files:
clean: 
	$(RM) mqtt-server *.o *~
