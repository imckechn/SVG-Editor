from ctypes import *
import json
from datetime import datetime

AllRects = ''
AllCircles = ''
img = ''
indexOfShape = 0

def viewImgContents(name, libc, imgJSON):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)

    rects = len(imgJSON[1])
    circles = len(imgJSON[3])


    #rects = libc.getAllRects(img)
    #circles = libc.getAllCircles(img)
    paths = libc.getAllPaths(img)
    groups = libc.getAllGroups(img)

    print('Name                  |' + name)
    print('Number of Rectangles  |' + str(rects))
    print('Number of Circles     |' + str(circles))
    print('Number of Paths       |' + str(paths))
    print('Number of Groups      |' + str(groups))
    print('\n')


def createRectangle(libc, img, imgJSON, metaData):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)

    print('\n')
    print('For the rectangle please give')

    x = ''
    while (True): 
        print('An X value(an int)')
        x = int(input())

        if (x >= 0):
            break
    
    
    y = ''
    while (True): 
        print('An Y value(an int)')
        y = int(input())

        if ( y >= 0 ):
            break  

    
    width = ''
    while (True): 
        print('A width value(an int)')
        width = int(input())

        if ( width > 0 ):
            break

    
    height = ''
    while (True): 
        print('A height value(an int)')
        height = int(input())

        if ( height > 0 ):
            break

    
    units = ''
    while (True): 
        print("What units (px, mm, cm, in, or enter for the default)")
        units = input()

        if ( units == "" or units.lower() == "mm" or units.lower() == 'in' or units.lower() == 'cm' or units.lower() == 'px' ):
            break
        print('units = ' + units)


    fill = ''
    print("What fill colour? (red, black... OR Hex value:#888888")
    colour = input()

    stroke = ''
    strokeWidth = ''
    
    
    while (True): 
        print('Add stroke? (y/n)')
        yesNo = input()

        if (yesNo.lower() == 'n'):
            break

        elif (yesNo.lower() == 'y'):
            print('Stroke colour: (red, black... OR Hex value:#888888')
            stroke = input()

            print('stroke width(px):')
            strokeWidth = input()
            break    

    #img = libc.addRectanleToImg(img, c_float(x), c_float(y), c_float(width), c_float(height), c_char_p(units.encode('utf-8')), c_char_p(colour.encode('utf-8')), c_char_p(stroke.encode('utf-8')), c_char_p(strokeWidth.encode('utf-8')))
    #create the rect JSON obj
    rect = {}
    rect["x"] = float(x)
    rect["y"] = float(y)
    rect["w"] = float(width)
    rect["h"] = float(height)
    rect["units"] = units
    rect["numAttr"] = int(1)
    rect["attributes"] = []

    #Add the fill obj
    attribute = {}
    attribute['name'] = 'fill'
    attribute['value'] = colour
    rect["attributes"].append(attribute)

    if (yesNo == 'y'):
        rect["numAttr"] = int(3)
        
        attributeTwo = {}
        attributeTwo['name'] = "stroke"
        attributeTwo['value'] = stroke
        rect["attributes"].append(attributeTwo)

        attributeThree = {}
        attributeThree['name'] = "stroke-width"
        attributeThree['value'] = str(strokeWidth)
        rect["attributes"].append(attributeThree)

    #add the rect JSON to imgJSON
    imgJSON[1].append(rect)
    updateMetaDataCreate(metaData, "rectangle")

    print('Rectangle complete')

    return imgJSON


