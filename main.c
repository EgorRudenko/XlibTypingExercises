#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/XKBlib.h>
#include <cairo/cairo-xlib.h>
#include <cairo/cairo.h>
//#include <cairo/cairo>

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
char *symbol;

void transformSymbol(char* ch);
void redefineToType(char* toType);
void draw(char* c, int x, int y);

int main()
{

	toProcess = malloc(sizeof(char)*stringLength);
	symsToProcess = stringLength;
	symbol = (char*)malloc(10*sizeof(char));
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
	//unsigned long val = GCBackground | GCForeground;
	//vals.background = 0xffffff;
	//vals.foreground = 0x000000;
	//gc = XCreateGC(MainDisplay, MainWindow, 0, &vals);

	int screen = DefaultScreen(MainDisplay);
	cairo_surface_t *sfc = cairo_xlib_surface_create(MainDisplay, MainWindow, DefaultVisual(MainDisplay, screen), 300, 300);
    cairo_xlib_surface_set_size(sfc, WIDTH, HEIGHT);
	cairo_t *cr = cairo_create(sfc);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_select_font_face (cr, "Consolas", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, 12);

	while (1){
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
			//XClearWindow(MainDisplay, MainWindow);
			cairo_set_source_rgb (cr, 255, 255, 255);
			cairo_rectangle (cr, 0, 0, WIDTH, HEIGHT);
			cairo_fill (cr);	
			cairo_surface_flush(sfc);

			for (int i = 0; i < stringLength; i++){
				char *currChar;
				if (*(te+i) == *(toType+i) & *(te+i) != 0) {
					cairo_set_source_rgb (cr, 0.0, 1.0, 0.0);
					currChar = te+i;
				}
				else if (*(te+i) != *(toType+i) & *(te+i) == 0) {
					cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
					currChar = toType+i;
				}
				else if (*(te+i) != *(toType+i) & *(te+i) != 0) {
					cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
					if (*(te+i) != *" ") currChar = te+i;
					else currChar = "*";
				}
				else {currChar = " ";};
				char toPrint[2] = " \0";
				toPrint[0] = *currChar;
				cairo_text_extents_t tE;
				cairo_text_extents(cr, toPrint, &tE);
				cairo_move_to (cr, i*10, 10);
				cairo_show_text (cr, toPrint);
				cairo_surface_flush(sfc);
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

void 
draw(char* c, int x, int y){
	XDrawString(MainDisplay, MainWindow, gc, x, y, c, 1);
}
