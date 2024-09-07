#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/XKBlib.h>


#define WIDTH 500
#define HEIGHT 500

Window RootWindow;
Window MainWindow;
GC gc; 
Display* MainDisplay;
XGCValues vals;
XEvent event;
int toCont = False;
char* toType;
char *te;
int currPos = 0;
int stringLength = 50;
FILE *src;
int symsToProcess;
int offset = 0;
char *toProcess;
int mistakes = 0;


void transformSymbol(char* ch);
void redefineToType(char* toType);


int main()
{
	toProcess = malloc(sizeof(char)*stringLength);
	symsToProcess = stringLength;
	src = fopen("text.txt", "r");
	toType = (char*)malloc(stringLength*sizeof(char));
	memcpy(toType, "Hello, world ", strlen("Hello, world "));
	te =(char*)malloc(stringLength*sizeof(char));
	XSizeHints* hints = XAllocSizeHints();
	hints -> flags = PMaxSize+PMinSize;
	hints -> max_width = WIDTH;
	hints -> max_height = HEIGHT;
	hints -> min_width = WIDTH;
	hints -> min_height = HEIGHT;
	MainDisplay = XOpenDisplay(0);
	RootWindow = XDefaultRootWindow(MainDisplay);
  	MainWindow = XCreateSimpleWindow(MainDisplay, RootWindow,
                            0, 0, WIDTH, HEIGHT, 2,
                            0x000000, 0xffffff);

	XSelectInput(MainDisplay,MainWindow,KeyPressMask);
	XSetWMNormalHints(MainDisplay, MainWindow, hints);
	XFree(hints);
	XMapWindow(MainDisplay, MainWindow);
	unsigned long val = GCBackground | GCForeground;
	vals.background = 0xffffff;
	vals.foreground = 0x000000;
	gc = XCreateGC(MainDisplay, MainWindow, 0, &vals);

	while (1){
		char *symbol;
		symbol = (char*)malloc(10*sizeof(char));
		toCont = XCheckTypedEvent(MainDisplay, KeyPress,&event);
		if (toCont) {
			if (event.xkey.keycode){
				strcpy(symbol, XKeysymToString(XkbKeycodeToKeysym(MainDisplay, event.xkey.keycode, 0, event.xkey.state & ShiftMask ? 1 : 0)));
				if (strcmp(symbol, "space") == 0 & currPos < symsToProcess) strcpy(symbol, " ");
				if (strcmp(symbol, "BackSpace") == 0 & currPos > 0) {
					currPos--;
					if (*(te+currPos) != *(toType+currPos)) mistakes--;
					strcpy(te+currPos*sizeof(char), "\0");
				}else if (! (!strcmp(symbol, "Shift_L") | !strcmp(symbol, "Shift_R")) & (currPos < symsToProcess)){
					transformSymbol(symbol);
					if (strlen(symbol) == 1){
						memcpy(te+currPos*sizeof(char), symbol, sizeof(char));
						if (*(te+currPos) != *(toType+currPos)) mistakes++;
						currPos++;
					}
				}
			};
redraw:
			XClearWindow(MainDisplay, MainWindow);
			for (int i = 0; i < stringLength; i++){
				char *currChar;
				if (*(te+i) == *(toType+i) & *(te+i) != 0) {
					XSetForeground(MainDisplay, gc, 0x3bb032);
					currChar = te+i;
				}
				else if (*(te+i) != *(toType+i) & *(te+i) == 0) {
					XSetForeground(MainDisplay, gc, 0xa9a9a9);
					currChar = toType+i;
				}
				else if (*(te+i) != *(toType+i) & *(te+i) != 0) {
					XSetForeground(MainDisplay, gc, 0xff5555); 
					if (*(te+i) != *" ") currChar = te+i;
					else currChar = "\0";
				}
				else {currChar = " ";};
				XDrawString(MainDisplay, MainWindow, gc, 10 + 7*i, 10, currChar, 1);
				XFlush(MainDisplay);
			};
			
			toCont = False;
		};
		if (toType[currPos] == '\0' & mistakes == 0) {
			redefineToType(toType);
			goto redraw;
		}
	}
	return 1;
}

void
transformSymbol(char* ch){
	if (strcmp(ch, "period") == 0)			strcpy(ch, ".");
	if (strcmp(ch, "comma") == 0)			strcpy(ch, ",");
	if (strcmp(ch, "minus") == 0)			strcpy(ch, "-");
	if (strcmp(ch, "underscore") == 0)		strcpy(ch, "_");
	if (strcmp(ch, "exclam") == 0)			strcpy(ch, "!");
	if (strcmp(ch, "question") == 0)		strcpy(ch, "?");
	if (strcmp(ch, "slash") == 0) 			strcpy(ch, "/");
	if (strcmp(ch, "backslash") == 0) 		strcpy(ch, "\\");
	if (strcmp(ch, "bar") == 0) 			strcpy(ch, "|");
	if (strcmp(ch, "apostrophe") == 0) 		strcpy(ch, "\'");
	if (strcmp(ch, "quotedbl") == 0) 		strcpy(ch, "\"");
	if (strcmp(ch, "parenright") == 0) 		strcpy(ch, ")");
	if (strcmp(ch, "parenleft") == 0) 		strcpy(ch, "(");
}

void 
redefineToType(char* toType){
	currPos = 0;
	memset(te, 0, stringLength*sizeof(char));

	if (fgets(toProcess + offset, symsToProcess+1, src) != NULL){
		int lastElement = 0;
		for (int i = offset; i < stringLength-1; i++){
			if (toProcess[i] == ' ') lastElement = i+1;
		} 

		memset(toType, 0, stringLength*sizeof(char));
		memcpy(toType, toProcess, lastElement*sizeof(char));
		offset =  stringLength - lastElement*sizeof(char);
		symsToProcess = lastElement;
		memcpy(toProcess, toProcess + lastElement*sizeof(char), offset);
	} else exit(0);
	
}