def createCircle(libc, img, imgJSON, metaData):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)

    print('\n')
    print('For the circle please give')
    
    x = ''
    while (True): 
        print('An X value(an int)')
        x = int(input())

        if (x >= 0):
            break
    
    y = ''
    while (True): 
        print('An Y value(an int)')
        y = int(input())

        if ( y >= 0 ):
            break
        
    r = ''
    while (True): 
        print('A radius value(an int)')
        r = int(input())

        if ( r > 0 ):
            break

    
    units = ''
    while (True): 
        print("What units (px, mm, cm, in)")
        units = input()

        if ( units.lower() == "mm" or units.lower() == 'in' or units.lower() == 'cm' or units.lower() == 'px' ):
            break
        print('units = ' + units)


    fill = ''
    print("What fill colour? (red, black... OR Hex value:#888888")
    colour = input()

    stroke = ''
    strokeWidth = ''
    
    while (True): 
        print('Add stroke? (y/n)')
        yesNo = input()

        if (yesNo.lower() == 'n'):
            break

        elif (yesNo.lower() == 'y'):
            print('Stroke colour: (red, black... OR Hex value:#888888')
            stroke = input()

            print('stroke width(px):')
            strokeWidth = input()
            break

    #img = libc.addCircleToImg(img, c_float(x), c_float(y), c_float(r), c_char_p(units.encode('utf-8')), c_char_p(colour.encode('utf-8')), c_char_p(stroke.encode('utf-8')), c_char_p(strokeWidth.encode('utf-8')))
    #Create the circle JSON obj
    circle = {}
    circle["cx"] = float(x)
    circle["cy"] = float(y)
    circle["r"] = float(r)
    circle["units"] = units
    circle["attributes"] = []
    circle['numAttr'] = int(1)

    #Add the fill obj
    attribute = {}
    attribute['name'] = 'fill'
    attribute['value'] = colour
    circle["attributes"].append(attribute)

    if (yesNo == 'y'):
        circle['numAttr'] = int(3)

        attributeTwo = {}
        attributeTwo['name'] = "stroke"
        attributeTwo['value'] = stroke
        circle["attributes"].append(attributeTwo)

        attributeThree = {}
        attributeThree['name'] = "stroke-width"
        attributeThree['value'] = str(strokeWidth)
        circle["attributes"].append(attributeThree)

    #add the circle JSON to imgJSON
    imgJSON[3].append(circle)
    updateMetaDataCreate(metaData, "circle")
    print('Circle complete')

    return imgJSON

    
def editRectangle(libc, imgJSON, metaData):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)

    #List all the rectangles
    counter = 0
    colourIndex = 0
    print("imgJSON[1] = " + str(imgJSON))
    for rect in imgJSON[1]:
        
        #get the colour
        colour = ''
        subCount = -1
        for attribute in rect['attributes']:
            subCount += 1

            if str(attribute['name']) == 'fill':
                colour = str(attribute['value'])
                colourIndex = subCount
                break

        print('Rectangle        |' + str(counter))
        print('x                |' + str(rect['x']))
        print('y                |' + str(rect['y']))
        print('width            |' + str(rect['w']))        
        print('height           |' + str(rect['h']))
        print('units            |' + str(rect['units']))
        print('Colour           |' + colour)
        print('# of attributes  |' + str(rect['numAttr']))
        print('--------------------------------------------\n')
        counter += 1

    #Get and check the user input
    while(True):
        print('What rectangle would you like to edit')
        answer = input()

        if (int(answer) >= 0 and int(answer) < counter):
            break

    #Get the new value(s)
    print('New x value (\'n\' to leave it as-is)')
    x = input()

    print('New y value (\'n\' to leave it as-is)')
    y = input()

    print('New width value (\'n\' to leave it as-is)')
    width = input()

    print('New height value (\'n\' to leave it as-is)')
    height = input()

    print('New unit type (cm, mm, px, or \'n\' to leave it as-is)')
    units = input()

    print('New Colour (In hex w/ hashtag or english)')
    colour = input()

    #Make the edit(s)
    if (x != 'n'):
        updateMetaDataEdit(metaData, "rectangle", int(answer), "x:" + str(imgJSON[1][int(answer)]['x']) + "->" + x)
        imgJSON[1][int(answer)]['x'] = float(x)

    if (y != 'n'):
        updateMetaDataEdit(metaData, "rectangle", int(answer), "y:" + str(imgJSON[1][int(answer)]['y']) + "->" + y)
        imgJSON[1][int(answer)]['y'] = float(y)

    if (width != 'n'):
        updateMetaDataEdit(metaData, "rectangle", int(answer), "w:" + str(imgJSON[1][int(answer)]['w']) + "->" + width)
        imgJSON[1][int(answer)]['w'] = float(width)

    if (height != 'n'):
        updateMetaDataEdit(metaData, "rectangle", int(answer), "h:" + str(imgJSON[1][int(answer)]['h']) + "->" + height)
        imgJSON[1][int(answer)]['h'] = float(height)

    if (units != 'n'):
        updateMetaDataEdit(metaData, "rectangle", int(answer), "units:" + str(imgJSON[1][int(answer)]['units']) + "->" + units)
        imgJSON[1][int(answer)]['units'] = units

    if (colour != 'n'):
        if (imgJSON[1][int(answer)]['numAttr'] == 0 or subCount == -1):
            updateMetaDataEdit(metaData, "rectangle", int(answer), "fillCreated:" + colour)
            imgJSON[1][int(answer)]['attributes'].append({"name":'fill', 'value':colour})

        else:
            updateMetaDataEdit(metaData, "rectangle", int(answer), "fill:" + imgJSON[1][int(answer)]['attributes'][colourIndex]['value'] + "->" + colour)
            imgJSON[1][int(answer)]['attributes'][colourIndex]['value'] = colour

    print('Edits Complete\n')      

    return imgJSON
    
    
