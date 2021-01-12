#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>

#include <math.h>

#include "LinkedListAPI.h"
#include "SVGParser.h"

#define  NUMFLOATINGPOINTS 10

/*      A2 Module 1 helper functions     */
bool checkValuesInSVGimage(SVGimage* image);
bool checkForNullElementsInSVGimage(SVGimage* image);
bool checkGroupListForAnyNULLchildren(List* groups);
bool checkSingleGroupForNULLchildren(Group* group);
bool checkElementsInSVGimage(SVGimage* image);

bool checkAttributeList(List *attributes);
bool checkRectangleList(List* rectangles);
bool checkCircleList(List* circles);
bool checkPathList(List* paths);
bool checkGroupList(List* groups);

bool checkSingleAttribute(Attribute *atty);
bool checkSingleRectangle(Rectangle *rect);
bool checkSingleCircle(Circle *circle);
bool checkSinglePath(Path *path);
bool checkSingleGroup(Group *group);

/*      Random helper functions that don't have a specific category     */
xmlDocPtr populateXmlDocPtr(char *fileName);
SVGimage* xmlDocToSvgDoc(xmlDoc *xmlDoc);
bool getXmlInformation(xmlNode *root, SVGimage *image);
bool generateGroup( xmlNode *root, Group *g);
bool groupGetXmlInformation(xmlNode *newRoot, Group *g);
int getSizeOfGroup(Group *gsub);
char* unitCheck(char *content);

/*  <Getting a list of every specific shape/element from a group tree>   */
void getRectsFromGroup(Group *g, List *newRectList);
void getCirclesFromGroup(Group *g, List *newCircleList);
void getGroupsFromGroup(Group *g, List *newGroupList);
int getGroupsFromParentGroup(Group *g, int len);
void getPathsFromGroup(Group *g, List *newPathList);
/*  </Getting a list of every specific shape/element from a group tree>   */

/*  <LL helper functions>   */
void deleteAttribute( void* data);
char* attributeToString( void* data);
int compareAttributes(const void *first, const void *second);

void deleteGroup(void* data);
char* groupToString( void* data);
int compareGroups(const void *first, const void *second);

void deleteRectangle(void* data);
char* rectangleToString(void* data);
int compareRectangles(const void *first, const void *second);

void deleteCircle(void* data);
char* circleToString(void* data);
int compareCircles(const void *first, const void *second);

void deletePath(void* data);
char* pathToString(void* data);
int comparePaths(const void *first, const void *second);

void universalNoDelete(void *data);
/*  </LL helper functions>   */

/*  <Struct pointer initialization functions>   */
SVGimage* initializeSvgImage();
Group* initializeGroup();
Rectangle* initializeRectangle();
Circle* initializeCircle();
Path* initializePath(char *data);
Attribute* initializeAttribute(char *name, char *value);
/*  </Struct pointer initialization functions>   */

/*  <Helper functions for 'numAttr'>   */
int getAttributeCountFromRectangleList(List *rectangleList);
int getAttributeCountFromCircleList(List *circleList);
int getAttributeCountFromPathList(List *pathList);
int getAttributeCountFromGroupList(List *groupsList);
int getAttributesFromGroupAndItsShapes(Group *group);
/*  </Helper functions for 'numAttr'>   */

/*  <A2 module 1 helper functions>   */
xmlDocPtr readFileAndValidate(char *XMLFileName, char *XSDFileName);
xmlDocPtr imageToXmlTree(SVGimage *image);
void createRectNode(Rectangle *rect, xmlNodePtr root_node);
xmlNodePtr svgElementToXmlNode(SVGimage *image);
void createCircleNode(Circle *circle, xmlNodePtr root_node);
void createPathNode(Path *path, xmlNodePtr root_node);
void createGroupNode(Group *group, xmlNodePtr root_node);
bool checkImageAgainstSchemaFile(SVGimage *image, char *schemaFile);
/*  </A2 module 1 helper functions>   */

/*  <A2 module 2 helper functions>   */
bool UpdateAttributes(ListIterator iter, Attribute *newAttribute, int length);
char* formatPathData(char *data, int start);    //Actually a helper for the Shopify Code Challenge
/*  </A2 module 2 helper functions>   */

/*  This is the main function. Calls all the helper functions so it can take an SVG file name and return all the SVG elements and
*   information back to the user.
*   @Param  The filename of the image.
*   @return The SVG information in a struct.
*/
SVGimage* createSVGimage(char* fileName) {

    if (fileName == NULL) return NULL;

    SVGimage* image;
    xmlDoc *doc = NULL;

    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        xmlFreeDoc(doc);
        return NULL;
    }

    //Now get the SVG content from the XML doc
    image = xmlDocToSvgDoc(doc);


    xmlFreeDoc(doc);
    //xmlFreeParserCtxt(ctxt);
    xmlCleanupParser();


    if (image == NULL) return NULL;
    return image;
}

/*
Used "parser.c" by Daniel Veillard as reference.
This reference was found on http://www.xmlsoft.org

This turns an XML doc (what SVG is based on) into a XML fill structure based on libxml2
*/
xmlDocPtr populateXmlDocPtr(char *fileName) {

    xmlParserCtxtPtr ctxt; /* the parser context */
    xmlDocPtr doc; /* The tree root */

    if (fileName == NULL) {
        return NULL;
    }

    //This makes an XML doc from the given file, otherwise returns null.
    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {

	    return NULL;
    }
    doc = xmlCtxtReadFile(ctxt, fileName, NULL, XML_PARSE_DTDVALID);
    if (doc == NULL) {
        return NULL;
        
    } else {
        if (ctxt->valid == 0) {
            return NULL;
            //Could not validate
        }

    }
    //On success
    return doc;
}

/*  This is another function handler function. Calls all the Functions that actually turn the XML information into SVG information.
*   @Param  the XML doc
*   @return the full SVG file full of the info from the xml file.
*/
SVGimage * xmlDocToSvgDoc(xmlDoc *doc) {
    if (!doc) return NULL;

    SVGimage *image = initializeSvgImage();

    xmlNode *root = xmlDocGetRootElement(doc);

    bool answer = getXmlInformation(root, image);
    if (!answer) {
        deleteSVGimage(image);
        return NULL;
    }
    return image;
}

/*
This loop structure was from tree.c
http://www.xmlsoft.org/examples/tree2.c
Written by Lucas Brasilino
*   This loops through the XML tree and disects it looking for SVG elements and the information about them.
*   @Param  the root xml Node, the SVG image that will be returned.
*/
bool getXmlInformation(xmlNode *root, SVGimage *image) {

    xmlNode *cur_node = NULL;

    if ( strcmp(image->namespace, "") == 0) {
        for (cur_node = root; cur_node; cur_node = cur_node->next) {
            if (cur_node->type == XML_ELEMENT_NODE) {   //This gets all the XML nodes recursivly

                if ( cur_node->ns == NULL) {
                    return false;
                }

                strncpy(image->namespace, (char *) cur_node->ns->href, 255);
                strcat(image->namespace, "\0");
                goto ENDLOOP;
            }
        }
    }
    ENDLOOP:        //Goto, only good case for it.

    for (cur_node = root; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {   //This gets all the XML nodes recursivly

            if ( strcmp( (char *) cur_node->name, "g") == 0 ) {     //If a group tag is encountered
                Group *g = initializeGroup();
                bool validate = generateGroup(cur_node, g);

                if (!validate) return false;

                insertBack(image->groups, g);
                //cur_node = cur_node->next;
                continue;
            }

            if (cur_node->children != NULL) {   //Get TITLE and DESC if they haven't been collected already

                if ( (strcmp( (char *) cur_node->name, "title") == 0)  &&  ( strcmp(image->title, "") == 0) )
                {
                    if ( (cur_node->children)->content != NULL) {

                        strncpy(image->title, (char *) (cur_node->children)->content, 255 );
                        strcat(image->title, "\0");

                    }

                } else if ( (strcmp( (char *) cur_node->name, "desc") == 0)  &&  ( strcmp(image->description, "") == 0) ) {
                    if ( (cur_node->children)->content != NULL) {
                        strncpy(image->description, (char *) (cur_node->children)->content, 255 );
                        strcat(image->description, "\0");
                    }
                }
            }

            if (cur_node->properties != NULL) {     //This gets the attibutes. All of them.
                xmlAttr *attribute = cur_node->properties;

                //If the element is a rectangle, circle, or path, it will initialize it, populate it with its attributes and add it to the SVG element
                if ( strcmp( (char *) cur_node->name, "rect") == 0 ) {
                    Rectangle *rect = initializeRectangle();
                    strcpy(rect->units, unitCheck((char *) attribute->children->content));

                    do {
                        if ( strcmp( (char *) attribute->name, "x") == 0 ) {
                            rect->x = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "y") == 0 ) {
                            rect->y = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "width") == 0 ) {
                            rect->width = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "height") == 0 ) {
                            rect->height = atof( (char *) attribute->children->content);

                        } else {
                            Attribute *atty = initializeAttribute((char *) attribute->name, (char *) attribute->children->content);
                            insertBack( rect->otherAttributes, atty);
                        }

                    } while ( (attribute = attribute->next) );

                    if (!checkSingleRectangle(rect)) return false;  //Check that the given values are valid

                    insertBack(image->rectangles, rect);

                } else if ( strcmp( (char *) cur_node->name, "circle") == 0 ) {
                    Circle *circle = initializeCircle();
                    strcpy(circle->units, unitCheck((char *) attribute->children->content));

                    do {
                        if ( strcmp( (char *) attribute->name, "cx") == 0 ) {
                            circle->cx = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "cy") == 0 ) {
                            circle->cy = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "r") == 0 ) {
                            circle->r = atof( (char *) attribute->children->content);

                            if (circle->r < 0) {
                                circle->r = abs(circle->r);
                            }

                        } else {
                            Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);

                            insertBack( circle->otherAttributes, atty);
                        }

                    } while ( (attribute = attribute->next) );

                    if (!checkSingleCircle(circle)) return false;  //Check that the given values are valid

                    insertBack(image->circles, circle);

                } else if ( strcmp( (char *) cur_node->name, "path") == 0 ) {
                    Path *path = NULL;

                    do {    //Need to loop once first to get the data attribute first.

                        if ( strcmp( (char *) attribute->name, "d") == 0 ) {
                            path = initializePath( (char *) attribute->children->content);
                        }

                        attribute = attribute->next;

                    } while ( path == NULL);

                    attribute = cur_node->properties;

                    do {    //Loop again, collecting all none data attributes as 'otherAttributes'

                        if ( strcmp( (char *) attribute->name, "d") == 0 ) {
                            continue;

                        } else {
                            Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);
                            insertBack(path->otherAttributes, atty);

                        }

                    } while ( (attribute = attribute->next) );

                    insertBack(image->paths, path);

                } else if ( strcmp( (char *) cur_node->name, "svg") == 0 ) {
                    do {    //Loop again, collecting all none data attributes as 'otherAttributes'

                        Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);
                        insertBack( image->otherAttributes, atty);

                    } while ( (attribute = attribute->next) );
                } else {
                    return false;
                }
            }
        }

        bool check = getXmlInformation(cur_node->children, image);
        if (!check) return false;
    }

    return true;
}


