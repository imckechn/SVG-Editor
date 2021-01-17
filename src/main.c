#include <stdio.h>
#include "SVGParser.h"

int main(void){
    /*SVGimage *img = createValidSVGimage("svgs/b.svg", "xsdFiles/svg.xsd");
    printf("THE JSON = %s\n", wholeSVGtoJSON(img));
    int aswner = writeSVGimage(img, "svgs/peen.svg");
    printf("Return val = %d\n", aswner);
    free(img);
    */


    SVGimage *img = initializeSvgImage();

    printf("Before\n");
    img = wholeJSONtoSVG(img, "['Rectangles', [], 'Circles', [{'cx': 5.0, 'cy': 5.0, 'r': 5.0, 'units': 'cm', 'attributes': [{'name': 'strokeWidth', 'value': 1}, {'name': 'strokeWidth', 'value': 1}, {'name': 'strokeWidth', 'value': 1}], 'numAttr': 3}]]", "http://www.w3.org/2000/svg", "title", "a desk");
    printf("After\n");

    printf("Whole svg to JSON = %s\n", wholeSVGtoJSON(img));

    printf("About to write\n");
    writeSVGimage(img, "svgs/peen.svg");
    free(img);

    return 0;
}