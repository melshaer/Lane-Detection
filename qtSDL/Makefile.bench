QTBASE = /usr/include
INCPATH = -I/usr/lib64/qt4/mkspecs/linux-g++ -I$(QTBASE)/QtCore -I$(QTBASE)/QtGui -I$(QTBASE)

LIBS= -L/usr/lib64 -lQtGui -lQtCore -lSDL -lm -lpthread -lrt 

MOC = /usr/lib64/qt4/bin/moc

all:	qtSDL pushframes

setgigeip:	setgigeip.cpp
	g++ -o setgigeip $(INCPATH) setgigeip.cpp $(LIBS) $(SPECIALOPS) $(PTGREYLIBS)

gigecamera:	gigecamera.cpp main.h
	g++ -g -o gigecamera $(INCPATH) gigecamera.cpp $(LIBS) $(SPECIALOPS) $(PTGREYLIBS)

pushframes:	pushframes.c main.h
	cc -o pushframes pushframes.c -lrt

.cpp.o: main.h
	g++ -DBENCHTEST -g -c $(INCPATH) -o $@ $<

main.o:	main.cpp main.h
	g++ -DTHISISMAIN -DBENCHTEST -g -c $(INCPATH) main.cpp

moc_mainwin.o:  mainwin.h main.h
	$(MOC) mainwin.h -o moc_mainwin.cpp
	g++ -g -c $(INCPATH) moc_mainwin.cpp

procthread.o: procthread.cpp main.h lanekar4.c lanekar4.h testkar1.c
	g++ -DBENCHTEST -g -c $(INCPATH) procthread.cpp

qtSDL: main.o moc_mainwin.o mainwin.o procthread.o
	g++ -g $(LIBDIR) -o qtSDL main.o mainwin.o moc_mainwin.o procthread.o $(LIBS)

clean:
	-rm qtSDL pushframes *.o moc*