/*  When a group is encountered, it calls this loop so you can add found elements to a group instead of the SVG element.
*   @Param  The the root xmlNode from the xml tree.
*   @Return an alloced and filled group element.
*/
bool generateGroup( xmlNode *root, Group *g) {
    //If the groups nodes being
    if (root->properties != NULL) {     //**Make this a function and move it (See notes)

        if (getLength(g->otherAttributes) == 0) {
            xmlAttr *attribute = root->properties;

            do {

                Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);
                insertBack( g->otherAttributes, atty);

            } while ( (attribute = attribute->next) );
        }
    }

    if (root->children == NULL) return g;

    xmlNode *cur_node;
    root = root->children;  //Pass in the child instead?????

    //Collects all of the groups children and their informaton.

    for (cur_node = root; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {   //This gets all the XML nodes recursivly

            if ( strcmp( (char *) cur_node->name, "g") == 0 ) { //If a group tag is encountered

                Group *g2 = initializeGroup();
                bool validate = generateGroup(cur_node, g2);

                if (!validate) return false;

                insertBack(g->groups, g2);
                continue;
            }

            if (cur_node->properties != NULL) {     //This gets the attibutes. All of them. (Even the woman and children attributes. They're animals, all of them)
                xmlAttr *attribute = cur_node->properties;

                //If the element is a rectangle, circle, or path, it will initialize it, populate it with its attributes and add it to the SVG element
                if ( strcmp( (char *) cur_node->name, "rect") == 0 ) {

                    Rectangle *rect = initializeRectangle();
                    strcpy(rect->units, unitCheck((char *) attribute->children->content));

                    do {
                        if ( strcmp( (char *) attribute->name, "x") == 0 ) {
                            rect->x = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "y") == 0 ) {
                            rect->y = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "width") == 0 ) {
                            rect->width = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "height") == 0 ) {
                            rect->height = atof( (char *) attribute->children->content);

                        } else {
                            Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);

                            insertBack( rect->otherAttributes, atty);
                        }

                    } while ( (attribute = attribute->next ));

                    if (!checkSingleRectangle(rect)) return false;  //Check that the given values are valid

                    insertBack(g->rectangles, rect);

                } else if ( strcmp( (char *) cur_node->name, "circle") == 0 ) {

                    Circle *circle = initializeCircle();
                    strcpy(circle->units, unitCheck((char *) attribute->children->content));

                    do {
                        if ( strcmp( (char *) attribute->name, "cx") == 0 ) {
                            circle->cx = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "cy") == 0 ) {
                            circle->cy = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "r") == 0 ) {
                            circle->r = atof( (char *) attribute->children->content);

                        } else {
                            Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);

                            insertBack( circle->otherAttributes, atty);
                        }

                    } while ( (attribute = attribute->next ));

                    if (!checkSingleCircle(circle)) return false;  //Check that the given values are valid

                    insertBack(g->circles, circle);

                } else if ( strcmp( (char *) cur_node->name, "path") == 0 ) {

                    Path *path = NULL;

                    do {    //Need to loop once first to get the data attribute first.

                        if ( strcmp( (char *) attribute->name, "d") == 0 ) {
                            path = initializePath( (char *) attribute->children->content);
                        }

                        attribute = attribute->next;

                    } while ( path == NULL);

                    attribute = cur_node->properties;

                    do {    //Loop again, collecting all none data attributes as 'otherAttributes'

                        if ( strcmp( (char *) attribute->name, "d") == 0 ) {
                            continue;

                        } else {
                            Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);

                            insertBack( path->otherAttributes, atty);
                        }

                    } while ( (attribute = attribute->next) );

                    insertBack(g->paths, path);
                } else {
                    return false;
                }
            }
        }
        //bool validate = generateGroup(cur_node->children, g);      //->Might cause errors, Im assuming that the group ends in rectangles/paths/circles, or another group with could call itself.
        //if (!validate) return false;


        bool check = groupGetXmlInformation(cur_node->children, g);  //Taken out
        if (!check) return false;
    }

    return true;
}

//gets the units from a string
char* unitCheck(char *content) {

    if(strstr(content, "mm") != NULL) {
        return "mm";

    } else if (strstr(content, "cm") != NULL) {
        return "cm";

    } else if (strstr(content, "px") != NULL) {
        return "px";
    }

    return "";
}

/*  Helper function for generateGroup. Doesn't get attributes everyloop so that's why I made it.
*   @param: The root xmlNode used for looping through the XML tree, The group everything is being added to.
*/
bool groupGetXmlInformation(xmlNode *newRoot, Group *g) {
    xmlNode *cur_node = NULL;

    for (cur_node = newRoot; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {   //This gets all the XML nodes recursivly

            if ( strcmp( (char *) cur_node->name, "g") == 0 ) {     //If a group tag is encountered
                //Group *g2 = generateGroup(cur_node);

                //insertBack(g->groups, g2);
                continue;
            }

            if (cur_node->properties != NULL) {     //This gets the attibutes. All of them.
                xmlAttr *attribute = cur_node->properties;

                //If the element is a rectangle, circle, or path, it will initialize it, populate it with its attributes and add it to the SVG element
                if ( strcmp( (char *) cur_node->name, "rectangle") == 0 ) {

                    Rectangle *rect = initializeRectangle();
                    strcpy(rect->units, unitCheck((char *) attribute->children->content));

                    do {

                        if ( strcmp( (char *) attribute->name, "x") == 0 ) {
                            rect->x = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "y") == 0 ) {
                            rect->x = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "width") == 0 ) {
                            rect->x = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "height") == 0 ) {
                            rect->x = atof( (char *) attribute->children->content);

                        } else {
                            Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);

                            insertBack( rect->otherAttributes, atty);
                        }

                    } while ( (attribute = attribute->next) );

                    insertBack(g->rectangles, rect);

                } else if ( strcmp( (char *) cur_node->name, "circle") == 0 ) {

                    Circle *circle = initializeCircle();
                    strcpy(circle->units, unitCheck((char *) attribute->children->content));

                    do {

                        if ( strcmp( (char *) attribute->name, "cx") == 0 ) {
                            circle->cx = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "cy") == 0 ) {
                            circle->cy = atof( (char *) attribute->children->content);

                        } else if ( strcmp( (char *) attribute->name, "r") == 0 ) {
                            circle->r = atof( (char *) attribute->children->content);

                        } else {
                            Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);

                            insertBack( circle->otherAttributes, atty);
                        }

                    } while ( (attribute = attribute->next) );

                    insertBack(g->circles, circle);

                } else if ( strcmp( (char *) cur_node->name, "path") == 0 ) {

                    Path *path = NULL;

                    do {    //Need to loop once first to get the data attribute first.

                        if ( strcmp( (char *) attribute->name, "d") == 0 ) {
                            path = initializePath( (char *) attribute->children->content);
                        }
                        attribute = attribute->next;

                    } while ( path == NULL);

                    attribute = cur_node->properties;

                    do {    //Loop again, collecting all none data attributes as 'otherAttributes'

                        if ( strcmp( (char *) attribute->name, "d") == 0 ) {
                            continue;

                        } else {
                            Attribute *atty = initializeAttribute((char *) attribute->name, (char *) (attribute->children)->content);

                            insertBack( path->otherAttributes, atty);
                        }

                    } while ( (attribute = attribute->next) );

                    insertBack(g->paths, path);
                }
            }
        }
        bool check = groupGetXmlInformation(cur_node->children, g);
        if (!check) return false;
    }
    return true;
}

/*  returns a string that has a desc of every element in the SVG image.
*   @Param the SVG image that is being is having it's information being printed out of.
*   @return the string of all the information about the image.
*/
char* SVGimageToString(SVGimage* img) {
    if (img == NULL) return NULL;

    char *text, *help;
    int size = 0;

    //Introduction: Namespace, title, and desk
    size += strlen("\nSVG image,\nNamespace = \ntitle = \ndescription = ");
    size += strlen(img->namespace) + strlen(img->title) + strlen(img->description);
    size += strlen("\n\nRectangles:\n\nCircles:\n\nPaths:\n\nGroups:\n\nOtherAttributes:\n");

    //rects
    help = toString(img->rectangles);
    size += strlen(help);
    free(help);

    //circles
    help = toString(img->circles);
    size += strlen(help);
    free(help);

    //paths
    help = toString(img->paths);
    size += strlen(help);
    free(help);

    //groups
    help = toString(img->groups);
    size += strlen(help);
    free(help);

    //otherAttributes
    help = toString(img->otherAttributes);
    size += strlen(help);
    free(help);

    text = malloc(sizeof(char) * size);

    strcpy(text, "\nSVG image,\nNamespace = ");
    strcat(text, img->namespace);
    strcat(text, "\ntitle = ");
    strcat(text, img->title);
    strcat(text, "\ndescription = ");
    strcat(text, img->description);

    //rects
    help = toString(img->rectangles);
    strcat(text, "\n\nRectangles:");
    strcat(text, help);
    free(help);

    //circles
    help = toString(img->circles);
    strcat(text, "\n\nCircles:");
    strcat(text, help);
    free(help);

    //paths
    help = toString(img->paths);
    strcat(text, "\n\nPaths:");
    strcat(text, help);
    free(help);

    //groups
    help = toString(img->groups);
    strcat(text, "\n\nGroups:");
    strcat(text, help);
    free(help);

    //otherAttributes
    help = toString(img->otherAttributes);
    strcat(text, "\n\nOtherAttributes:");
    strcat(text, help);
    free(help);

    return text;
}

/*  This deletes an SVG image.
*   @Param The image that will be deleted.
*/
void deleteSVGimage(SVGimage* img) {
    if (img == NULL) return;
    
    freeList(img->rectangles);
    freeList(img->circles);
    freeList(img->paths);
    freeList(img->groups);
    freeList(img->otherAttributes);    

    free(img);
}
/*------------------- Module 2 Functions (their helpers are at the bottom) -------------------*/

//Gets all the rects in the image
int getAllRects(SVGimage* img) {
    if (img == NULL) return 0;

    List *rectList = getRects(img);
    int length = getLength(rectList);
    freeList(rectList);
    return length;


}

//Gets all the circles in the image
int getAllCircles(SVGimage* img) {
    if (img == NULL) return 0;

    List *circleList = getCircles(img);
    int length = getLength(circleList);
    freeList(circleList);
    return length;
}

//Gets all the paths in the image
int getAllPaths(SVGimage* img) {
    if (img == NULL) return 0;

    List *pathList = getPaths(img);
    int length = getLength(pathList);
    freeList(pathList); 
    return length;
}

//Gets all the groups in the image
int getAllGroups(SVGimage* img) {
    if (img == NULL) return 0;

    int totalCount = 0;

    List *groups = getGroups(img);
    int numGroups = getLength(groups);

    ListIterator iter = createIterator(groups);

    for (int i = 0; i < numGroups; i++) {
        Group *gSub = nextElement(&iter);

        totalCount++;

        totalCount += getSubGroups(gSub);
    }

    freeList(groups);
    return totalCount;
}

int getSubGroups(Group *g) {
    int totalCount = 0;
    int childrenCount = getLength(g->groups);
    ListIterator iter = createIterator(g->groups);

    for (int i = 0; i < childrenCount; i++) {
        Group *gSub = nextElement(&iter);

        totalCount++;

        totalCount += getSubGroups(gSub);
    }
    return totalCount;
}

// Function that returns a list of all rectangles in the image.
List* getRects(SVGimage* img) {
    if (img == NULL) return NULL;

    int rectListLength = getLength(img->rectangles);
    int groupListLength = getLength(img->groups);

    List *newRectList = initializeList( &rectangleToString, &universalNoDelete, &compareRectangles);
    ListIterator iter = createIterator(img->rectangles);

    //Loops through the entire SVG rectangle list and gets all the rectangles.
    for (int i = 0; i < rectListLength; i++) {
        Rectangle *rect = (Rectangle *) nextElement(&iter);

        insertBack(newRectList, rect);
    }

    //This loops through all the groups in the SVG image, finds any lists of rectangles and gets the rectangles.
    ListIterator svgGroupIter = createIterator(img->groups);

    for (int i = 0; i < groupListLength; i++) {
        Group *g = (Group *) nextElement(&svgGroupIter);

        //This gets called for every group in the svg element.
        getRectsFromGroup(g, newRectList);
    }

    return newRectList;
}

