#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include "flags.h"
#include "vgapride.h"
#include "display.h"
#include "keys.h" 
#include <alloc.h>

extern "C" const char far depackername[];
extern "C" const char far depackerattribution[];
extern "C" void far savetime(void);
extern "C" void far disptime(const char far * flagname, unsigned repeat);

unsigned int waitForKey(){
	unsigned int ret=getch();
	if(ret==0){
		// Extended key
		ret=getch();
		return 0xFF00 | ret;
	}
	return ret;
}

void truncateAfter(char *buffer, char target){
	char *p=strchr(buffer, target);
	if(p!=NULL){
		*p='\0';
	}
}

void showFlag(Flag *flag, bool showTitle){
	displayFlag(flag);
	if(showTitle){
		char buffer[200];
		int color = getPalette(flag->text_color);
		setcolor(color);
		setfillstyle(SOLID_FILL, color);

		strcpy(buffer, flag->name);
		truncateAfter(buffer,'.');
		truncateAfter(buffer,',');
		truncateAfter(buffer,':');
		settextstyle(TRIPLEX_FONT, HORIZ_DIR, flag->text_size);
		int x;
		if(flag->text_layout==LAYOUT_CENTER){
			settextjustify(CENTER_TEXT, BOTTOM_TEXT);
			x=320;
		}
		if(flag->text_layout==LAYOUT_RIGHT){
			x=620;
			settextjustify(RIGHT_TEXT, BOTTOM_TEXT);	
		}
		if(flag->text_layout==LAYOUT_LEFT){
			x=20;
			settextjustify(LEFT_TEXT, BOTTOM_TEXT);	
		}
		outtextxy(x, 460, buffer);
	}
}

int startGraphics(){
	int gdriver = VGA, gmode=VGAHI;
	int errorcode = registerbgidriver(EGAVGA_driver);
	if(errorcode<0){
		printf("Couldn't register compiled-in BGI driver!");
		return 5;
	}
	initgraph(&gdriver, &gmode, "");
	errorcode = graphresult();
	if(errorcode != grOk){
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		return 1;
	}
	errorcode=registerbgifont(triplex_font);
	return 0;
}


int main(int argc, char**argv){
	int i; // Borland Turbo C++ has leaky forloops
	unsigned namearg = 1, benchmark = 0, repeat = 1, repeatoriginal = 1;

	if ((argc == 3 || argc == 4)
		&& (stricmp(argv[1], "benchmark") == 0
			|| stricmp(argv[1], "bench") == 0)) {
		++ namearg;
		++ benchmark;
		if (argc == 4) {
			repeat = atoi(argv[3]);
			if (!repeat) {
				printf("\nInvalid repeat amount: \"%s\"\n",argv[3]);
				return 2;
			}
			repeatoriginal = repeat;
		}
	} else if (argc !=2 ) {
		displayUsage();
		return 2;
	}
	for(i=0;;i++){
		Flag *flag=PRIDE_FLAGS[i];
		if(flag==NULL)break;
		if(flag->match(argv[namearg])){
			int ret;
			savetime();
			ret = startGraphics();
			if(ret!=0){
				return 0;
			}
			while (repeat--) {
				showFlag(flag, false);
			}
			if (!benchmark) {
				getch();
			}
			closegraph();
			if (benchmark) {
				disptime(argv[namearg], repeatoriginal);
				printf("Benchmark flag \"%s\" depacker \"%s\"",
					argv[namearg], depackername);
				if (repeatoriginal > 1) {
					printf(" for %u runs", repeatoriginal);
				}
				printf("\n");
			}
			return 0;
		}
	}
	if (benchmark) {
		printf("\nFlag \"%s\" not found.\n",argv[namearg]);
		return 2;
	}
	// If we didn't find a flag, see if they were asking for a LIST
	if(stricmp(argv[1], "list")==0){
		displayFlags();
		return 2;
	}
	// If we didn't find a flag, see if they were asking for a SLIDE
	if(
		stricmp(argv[1], "slideshow")==0 || 
		stricmp(argv[1], "slides")==0 || 
		stricmp(argv[1], "slide")==0
	){
		startGraphics();
		i=0;
		bool running = true;
		while(running){
			if(i<0)i=0; // Don't let the user see the secret pride flag -1
			if(PRIDE_FLAGS[i]==NULL){
				break;
			}
			showFlag(PRIDE_FLAGS[i], true);
			unsigned int key = waitForKey();
			switch(key){
				case KEY_LEFT:
					i-=1;
				break;
				case KEY_RIGHT:
					i+=1;
					if(PRIDE_FLAGS[i]==NULL)i--; // Back off from the end
				break;
				case KEY_ESCAPE:
					running=false;
				break;
				default:
					i+=1;
				break;
			}
		}
		closegraph();
		return 0;
	}
	displayUsage();
	printf("\nFlag \"%s\" not found.\n",argv[1]);
	return 2;
}

void displayFlags(){
	printf("Available flags:\n");
	for(int i=0;;i++){
		Flag *flag=PRIDE_FLAGS[i];
		if(flag==NULL)break;
		printf("\n* %s:\n", flag->shortname);
		const char *aliases=flag->listAliases();
		if(aliases!=NULL){
			printf("  aliases: %s\n", aliases);
		}
		printf("  %s\n", flag->name);
		if(flag->credit!=NULL){
			printf("  %s\n", flag->credit);
		}
	}
}
void quickFlags(){
	for(int i=0;;i++){
		Flag *flag=PRIDE_FLAGS[i];
		if(flag==NULL)break;
		if(i>0){
			printf(", ");
		}
		printf("%s", flag->shortname);
	}
	printf("\n");
}

void displayUsage(){
	printf("VGAPride %s by Foone Turing (@foone@digipres.club), 2022-2023. Other credits:\n", VERSION);
	printf(" * %s\n", depackerattribution);
	printf(" * Aegosexual Flag: Keiya (@keiyakins)\n");
	printf(" * Many flags: Rebecca G. Bettencourt (@RebeccaRGB)\n");
	printf(" * Autistic Pride Flag licensed under a Creative Commons Attribution-ShareAlike 4.0 International License, from autisticempire.com/autistic-pride/\n");
	printf("\nUsage: VGAPRIDE <FLAGNAME>\n");
	printf("run VGAPRIDE LIST | MORE for a list of flags, or try VGAPRIDE SLIDESHOW:\n");
	printf("left/right to scroll through flags, Escape to quit.\n");
	printf("\nQuick List:\n");
	quickFlags();

}