def editCircle(libc, imgJSON, metaData):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)
       
    counter = 0
    colourIndex = 0
    for circle in imgJSON[3]:

        #get the colour
        colour = ''
        subCount = -1
        for attribute in circle['attributes']:
            subCount += 1
            
            if str(attribute['name']) == 'fill':
                colour = str(attribute['value'])
                colourIndex = subCount
                break


        print('Circle           |' + str(counter))
        print('x                |' + str(circle['cx']))
        print('y                |' + str(circle['cy']))
        print('r                |' + str(circle['r']))   
        print('units            |' + str(circle['units']))
        print('Colour           |' + colour)
        print('# of attributes  |' + str(circle['numAttr']))
        print('--------------------------------------------\n')
        counter += 1

    #Get and check the user input
    while(True):
        print('What circle would you like to edit')
        answer = input()

        if (counter == 0):
            print("No circles to edit")
            return imgJSON

        if (int(answer) >= 0 and int(answer) < counter):
            break

    #Get the new value(s)
    print('New x value (\'n\' to leave it as-is)')
    x = input()

    print('New y value (\'n\' to leave it as-is)')
    y = input()

    print('New r value (\'n\' to leave it as-is)')
    r = input()

    print('New unit type (cm, mm, px, or \'n\' to leave it as-is)')
    units = input()

    print('New Colour (In hex w/ hashtag or english)')
    colour = input()

    #Make the edit(s)
    if (x != 'n'):
        updateMetaDataEdit(metaData, "circle", int(answer), "cx:" + str(imgJSON[3][int(answer)]['cx']) + "->" + x)
        imgJSON[3][int(answer)]['cx'] = float(x)

    if (y != 'n'):
        updateMetaDataEdit(metaData, "circle", int(answer), "cy:" + str(imgJSON[3][int(answer)]['cy']) + "->" + y)
        imgJSON[3][int(answer)]['cy'] = float(y)

    if (r != 'n'):
        updateMetaDataEdit(metaData, "circle", int(answer), "r:" + str(imgJSON[3][int(answer)]['r']) + "->" + r)
        imgJSON[3][int(answer)]['r'] = float(r)

    if (units != 'n'):
        updateMetaDataEdit(metaData, "circle", int(answer), "units:" + imgJSON[3][int(answer)]['units'] + "->" + units)
        imgJSON[3][int(answer)]['units'] = units
    
    if (colour != 'n'):
        if (imgJSON[3][int(answer)]['numAttr'] == 0 or subCount == -1):
            updateMetaDataEdit(metaData, "circle", int(answer), "fillCreated:" + colour)
            imgJSON[3][int(answer)]['attributes'].append({"name":'fill', 'value':colour})

        else:
            updateMetaDataEdit(metaData, "circle", str(answer), "fill:" + imgJSON[3][int(answer)]['attributes'][colourIndex]['value'] + "->" + colour)
            imgJSON[3][int(answer)]['attributes'][colourIndex]['value'] = colour

    print('Edits complete')

    return imgJSON


