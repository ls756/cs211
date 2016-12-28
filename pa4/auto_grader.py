#! /usr/bin/python

import os, sys, glob, time, subprocess, signal
import popen2

#subdirectories = ['first', 'second', 'third', 'fourth', 'fifth']
subdirectories = ['comb']
#formats = {'first':'line', 'second':'line', 'third':'file', 'fourth':'file', 'fifth':'file'}# if a program has single liner input and output, we put all test cases in single file. Otherwise, we have a file for test and associated file with results
formats = {'comb':'file'}

testcases = "../../testcases/"
testgrades = {'comb':[0, 11, 11, 11, 11, 11 , 11 , 11 , 11, 11]} #it assumes the test files are properly sorted
return_content = ""
return_total_grade = 0

class ExperimentError(Exception):
    def __init__(self, command, output):
        self.command = command
        limit = 10000
        if(len(output) > limit):
          self.output = output[:limit/2] + "\n\n...TRUNCATED...\n\n" + output[-limit/2:]
        else:
          self.output = output
    def __str__(self):
        return "ExperimentError:" + `self.command`

def run_command(command_string, input_string="", max_lines=0, verbose=False, echo=True, throw_exception=True, return_valgrind_output=False, user_program=False):
    if echo:
        print "executing:", command_string
    obj = popen2.Popen4(command_string)
    output = ""
    valgrind_output = ""

    obj.tochild.write(input_string)
    obj.tochild.close()
    valgrind_prefix = "==%d==" % obj.pid
    maxSleep = 20
    if user_program: #program may have an infinite loop        
        while maxSleep>0:
            time.sleep(0.25)
            maxSleep-=1            
            if obj.poll()!=-1:
                break
        if maxSleep==0 and obj.poll()==-1:
            os.kill(obj.pid, signal.SIGKILL)
            print command_string, " taking longer than expected. Killed."
            return ""
        
    line = obj.fromchild.readline()
    while (line):
        if verbose == 1:
            print line,
        if line.startswith(valgrind_prefix):
            valgrind_output += line
        output += line
        line = obj.fromchild.readline()
    exit_status = obj.wait()

    if(max_lines != 0):
        lines = output.split("\n");
        output = string.join(lines[-max_lines:], "\n")

    if throw_exception and exit_status != 0:
        #raise ExperimentError(command_string, output)
        print "Error ", command_string, output

    if return_valgrind_output:
        return valgrind_output
    else:
        return output

def compare_string_file(ref_file, test_string, show_difference=False):
    global return_content

    test_list=test_string.split("\n")    
    fd = open(ref_file)
    i=0
    flag=True
    for line in fd:        
        if i<len(test_list):
            if line.strip()!=test_list[i].strip():            
                flag=False
                if show_difference:
                    print "Line mismatch. Expecting %s but found %s"%(line.strip(), test_list[i].strip())
                    return_content += "Line mismatch. Expecting %s but found %s\n"%(line.strip(), test_list[i].strip())
        elif len(line.strip())>0:
            print "Output missing: ", line.strip()
            return_content += "Output missing: %s.\n"%(line.strip())
            flag=False
            break
        i+=1
    
    fd.close()
    while(i<len(test_list)):
        if len(test_list[i].strip())==0:
            i+=1
            continue
        print "Extra output: ", test_list[i]
        return_content += "Extra output found %s.\n"%(test_list[i].strip())
        i+=1
        flag=False
        break
    return flag

def compare_string(ref, test):
    ref = ref.strip()
    test = test.strip()

    if(ref==test):
        return True
    if(ref==test.lower()):
        print "%s and %s are in different case. Please print your output in correct case."%(ref, test)
    return False


def make_executable(dirname, useMakefile=True):
    if useMakefile and (os.path.isfile('Makefile') or os.path.isfile('makefile')):
       run_command("make -s clean", verbose=False)
       run_command("make -s", verbose=True)
    elif useMakefile==False:
       #print "No Makefile found in", dirname
       #print "Please submit a Makefile to receive full grade."
       run_command("g++ -o %s *.c"%(dirname), verbose=False)
    #run_command("make -s clean", verbose=False)
    #run_command("gcc -o %s *.c *.h"%(dirname), verbose=False)

def allfiles():
    flag = [0, 0, 0] #.c, .h and Makefile
    count = 0
    for filename in os.listdir("."):
        if filename.endswith(".c"):
            flag[0] = 1
        elif filename.endswith(".h"):
            flag[1] = 1
        elif filename=="makefile" or filename=="Makefile":
            flag[2] = 1
    return sum(flag)        

