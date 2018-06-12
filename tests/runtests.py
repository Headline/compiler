# usage python runtests.py <compiler>

import sys
import os.path
import subprocess
import tempfile
import time

def compare_files(comparisonfile, err):
    lines = comparisonfile.readlines()
    err.seek(0)
    complines = err.readlines()
	
	# clean filenames from C:\dir\dir\file.txt -> file.txt
    comp = comparisonfile.name.split("\\")
    comp = comp[len(comp)-1]
	
    if lines != complines:
        print("\n*******")
        print(comp + ": Failure!")
        print("Recieved: " + str(complines))
        print("Expected: " + str(lines))
        print("*******\n") # clean look
        return False
    else:
        print(comp + ": Success")
        return True

failure = 0
start = time.time()
path = os.path.realpath(__file__).replace(sys.argv[0], "")

for dirpath, dirnames, filenames in os.walk(path):
    for filename in filenames:
        current = dirpath + '\\' + filename
        if filename.endswith(".ds"):
            outfilename = current.replace(".ds", ".out")
            with open(outfilename, 'w+') as err:
                args = [sys.argv[1], current]

                garbage, name = tempfile.mkstemp()
                p = subprocess.Popen(args, shell=True, stderr=err, stdout=garbage)
                p.wait()

                os.close(garbage)
                
            with open(current.replace(".ds", ".txt")) as comp:
                if not compare_files(comp, open(outfilename)):
                    failure = failure + 1
            
            os.unlink(outfilename) # remove .out, lets not be messy
           
end = time.time()
time = '%.2f'%(end-start)
if failure > 0:
    print("Test bank failed: " + str(failure) + " failure(s) reported in " + time + " seconds")
    exit(1)
else:
    print("Test bank completed sucessfully in " + time + " seconds")
