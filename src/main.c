#include <stdio.h>
#include "SVGParser.h"

int main(void){

    SVGimage *img;
    char* text;
    
    
    //b
    img = createSVGimage("d.svg");
    text = getPathsToJSON(img);

    printf("Paths = '%s'\n\n", text);
    
    free(text);
    deleteSVGimage(img);
    //if (strlen(namespace) > 0) free(namespace);
    //if (strlen(title) > 0) free(title);
    //if (strlen(desc) > 0) free(desc);
    
    

    /*
    //c
    img = createSVGimage("c.svg");
    text = wholeSVGtoJSON(img);
    //printf("%s\n\n", text);
    free(text);
    deleteSVGimage(img);
    

    //d
    img = createSVGimage("d.svg");
    text = wholeSVGtoJSON(img);
    //printf("%s\n\n", text);
    free(text);
    deleteSVGimage(img);
    

    //e
    img = createSVGimage("e.svg");
    text = wholeSVGtoJSON(img);
    //printf("%s\n\n", text);
    free(text);
    deleteSVGimage(img);   
    */
    

    return 0;
}