#!/usr/bin/python

import sys
import re



def writeInt(lastLine, content, outputFile):
    defineVariable.append("%02d SET %04d\n" % (lastLine, int(content)))
    lastLine += 1
    return lastLine

def writeLineToFile(memoryLoc, value, outputFile):
    lineText = "%02d SET %04d\n" % (memoryLoc, value)
    outputFile.write(lineText)
    return

def writeString(memoryLoc, content, outputFile):
    content = content.replace("#newline", "\n")
    format = "%02d SET %s\n"
    index = 1
    value = ""
    for char in content:
        value += ("%02d" % ord(char))
        if index % 2 == 0:
            defineVariable.append(format % (memoryLoc, value))
            index = 0
            value = ""
            memoryLoc += 1

        index += 1
    if len(content) % 2 == 0:
        defineVariable.append(format % (memoryLoc, "0000"))
        memoryLoc += 1
    else:
        value += "00"
        defineVariable.append(format % (memoryLoc, value))


    return memoryLoc


if __name__ == "__main__":
    numFormat = r"(?P<whole>\{\s(?P<variable>\w+)(\s[=]\s\@(?P<type>\w+)\((?P<content>.+)\))?\s\})"
    giveValue = r"([=]\s@(?P<type>\w+)\((?P<content>.*)\)\s\})"
    curly = r"\{"


    fileName = sys.argv[1]
    inputFile = open(fileName, "r")
    outputFile = open(fileName + ".txt", "w")
    variableDict = {}

    lines = [line for line in inputFile]

    defineVariable = []

    memoryLoc = 0

    lastLine = len(lines)

    print lastLine
    for line in lines:
        match = re.search(numFormat, line)

        if(match):
            variable = match.group('variable')
            typeOfVariable = match.group('type');
            content = match.group('content')
            whole = match.group('whole')

            print "Found on line %s, %s, and %s" % (variable, typeOfVariable, content)

            if( typeOfVariable == None ):
                if( variableDict.has_key(variable) ):
                    locationOfVariable = variableDict[variable]
                    print "found variable %s" % variable
                else:
                    print "did not find variable %s" % variable
                    locationOfVariable = lastLine
                    variableDict[variable] = locationOfVariable;
                    writeInt(locationOfVariable, "0000", outputFile )
                    lastLine += 1
                outputFile.write(line.replace("{ " + variable + " }", str(locationOfVariable)))
            else:
                variableDict[variable] = lastLine;

                if(typeOfVariable == "string"):
                    lastLine = writeString(lastLine, content, outputFile)

                if(typeOfVariable == "num"):
                    lastLine = writeInt(lastLine, content, outputFile)

                text = line.replace(whole, "%02d" % variableDict[variable])

                outputFile.write(text)

        else:
            outputFile.write(line)

    outputFile.writelines(defineVariable)
    print "what"