def deleteShape(imgJSON):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)

    #Get the shape the user wants to delete
    shape = ''
    while(True):
        print('Would you like to delete a circle or a rectangle(c/r)')
        shape = input()

        if (shape == 'c' or shape == 'r'):
            break

    counter = 0
    if (shape == 'c'):
        print('outside')
        for circle in imgJSON[3]:
            print('Circle           |' + str(counter))
            print('x                |' + str(circle['cx']))
            print('y                |' + str(circle['cy']))
            print('r                |' + str(circle['r']))   
            print('units            |' + str(circle['units']))
            print('# of attributes  |' + str(circle['numAttr']))
            print('--------------------------------------------\n')
            counter += 1

    else:
        for rect in imgJSON[1]:
            print('Rectangle        |' + str(counter))
            print('x                |' + str(rect['x']))
            print('y                |' + str(rect['y']))
            print('width            |' + str(rect['w']))        
            print('height           |' + str(rect['h']))
            print('units            |' + str(rect['units']))
            print('# of attributes  |' + str(rect['numAttr']))
            print('--------------------------------------------\n')
            counter += 1

    #check for zero shapes
    if (counter == 0):
        print('No shapes of that type to delete')
        return

    #Get the shape number
    while(True):
        print('Which one would you like to delete?(insert number)')
        number = input()
        if (int(number) >= 0 and int(number) < counter):
            break

    if (shape == 'c'):
        updateMetaDataDelete(metaData, "circle", int(number), imgJSON[3][int(number)])
        imgJSON[3].pop(int(number))
    
    else: 
        updateMetaDataDelete(metaData, "rectangle", int(number), imgJSON[1][int(number)])
        imgJSON[1].pop(int(number))
    
    print('Deleted')
    return imgJSON

    
def checkValidity(answer):
    if int(answer) == 0: #It's invalid
        print("Sorry, this is not a valid SVG image, please try again with a different image")
        
    return answer


def viewPaths(libc, img):
    #get the paths
    print("before")
    paths = libc.getPathsToJSON(img)
    print("After")
    paths = (c_char_p(paths).value.decode("utf-8"))
    paths = json.loads(paths)
    
    counter = 0
    for path in paths:
        print('Singel path = ' + str(path))
        print('Path: ' + str(counter))
        print('Data: ' + path['d'])
        print('# of attributes:' + str(path['numAttr']))
        print('--------------------------------------------\n')
        counter += 1


def viewRectangles(libc, img, imgJSON):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)

    #List all the rectangles
    counter = 0
    for rect in imgJSON[1]:
        
        #get the colour
        colour = ''
        subCount = -1
        for attribute in rect['attributes']:
            subCount += 1

            if str(attribute['name']) == 'fill':
                colour = str(attribute['value'])
                break

        print('Rectangle        |' + str(counter))
        print('x                |' + str(rect['x']))
        print('y                |' + str(rect['y']))
        print('width            |' + str(rect['w']))        
        print('height           |' + str(rect['h']))
        print('units            |' + str(rect['units']))
        print('Colour           |' + colour)
        print('# of attributes  |' + str(rect['numAttr']))
        print('--------------------------------------------\n')
        counter += 1

    return imgJSON


def viewCircles(libc, img, imgJSON):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)
       
    counter = 0
    for circle in imgJSON[3]:

        #get the colour
        colour = ''
        subCount = -1
        for attribute in circle['attributes']:
            subCount += 1
            
            if str(attribute['name']) == 'fill':
                colour = str(attribute['value'])
                break

        print('Circle           |' + str(counter))
        print('x                |' + str(circle['cx']))
        print('y                |' + str(circle['cy']))
        print('r                |' + str(circle['r']))   
        print('units            |' + str(circle['units']))
        print('Colour           |' + colour)
        print('# of attributes  |' + str(circle['numAttr']))
        print('--------------------------------------------\n')
        counter += 1

    return imgJSON


