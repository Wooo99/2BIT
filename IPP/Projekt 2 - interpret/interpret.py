import sys
import re
import xml.etree.ElementTree as ET
import os.path
from os import path

Firstarg = True
Secondarg = True
Thirdarg = True
g=0
inputfilename = None
sourcefile = None

i = 0
first = True
skipings = False
skipingorder = None
instructionslist = []
orderstatus = 0
labels = {}
callstack = []
datastack = []
##DATA FOR OPERATIONS WITH STACK
class Data:
    def __init__(self,datatype,value):
        self.datatype = datatype
        self.value = value
##FRAMES OPERATIONS 
class Frames:
    def __init__(self):
        self.exist = False
        self.framestack = []
        self.TF = {}
        self.GF = {}
    def CREATEFRAME(self):
        self.TF = {}
        self.exist = True
    def PUSHFRAME(self):
        if self.exist != True:
            sys.stderr.write("Undefined frame\n")
            exit(55)
        else:
            self.framestack.append(self.TF)
    def POPFRAME(self):
        
        if len(self.framestack) <= 0:
            sys.stderr.write("Empty frame\n")
            exit(55)
        else:
            self.TF = self.framestack.pop()
    def DEFVAR(self,arg):
        frame, name = arg.value.split('@',1)
        myframe = self.FRAME(frame)
        if myframe == "Falsee":
            sys.stderr.write("Undefined frame\n")
            exit(55)
        if name in myframe:
            sys.stderr.write("Already defined variable\n")
            exit(58)
        myframe[name]={'value':None, 'type':None}
    def FRAME(self,frame):
        if frame == 'LF':
            if len(self.framestack) > 0:
                return self.framestack[len(self.framestack) - 1]
            else:
                return "Falsee"
        elif frame == 'GF':
            return self.GF
        elif frame == 'TF':
            if self.exist == False:
                return "Falsee"
            else:
                return self.TF
        else:
            sys.stderr.write("Unexpected XML structure\n")
            exit(32)
    #### PARSING ARGUMENTS          
    def TYPE_VALUE_VAR(self,arg):
        if arg.type not in ['string','label','int', 'bool', 'type']:
            frame, name = arg.value.split('@',1)
            myframe = self.FRAME(frame)
            if myframe == False:
                sys.stderr.write("Undefined frame\n")
            if name not in myframe:
                sys.stderr.write("Undefined variable\n")
            finishvalue = myframe[name]['value']
            finishtype = myframe[name]['type']
            return (finishtype,finishvalue, True)
        else:
            return(arg.type, arg.value,False)
    ### SETING VARIABLES
    def SETUP(self,var,type,value):
        frame, name = var.value.split('@',1)
        myframe = self.FRAME(frame)
        if myframe == "Falsee":
                sys.stderr.write("Undefined frame\n")
                exit(55)
        if name not in myframe:
                sys.stderr.write("Undefined variable\n")
                exit(54)
        myframe[name]['type'] = type
        myframe[name]['value'] = value
    ####ARGS FOR INSTRUCTIONS
class arg:
    def __init__(self,arg_type,value):
        self.type = arg_type
        self.value = value
    
class instruction:
    def __init__(self,name,number):
        self.name = name
        self.number = number
        self.args = []
    def add_argumnent(self,arg_type,value):
        self.args.append(arg(arg_type,value))

##CEHCK ARGUMENTS
if len(sys.argv) > 3 or len(sys.argv) < 2:
    sys.stderr.write("Wrong number of arguments, type --help for usage options\n")
    exit(10)
if len(sys.argv) == 2 and sys.argv[1] == "--help":
    sys.stdout.write("\nUSAGE: \n\tpython3 interpret.py --sorce=sourcefile --input=inputfile\n\n")
    exit(10)
if len(sys.argv) == 2:
    if re.match(r'^--source=',sys.argv[1]):
        sourcefile = sys.argv[1][9:]
    elif re.match(r'^--input=',sys.argv[1]):
        inputfilename = sys.argv[1][8:]
    else:
        sys.stderr.write("Error in arguments, type --help for usage options\n")
        exit(10)