def file_grade(dirname):
    global return_content
    
    print "Grading", dirname
    return_content += "Grading %s\n"%(dirname)

    prevdir = os.getcwd()
    os.chdir(dirname)
    total = 0
    files = allfiles()
    penalty = 1
    if dirname=='third':
        max_grade = 20
    else:
        max_grade = 99
    
    os.system("rm -f %s"%(dirname))
    make_executable(dirname)
    if not os.path.isfile(dirname):
        print "Executable %s missing after makefile."%(dirname)
        return_content += "Program couldn't be compiled with make. 5% point will be taken off.\n"
        penalty -= 0.05    
        make_executable(dirname, useMakefile=False)
    if not os.path.isfile(dirname):
        print "Executable %s missing. Please check the compilation output."%(dirname)
        return_content += "Compilation error.\n"
        return_content += "Grade:: %s/%s.\n"%(total, max_grade)
        os.chdir(prevdir)
        return total
    
    os.system("rm -f test*.txt")
    os.system("rm -f result*.txt")
    os.system("cp %s/%s/*.txt ."%(testcases, dirname))
    
    
    test_ind = 1 #test_ind 0 is for presence of the files
    
    if dirname=='third':
        ret = run_command("./%s %s"%(dirname, "nofile.txt"), user_program=True)
        if compare_string_file("result_no.txt", ret, show_difference=True):
            print "The output is correct for input file nofile.txt."
            #return_content += "The output is correct for input file %s."%(testfile)
            total += testgrades[dirname][test_ind]
        else:
            print "The output is not correct for input file nofile.txt. %s points lost."%(str(testgrades[dirname][test_ind]))
            return_content += "The output is not correct for input file nofile.txt. %s points lost.\n"%(str(testgrades[dirname][test_ind]))
        print ""
        test_ind += 1
        
    
    for testfile in sorted(os.listdir(".")):
        if os.path.isdir(testfile) or not testfile.startswith("test") or not testfile.endswith(".txt") or testfile.startswith("testcases"):
            continue
        resultfile = "result"+testfile[4:len(testfile)]
        inputfile = "input"+testfile[4:len(testfile)]
        if not os.path.isfile(resultfile):
            print "Found a test file %s. But, no associated result file."%(testfile)
            continue
        print "Found a test file %s. The output will be compared to %s."%(testfile, resultfile)
        ret = run_command("./%s %s %s %s"%(dirname, testfile, inputfile, resultfile), user_program=True)
        if compare_string_file(resultfile, ret, show_difference=True):
            print "The output is correct for input file %s."%(testfile)
            #return_content += "The output is correct for input file %s."%(testfile)
            total += testgrades[dirname][test_ind]
        else:
            print "The output is not correct for input file %s. %s points lost."%(testfile, str(testgrades[dirname][test_ind]))
            return_content += "The output is not correct for input file %s. %s points lost.\n"%(testfile, str(testgrades[dirname][test_ind]))
        print ""
        test_ind += 1
    print "Grade:: %s/%s.\n"%(total*penalty, max_grade)
    return_content += "Grade:: %s/%s.\n"%(total*penalty+files, max_grade)
    os.chdir(prevdir)
    return total*penalty+files
    
    
def single_grade(dirname):
    global return_content
    
    print "Grading", dirname
    return_content += "Grading %s\n"%(dirname)
    prevdir = os.getcwd()
    os.chdir(dirname)
    files = allfiles()
    total = 0
    penalty = 1
    max_grade = 15
    
    os.system("rm -f %s"%(dirname))
    make_executable(dirname)
    if not os.path.isfile(dirname):
        print "Executable %s missing after makefile."%(dirname)
        return_content += "Program couldn't be compiled with make. 5% point will be taken off.\n"
        penalty -= 0.05
    
    make_executable(dirname, useMakefile=False)
    if not os.path.isfile(dirname):
        print "Executable %s missing. Please check the compilation output."%(dirname)
        return_content += "Compilation error.\n"
        return_content += "Grade:: %s/%s.\n"%(total+files, max_grade)
        os.chdir(prevdir)
        return total+files
    
    os.system("rm -f test.txt")
    os.system("cp %s/%s/test.txt ."%(testcases, dirname))
    
    if not os.path.isfile("test.txt"):
        print "Expecting the test cases in test.txt. Not found."
        os.chdir(prevdir)
        return
    else:
        print "Using test.txt for grading."   
    
    
    fd = open("test.txt")
    state = 0
    test_ind = 1 #test_ind 0 is for presence of the files
    for line in fd:
        if state==0:
            inputline = line
            state = 1
        else:
            outputline = line
            state = 0
            ret = run_command("./%s %s"%(dirname, inputline.strip()), user_program=True)            
            if compare_string(outputline, ret):
                print "The output is correct for input %s."%(inputline.strip())	
                #return_content += "The output is correct for input %s."%(inputline.strip()) 
                total += testgrades[dirname][test_ind]
            else:
                print "The program generated %s. The correct answer is %s."%(ret.strip(), outputline.strip())
                print "The output is not correct for input %s. %s points lost."%(inputline.strip(), str(testgrades[dirname][test_ind]))
                return_content += "The program generated %s. The correct answer is %s.\n"%(ret.strip(), outputline.strip())
                return_content += "The output is not correct for input %s. %s points lost.\n"%(inputline.strip(), str(testgrades[dirname][test_ind]))
            test_ind += 1
    fd.close()    
    print ""
    return_content += "Grade:: %s/15.\n"%(total*penalty+files)
    os.chdir(prevdir)
    return total*penalty+files
    

