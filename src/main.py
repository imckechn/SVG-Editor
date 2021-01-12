from ctypes import *
import json

AllRects = ''
AllCircles = ''
img = ''

def viewImgContents(name, libc):
    
    rects = libc.getAllRects(img)
    circles = libc.getAllCircles(img)
    paths = libc.getAllPaths(img)
    groups = libc.getAllGroups(img)

    print('Name                  |' + name)
    print('Number of Rectangles  |' + str(rects))
    print('Number of Circles     |' + str(circles))
    print('Number of Paths       |' + str(paths))
    print('Number of Groups      |' + str(groups))
    print('\n')


def createRectangle(libc, img):
    print('\n')
    print('For the rectangle please give')
    
    
    x = ''
    while (True): 
        print('An X value(an int)')
        x = int(input())

        if (x > 0):
            break

    
    
    y = ''
    while (True): 
        print('An Y value(an int)')
        y = int(input())

        if ( y > 0 ):
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
        print("What units (mm, cm, px)")
        units = input()

        if ( units.lower() == "mm" or units.lower() == 'cm' or units.lower() == 'px' ):
            break
        print('units = ' + units)


    fill = ''
    print("What fill colour? (red, back... OR Hex value:#888888")
    colour = input()

    stroke = ''
    strokeWidth = ''
    
    
    while (True): 
        print('Add stroke? (y/n)')
        yesNo = input()

        if (yesNo.lower() == 'n'):
            break

        elif (yesNo.lower() == 'y'):
            print('Stroke colour: (red, back... OR Hex value:#888888')
            stroke = input()

            print('stroke width(px):')
            strokeWidth = input()
            break    

    img = libc.addRectanleToImg(img, c_float(x), c_float(y), c_float(width), c_float(height), c_char_p(units.encode('utf-8')), c_char_p(colour.encode('utf-8')), c_char_p(stroke.encode('utf-8')), c_char_p(strokeWidth.encode('utf-8')))
    print('Rectangle complete')

    return img


def createCircle(libc, img):
    print('\n')
    print('For the circle please give')
    
    
    x = ''
    while (True): 
        print('An X value(an int)')
        x = int(input())

        if (x > 0):
            break

    
    
    y = ''
    while (True): 
        print('An Y value(an int)')
        y = int(input())

        if ( y > 0 ):
            break
        

    
    width = ''
    while (True): 
        print('A radius value(an int)')
        r = int(input())

        if ( r > 0 ):
            break

    
    units = ''
    while (True): 
        print("What units (mm, cm, px)")
        units = input()

        if ( units.lower() == "mm" or units.lower() == 'cm' or units.lower() == 'px' ):
            break
        print('units = ' + units)


    fill = ''
    print("What fill colour? (red, back... OR Hex value:#888888")
    colour = input()

    stroke = ''
    strokeWidth = ''
    
    
    while (True): 
        print('Add stroke? (y/n)')
        yesNo = input()

        if (yesNo.lower() == 'n'):
            break

        elif (yesNo.lower() == 'y'):
            print('Stroke colour: (red, back... OR Hex value:#888888')
            stroke = input()

            print('stroke width(px):')
            strokeWidth = input()
            break

        


    img = libc.addCircleToImg(img, c_float(x), c_float(y), c_float(r), c_char_p(units.encode('utf-8')), c_char_p(colour.encode('utf-8')), c_char_p(stroke.encode('utf-8')), c_char_p(strokeWidth.encode('utf-8')))
    print('Circle complete')

    return img

    
def editRectangle(libc, imgJSON):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)

    #List all the rectangles
    counter = 0
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

    #Make the edit(s)
    if (x != 'n'):
        imgJSON[1][int(answer)]['x'] = float(x)

    if (y != 'n'):
        imgJSON[1][int(answer)]['y'] = float(y)

    if (width != 'n'):
        imgJSON[1][int(answer)]['w'] = float(width)

    if (height != 'n'):
        imgJSON[1][int(answer)]['h'] = float(height)

    if (units != 'n'):
        imgJSON[1][int(answer)]['units'] = units

    print('Edits Commplete\n')      

    return imgJSON
    
    