elif len(sys.argv) == 3:
    if re.match(r'^--source=',sys.argv[1]):
        if re.match(r'^--input=',sys.argv[2]):
            inputfilename = sys.argv[2][8:]
            sourcefile = sys.argv[1][9:]
        else:
            sys.stderr.write("Error in arguments, type --help for usage options\n")
            exit(10)
        
    elif re.match(r'^--input=',sys.argv[1]):
        if re.match(r'^--source=',sys.argv[2]):
            inputfilename = sys.argv[1][8:]
            sourcefile = sys.argv[2][9:]
        else:
            sys.stderr.write("Error in arguments, type --help for usage options\n")
            exit(10)
    else:
        sys.stderr.write("Error in arguments, type --help for usage options\n")
        exit(10)
else:
    sys.stderr.write("Error in arguments, type --help for usage options\n")
    exit(10)
####CHOOSING BETWEEN STDIN OR FILE INPUT
if sourcefile != None:
    if not (path.exists(sourcefile)):
        sys.stderr.write("Source file does not exist\n")
        exit(11)
    tree = ET.parse(sourcefile)
else:
    tree = ET.parse(sys.stdin)

if inputfilename != None:
    if not (path.exists(inputfilename)):
        sys.stderr.write("Input file does not exist\n")
        exit(11)
    else:
        inputfilemy = open(inputfilename,"r")

####BASIC XML PARSING
root = tree.getroot()

if root.tag != "program":
    sys.stderr.write("Unexpected XML structure\n")
    exit(32)
if root.attrib["language"] != "IPPcode21":
    sys.stderr.write("1Unexpected XML structure\n")
    exit(32)
for child in root:
    if child.tag != "instruction":
        sys.stderr.write("Unexpected XML structure\n")
        exit(32)
    oo = list(child.attrib.keys())
    if not ("order" in oo) or not ("opcode" in oo):
        sys.stderr.write("Unexpected XML structure\n")
        exit(32)
    g=0
    for subelem in child:
        g+=1
        if not(re.match(r"arg[123]",subelem.tag)):
            sys.stderr.write("Unexpected XML structure\n")
            exit(32)
        if subelem.tag == "arg1" and g==1:
            continue
        elif subelem.tag == "arg2" and g==2:
            continue
        elif subelem.tag == "arg3" and g==3:
            continue
        else:
            sys.stderr.write("Unexpected XML structure\n")
            exit(32)
for e in root:
    instructionslist.append(instruction(e.attrib['opcode'],e.attrib['order']))
    for c in e:
        instructionslist[i].add_argumnent(c.attrib['type'], c.text)
    i += 1

temporder = 0
############INTERPRETING######################
frames = Frames()
##SAVES LABELS TO THE LIST, FOR CHECKING
for line in instructionslist:
    if line.name == "LABEL":
        if len(line.args) != 1:
            sys.stderr.write("Wrong number of arguments at instruction: "+line.name+"\n")
            sys.stderr.write("At order: "+line.number+"\n")
            exit(32)
        if line.args[0].type != "label":
            sys.stderr.write("Operand type error \n")
            exit(53)
        for i in labels:
            if line.args[0].value in labels:
                sys.stderr.write("Order error\n")
                exit(56)
            else:
                continue
        labels[line.args[0].value] = line