def checkName(name):
    if len(name) == 0:
        return False

    if '0' in name:
        return False
    elif '1' in name:
        return False
    elif '2' in name:
        return False
    elif '3' in name:
        return False
    elif '4' in name:
        return False
    elif '5' in name:
        return False
    elif '6' in name:
        return False
    elif '7' in name:
        return False
    elif '8' in name:
        return False
    elif '9' in name:
        return False
    elif '.' in name:
        return False
    elif ',' in name:
        return False
    elif '/' in name:
        return False
    elif '\\' in name:
        return False
    elif '[' in name:
        return False
    elif ']' in name:
        return False
    elif '{' in name:
        return False
    elif '}' in name:
        return False 
    elif '`' in name:
        return False 
    elif '~' in name:
        return False 
    elif '!' in name:
        return False 
    elif '@' in name:
        return False 
    elif '#' in name:
        return False 
    elif '$' in name:
        return False 
    elif '%' in name:
        return False 
    elif '^' in name:
        return False 
    elif '&' in name:
        return False 
    elif '*' in name:
        return False 
    elif '(' in name:
        return False 
    elif '-' in name:
        return False 
    elif '_' in name:
        return False 
    elif '+' in name:
        return False 
    elif '=' in name:
        return False 
    elif '|' in name:
        return False 
    elif '?' in name:
        return False 
    elif '>' in name:
        return False 
    elif '<' in name:
        return False 
    elif '  ' in name:
        return False 
    elif ' ' in name:
        return False 
    else:
        return True 


def searchElements(atty, value, libc, imgJSON):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)

    if (atty == "x" or atty == "y"):    #only checking circles and squares
        counter = 0
        for circle in imgJSON[3]:
            if (str(circle['cx']) == value ):

                #get the colour
                colour = ''
                for attribute in circle['attributes']:

                    if str(attribute['name']) == 'fill':
                        colour = str(attribute['value'])
                        break


                print('Circle           |' + str(counter))
                print('x                |' + str(circle['cx']))
                print('y                |' + str(circle['cy']))
                print('r                |' + str(circle['r']))   
                print('units            |' + str(circle['units']))
                print('Colour           |' + colour)
                print('# of attributes  |' + str(circle['numAttr']))
                print('--------------------------------------------\n')
                counter += 1

        counter = 0
        for rect in imgJSON[1]:
            if (str(rect['x']) == value ):

                #get the colour
                colour = ''
                for attribute in rect['attributes']:

                    if str(attribute['name']) == 'fill':
                        colour = str(attribute['value'])
                        break

                print('Rectangle        |' + str(counter))
                print('x                |' + str(rect['x']))
                print('y                |' + str(rect['y']))
                print('width            |' + str(rect['w']))        
                print('height           |' + str(rect['h']))
                print('units            |' + str(rect['units']))
                print('Colour           |' + colour)
                print('# of attributes  |' + str(rect['numAttr']))
                print('--------------------------------------------\n')
                counter += 1

    elif (atty == "r") :#Only cirles

        counter = 0
        for circle in imgJSON[3]:
            if (str(circle['r']) == value ):
                #get the colour
                colour = ''
                for attribute in circle['attributes']:

                    if str(attribute['name']) == 'fill':
                        colour = str(attribute['value'])
                        break

                print('Circle           |' + str(counter))
                print('x                |' + str(circle['cx']))
                print('y                |' + str(circle['cy']))
                print('r                |' + str(circle['r']))   
                print('units            |' + str(circle['units']))
                print('Colour           |' + colour)
                print('# of attributes  |' + str(circle['numAttr']))
                print('--------------------------------------------\n')
                counter += 1

    else:   #check everything inluding paths
        counter = 0
        for rect in imgJSON[1]:
            if (atty in str(rect) and value in str(rect)):
                print('Rectangle        |' + str(counter))
                print('x                |' + str(rect['x']))
                print('y                |' + str(rect['y']))
                print('width            |' + str(rect['w']))        
                print('height           |' + str(rect['h']))
                print('units            |' + str(rect['units']))
                print( atty + '           |' + value)
                print('# of attributes  |' + str(rect['numAttr']))
                print('--------------------------------------------\n')
                counter += 1

        counter = 0
        for circle in imgJSON[3]:
            if (atty in str(circle) and value in str(circle)):
                print('Circle           |' + str(counter))
                print('x                |' + str(circle['cx']))
                print('y                |' + str(circle['cy']))
                print('r                |' + str(circle['r']))   
                print('units            |' + str(circle['units']))
                print( atty + '           |' + value)
                print('# of attributes  |' + str(circle['numAttr']))
                print('--------------------------------------------\n')
                counter += 1

        paths = libc.getPathsToJSON(img)
        paths = (c_char_p(paths).value.decode("utf-8"))
        paths = json.loads(paths)
        
        counter = 0
        for path in paths:
            if (atty in str(path) and value in str(path)):
                print('Singel path = ' + str(path))
                print('Path: ' + str(counter))
                print('Data: ' + path['d'])
                print('# of attributes:' + str(path['numAttr']))
                print('--------------------------------------------\n')
                counter += 1

    print('Finished')