// Function that returns a list of all circles in the image.
List* getCircles(SVGimage* img) {
    if (img == NULL) return NULL;

    int circleListLength = getLength(img->circles);
    int groupListLength = getLength(img->groups);

    List *newCircleList = initializeList( &circleToString, &universalNoDelete, &compareCircles);
    ListIterator iter = createIterator(img->circles);

    //Loops through the entire SVG circle list and gets all the circles.
    for (int i = 0; i < circleListLength; i++) {
        Circle *circle = (Circle *) nextElement(&iter);

        insertBack(newCircleList, circle);
    }

    //This loops through all the groups in the SVG image, finds any lists of rectangles and gets the rectangles.
    ListIterator svgGroupIter = createIterator(img->groups);
    for (int i = 0; i < groupListLength; i++) {
        Group *g = (Group *) nextElement(&svgGroupIter);

        //This gets called for every group in the svg element.
        getCirclesFromGroup(g, newCircleList);
    }

    return newCircleList;

}

// Function that returns a list of all groups in the image.
List* getGroups(SVGimage* img) {
    if (img == NULL) return NULL;

    List *newGroupList = initializeList( &groupToString, &universalNoDelete, &compareGroups);

    //This loops through all the groups in the SVG image, finds any lists of rectangles and gets the rectangles.
    ListIterator svgGroupIter = createIterator(img->groups);
    for (int i = 0; i < getLength(img->groups); i++) {
        Group *g = (Group *) nextElement(&svgGroupIter);

        insertBack(newGroupList, g);

        //This gets called for every group in the svg element.
        getGroupsFromGroup(g, newGroupList);
    }

    return newGroupList;

}

// Function that returns a list of all paths in the image.
List* getPaths(SVGimage* img) {
    if (img == NULL) return NULL;

    int pathListLength = getLength(img->paths);
    int groupListLength = getLength(img->groups);

    List *newPathList = initializeList( &pathToString, &universalNoDelete, &comparePaths);
    ListIterator iter = createIterator(img->paths);

    //Loops through the entire SVG path list and gets all the path.
    for (int i = 0; i < pathListLength; i++) {
        Path *path = (Path *) nextElement(&iter);

        insertBack(newPathList, path);
    }

    //This loops through all the groups in the SVG image, finds any lists of rectangles and gets the rectangles.
    ListIterator svgGroupIter = createIterator(img->groups);
    for (int i = 0; i < groupListLength; i++) {
        Group *g = (Group *) nextElement(&svgGroupIter);

        //This gets called for every group in the svg element.
        getPathsFromGroup(g, newPathList);
    }

    return newPathList;

}

// Function that returns the number of all rectangles with the specified area
int numRectsWithArea(SVGimage* img, float area) {
    if (img == NULL) return 0;

    List *rectList = getRects(img);
    int length = getLength(rectList);
    int totalCount = 0;
    ListIterator iter = createIterator(rectList);

    for (int i = 0; i < length; i++) {

        Rectangle *rect = (Rectangle *) nextElement(&iter);

        if ( ceil(rect->width * rect->height) == ceil(area) ) totalCount++;
    }

    freeList(rectList);
    return totalCount;
}
// Function that returns the number of all circles with the specified area
int numCirclesWithArea(SVGimage* img, float area) {
    if (img == NULL) return 0;

    List *circleList = getCircles(img);
    int length = getLength(circleList);
    int totalCount = 0;
    ListIterator iter = createIterator(circleList);

    for (int i = 0; i < length; i++) {

        Circle *circle = (Circle *) nextElement(&iter);

        if ( ceil(circle->r * circle->r * 3.1415) == ceil(area) ) totalCount++;
    }

    freeList(circleList);
    return totalCount;
}
// Function that returns the number of all paths with the specified data - i.e. Path.data field
int numPathsWithdata(SVGimage* img, char* data) {
    if (img == NULL) return 0;

    List *pathList = getPaths(img);
    int totalCount = 0;
    ListIterator iter = createIterator(pathList);

    for (int i = 0; i < getLength(pathList); i++) {

        Path *path = (Path *) nextElement(&iter);
        if ( strcmp(path->data, data) == 0) totalCount++;
    }

    freeList(pathList);
    return totalCount;
}
// Function that returns the number of all groups with the specified length - see A1 Module 2 for details
int numGroupsWithLen(SVGimage* img, int len) {
    if (img == NULL) return 0;

    int totalCount = 0;
    int comparitor;

    List *groups = getGroups(img);
    int numGroups = getLength(groups);

    ListIterator iter = createIterator(groups);

    for (int i = 0; i < numGroups; i++) {
        Group *gSub = nextElement(&iter);

        comparitor = getSizeOfGroup(gSub);

        if (comparitor == len) totalCount++;

        totalCount += getGroupsFromParentGroup(gSub, len);
    }

    freeList(groups);
    return totalCount;
}

/*  Function that returns the total number of Attribute structs in the SVGimage - i.e. the number of Attributes
    contained in all otherAttributes lists in the structs making up the SVGimage
    *@pre SVGimgage  exists, is not null, and has not been freed.
    *@post SVGimage has not been modified in any way
    *@return the total length of all attribute structs in the SVGimage
    *@param obj - a pointer to an SVG struct
*/
int numAttr(SVGimage* img) {
    if (img == NULL) return 0;

    int totalCount = 0;

    //SVG attributes
    totalCount += getLength(img->otherAttributes);

    //Rectangles;
    totalCount += getAttributeCountFromRectangleList(img->rectangles);

    //Circles
    totalCount += getAttributeCountFromCircleList(img->circles);

    //Paths
    totalCount += getAttributeCountFromPathList(img->paths);

    //Groups
    totalCount += getAttributeCountFromGroupList(img->groups);

    return totalCount;
}

/*------------------- ALL the struct initialization functions helper functions -------------------*/

/* "One comment to rule them all" -> This stands for all of the initialization functions. Initializes an object of what ever is specified.
*   This means everything is malloced, lists are initialized correctly, but no data is actually in the pointers.
*
*   @Param If theres a string that is passed into one of the functions (Attribute/path), you pass the string in as a parameter.
*/

//See above for explaination
SVGimage* initializeSvgImage() {
    SVGimage *image = malloc(sizeof(SVGimage));

    strcpy(image->namespace, "");
    strcpy(image->title, "");
    strcpy(image->description, "");

    image->rectangles = initializeList( &rectangleToString, &deleteRectangle, &compareRectangles);
    image->circles = initializeList( &circleToString, &deleteCircle, &compareCircles);
    image->paths = initializeList( &pathToString, &deletePath, &comparePaths);
    image->groups = initializeList( &groupToString, &deleteGroup, &compareGroups);
    image->otherAttributes = initializeList(  &attributeToString, &deleteAttribute, &compareAttributes);

    return image;
}

//See above for explaination
Group* initializeGroup() {
    Group *g = malloc( sizeof(Group));


    g->rectangles = initializeList(  &rectangleToString, &deleteRectangle, &compareRectangles);
    g->circles = initializeList(  &circleToString, &deleteCircle, &compareCircles);
    g->paths = initializeList(  &pathToString, &deletePath, &comparePaths);
    g->groups = initializeList(  &groupToString, &deleteGroup, &compareGroups);
    g->otherAttributes = initializeList(  &attributeToString, &deleteAttribute, &compareAttributes);

    return g;
}

//See above for explaination
Rectangle* initializeRectangle() {
    Rectangle *rect = malloc( sizeof(Rectangle));

    rect->x = 0;
    rect-> y = 0;
    rect->height = -1;
    rect->width = -1;

    rect->otherAttributes = initializeList( &attributeToString, &deleteAttribute, &compareAttributes);

    return rect;
}

//See above for explaination
Circle* initializeCircle() {
    Circle *circle = (Circle *) malloc( sizeof(Circle));

    circle->cx = 0;
    circle->cy = 0;
    circle->r = 0;

    circle->otherAttributes = initializeList( &attributeToString, &deleteAttribute, &compareAttributes);


    return circle;
}

//See above for explaination
Path* initializePath(char *data) {
    Path *path = malloc( sizeof(Path));

    path->data = malloc( sizeof(char) * strlen(data)  + 1);
    strcpy(path->data, data);
    strcat(path->data, "\0");

    path->otherAttributes = initializeList( &attributeToString, &deleteAttribute, &compareAttributes);


    return path;
}

//See above for explaination
Attribute* initializeAttribute(char *name, char *value) {
    Attribute *atty = malloc( sizeof(Attribute));

    atty->name = malloc( sizeof(char) * strlen(name) + 1);
    atty->value = malloc( sizeof(char) * strlen(value) + 1);

    strcpy(atty->name, name);
    strcpy(atty->value, value);

    strcat(atty->name, "\0");
    strcat(atty->value, "\0");

    return atty;
}


/*------------------- ALL the Linked List helper functions -------------------*/
// *** ATTRIBUTES ***
char* attributeToString( void* data) {

    Attribute *atty = (Attribute *) data;
    char *returnVal;

    returnVal = malloc( sizeof(char) * (strlen(atty->name) + strlen(atty->value) + strlen(": ") + 1));   // +1 for the ': ' and +1 for the '\0'
    strcpy(returnVal, atty->name);
    strcat(returnVal, ": ");
    strcat(returnVal, atty->value);
    strcat(returnVal, "\0");

    return returnVal;
}

void deleteAttribute( void* data) {
    Attribute *atty = (Attribute *) data;

    free(atty->name);
    free(atty->value);
    free(atty);
}

int compareAttributes(const void *first, const void *second) {   // <0 for first bigger, 0 on equal, >0 for second, NULL when they're different elements.

    if (first == NULL) {
        return -1;

    } else if (second == NULL) {
        return 1;
    }

    Attribute *a = (Attribute *) first;
    Attribute *b = (Attribute *) second;

    if (strcmp(a->name, b->name) == 0) {
        return strcmp(a->value, b->value);

    }

    return 0;
}


// *** RECTANGLE ***
char* rectangleToString(void* data) {

    Rectangle *rect = (Rectangle *) data;
    char x[15], y[15], width[15], height[15], *returnVal;

    returnVal = malloc(sizeof(char) * 1000);

    //x value
    snprintf(x, NUMFLOATINGPOINTS, "%f", rect->x);
    //returnVal = malloc( sizeof(char) * (strlen("X: ") + 10));
    strcpy(returnVal, "X: ");
    strcat(returnVal, x);

    //y value
    snprintf(y, NUMFLOATINGPOINTS, "%f", rect->y);
    //returnVal = realloc(returnVal, sizeof(char) * (strlen(", Y: ") + 10 + strlen(returnVal)));
    strcat(returnVal, ", Y: ");
    strcat(returnVal, y);

    //width value
    snprintf(width, NUMFLOATINGPOINTS, "%f", rect->width);
    //returnVal = realloc(returnVal, sizeof(char) * (strlen(", width: ") + 10  + strlen(returnVal)));
    strcat(returnVal, ", width: ");
    strcat(returnVal, width);

    //height value
    snprintf(height, NUMFLOATINGPOINTS, "%f", rect->height);
    //returnVal = realloc(returnVal, sizeof(char) * (strlen(", height: ") + 10  + strlen(returnVal)));
    strcat(returnVal, ", height: ");
    strcat(returnVal, height);

    //Units value
    if (strlen(rect->units) != 0) {

        //returnVal = realloc(returnVal, sizeof(char) * (strlen(", Units: ") + strlen(rect->units) + strlen(returnVal)));
        strcat(returnVal, ", Units: ");
        strcat(returnVal, rect->units);
     }

    //Attributes value
    //returnVal = realloc(returnVal, sizeof(char) * (strlen( toString(rect->otherAttributes)) + strlen(returnVal)));
    char *temp = toString(rect->otherAttributes);

    strcat( returnVal, ", Attributes: ");
    strcat( returnVal, temp);
    free(temp);

    char *actualReturnVal;
    actualReturnVal = malloc(sizeof(char) * strlen(returnVal) + 1);

    strcpy(actualReturnVal, returnVal);
    strcat(actualReturnVal, "\0");
    free(returnVal);

    return actualReturnVal;
}