def editCircle(libc, imgJSON):
    if imgJSON == '':
        imgJSON = libc.wholeSVGtoJSON(img)
        imgJSON = (c_char_p(imgJSON).value.decode("utf-8"))
        imgJSON = json.loads(imgJSON)
       
    counter = 0
    for circle in imgJSON[3]:
        print('Circle           |' + str(counter))
        print('x                |' + str(circle['cx']))
        print('y                |' + str(circle['cy']))
        print('r                |' + str(circle['r']))   
        print('units            |' + str(circle['units']))
        print('# of attributes  |' + str(circle['numAttr']))
        print('--------------------------------------------\n')
        counter += 1

    #Get and check the user input
    while(True):
        print('What circle would you like to edit')
        answer = input()

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

    #Make the edit(s)
    if (x != 'n'):
        imgJSON[3][int(answer)]['cx'] = float(x)

    if (y != 'n'):
        imgJSON[3][int(answer)]['cy'] = float(y)

    if (r != 'n'):
        imgJSON[3][int(answer)]['r'] = float(r)

    if (units != 'n'):
        imgJSON[3][int(answer)]['units'] = units
        print('update = ' + imgJSON[3][int(answer)]['units'])
    
    print('imgJSON = ' + str(imgJSON))

    print('Edits Commplete\n')      

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
            print('inside')
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
        imgJSON[3].pop(int(number))
    
    else: 
        imgJSON[1].pop(int(number))
    
    print('Deleted')
    return imgJSON

    
def checkValidity(answer):
    if int(answer) == 0: #It's invalid
        print("Sorry, this is not a valid SVG image, please try again with a different image")
        
    return answer


def viewPaths(libc, img):
    #get the paths
    paths = libc.getPathsToJSON(img)
    paths = (c_char_p(paths).value.decode("utf-8"))
    paths = json.loads(paths)
    print('Paths = ' + str(paths))
    print('Paths[0] = ' + str(paths[0]))
    
    counter = 0
    for path in paths:
        print('Singel path = ' + str(path))
        print('Path: ' + str(counter))
        print('Data: ' + path['d'])
        print('# of attributes:' + str(path['numAttr']))
        print('--------------------------------------------\n')
        counter += 1


def checkName(name):
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
if (answer == 'l'):
    while(True):
        print('Please give a filename')
        fileName = input()

        try:
            c_fileName = c_char_p(fileName.encode('utf-8'))
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

    print('Image Title           |' + str(title))       #Needs to be reworked
    print('Image Description     |' + str(desc) + '\n\n')


#create a new file       
else:
    while(True):
        print('Please give a file name you want to use')
        fileName = input()

        if (checkName(fileName)):
            fileName += '.svg'

            img = libc.initializeSvgImage()

            ns = ''
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
            break

imgJSON = ''    
while (True):
    viewImgContents(fileName, libc)   
    
    print('Would you like to edit a shape, create a shape, delete a shape, View the paths, or save and quit? (edit, create, delete, view, quit)')
    answer = input()

    if (answer.lower() == 'create'):    #create
        while (True):
            print ('would you like to add a circle, rectangle')

            answer = input()

            if (answer.lower() != 'rectangle' and answer.lower() != 'circle'):
                print('Please reply with "rectangle" or "circle"')
                continue


            elif (answer.lower() == 'rectangle'):
                img = createRectangle(libc, img)
                
            else:
                createCircle(libc, img)

            break


    elif (answer.lower() == 'quit'):    #quit
        if (ns == ''):
            ns = libc.getNameSpace(img)

        img = wholeJSONtoSVG(c_char_p(imgJSON), ns, title, desc)

        returnedVal = libc.writeSVGimage(img, c_fileName)

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
                imgJSON = editRectangle(libc, imgJSON)

            else:
                imgJSON = editCircle(libc, imgJSON)
                
            break


    elif (answer.lower() == 'delete'):  #Delete a shape
        imgJSON = deleteShape(imgJSON)


    elif (answer.lower() == 'view'):    #User wants to view all the paths
        viewPaths(libc, img)

    else:
        print('Invalid, try again\n')

    