def getMetaDataFileIndex(metaData, fileName):
    try:
        for x in range(len(metaData['data'])):
            if metaData['data'][str(x)]['filename'] == fileName:
                return x

        return len(metaData['data'])

    except Exception as e: #if there is no name nothing is initialized
        #print("the error was: '" + str(e) + "'")
        return -1


def checkMetaData(metaData, fileName, libc):
    #Get all the file info
    imgJSON = libc.wholeSVGtoJSON(img)
    imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
    imgJSON = json.loads(imgJSON)

    indexOfShape = getMetaDataFileIndex(metaData, fileName)
    
    #-1 implies the program needs to initialize the metaData
    if (indexOfShape == -1):
        indexOfShape = str(0)
        metaData = {}
        metaData['name'] = 'metaData'   #Name the file
        metaData['data'] = {}
        metaData['data'][indexOfShape] = {}
        metaData['data'][indexOfShape]['filename'] = fileName
        metaData['data'][indexOfShape]['data'] = {}
        metaData['data'][indexOfShape]['data']['rectangles'] = []
        metaData['data'][indexOfShape]['data']['circles'] = []
        metaData['data'][indexOfShape]['data']['deleted'] = []      #The deleted shapes


        #Add the rectangles
        for x in range(len(imgJSON[1])):
            metaData['data'][indexOfShape]['data']['rectangles'].append([]) #This will hold all the edit info

        #Add the circles
        for x in range(len(imgJSON[3])):
            metaData['data'][indexOfShape]['data']['circles'].append([]) #This will hold all the edit info

        
        #Not going to include path data as the user isn't alowed to edit it anyways so it will always stay the same
        return metaData

    
    #---- If here is reached, it's only because meta data was already 'valid' now we are just checking for this file and the shapes it contains-------------
    #Find the index of the file we are using if it is there
    #indexOfShape = 0
    found = False
    for x in range(len(metaData['data'])):
        if metaData['data'][str(x)]['filename'] == fileName:
            indexOfShape = x
            found = True
            break

    indexOfShape = str(indexOfShape)
    
    #If this is a new file, initialize it
    if (found == False):
        indexOfShape = str(len(metaData['data']))
        metaData['data'][indexOfShape] = {}
        metaData['data'][indexOfShape]['filename'] = fileName
        metaData['data'][indexOfShape]['data'] = {}
        metaData['data'][indexOfShape]['data']['rectangles'] = []
        metaData['data'][indexOfShape]['data']['circles'] = []
        metaData['data'][indexOfShape]['data']['deleted'] = []


    if (len(metaData['data'][indexOfShape]['data']['rectangles']) != len(imgJSON[1])):  
        for x in range(len(imgJSON[1])):
            metaData['data'][indexOfShape]['data']['rectangles'].append([]) #This will hold all the edit info

    if (len(metaData['data'][indexOfShape]['data']['circles']) != len(imgJSON[3])):
        for x in range(len(imgJSON[3])):
            metaData['data'][indexOfShape]['data']['circles'].append([]) #This will hold all the edit info

    return metaData