void deleteRectangle(void *data) {
    Rectangle *rect = (Rectangle *) data;

    freeList(rect->otherAttributes);
    free(rect);
}

int compareRectangles(const void* first, const void* second) {   //1 for same, 0 for different.

    if (first == NULL) {
        return -1;

    } else if (second == NULL) {
        return 1;
    }

    Rectangle *a = (Rectangle *) first;
    Rectangle *b = (Rectangle *) second;

    //Check all the values first.
    if (a->x != b->x) {
        return 0;

    } else if (a->y != b->y) {
        return 0;

    } else if (a->width != b->width) {
        return 0;

    } else if (a->height != b->height) {
        return 0;

    } else if ( strcmp(a->units, b->units) != 0 ) {
        return 0;

    } else if ( strcmp( toString(a->otherAttributes), toString(b->otherAttributes) ) != 0) {
        return 0;
    }

    return 1;

}


// *** CIRCLE ***
char* circleToString(void *data) {

    Circle *circle = (Circle *) data;
    char cx[15], cy[15], r[15], *returnVal;

    returnVal = malloc(sizeof(char) * 5000);  //Endline char

    //cx value
    snprintf(cx, NUMFLOATINGPOINTS, "%f", circle->cx);
    //returnVal = realloc(returnVal, sizeof("CX: ") + sizeof(cx)  + sizeof(returnVal) );
    strcpy(returnVal, "CX: ");
    strcat(returnVal, cx);

    //cy value
    snprintf(cy, NUMFLOATINGPOINTS, "%f", circle->cy);
    //returnVal = realloc(returnVal, sizeof(", CY: ") + sizeof(cy) + sizeof(returnVal) );
    strcat(returnVal, ", CY: ");
    strcat(returnVal, cy);

    //r value
    snprintf(r, NUMFLOATINGPOINTS, "%f", circle->r);
    //returnVal = realloc(returnVal, sizeof(", R: ") + sizeof(r) + sizeof(returnVal) );
    strcat(returnVal, ", r: ");
    strcat(returnVal, r);

    //Units value
    if (strlen(circle->units) != 0) {

        //returnVal = realloc(returnVal, sizeof(", Units: ") + sizeof(circle->units) + sizeof(returnVal) );
        strcat(returnVal, ", Units: ");
        strcat(returnVal, circle->units);
     }

    //Attributes value
    //returnVal = realloc(returnVal, sizeof(char) * strlen( toString(circle->otherAttributes) )+ sizeof(returnVal) );

    char *temp = toString(circle->otherAttributes);
    strcat( returnVal, temp );
    free(temp);

    char *actualReturnVal = malloc(sizeof(char) * strlen(returnVal) + 1);
    strcpy(actualReturnVal, returnVal);
    strcat(actualReturnVal, "\0");
    free(returnVal);

    return actualReturnVal;
}

void deleteCircle(void *data) {
    Circle *circle = (Circle *) data;

    freeList(circle->otherAttributes);
    free(circle);
}

int compareCircles(const void* first, const void* second) {   //1 for same, 0 for different.

    if (first == NULL) {
        return -1;

    } else if (second == NULL) {
        return 1;
    }

    Circle *a = (Circle *) first;
    Circle *b = (Circle *) second;

    if (a->cx != b->cx) {
        return 0;

    } else if (a->cy != b->cy) {
        return 0;

    } else if (a->r != b->r) {
        return 0;

    } else if ( strcmp(a->units, b->units) != 0 ) {
        return 0;

    } else if ( strcmp( toString(a->otherAttributes), toString(b->otherAttributes)) != 0) {
        return 0;
    }

    return 1;
}


// *** PATHS ***
char* pathToString(void *data) {

    Path *path = (Path *) data;
    char *returnVal = malloc(sizeof(char) * 5000);

    char *intermediateValue = toString(path->otherAttributes);

    strcpy(returnVal, "Data: ");
    strcat(returnVal, path->data);
    strcat(returnVal, ", Attributes: ");
    strcat(returnVal, intermediateValue);

    free(intermediateValue);

    char *actualReturn = malloc(sizeof(char) * strlen(returnVal) + 1);
    strcpy(actualReturn, returnVal);
    strcat(actualReturn, "\0");

    free(returnVal);

    return actualReturn;
}

void deletePath(void *data) {
    Path *path = (Path *) data;

    free(path->data);
    freeList(path->otherAttributes);
    free(path);
}

int comparePaths(const void* first, const void* second) {    //1 for same, 0 for different.

    if (first == NULL) {
        return -1;

    } else if (second == NULL) {
        return 1;
    }

    Path *a = (Path *) first;
    Path *b = (Path *) second;

    if ( strcmp(a->data, b->data) != 0 ) {
        return 0;

    } else if ( strcmp( toString(a->otherAttributes), toString(b->otherAttributes)) != 0) {
        return 0;
    }

    return 1;
}


// *** GROUP ***
char* groupToString(void *data) {

    Group *group = (Group *) data;

    int totalLength;
    char *returnVal;

    char *groupRects = toString(group->rectangles);
    char *groupCircles = toString(group->circles);
    char *groupPaths = toString(group->paths);
    char *groupGroups = toString(group->groups);
    char *groupAttributes = toString(group->otherAttributes);

    totalLength = strlen( groupRects ) + strlen( groupCircles ) + strlen( groupPaths )
                + strlen( groupGroups ) + strlen( groupAttributes );

    totalLength += strlen("Rectangles: ") + strlen(" | Circles: ") + strlen(" | Paths: ") + strlen(" | Groups: ") +
                    strlen(" | Attributes: ") + 1; //Plus one for the null terminator

    returnVal = malloc( sizeof(char) * totalLength);

    strcpy(returnVal, "Rectangles: ");
    strcat(returnVal, groupRects);
    strcat(returnVal, " | Circles: ");
    strcat(returnVal, groupCircles);
    strcat(returnVal, " | Paths: ");
    strcat(returnVal, groupPaths);
    strcat(returnVal, " | Groups: ");
    strcat(returnVal, groupGroups);
    strcat(returnVal, " | Attributes: ");
    strcat(returnVal, groupAttributes);
    strcat(returnVal, "\0");

    free(groupRects);
    free(groupCircles);
    free(groupPaths);
    free(groupGroups);
    free(groupAttributes);

    return returnVal;
}

void deleteGroup(void *data) {

    Group *group = (Group *) data;

    freeList(group->rectangles);
    freeList(group->circles);
    freeList(group->paths);
    freeList(group->groups);
    freeList(group->otherAttributes);
    free(group);
}

int compareGroups(const void* first, const void* second) {   //1 for same, 0 for different.

    if (first == NULL) {
        return -1;

    } else if (second == NULL) {
        return 1;
    }

    Group *a = (Group *) first;
    Group *b = (Group *) second;

    if ( getLength(a->rectangles) != getLength(a->rectangles)) {    //Compar the lengths of all the LL first, faster compute times this way.
        return 0;

    } else if ( getLength(a->circles) != getLength(a->circles)) {
        return 0;

    } else if ( getLength(a->paths) != getLength(a->paths)) {
        return 0;

    } else if ( getLength(a->groups) != getLength(a->groups)) {
        return 0;

    } else if ( getLength(a->otherAttributes) != getLength(a->otherAttributes)) {   //Then compar all the actual elements of the LL using their strings.
        return 0;

    } else if ( strcmp(toString(a->rectangles), toString(b->rectangles) ) != 0 ) {
        return 0;

    } else if ( strcmp(toString(a->circles), toString(b->circles) ) != 0 ) {
        return 0;

    } else if ( strcmp(toString(a->paths), toString(b->paths) ) != 0 ) {
        return 0;

    } else if ( strcmp(toString(a->groups), toString(b->groups) ) != 0 ) {
        return 0;

    } else if ( strcmp(toString(a->otherAttributes), toString(b->otherAttributes) ) != 0 ) {
        return 0;
    }

    return 1;
}

// For module 2 nothing should be deleted when a list frees something so this is inputted instead of the correct delete function
void universalNoDelete(void *data) {
    return;
}

/*  This is a recursive algorithm that gets all the rectangles from a group. Then calls itself on each of the subgroups in the group.
*   @Param  the group to be disected, the list of the rectangles.
*/
void getRectsFromGroup(Group *g, List *newRectList) {

    //For the rectangles.
    int rectListLength = getLength(g->rectangles);
    ListIterator iter = createIterator(g->rectangles);

    for (int i = 0; i < rectListLength; i++) {
        Rectangle *rect = (Rectangle *) nextElement(&iter);

        insertBack(newRectList, rect);
    }

    //Now the subgroups

    int groupListLength = getLength(g->groups);
    ListIterator svgGroupIter = createIterator(g->groups);
    for (int i = 0; i < groupListLength; i++) {
        Group *gSub = (Group *) nextElement(&svgGroupIter);

        //This gets called for every group in the svg element.
        getRectsFromGroup(gSub, newRectList);
    }
}

/*  This is a recursive algorithm that gets all the circles from a group. Then calls itself on each of the subgroups in the group.
*   @Param  the group to be disected, the list of the circles.
*/
void getCirclesFromGroup(Group *g, List *newCircleList) {

    //For the circles.
    int circleListLength = getLength(g->circles);
    ListIterator iter = createIterator(g->circles);

    for (int i = 0; i < circleListLength; i++) {
        Circle *circle = (Circle *) nextElement(&iter);

        insertBack(newCircleList, circle);
    }

    //Now the subgroups

    int groupListLength = getLength(g->groups);
    ListIterator svgGroupIter = createIterator(g->groups);
    for (int i = 0; i < groupListLength; i++) {
        Group *gSub = (Group *) nextElement(&svgGroupIter);

        //This gets called for every group in the svg element.
        getCirclesFromGroup(gSub, newCircleList);
    }
}

/*  This is a recursive algorithm that gets all the groups from a group. Then calls itself on each of the subgroups in the group.
*   @Param  the group to be disected, the list of the groups.
*/
void getGroupsFromGroup(Group *g, List *newGroupList) {

    int groupListLength = getLength(g->groups);
    ListIterator iter = createIterator(g->groups);

    for (int i = 0; i < groupListLength; i++) {
        Group *gSub = (Group *) nextElement(&iter);

        insertBack(newGroupList, gSub);

        //This gets called for every group in the svg element.
        getGroupsFromGroup(gSub, newGroupList);
    }
}

/*  This is a recursive algorithm that gets all the paths from a group. Then calls itself on each of the subgroups in the group.
*   @Param  the group to be disected, the list of the paths.
*/
void getPathsFromGroup(Group *g, List *newPathList) {

    //For the path.
    int pathListLength = getLength(g->paths);
    ListIterator iter = createIterator(g->paths);

    for (int i = 0; i < pathListLength; i++) {
        Path *path = (Path *) nextElement(&iter);

        insertBack(newPathList, path);
    }

    //Now the subgroups

    int groupListLength = getLength(g->groups);
    ListIterator groupIter = createIterator(g->groups);
    for (int i = 0; i < groupListLength; i++) {
        Group *gSub = (Group *) nextElement(&groupIter);

        //This gets called for every group in the svg element.
        getPathsFromGroup(gSub, newPathList);
    }
}

