ALL: user1 user2

user1: librsocket.a user1.o 
	gcc -pthread user1.o librsocket.a -o user1

user2: librsocket.a user2.o
	gcc -pthread user2.o librsocket.a -o user2
		
librsocket.a: rsocket.o
	ar rs librsocket.a rsocket.o

rsocket.o:
	gcc -pthread -c rsocket.c -o rsocket.o

user1.o: 
	gcc -c user1.c -o user1.o

user2.o:
	gcc -c user2.c -o user2.o

clean:
	rm -rf *.o 
	rm -rf *.a
	rm -rf *.out
	rm -rf user1
	rm -rf user2