#For when a shape is created
def updateMetaDataCreate(metaData, shape):
    temp = []
    temp.append( {"created": datetime.now().strftime("%d/%m/%Y|%H:%M:%S")} )
    metaData["data"][str(indexOfShape)]['data'][shape + "s"].append(temp)

    
def updateMetaDataEdit(metaData, shape, index, value):
    metaData["data"][str(indexOfShape)]['data'][shape + "s"][index].append({datetime.now().strftime("%d/%m/%Y|%H:%M:%S"): value })
    

def updateMetaDataDelete(metaData, shape, index, data):
    #create the obj
    deletedData = {}
    deletedData["finalShape"] =  data
    deletedData["deleted"] = datetime.now().strftime("%d/%m/%Y|%H:%M:%S")
    
    #append the obj to the metaData
    metaData["data"][str(indexOfShape)]['data']["deleted"].append(deletedData)

    #Take the shape out of the list of shapes
    metaData['data'][str(indexOfShape)]['data'][shape + "s"].pop(index)


#------------------------ Start of program -----------------------
libc = cdll.LoadLibrary("bin/libsvgparse.so")

print('Welcome to the SVG File Editor!\n')

#See if the user wants to create an image or load one from memory
while(True):
    print('Would you like to create or load a SVG file(c/l):')
    answer = input()
    if (answer == 'c' or answer == 'l'):
        break

#Open an existing file
img = ''
ns = ''
if (answer == 'l'):
    while(True):
        print('Please give a filename')
        fileName = input()

        try:
            c_fileName = c_char_p( ( "svgs/" + fileName).encode('utf-8'))
            img = libc.createSVGimage(c_fileName)
            
            answer = libc.validateSVGimage(img, b"xsdFiles/namespace.xsd")
            if checkValidity(answer) == False: 
                print('Invalid/Broken SVG file, try again')
                continue

            answer = libc.validateSVGimage(img, b"xsdFiles/svg.xsd")
            if checkValidity(answer) == False:
                print('Invalid/Broken SVG file, try again')
                continue

            answer = libc.validateSVGimage(img, b"xsdFiles/xlink.xsd")
            if checkValidity(answer) == False:
                print('Invalid/Broken SVG file, try again')
                continue
            #print(c_char_p(foo2).value)    #printing a string that was returned

            print("Opened the image successfully\n")
            break
            

        except:
            print('\n')
            print("Failed to find file, make sure it's located in the correct location or you spelled it correctly")
            continue

    #Print some information about the file
    title = c_char_p( libc.getTitle(img) ).value
    desc = c_char_p( libc.getDesc(img) ).value

    print('Image Title           |' + str(title.decode("utf-8")))       #Needs to be reworked
    print('Image Description     |' + str(desc.decode("utf-8")) + '\n\n')


#create a new file       
else:
    while(True):
        print('Please give a file name you want to use')
        fileName = input()

        if (checkName(fileName)):
            fileName += '.svg'
            c_fileName = c_char_p( ( "svgs/" + fileName).encode('utf-8'))   #Use this later when saving the file

            img = libc.initializeSvgImage()

            while(True):    #Get a namespace
                print('Autogenerate a Namespace?(y/n) *Please not an invalid namespace will result in an invalid file when you try to open it again')
                answer = input()

                if (answer == 'n'):
                    print('Give a Namespace')
                    ns = input()
                    break

                elif (answer == 'y'):
                    ns = 'http://www.w3.org/2000/svg'
                    break

            print('Give the file a title')
            title = input()

            print('Give the file a description')
            desc = input()

            #Format the data for when it's saved
            ns = c_char_p(ns.encode('utf-8'))
            title = c_char_p(title.encode('utf-8'))
            desc = c_char_p(desc.encode('utf-8'))
            break