/*  Help function for 'numGroupsWithLen'. This recursively goes through all the groups (Depth-first) and finds all groups with that length
    adding them all up and returning the total count.
*   @Param the group being dissected. The length we are comparing against the length of the group list.
*   @Return the total number of length(g->groups) we found for every group in the svg image.
*/
int getGroupsFromParentGroup(Group *g, int len) {
    int comparitor, totalCount = 0;
    int childrenCount = getLength(g->groups);
    ListIterator iter = createIterator(g->groups);

    for (int i = 0; i < childrenCount; i++) {
        Group *gSub = nextElement(&iter);

        comparitor = getSizeOfGroup(gSub);

        if (comparitor == len) totalCount++;

        totalCount += getGroupsFromParentGroup(gSub, len);
    }
    return totalCount;
}

/*------------------- Helper functions for 'numAttr', gets all the attributes for each list -------------------*/
//Rectangles;
int getAttributeCountFromRectangleList(List *rectangleList) {
    int totalCount = 0;
    ListIterator iter = createIterator(rectangleList);

    for (int i = 0; i < getLength(rectangleList); i++) {
        Rectangle *rect = nextElement(&iter);

        totalCount += getLength(rect->otherAttributes);
    }
    return totalCount;
}

//Circles
int getAttributeCountFromCircleList(List *circleList) {
    int totalCount = 0;
    ListIterator iter = createIterator(circleList);

    for (int i = 0; i < getLength(circleList); i++) {
        Circle *circle = nextElement(&iter);

        totalCount += getLength(circle->otherAttributes);
    }
    return totalCount;
}

//Paths
int getAttributeCountFromPathList(List *pathList) {
    int totalCount = 0;
    ListIterator iter = createIterator(pathList);

    for (int i = 0; i < getLength(pathList); i++) {
        Path *path = nextElement(&iter);

        totalCount += getLength(path->otherAttributes);
    }
    return totalCount;
}

//Groups -> This one is a little different in that it calls the above functions and calls itself recursively.
int getAttributeCountFromGroupList(List *groupsList) {
    int totalCount = 0;

    ListIterator iter = createIterator(groupsList);

    for (int i = 0; i < getLength(groupsList); i++) {
        Group *subgroup = nextElement(&iter);

        totalCount += getAttributesFromGroupAndItsShapes(subgroup);

        totalCount += getAttributeCountFromGroupList(subgroup->groups);
    }
    return totalCount;
}

/*  Helper for 'getAttributeCountFromGroupList' (above), calls all the other functions that get the attribute count from a list of shapes.
*   @Param the group whos shape lists that the attributes will be counted.
*   @Return the count of all attributes from that group and it's shape children.
*/
int getAttributesFromGroupAndItsShapes(Group *group) {
    int totalCount = 0;

    totalCount += getLength(group->otherAttributes);
    totalCount += getAttributeCountFromRectangleList(group->rectangles);
    totalCount += getAttributeCountFromCircleList(group->circles);
    totalCount += getAttributeCountFromPathList(group->paths);

    return totalCount;
}

int getSizeOfGroup(Group *gsub) {
    int length = 0;

    length += getLength(gsub->rectangles);
    length += getLength(gsub->circles);
    length += getLength(gsub->paths);
    length += getLength(gsub->groups);

    return length;
}



/* ------------------------------------------------ MODULE 2 FUNCTIONS ------------------------------------------------ */

xmlDocPtr imageToXmlTree(SVGimage *image) {
    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;
    ListIterator iter;

    doc = xmlNewDoc(BAD_CAST "1.0");    //Create the new doc

    //Create the root node: an SVG node and set it to the root node
    root_node = svgElementToXmlNode(image);
    xmlDocSetRootElement(doc, root_node);

    //Adding the namespace
    xmlNsPtr ns = xmlNewNs (root_node, BAD_CAST image->namespace, BAD_CAST NULL);
    xmlSetNs(root_node, ns);

    //Adding title and dec
    if (strcmp(image->title, "") != 0) {
        xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "title", NULL);
        xmlNodePtr node1 = xmlNewText(BAD_CAST image->title);
        xmlAddChild(node, node1);
    }

    if (strcmp(image->description, "") != 0) {
        xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "desc", NULL);
        xmlNodePtr node1 = xmlNewText(BAD_CAST image->description);
        xmlAddChild(node, node1);
    }

    //Rectangles
    iter = createIterator(image->rectangles);
    for (int i = 0; i < getLength(image->rectangles); i++) {
        Rectangle *rect = (Rectangle *) nextElement(&iter);
        createRectNode(rect, root_node);
    }

    //Circles
    iter = createIterator(image->circles);
    for (int i = 0; i < getLength(image->circles); i++) {
        Circle *circle = (Circle *) nextElement(&iter);
        createCircleNode(circle, root_node);
    }

    //Paths
    iter = createIterator(image->paths);
    for (int i = 0; i < getLength(image->paths); i++) {
        Path *path = (Path *) nextElement(&iter);
        createPathNode(path, root_node);
    }

    //Groups
    iter = createIterator(image->groups);
    for (int i = 0; i < getLength(image->groups); i++) {
        Group *group = (Group *) nextElement(&iter);
        createGroupNode(group, root_node);
    }

    return doc;
}

//Takes in a single group element, and turns it and it's attributes into a xmlNodePtr
void createGroupNode(Group *group, xmlNodePtr root_node) {
    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "g", NULL);

    //The groups attributes
    int attributeListLength = getLength(group->otherAttributes);
    ListIterator iter = createIterator(group->otherAttributes);
    for (int i = 0; i < attributeListLength; i++) {
        Attribute *atty = (Attribute *) nextElement(&iter);

        xmlNewProp(node, BAD_CAST atty->name, BAD_CAST atty->value);
    }

    //Rectangles
    iter = createIterator(group->rectangles);
    for (int i = 0; i < getLength(group->rectangles); i++) {
        Rectangle *rect = (Rectangle *) nextElement(&iter);
        createRectNode(rect, node);
    }

    //Circles
    iter = createIterator(group->circles);
    for (int i = 0; i < getLength(group->circles); i++) {
        Circle *circle = (Circle *) nextElement(&iter);
        createCircleNode(circle, node);
    }

    //Paths
    iter = createIterator(group->paths);
    for (int i = 0; i < getLength(group->paths); i++) {
        Path *path = (Path *) nextElement(&iter);
        createPathNode(path, node);
    }

    //Groups
    iter = createIterator(group->groups);
    for (int i = 0; i < getLength(group->groups); i++) {
        Group *group = (Group *) nextElement(&iter);
        createGroupNode(group, node);
    }
}

//Takes in a single path element, and turns it and it's attributes into a xmlNodePtr
void createPathNode(Path *path, xmlNodePtr root_node) {
    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "path", NULL);

    //Add in the data
    xmlNewProp(node, BAD_CAST "d", BAD_CAST path->data);

    //Adding all the attributes
    int attributeListLength = getLength(path->otherAttributes);
    ListIterator iter = createIterator(path->otherAttributes);

    for (int i = 0; i < attributeListLength; i++) {
        Attribute *atty = (Attribute *) nextElement(&iter);

        xmlNewProp(node, BAD_CAST atty->name, BAD_CAST atty->value);
    }
}

//Takes in a single circle element, and turns it and it's attributes into a xmlNodePtr
void createCircleNode(Circle *circle, xmlNodePtr root_node) {
    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "circle", NULL);

    char* buff = malloc(sizeof(char) * 100);

    //Add in the cx, cy, and r.
    sprintf(buff, "%f", circle->cx);
    strcat(buff, circle->units);
    xmlNewProp(node, BAD_CAST "cx", BAD_CAST buff);

    sprintf(buff, "%f", circle->cy);
    strcat(buff, circle->units);
    xmlNewProp(node, BAD_CAST "cy", BAD_CAST buff);

    sprintf(buff, "%f", circle->r);
    strcat(buff, circle->units);
    xmlNewProp(node, BAD_CAST "r", BAD_CAST buff);

    //Getting the attributes
    int attributeListLength = getLength(circle->otherAttributes);
    ListIterator iter = createIterator(circle->otherAttributes);

    for (int i = 0; i < attributeListLength; i++) {
        Attribute *atty = (Attribute *) nextElement(&iter);

        xmlNewProp(node, BAD_CAST atty->name, BAD_CAST atty->value);
    }

    free(buff);
}

//Takes in a single path element, and turns it and it's attributes into a xmlNodePtr
void createRectNode(Rectangle *rect, xmlNodePtr root_node) {
    xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "rect", NULL);

    char* buff = malloc(sizeof(char) * 100);

    //Add in the x, y, heigth, and width.
    sprintf(buff, "%f", rect->x);
    strcat(buff, rect->units);
    xmlNewProp(node, BAD_CAST "x", BAD_CAST buff);

    sprintf(buff, "%f", rect->y);
    strcat(buff, rect->units);
    xmlNewProp(node, BAD_CAST "y", BAD_CAST buff);

    sprintf(buff, "%f", rect->width);
    strcat(buff, rect->units);
    xmlNewProp(node, BAD_CAST "width", BAD_CAST buff);

    sprintf(buff, "%f", rect->height);
    strcat(buff, rect->units);
    xmlNewProp(node, BAD_CAST "height", BAD_CAST buff);


    //The attributes
    int attributeListLength = getLength(rect->otherAttributes);
    ListIterator iter = createIterator(rect->otherAttributes);

    for (int i = 0; i < attributeListLength; i++) {
        Attribute *atty = (Attribute *) nextElement(&iter);

        xmlNewProp(node, BAD_CAST atty->name, BAD_CAST atty->value);
    }

    free(buff);
}

//This just returns a node initialized with the svg element as it's name and all it's attributes attached to it.
xmlNodePtr svgElementToXmlNode(SVGimage *image) {
    xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "svg");

    int attributeListLength = getLength(image->otherAttributes);
    ListIterator iter = createIterator(image->otherAttributes);

    for (int i = 0; i < attributeListLength; i++) {
        Attribute *atty = (Attribute *) nextElement(&iter);

        xmlNewProp(node, BAD_CAST atty->name, BAD_CAST atty->value);
    }
    return node;
}

//Takes in a xml fileName, and its xsd schema file name and creates a svgImage tree using those two. The schema file validates the xml file.
SVGimage* createValidSVGimage(char* fileName, char* schemaFile) {
    if (!fileName) return NULL;
    if (!schemaFile) return NULL;   //Check for valid args

    SVGimage* image;
    xmlDoc *doc = NULL;

    doc = readFileAndValidate(fileName, schemaFile);    //Make a xmlDoc from the image and scheme file.

    if (!doc) {
        xmlCleanupParser();
        xmlFreeDoc(doc);
        return NULL;
    }

    //Now get the SVG content from the XML doc
    image = xmlDocToSvgDoc(doc);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    if (!image) return NULL;
    return image;
}

//Parser that check it against an XSD file from http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
xmlDocPtr readFileAndValidate(char *XMLFileName, char *XSDFileName) {
    xmlDocPtr doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(XSDFileName);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    //xmlSchemaDump(stdout, schema); //To print schema dump

    doc = xmlReadFile(XMLFileName, NULL, 0);

    if (doc == NULL)
    {
        if(schema != NULL) xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlMemoryDump();
        return NULL;
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        xmlSchemaFreeValidCtxt(ctxt);
        //xmlFreeDoc(doc);
    }

    // free the resource
    if(schema != NULL) xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlMemoryDump();


    return doc;
}

//Write an svg image to a file at the end.
bool writeSVGimage(SVGimage *doc, char *filename) {
    if (!doc) return NULL;
    if (!filename) return NULL;

    //Turn the SVGimage back into an xml file tree
    xmlDocPtr SVGdoc = imageToXmlTree(doc);

    int check = xmlSaveFormatFileEnc(filename, SVGdoc, "UTF-8", 0);
    xmlCleanupParser();
    xmlMemoryDump();

    xmlFreeDoc(SVGdoc);

    if (check < 0) return false;
    return true;
}