# MAIN LOOP FOR INTEPRETING INSTRUCTION BY INSTRUCTION
while(skipings == True or first == True):
    orderstatus = 0
    first = False
    for instruct in instructionslist:
        ###CHECK IF WE DONT HAVE TO SKIP WHILE JUMPING TO LABEL
        if skipings == True:
            if int(instruct.number) > skipingorder:
                break
            elif int(instruct.number)!= skipingorder:
                skipings = False
                continue
        ## CONTROL ORDER VALUES
        if int(instruct.number) <= orderstatus:
            sys.stderr.write("Order error\n")
            exit(56)
        else:
            orderstatus = int(instruct.number)
        #### START CHECKING INSTRUCTION NAMES
        if instruct.name == "MOVE":
            if len(instruct.args) != 2:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[1])
            frames.SETUP(instruct.args[0],type,value)
        elif instruct.name == "CREATEFRAME":
            if len(instruct.args) != 0:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            frames.CREATEFRAME()
        elif instruct.name == "PUSHFRAME":
            if len(instruct.args) != 0:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            frames.PUSHFRAME()
        elif instruct.name == "POPFRAME":
            if len(instruct.args) != 0:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            frames.POPFRAME()
        elif instruct.name == "DEFVAR":
            if len(instruct.args) != 1:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            frames.DEFVAR(instruct.args[0])
        elif instruct.name == "WRITE":
            if len(instruct.args) != 1:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[0])
            if value == None:
                sys.stderr.write("Missing value\n")
                exit(56)
            else:
                print(value)
        elif instruct.name == "DPRINT":
            if len(instruct.args) != 1:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[0])
            if value == None:
                sys.stderr.write("Missing value\n")
                exit(56)
            else:
                sys.stderr.write(value)
                    
        elif instruct.name in ["ADD", "SUB", "MULL", "DIV"]:
            if len(instruct.args) != 3:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type0,value0,var0 = frames.TYPE_VALUE_VAR(instruct.args[1])
            type1,value1,var1 = frames.TYPE_VALUE_VAR(instruct.args[2])
            if type0 != "int" or type1 != "int":
                sys.stderr.write("Wrong type of operands\n")
                exit(53)
            else:
                if instruct.name == "ADD":
                    frames.SETUP(instruct.args[0],"int",str(int(value0)+int(value1)))
                elif instruct.name == "SUB":
                    frames.SETUP(instruct.args[0],"int",str(int(value0)-int(value1)))
                elif instruct.name == "IDIV":
                    if(int(value1)==0):
                        sys.stderr.write("Dividing by zero\n")
                        exit(57)
                    frames.SETUP(instruct.args[0],"int",str(int(value0)//int(value1)))
                else:
                    frames.SETUP(instruct.args[0],"int",str(int(value0)*int(value1)))
        elif instruct.name == "TYPE":
            if len(instruct.args) != 2:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[1])
            if type is None:
                type  = ""
            frames.SETUP(instruct.arg[0],"string", type)
        elif instruct.name == "AND":
            if len(instruct.args) != 3:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type0,value0,var0 = frames.TYPE_VALUE_VAR(instruct.args[1])
            type1,value1,var1 = frames.TYPE_VALUE_VAR(instruct.args[2])
            if type0 != "bool" or type1 != "bool":
                sys.stderr.write("Wrong type of operands\n")
                exit(53)
            if value0 == True and value1 == True:
                frames.SETUP(instruct.arg[0], "bool", "True")
            else:
                frames.SETUP(instruct.arg[0], "bool", "False")
        elif instruct.name == "OR":
            if len(instruct.args) != 3:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type0,value0,var0 = frames.TYPE_VALUE_VAR(instruct.args[1])
            type1,value1,var1 = frames.TYPE_VALUE_VAR(instruct.args[2])
            if type0 != "bool" or type1 != "bool":
                sys.stderr.write("Wrong type of operands\n")
                exit(53)
            if value0 == True or value1 == True:
                frames.SETUP(instruct.arg[0], "bool", "True")
            else:
                frames.SETUP(instruct.arg[0], "bool", "False")
        elif instruct.name == "CONCAT":
            if len(instruct.args) != 3:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type0,value0,var0 = frames.TYPE_VALUE_VAR(instruct.args[1])
            type1,value1,var1 = frames.TYPE_VALUE_VAR(instruct.args[2])
            if type0 != "string" or type1 != "string":
                sys.stderr.write("Wrong type of operands\n")
                exit(53)
            else:
                frames.SETUP(instruct.args[0],"string",value0+value1)
        elif instruct.name == "NOT":
            if len(instruct.args) != 2:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[1])
            if type != "bool":
                sys.stderr.write("Wrong type of operands\n")
                exit(53)
            else:
                if value == "True":
                    frames.SETUP(instruct.args[0],"bool","False")
                elif value == "False":
                    frames.SETUP(instruct.args[0],"bool","True")
        elif instruct.name == "LABEL":
            continue
        elif instruct.name == "JUMP":
            if len(instruct.args) != 1:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            found = False
            for i in labels:
                if instruct.args[0].value in labels:
                    found = True
                    break
                else:
                    continue
            if found == False:
                    sys.stderr.write("Label was not found\n")
                    exit(52)
            else: 
                label = instruct.args[0].value
                skipings = True
                skipingorder = int(labels[label].number)
                instruct = instructionslist[0]
                continue
        elif instruct.name in ["JUMPIFEQ", "JUMPIFNEQ"]:
            if len(instruct.args) != 3:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type0,value0,var0 = frames.TYPE_VALUE_VAR(instruct.args[1])
            type1,value1,var1 = frames.TYPE_VALUE_VAR(instruct.args[2])
            if type0 != type1:
                sys.stderr.write("Operand types doesn't match \n")
                exit(53)
            else:
                if instruct.name == "JUMPIFEQ":
                    if value0 == value1:
                        found = False
                        for i in labels:
                            if instruct.args[0].value in labels:
                                found = True
                                break
                            else:
                                continue
                        if found == False:
                                sys.stderr.write("Label was not found\n")
                                exit(52)
                        else:
                            label = instruct.args[0].value
                            skipings = True
                            skipingorder = int(labels[label].number)
                            instruct = instructionslist[0]
                            continue
                    else:
                        continue
                else:
                    if value0 != value1:
                        found = False
                        for i in labels:
                            if instruct.args[0].value in labels:
                                found = True
                                break
                            else:
                                continue
                        if found == False: 
                            sys.stderr.write("Label was not found\n")
                            exit(52)
                        else:
                            label = instruct.args[0].value
                            skipings = True
                            skipingorder = int(labels[label].number)
                            instruct = instructionslist[0]
                            continue
                    else:
                        continue
        elif instruct.name == "CALL":
            if len(instruct.args) != 1:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            callposition = int(instruct.number)
            callstack.append(callposition)
            found = False
            for i in labels:
                if instruct.args[0].value in labels:
                    found = True
                    break
                else:
                    continue
            if found == False:
                    sys.stderr.write("Label was not found\n")
                    exit(52)
            else:
                label = instruct.args[0].value
                skipings = True
                skipingorder = int(labels[label].number)
                continue
        elif instruct.name == "RETURN":
            if len(instruct.args) != 0:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            if callstack == []:
                sys.stderr.write("Call Stack is empty\n")
                exit(56)
            else:
                instruct = instructionslist[0]
                skipings = True
                skipingorder = callstack.pop()
        elif instruct.name in ["LT","GT","EQ"]:
            if len(instruct.args) != 3:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type0,value0,var0 = frames.TYPE_VALUE_VAR(instruct.args[1])
            type1,value1,var1 = frames.TYPE_VALUE_VAR(instruct.args[2])
            if type0 != type1:
                sys.stderr.write("Operand types doesn't match \n")
                exit(53)
            else:
                if instruct.name == "EQ":
                    if value0 == value1:
                        frames.SETUP(instruct.args[0],"bool", "True")
                    else:
                        frames.SETUP(instruct.args[0],"bool", "False")
                elif instruct.name == "LT":
                    if type0 == "int":
                        if int(value0) < int(value1):
                            frames.SETUP(instruct.args[0],"bool", "True")
                        else:
                            frames.SETUP(instruct.args[0],"bool", "False")
                    elif type0 == "string":
                        if value0 < value1:
                            frames.SETUP(instruct.args[0],"bool", "True")
                        else:
                            frames.SETUP(instruct.args[0],"bool", "False")
                    else:
                        if value0 == "False" and value1 == "True":
                            frames.SETUP(instruct.args[0],"bool", "True")
                        else:
                            frames.SETUP(instruct.args[0],"bool", "False")
                else:
                    if type0 == "int":
                        if int(value0) > int(value1):
                            frames.SETUP(instruct.args[0],"bool", "True")
                        else:
                            frames.SETUP(instruct.args[0],"bool", "False")
                    elif type0 == "string":
                        if value0 > value1:
                            frames.SETUP(instruct.args[0],"bool", "True")
                        else:
                            frames.SETUP(instruct.args[0],"bool", "False")
                    else:
                        if value0 == "True" and value1 == "False":
                            frames.SETUP(instruct.args[0],"bool", "True")
                        else:
                            frames.SETUP(instruct.args[0],"bool", "False")
        elif instruct.name == "STRLEN":
            if len(instruct.args) != 2:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[1])
            if type != "string":
                sys.stderr.write("Operand types error \n")
                exit(53)
            else:
                frames.SETUP(instruct.args[0], "int", len(value))
        elif instruct.name == "GETCHAR":
            if len(instruct.args) != 3:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type0,value0,var0 = frames.TYPE_VALUE_VAR(instruct.args[1])
            type1,value1,var1 = frames.TYPE_VALUE_VAR(instruct.args[2])
            if type0 != "string" and type1 != "int":
                sys.stderr.write("Operand types error \n")
                exit(53)
            else:
                char = int(value1)
                if char < 0 and char >len(value1)-1:
                    sys.stderr.write("Index out of string \n")
                    exit(58)
                else:
                    frames.SETUP(instruct.args[0], "string", value0[char])
        elif instruct.name == "SETCHAR":
            if len(instruct.args) != 3:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type0,value0,var0 = frames.TYPE_VALUE_VAR(instruct.args[0])
            type1,value1,var1 = frames.TYPE_VALUE_VAR(instruct.args[1])
            type2,value2,var2 = frames.TYPE_VALUE_VAR(instruct.args[2])
            if type0 != "string" or type1 != "int" or type2 != "string":
                sys.stderr.write("Operand types error \n")
                exit(53)
            else:
                char = int(value1)
                if value2 ==  "":
                    sys.stderr.write("String can't be empty \n")
                    exit(58)
                elif char < 0 and char >len(value1)-1:
                    sys.stderr.write("Index out of string \n")
                    exit(58)
                else:
                    value0 = list(value0)
                    value0[char] =value2[0]
                    value0 = "".join(value0)
                    frames.SETUP(instruct.args[0], "string", value0)
        elif instruct.name == "STRI2INT":
            if len(instruct.args) != 3:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type0,value0,var0 = frames.TYPE_VALUE_VAR(instruct.args[1])
            type1,value1,var1 = frames.TYPE_VALUE_VAR(instruct.args[2])
            if type0 != "string" or type1 != "int":
                sys.stderr.write("Operand types error \n")
                exit(53)
            else:
                char = int(value1)
                if char < 0 and char >len(value1)-1:
                    sys.stderr.write("Index out of string \n")
                    exit(58)
                else:
                    ordinal = ord(value0[char])
                    frames.SETUP(instruct.args[0],"int",ordinal)
        elif instruct.name == "INT2CHAR":
            if len(instruct.args) != 2:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[1])
            if type != "int":
                sys.stderr.write("Operand types error \n")
                exit(53)
            else:
                try:
                    char = chr(int(value))
                except:
                    sys.stderr.write("Not valid value \n")
                    exit(58)
        elif instruct.name == "READ":
            if len(instruct.args) != 2:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[1])
            try:
                if(inputfilename == None):
                    inputstring = input()
                else:
                    inputstring = inputfilemy.readline()
            except:
                inputstring = ""
            if value == "bool":
                if inputstring.lower() == "True":
                    frames.SETUP(instruct.args[0],"bool","False")
                else:
                    frames.SETUP(instruct.args[0],"bool","True")
                
            elif value == "int":
                try:
                    myint = int(inputstring)
                except:
                    myint = 0
                frames.SETUP(instruct.args[0],"int",myint)
            else:
                try:
                    mystring = str(inputstring)
                except:
                    mystring = ""
                frames.SETUP(instruct.args[0],"string",mystring)
        elif instruct.name == "EXIT":
            if len(instruct.args) != 1:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[1])
            if type != "int":
                sys.stderr.write("Operand types error \n")
                exit(53)
            elif value < 0 or value > 49:
                sys.stderr.write("Exit value error\n")
                exit(57)
            else:
                exit(1)
        elif instruct.name == "BREAK":
            if len(instruct.args) != 0:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            sys.stderr.write("Program ends at :",instruct.number)
        elif instruct.name == "PUSHS":
            if len(instruct.args) != 1:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")           
                exit(32)
            type,value,var = frames.TYPE_VALUE_VAR(instruct.args[0])
            datastack.append(Data(type,value))
        elif instruct.name == "POPS":
            if len(instruct.args) != 1:
                sys.stderr.write("Wrong number of arguments at instruction: "+instruct.name+"\n")
                sys.stderr.write("At order: "+instruct.number+"\n")
                exit(32)
            var = datastack.pop()
            type = var.datatype
            value = var.value
            frames.SETUP(instruct.args[0],type,value)
        else:
            ### NAME WE DONT KNOW
            sys.stderr.write("Unknown instruction name\n")
            exit(32)
exit(1)
    