def global_grade(dirname):
    global return_total_grade
    global return_content

    target = len(subdirectories)
    found = 0
    for subdir in subdirectories:
        if not os.path.isdir(subdir):
            continue        
        print subdir, " found!"
        found += 1
        if subdir in formats and formats[subdir]=='line':
            return_total_grade += single_grade(subdir)
        elif subdir in formats and formats[subdir]=='file':
            return_total_grade += file_grade(subdir)
    if found==0:
        return_content += "No subdirectory (first, second etc.) found. Please check your directory structure.\n"
        print os.getcwd(), os.listdir(".")
        

def get_latest(dirname, pattern):        
    filename = pattern
    for f in sorted(os.listdir(dirname)):
        if not f.startswith(pattern):
            continue
        filename = f
    return filename    

def tar_extract(dirname, netid):    
    mossdir = os.path.join("/home/muhammed/cs211/grading/pa4_students",netid)
    prevdir = os.getcwd()
    os.chdir(dirname)    
    filename = get_latest(".", "pa4.tar")        
    if os.path.exists(filename):
        run_command("mkdir %s"%(mossdir), verbose=False)
        run_command("tar -xvf %s -C %s"%(filename, mossdir))
    os.chdir(prevdir)
    return "", ""
    
def tar_grade(dirname, fd=None):
    global return_content
    global return_total_grade

    return_content = ""
    return_total_grade = 0

    prevdir = os.getcwd()
    os.chdir(dirname)
    filename = "pa4.tar"
    tmpdir = "/tmp/obj_temp"
    #if fd!=None:
    filename = get_latest(".", "pa4.tar")        
    
    if not os.path.exists(filename):
        if fd!=None:
            print >>fd, "Expecting pa4.tar in current directory. Current directory is %s"%(prevdir)                    
        else:
            print "Expecting pa4.tar in current directory. Current directory is %s"%(prevdir)        
            print "Please make sure you created pa4.tar in the right directory"            
            return_content += "No pa4.tar found.\n"
        #sys.exit(1)
        os.chdir(prevdir)
        return_content += "\nTotal grade: %s/100.\n"%(return_total_grade)
        return return_content, return_total_grade
    if os.path.exists("/tmp/obj_temp"):
        print "Deleting the directory obj_temp."
        run_command("rm -rf /tmp/obj_temp", verbose=False)
    run_command("mkdir /tmp/obj_temp", verbose=False)    
    run_command("tar -xvf %s -C %s"%(filename, tmpdir))
    
    os.chdir("/tmp/obj_temp")
    if os.path.isdir("pa4"):
        os.chdir("pa4")
        global_grade("pa4")
    else:
        if fd!=None:
            print >>fd, "There is not directory named pa4 in pa4.tar."            
        else:
            print "There is no directory named pa4 in pa4.tar."
            print "Please check your tar file."
            return_content += "There is no directory named pa4 in pa4.tar. Please check your tar file.\n"
    os.chdir(prevdir)
    return_content += "\nTotal grade: %s/100.\n"%(return_total_grade)
    
    return return_content, return_total_grade
            
if __name__ == '__main__':
    basepath = "pa4"   
    tarmode = False #by default check the directory
    
    if len(sys.argv)>1:        
        if sys.argv[1].strip()=='tar':
            tarmode=True    
    
    if tarmode==False:
        if not os.path.isdir(basepath):
            print "pa4 is not presnt in this directory."
            sys.exit(1)
        else:
            print "Grading the content of pa4."
            os.chdir(basepath)
            global_grade(basepath)
    else:
        tar_grade(".")