//Returns true/false depending on if a svgImage is valid using the XSD file as a reference.
bool validateSVGimage(SVGimage* image, char* schemaFile){

    //Check the inputs
    if (image == NULL)  return false;
    if (schemaFile == NULL)  return false;

    //Validate against XSD file here
    bool answer = checkImageAgainstSchemaFile(image, schemaFile);
    if (answer == false) return false;

    //Validate all values in the file here. (Ensure not NULL, or weird negative values)
    if (!checkValuesInSVGimage(image)) return false;

    return true;
}

//Helper for validateSVGimage
bool checkImageAgainstSchemaFile(SVGimage *image, char *schemaFile) {
    xmlDocPtr doc = imageToXmlTree(image); //Turns the SVGImage into a xml tree.
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(schemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);        //This could def be an error later, watch out.
    xmlSchemaFreeParserCtxt(ctxt);
    //xmlSchemaDump(stdout, schema); //To print schema dump

    if (doc == NULL)
    {
        if(schema != NULL) xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlMemoryDump();
        return false;
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        xmlSchemaFreeValidCtxt(ctxt);
    }

    // free the resource
    if(schema != NULL) xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlMemoryDump();

    return true;
}

/** This checks all the values in the svg file, making sure nothing is initialized as null.
*   @param image the svg image being checked
*   @return the truth value if it's valid
*/
bool checkValuesInSVGimage(SVGimage* image) {

    if (!checkForNullElementsInSVGimage(image)) return false;

    if (!checkElementsInSVGimage(image)) return false;

    return true;
}

//This checks that every element the SVG is supposed to have has been initialized. If not it fails.
bool checkForNullElementsInSVGimage(SVGimage* image) {

    if (!image->rectangles) return false;

    if (!image->circles) return false;

    if (!image->paths) return false;

    if (!image->groups) return false;

    if (!image->otherAttributes) return false;

    if (strcmp("", image->namespace) == 0) return false;

    if (!checkGroupListForAnyNULLchildren(image->groups)) return false;

    return true;
}

bool checkGroupListForAnyNULLchildren(List* groups) {
    ListIterator iter;
    Group *group;

    iter = createIterator(groups);

    for (int i = 0; i < getLength(groups); i++){
        group = nextElement(&iter);

        if (!checkSingleGroupForNULLchildren(group)) return false;
    }

    return true;
}

bool checkSingleGroupForNULLchildren(Group *group) {
    if (!group->rectangles) return false;

    if (!group->circles) return false;

    if (!group->paths) return false;

    if (!group->groups) return false;

    if (!group->otherAttributes) return false;

    if (!checkGroupListForAnyNULLchildren(group->groups)) return false;

    return true;
}
//Checks the rect, circle, path, and attributes list ensuring each element in each list is valid.
bool checkElementsInSVGimage(SVGimage* image) {

    if (!checkRectangleList(image->rectangles)) return false;

    if (!checkCircleList(image->circles)) return false;

    if (!checkPathList(image->paths)) return false;

    if (!checkGroupList(image->groups)) return false;

    if (!checkAttributeList(image->otherAttributes)) return false;

    return true;
}

//This is for lists of elements
bool checkAttributeList(List *attributes) {
    ListIterator iter;
    Attribute *atty;

    if (!attributes) return false;

    iter = createIterator(attributes);

    for (int i = 0; i < getLength(attributes); i++){
        atty = nextElement(&iter);

        if (!atty) return false;

        if (!checkSingleAttribute(atty)) return false;
    }
    return true;
}

bool checkRectangleList(List* rectangles) {
    ListIterator iter;
    Rectangle *rect;

    if (!rectangles) return false;

    iter = createIterator(rectangles);

    for (int i = 0; i < getLength(rectangles); i++){
        rect = nextElement(&iter);
        if (!rect) return false;

        if (!checkSingleRectangle(rect)) return false;
    }

    return true;
}

bool checkCircleList(List* circles) {
    ListIterator iter;
    Circle *circle;

    if (!circles) return false;

    iter = createIterator(circles);

    for (int i = 0; i < getLength(circles); i++){
        circle = nextElement(&iter);

        if (!circle) return false;

        if (!checkSingleCircle(circle)) return false;
    }

    return true;
}

bool checkPathList(List* paths) {
    ListIterator iter;
    Path *path;

    if (!paths) return false;

    iter = createIterator(paths);

    for (int i = 0; i < getLength(paths); i++){
        path = nextElement(&iter);

        if (!path) return false;

        if (!checkSinglePath(path)) return false;
    }

    return true;
}

bool checkGroupList(List* groups) {
    ListIterator iter;
    Group *group;

    if (!groups) return false;

    iter = createIterator(groups);

    for (int i = 0; i < getLength(groups); i++){
        group = nextElement(&iter);

        if (!group) return false;

        if (!checkSingleGroup(group)) return false;
    }
    return true;
}

//This checks single shapes and elements for being valid
bool checkSingleAttribute(Attribute *atty) {

    if (!atty->name) return false;
    if (!atty->value) return false;

    if ( strcmp(atty->name, "") == 0) return false;

    if ( strcmp(atty->value, "") == 0) return false;
    return true;
}

bool checkSingleRectangle(Rectangle *rect) {

    if (rect->width < 0) return false;

    if (rect->height < 0) return false;

    if (!rect->otherAttributes) return false;

    if (!checkAttributeList(rect->otherAttributes)) return false;

    return true;
}

bool checkSingleCircle(Circle *circle) {

    if (circle->r < 0) return false;

    if (!circle->otherAttributes) return false;

    if (!checkAttributeList(circle->otherAttributes)) return false;

    return true;
}

bool checkSinglePath(Path *path) {

    if (!path->data) return false;

    if ( strcmp(path->data, "") == 0) return false;     //Not sure if I need this or not.

    if (!path->otherAttributes) return false;

    if (!checkAttributeList(path->otherAttributes)) return false;

    return true;
}

bool checkSingleGroup(Group *group) {

    if (!checkRectangleList(group->rectangles)) return false;

    if (!checkCircleList(group->circles)) return false;

    if (!checkPathList(group->paths)) return false;

    if (!checkAttributeList(group->otherAttributes)) return false;

    if (!checkGroupList(group->groups)) return false;

    return true;
}

void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute) {
    if (!image) return;
    if (!newAttribute) return;
    if (elemType > 4 || elemType < 0) return;

    if (!checkSingleAttribute(newAttribute)) return;

    ListIterator iter;

    if (elemType == SVG_IMAGE) {
        int size = getLength(image->otherAttributes);

        iter = createIterator(image->otherAttributes);
        Attribute *atty;

        for (int i = 0; i < size; i++) {
            atty = nextElement(&iter);

            if ( strcmp(atty->name, newAttribute->name) == 0) {
                if (strlen(atty->value) > strlen(newAttribute->value)) {

                    atty->value = realloc(atty->value, sizeof(char) * strlen(newAttribute->value) + 1);
                }

                strcpy(atty->value, newAttribute->value);
                strcat(atty->value, "\0");

                return;
            }
        }

        insertBack(image->otherAttributes, newAttribute);

    } else {

        if (elemIndex < 0) return;

        if (elemType == CIRC) {

            int size = getLength(image->circles);
            if (elemIndex > size) goto END;

            iter = createIterator(image->circles);
            Circle *circle;
            for (int i = 0; i <= elemIndex; i++) circle = nextElement(&iter);

            if (strcmp(newAttribute->name, "cx") == 0) {

                float ftemp = atof(newAttribute->value);

                circle->cx = ftemp;

            } else if (strcmp(newAttribute->name, "cy") == 0) {

                float ftemp = atof(newAttribute->value);

                circle->cy = ftemp;

            } else if (strcmp(newAttribute->name, "r") == 0) {

                float ftemp = atof(newAttribute->value);

                circle->r = ftemp;

            } else {

                bool insert = UpdateAttributes( createIterator(circle->otherAttributes), newAttribute, getLength(circle->otherAttributes));
                if(insert) insertBack(circle->otherAttributes, newAttribute);
            }

        } else if (elemType == RECT) {
            int size = getLength(image->rectangles);
            if (elemIndex > size) goto END;

            iter = createIterator(image->rectangles);
            Rectangle *rect;

            for (int i = 0; i <= elemIndex; i++) rect = nextElement(&iter);

            if (strcmp(newAttribute->name, "x") == 0) {

                float ftemp = atof(newAttribute->value);

                rect->x = ftemp;

            } else if (strcmp(newAttribute->name, "y") == 0) {

                float ftemp = atof(newAttribute->value);

                rect->y = ftemp;

            } else if (strcmp(newAttribute->name, "width") == 0) {

                float ftemp = atof(newAttribute->value);

                rect->width = ftemp;

            } else if (strcmp(newAttribute->name, "height") == 0) {

                float ftemp = atof(newAttribute->value);

                rect->height = ftemp;

            } else {
                bool insert = UpdateAttributes( createIterator(rect->otherAttributes), newAttribute, getLength(rect->otherAttributes));
                if(insert) insertBack(rect->otherAttributes, newAttribute);
            }

        } else if (elemType == PATH) {
            int size = getLength(image->paths);
            if (elemIndex > size) goto END;

            iter = createIterator(image->paths);
            Path *path;

            for (int i = 0; i <= elemIndex; i++) path = nextElement(&iter);

            if (strcmp(newAttribute->name, "d") == 0) {

                strcpy(path->data, newAttribute->value);

            } else {
                bool insert = UpdateAttributes( createIterator(path->otherAttributes), newAttribute, getLength(path->otherAttributes));
                if(insert) insertBack(path->otherAttributes, newAttribute);
            }

        } else {
            int size = getLength(image->groups);
            if (elemIndex > size) goto END;

            iter = createIterator(image->groups);
            Group *group;

            for (int i = 0; i <= elemIndex; i++) group = nextElement(&iter);

            bool insert = UpdateAttributes( createIterator(group->otherAttributes), newAttribute, getLength(group->otherAttributes));
            if(insert) insertBack(group->otherAttributes, newAttribute);
        }
    }

    return;
    END:    //If the list is shorter than the index, free the attribute and end
    deleteAttribute(newAttribute);
}

//This takes in a list of attributes, if theres an attribute w/ the same name as new attribute, its value is updated to
// newAttribute's value, else, newAttribute is added to the list.
bool UpdateAttributes(ListIterator iter, Attribute *newAttribute, int length) {
    Attribute *atty;

    for (int i = 0; i < length; i++) {
        atty = nextElement(&iter);

        if ( strcmp(atty->name, newAttribute->name) == 0) {
            strcpy(atty->value, newAttribute->value);
            deleteAttribute(newAttribute);
            return false;
        }
    }

    return true;
}

//Addes a new shape to the SVG image.
void addComponent(SVGimage* image, elementType elemType, void* newComponent) {
    if (elemType == SVG_IMAGE) return;
    if (!image) return;
    if (!newComponent) return;

    if (elemType == RECT) {
        insertBack(image->rectangles, newComponent);

    } else if (elemType == CIRC) {
        insertBack(image->circles, newComponent);

    } else if (elemType == PATH) {
        insertBack(image->paths, newComponent);

    } else {
        insertBack(image->groups, newComponent);
    }
}

/*
*   The A2 M3 SVGimage to JSON string functions and their helper functions.
*/

char* attrToJSON(const Attribute *a) {
    if (!a) return "{}";

    int combinedLenth = 22;

    combinedLenth += strlen(a->name);
    combinedLenth += strlen(a->value);

    char *JSONstring = malloc(sizeof(char) * combinedLenth + 1);

    strcpy(JSONstring, "{\"name\":\"");
    strcat(JSONstring, a->name);
    strcat(JSONstring, "\",\"value\":\"");
    strcat(JSONstring, a->value);
    strcat(JSONstring, "\"}" );
    strcat(JSONstring, "\0");

    return JSONstring;
}