#Open metaData
try:
    f = open("MetaData.txt", "r+")
except:
    f = open("MetaData.txt", "w+")

all_of_it = f.read()
f.close()


metaData = json.loads(all_of_it or 'null')
metaData = checkMetaData(metaData, fileName, libc)
indexOfShape = getMetaDataFileIndex(metaData, fileName)

imgJSON = ''    
while (True):
    viewImgContents(fileName, libc, imgJSON)   
    
    print('Would you like to edit a shape, create a shape, delete a shape, View the shapes(including paths), search for a shape by an attribute, or save and quit ? (Type: edit, create, delete, view, search, or quit)')
    answer = input()

    if (answer.lower() == 'create'):    #create
        while (True):
            print ('would you like to add a circle, rectangle')

            answer = input()

            if (answer.lower() != 'rectangle' and answer.lower() != 'circle'):
                print('Please reply with "rectangle" or "circle"')
                continue


            elif (answer.lower() == 'rectangle'):
                imgJSON = createRectangle(libc, img, imgJSON, metaData)
                
            else:
                imgJSON = createCircle(libc, img, imgJSON, metaData)

            break


    elif (answer.lower() == 'quit'):    #quit
        if (ns == ''):
            ns = libc.getNameSpace(img)

        if imgJSON == '':
            imgJSON = libc.wholeSVGtoJSON(img)
            imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
            imgJSON = json.loads(imgJSON)
        #print("ns = " + str(c_char_p(ns).value))
        #print("title = " + str(title))
        #print("desc = " + str(desc))

        #print("THE IMAGE BEFORE = " + str(imgJSON) + "\n\n")

        stringImgJSON = str(imgJSON)
        stringImgJSON = c_char_p(stringImgJSON.encode('utf-8'))

        img = libc.wholeJSONtoSVG(img, stringImgJSON, ns, (title), (desc))

        returnedVal = libc.writeSVGimage(img, c_fileName)

        #Write the updates to metaData
        f = open("MetaData.txt", "w")
        f.write(json.dumps(metaData))
        f.close()
        #Done

        if (returnedVal == 1):
            print('Saved svg file')
            libc.deleteSVGimage(img)
            quit()
            
        else:
            print('Failed to save the svg correctly')
        break


    elif (answer.lower() == 'edit'):    #EDIT
        while (True):
            print ('would you like to edit a circle, rectangle')
            answer = input()

            if (answer.lower() != 'rectangle' and answer.lower() != 'circle'):
                print('Please reply with "rectangle" or "circle"')
                continue


            elif (answer.lower() == 'rectangle'):
                imgJSON = editRectangle(libc, imgJSON, metaData)

            else:
                imgJSON = editCircle(libc, imgJSON, metaData)
                
            break


    elif (answer.lower() == 'delete'):  #Delete a shape
        imgJSON = deleteShape(imgJSON)


    elif (answer.lower() == 'view'):    #User wants to view the shapes
        while (True):
            print ('would you like to view the circles, rectangles, or paths (c/r/p) ')
            answer = input()

            if (answer.lower() != 'r' and answer.lower() != 'c' and answer.lower() != 'p'):
                print('Please reply with "c" or "p" or "r"')
                continue

            elif (answer.lower() == 'r'):
                imgJSON = viewRectangles(libc, img, imgJSON)

            elif (answer.lower() == 'c'):
                imgJSON = viewCircles(libc, img, imgJSON)

            else:
                viewPaths(libc, img)
                
            break

    elif (answer.lower() == 'search'):    #SEARCHING
        print('What attribute do you want to find (fill(background colour), radius, x value, stroke)')
        atty = input()

        print('What value asscoiated with that attribute')
        value = input()

        searchElements(atty, value, libc, imgJSON)

    else:
        print('Invalid, try again\n')

    