char* circleToJSON(const Circle *c) {
    if (!c) return "{}";

    char *cx, *cy, *r, *numAttr, *units, *JSONstring;

    cx = malloc(sizeof(char) * 15);
    cy = malloc(sizeof(char) * 15);
    r = malloc(sizeof(char) * 15);
    numAttr = malloc(sizeof(char) * 2);
    units = malloc(sizeof(char) * 3);
    //strcpy(units, "\0");

    //Setting the Circles attributes to strings
    sprintf(cx, "%.2lf", c->cx);
    sprintf(cy, "%.2lf", c->cy);
    sprintf(r, "%.2lf", c->r);
    sprintf(numAttr, "%d", getLength(c->otherAttributes) );

    strcpy(units, c->units);

    int combinedLength = strlen("{\"cx\":,\"cy\":,\"r\":,\"numAttr\":,\"units\":\"\"}");

    //Getting the lengths of all the strings
    combinedLength += strlen(cx);
    combinedLength += strlen(cy);
    combinedLength += strlen(r);
    combinedLength += strlen(numAttr);
     if ( strcmp(units, "") != 0) combinedLength += strlen(units);

    //JSONstring = malloc(sizeof(char) * combinedLength + 1);
    JSONstring = malloc(sizeof(char) * 1000);

    strcpy(JSONstring, "{\"cx\":" );
    strcat(JSONstring, cx);
    strcat(JSONstring, ",\"cy\":");
    strcat(JSONstring, cy);
    strcat(JSONstring, ",\"r\":");
    strcat(JSONstring, r);
    strcat(JSONstring, ",\"numAttr\":");
    strcat(JSONstring, numAttr);
    strcat(JSONstring, ",\"units\":\"");
    if ( strcmp(c->units, "") != 0) strcat(JSONstring, units);
    strcat(JSONstring, "\"}");
    strcat(JSONstring, "\0");

    free(cx);
    free(cy);
    free(r);
    free(units);
    free(numAttr);

    return JSONstring;
}

char* rectToJSON(const Rectangle *r) {
    if (!r) return "{}";

    char *x, *y, *width, *height, *numAttr, *units, *JSONstring;

    x = malloc(sizeof(char) * 15);
    y = malloc(sizeof(char) * 15);
    width = malloc(sizeof(char) * 15);
    height = malloc(sizeof(char) * 15);
    numAttr = malloc(sizeof(char) * 2);
    units = malloc(sizeof(char) * 3);
    //strcpy(units, "\0");

    //Setting the Circles attributes to strings
    sprintf(x, "%.2lf", r->x);
    sprintf(y, "%.2lf", r->y);
    sprintf(width, "%.2lf", r->width);
    sprintf(height, "%.2lf", r->height);
    sprintf(numAttr, "%d", getLength(r->otherAttributes) );

    //if ( strcmp(r->units, "") != 0) {
        //free(units);
        //units = malloc(sizeof(char) * (strlen(r->units) + 1) );
    strcpy(units, r->units);
    //}

    int combinedLength = strlen("{\"x\":,\"y\":,\"w\":,\"h\":,\"numAttr\":,\"units\":\"\"}");

    //Getting the lengths of all the strings
    combinedLength += strlen(x);
    combinedLength += strlen(y);
    combinedLength += strlen(width);
    combinedLength += strlen(height);
    combinedLength += strlen(numAttr);
    if (strcmp(units, "") != 0) combinedLength += strlen(units);

    JSONstring = malloc(sizeof(char) * combinedLength + 1);

    strcpy(JSONstring, "{\"x\":" );
    strcat(JSONstring, x);
    strcat(JSONstring, ",\"y\":");
    strcat(JSONstring, y);
    strcat(JSONstring, ",\"w\":");
    strcat(JSONstring, width);
    strcat(JSONstring, ",\"h\":");
    strcat(JSONstring, height);
    strcat(JSONstring, ",\"numAttr\":");
    strcat(JSONstring, numAttr);
    strcat(JSONstring, ",\"units\":\"");

    if ( strcmp(r->units, "") != 0) strcat(JSONstring, units);
    strcat(JSONstring, "\"}");
    strcat(JSONstring, "\0");

    free(x);
    free(y);
    free(width);
    free(height);
    free(units);
    free(numAttr);

    return JSONstring;

}

char* formatPathData(char *data, int start) {
    int len = strlen(data);

    for (int i = start; i < len; i++) {
        if (data[i] == '"' && data[i-1] != '\\'){

            char *newStr = malloc(sizeof(char) * (len + 1) );   //Copy the new string over
            strncpy(newStr, data, i);
            strcat(newStr, "\\");
            strcat(newStr, data + i + 1);
            
            if (start > 0) free(data);  //It's not the original if the start is greater than the default
            return formatPathData(newStr, i);
        }
    }

    return data;
}

char* pathToJSON(const Path *p) {
    if (!p) return "{}";

    char *JSONstring, *numAttr;

    numAttr = malloc(sizeof(char) * 2);

    //Setting the Circles attributes to strings
    sprintf(numAttr, "%d", getLength(p->otherAttributes) );

    int combinedLength = strlen("{\"d\":\"\",\"numAttr\":}");

    //Getting the lengths of all the strings
    combinedLength += strlen(p->data);
    combinedLength += strlen(numAttr);

    JSONstring = malloc(sizeof(char) * combinedLength + 1);

    strcpy(JSONstring, "{\"d\":\"" );
    strcat(JSONstring, formatPathData(p->data, 0));
    strcat(JSONstring, "\",\"numAttr\":");
    strcat(JSONstring, numAttr);
    strcat(JSONstring, "}");
    strcat(JSONstring, "\0");

    free(numAttr);

    return JSONstring;
}

char* groupToJSON(const Group *g) {
    if (!g) return "{}";

    char *numChildren, *numAttributes;

    numChildren = malloc(sizeof(char) * 2);
    numAttributes = malloc(sizeof(char) * 2);

    int lengthOfChildren = 0;
    lengthOfChildren += getLength(g->rectangles);
    lengthOfChildren += getLength(g->circles);
    lengthOfChildren += getLength(g->paths);
    lengthOfChildren += getLength(g->groups);

    int numberAttributes = getLength(g->otherAttributes);

    sprintf(numChildren, "%d", lengthOfChildren);
    sprintf(numAttributes, "%d", numberAttributes);

    int size = strlen("{\"children\":,\"numAttr\":}");
    size += strlen(numChildren) + strlen(numAttributes);
    size++;

    char *JSONstring = malloc(sizeof(char) * size);

    strcpy(JSONstring, "{\"children\":" );
    strcat(JSONstring, numChildren);
    strcat(JSONstring, ",\"numAttr\":");
    strcat(JSONstring, numAttributes);
    strcat(JSONstring, "}");
    strcat(JSONstring, "\0");

    free(numChildren);
    free(numAttributes);

    return JSONstring;
}


char* SVGtoJSON(const SVGimage* image) {
    if (!image) return "{}";

    //SVGimage img = image;

    int rectCount, circleCount, pathCount, groupCount;

    List *rectList = getRects( (SVGimage *) image);
    List *circleList = getCircles( (SVGimage *) image);
    List *pathList = getPaths( (SVGimage *) image);
    List *groupList = getGroups( (SVGimage *) image);

    rectCount = getLength(rectList);
    circleCount = getLength(circleList);
    pathCount = getLength(pathList);
    groupCount = getLength(groupList);

    char *numR, *numC, *numP, *numG, *JSONstring;

    numR = malloc(sizeof(char) * 2);
    numC = malloc(sizeof(char) * 2);
    numP = malloc(sizeof(char) * 2);
    numG = malloc(sizeof(char) * 2);

    sprintf(numR, "%d", rectCount);
    sprintf(numC, "%d", circleCount);
    sprintf(numP, "%d", pathCount);
    sprintf(numG, "%d", groupCount);

    int size = strlen("{\"numRect\":,\"numCirc\":,\"numPaths\":,\"numGroups\":}");

    size += strlen(numR);
    size += strlen(numC);
    size += strlen(numP);
    size += strlen(numG);
    size++;     //For the null char

    JSONstring = malloc(sizeof(char) * size);

    strcpy(JSONstring, "{\"numRect\":");
    strcat(JSONstring, numR);
    strcat(JSONstring, ",\"numCirc\":");
    strcat(JSONstring, numC);
    strcat(JSONstring, ",\"numPaths\":");
    strcat(JSONstring, numP);
    strcat(JSONstring, ",\"numGroups\":");
    strcat(JSONstring, numG);
    strcat(JSONstring, "}");
    strcat(JSONstring, "\0");

    free(numR);
    free(numC);
    free(numP);
    free(numG);
    freeList(rectList);
    freeList(circleList);
    freeList(pathList);
    freeList(groupList);

    return JSONstring;
}

char* attrListToJSON(const List *list) {
    if (!list) return "[]";

    ListIterator iter = createIterator( (List *) list);

    int length = getLength( (List *) list);
    int size = 0;

    for (int i = 0; i < length; i++) {
        Attribute *atty = nextElement(&iter);

        char *temp = attrToJSON(atty);

        size += strlen(temp);

        free(temp);
    }

    size += 3 + (size - 1); //Commas and brackets

    char *JSONstring = malloc(sizeof(char) * size);
    strcpy(JSONstring, "[");

    iter = createIterator( (List *) list);
    for (int i = 0; i < length; i++) {
        Attribute *atty = nextElement(&iter);

        char *temp = attrToJSON(atty);

        strcat(JSONstring, temp);

        if (i + 1 != length) strcat(JSONstring, ",");
        free(temp);
    }

    strcat(JSONstring, "]");
    strcat(JSONstring, "\0");

    return JSONstring;
}

char* circListToJSON(const List *list) {
    if (!list) return "[]";

    ListIterator iter = createIterator( (List *) list);

    int length = getLength( (List *) list);
    int size = 0;

    for (int i = 0; i < length; i++) {
        Circle *circle = nextElement(&iter);

        char *temp = circleToJSON(circle);

        size += strlen(temp);
        free(temp);
    }

    size += 2 + (size - 1); //Commas and brackets
    size++;     //For null

    char *JSONstring = malloc(sizeof(char) * size);
    strcpy(JSONstring, "[");

    iter = createIterator( (List *) list);
    for (int i = 0; i < length; i++) {
        Circle *circle = nextElement(&iter);

        char *temp = circleToJSON(circle);

        strcat(JSONstring, temp);

        if (i + 1 != length) strcat(JSONstring, ",");
        free(temp);
    }

    strcat(JSONstring, "]");
    strcat(JSONstring, "\0");

    return JSONstring;
}

char* rectListToJSON(const List *list) {
    if (!list) return "[]";

    ListIterator iter = createIterator( (List *) list);

    int length = getLength( (List *) list);
    int size = 0;


    for (int i = 0; i < length; i++) {
        Rectangle *rect = nextElement(&iter);

        char *temp = rectToJSON(rect);

        size += strlen(temp);

        free(temp);
    }

    //size += 2 + (size - 1); //Commas and brackets
    //size++; //for null

    size += 100;

    char *JSONstring = malloc(sizeof(char) * size);
    strcpy(JSONstring, "[");

    iter = createIterator( (List *) list);
    for (int i = 0; i < length; i++) {
        Rectangle *rect = nextElement(&iter);

        char *temp = rectToJSON(rect);

        strcat(JSONstring, temp);

        if (i + 1 != length) strcat(JSONstring, ",");
        free(temp);
    }

    strcat(JSONstring, "]");
    strcat(JSONstring, "\0");

    return JSONstring;
}

char* pathListToJSON(const List *list) {
    if (!list) return "[]";

    ListIterator iter = createIterator( (List *) list);

    int length = getLength( (List *) list);
    int size = 0;

    for (int i = 0; i < length; i++) {
        Path *path = nextElement(&iter);

        char *temp = pathToJSON(path);

        size += strlen(temp);

        free(temp);
    }

    //size += 2 + (size - 1); //Commas and brackets
    //size++;     //for null
    size += 100;

    char *JSONstring = malloc(sizeof(char) * size);
    strcpy(JSONstring, "[");

    iter = createIterator( (List *) list);
    for (int i = 0; i < length; i++) {
        Path *path = nextElement(&iter);

        char *temp = pathToJSON(path);

        strcat(JSONstring, temp);

        if (i + 1 != length) strcat(JSONstring, ",");
        free(temp);
    }

    strcat(JSONstring, "]");
    strcat(JSONstring, "\0");

    return JSONstring;
}

char* groupListToJSON(const List *list) {
    if (!list) return "[]";

    ListIterator iter = createIterator( (List *) list);

    int length = getLength( (List *) list);
    int size = 0;

    for (int i = 0; i < length; i++) {
        Group *group = nextElement(&iter);

        char *temp = groupToJSON(group);

        size += strlen(temp);

        free(temp);
    }

    size += 2 + (size - 1); //Commas and brackets
    size++;     //For null

    char *JSONstring = malloc(sizeof(char) * size);
    strcpy(JSONstring, "[");

    iter = createIterator( (List *) list);
    for (int i = 0; i < length; i++) {
        Group *group = nextElement(&iter);

        char *temp = groupToJSON(group);

        if (temp) strcat(JSONstring, temp);

        if (i + 1 != length) strcat(JSONstring, ",");
        free(temp);
    }

    strcat(JSONstring, "]");
    strcat(JSONstring, "\0");

    return JSONstring;
}

SVGimage* addRectanleToImg(SVGimage* img, float x, float y, float height, float width, char *units, char* colour, char* stroke, char* strokeWidth) {
    Rectangle *rect = initializeRectangle();

    rect->x = x; 
    rect->y = y;
    rect->width = width; 
    rect->height = height;
    strcpy(rect->units, units);

    Attribute *afill = initializeAttribute("fill", colour);
    insertBack(rect->otherAttributes, afill);

    Attribute *astroke = initializeAttribute("stroke", stroke);
    insertBack(rect->otherAttributes, astroke);

    Attribute *astrokeWidth = initializeAttribute("stroke-width", strokeWidth);
    insertBack(rect->otherAttributes, astrokeWidth);


    insertBack(img->rectangles, rect); 
    return img;
}

SVGimage* addCircleToImg(SVGimage* img, float cx, float cy, float r, char *units, char *colour, char *stroke, char *strokeWidth) {

    Circle *circle = initializeCircle();

    circle->cx = cx; 
    circle->cy = cy;
    circle->r = r; 
    strcpy(circle->units, units);

    Attribute *afill = initializeAttribute("fill", colour);
    insertBack(circle->otherAttributes, afill);

    Attribute *astroke = initializeAttribute("stroke", stroke);
    insertBack(circle->otherAttributes, astroke);

    Attribute *astrokeWidth = initializeAttribute("stroke-width", strokeWidth);
    insertBack(circle->otherAttributes, astrokeWidth);

    insertBack(img->circles, circle); 
    return img;
}

char* getAllRectsJSON(SVGimage* img) {
    if (img == NULL) return "[]";

    List* list = getRects(img);

    char *jsonStr = (char*) malloc(sizeof(char) *5000);   //For the '['
    strcpy(jsonStr, "[");

    Rectangle *rect;
    ListIterator iter = createIterator(list);
    int counter = 0;
    while((rect = nextElement(&iter)) != NULL){
        
        char *temp = rectToJSON(rect);
        //jsonStr = (char*) realloc(jsonStr, sizeof(char) * (strlen(temp) + 3));

        strcat(jsonStr, temp);
        strcat(jsonStr, ",");

        free(temp);
        counter++;
    }

    freeList(list);

    if (counter > 0) {
        jsonStr[strlen(jsonStr) - 1] = ']';
    
    } else {
        strcat(jsonStr, "]");
    }
    
    return jsonStr;
}

char* getAllCirclesJSON(SVGimage* img) {
    if (img == NULL) return "[]";

    List* list = getCircles(img);

    char *jsonStr = (char*) malloc(sizeof(char) * 5000);   //For the '['
    strcpy(jsonStr, "[");

    Circle *circle;
    ListIterator iter = createIterator(list);
    int counter = 0;
    while((circle = nextElement(&iter)) != NULL){
        
        char *temp = circleToJSON(circle);

        //jsonStr = (char*) realloc(jsonStr, sizeof(char) * (strlen(temp) + 3));

        strcat(jsonStr, temp);
        strcat(jsonStr, ",");

        free(temp);
        counter++;
    }

    freeList(list);

    if (counter > 0) {
        jsonStr[strlen(jsonStr) - 1] = ']';
    
    } else {
        strcat(jsonStr, "]");
    }
    
    return jsonStr;
}

char *wholeSVGtoJSON(SVGimage *img) {
    if (img == NULL) return "[]";

    char* rects, *circles, *returnVal;

    rects = getAllRectsJSON(img);
    circles = getAllCirclesJSON(img);

    returnVal = malloc(sizeof(char) * (strlen(circles) + strlen(rects) + strlen("[\"Rectangles\",,\"Circles\",]") + 1));
    

    strcpy(returnVal, "[\"Rectangles\"," );
    strcat(returnVal, rects);
    strcat(returnVal, ",\"Circles\",");
    strcat(returnVal, circles);
    strcat(returnVal, "]");

    free(rects);
    free(circles);

    return returnVal;
}


Rectangle* JSONtoRect(const char* svgString) {
    Rectangle *rect = initializeRectangle();

    if (svgString == NULL) return rect;

    char *charStart, *charEnd;
    int start, end;
    char subStr[10];

    int cursor = 0;    

    //Find x
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    strncpy(subStr, svgString + start + 1, end - start);
    subStr[end - start] = '\0';
    rect->x = atof(subStr);
    cursor = end + 1; 
    //printf("x = %f\n", rect->x);

    
    //Find y
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    strncpy(subStr, svgString + start + 1, end - start);
    subStr[end - start] = '\0';
    rect->y = atof(subStr);
    cursor = end + 1; 
    //printf("y = %f\n", rect->y);

    
    //Find width
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    strncpy(subStr, svgString + start + 1, end - start);
    subStr[end - start] = '\0';
    rect->width = atof(subStr);
    cursor = end + 1; 
    //printf("width = %f\n", rect->width);

    //Find height
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    strncpy(subStr, svgString + start + 1, end - start);
    subStr[end - start] = '\0';
    rect->height = atof(subStr);
    cursor = end + 1; 
    //printf("height = %f\n", rect->height);

    //Find Number of Attrbutes  -> Don't need this
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    cursor = end + 1; 

    //Find units
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    strncpy(subStr, svgString + start + 1, end - start);
    subStr[end - start] = '\0';
    Attribute *atty = initializeAttribute("units", subStr);
    insertBack(rect->otherAttributes, atty);

    return rect;
}


Circle* JSONtoCircle(const char* svgString) {
    Circle *circle = initializeCircle();

    if (svgString == NULL) return circle;

    char *charStart, *charEnd;
    int start, end;
    char subStr[10];

    int cursor = 0;    

    //Find cx
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    strncpy(subStr, svgString + start + 1, end - start);
    subStr[end - start] = '\0';
    circle->cx = atof(subStr);
    cursor = end + 1; 
    //printf("x = %f\n", rect->x);

    
    //Find cy
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    strncpy(subStr, svgString + start + 1, end - start);
    subStr[end - start] = '\0';
    circle->cy = atof(subStr);
    cursor = end + 1; 
    //printf("y = %f\n", rect->y);

    
    //Find r
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    strncpy(subStr, svgString + start + 1, end - start);
    subStr[end - start] = '\0';
    circle->r = atof(subStr);
    cursor = end + 1; 
    //printf("width = %f\n", rect->width);

    //Find num attributes
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    cursor = end + 1; 
    //printf("height = %f\n", rect->height);

    //Find units
    charStart = strchr(svgString + cursor, ':');  //+1 for the colon
    start = charStart - svgString;
    charEnd = strchr(svgString + start + 1, ',');    //-1 for the comma 
    end = charEnd - svgString;
    strncpy(subStr, svgString + start + 1, end - start);
    subStr[end - start] = '\0';
    Attribute *atty = initializeAttribute("units", subStr);
    insertBack(circle->otherAttributes, atty);

    return circle;
}


SVGimage* wholeJSONtoSVG(char *json, char *namespace, char *title, char *desc ) {
    SVGimage *img = initializeSvgImage();
    if (strcmp(json, "[]") == 0) return img;

    printf("JSON = %s\n\n", json);
    
    //Insert title, ns, and desc
    strcpy(img->namespace, namespace);
    strcpy(img->title, title);
    strcpy(img->description, desc);

    char *charIndexOfCircle = strstr(json, "Circles");
    int indexOfCircle = charIndexOfCircle - json;

    //I know the rect data starts at index 15 so...
    if (json[15] != ']') {  //Has rects 

        char *charEndIndex;
        int endIndex;
        int currentIndex = 15;
        char subStr[200];
        while(true) {       //For rects
            if (currentIndex >= indexOfCircle) break;

            charEndIndex = strchr(json + currentIndex, '}');
            
            if (charEndIndex == NULL) break;

            endIndex = (charEndIndex - json) + 1;

            //printf("Len being copied = %d\n", endIndex - currentIndex);

            strncpy(subStr, json + currentIndex, endIndex - currentIndex);
            subStr[endIndex - currentIndex] = '\0';
            //insertBack(img->rectangles, JSONtoRect(subStr));

            currentIndex = endIndex + 1;
        }
    }

    
    //For the circles
    char *charEndIndex;
    int endIndex;
    int currentIndex = indexOfCircle + 10;
    char subStr[200];
    if (json[currentIndex] == ']') return img;

    while(true) {       //For cirles
        if (currentIndex >= indexOfCircle) break;

        charEndIndex = strchr(json + currentIndex, '}');
        
        if (charEndIndex == NULL) break;

        endIndex = (charEndIndex - json) + 1;

        //printf("Len being copied = %d\n", endIndex - currentIndex);

        strncpy(subStr, json + currentIndex, endIndex - currentIndex);
        subStr[endIndex - currentIndex] = '\0';
        insertBack(img->circles, JSONtoCircle(subStr));

        currentIndex = endIndex + 1;
    };  
    
    return img;
}

//Returns the namespace to the user ifimg is valid, else returns a empty string
char* getNameSpace(SVGimage *img) {
    if (img == NULL) return "";

    return img->namespace;
}

//Returns the title to the user ifimg is valid, else returns a empty string
char* getTitle(SVGimage *img) {
    if (img == NULL) return "";

    return img->title;
}

//Returns the description to the user ifimg is valid, else returns a empty string
char* getDesc(SVGimage *img) {
    if (img == NULL) return "";

    return img->description;
}


char* getPathsToJSON(SVGimage *img) {
    if (img == NULL) return "[]";

    List* list = getPaths(img);

    char *jsonStr = (char*) malloc(sizeof(char) * 5000);   //For the '['
    strcpy(jsonStr, "[");

    Path *path;
    ListIterator iter = createIterator(list);
    int counter = 0;
    while((path = nextElement(&iter)) != NULL){
        
        char *temp = pathToJSON(path);

        //jsonStr = (char*) realloc(jsonStr, sizeof(char) * (strlen(temp) + 3));

        strcat(jsonStr, temp);
        strcat(jsonStr, ",");

        free(temp);
        counter++;
    }

    freeList(list);

    if (counter > 0) {
        jsonStr[strlen(jsonStr) - 1] = ']';
    
    } else {
        strcat(jsonStr, "]");
    }
    
    return jsonStr;
}

