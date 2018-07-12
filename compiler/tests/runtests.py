# usage python runtests.py <compiler>

import sys
import os.path
import subprocess
import tempfile
import time
from pathlib import Path

def compare_files(comparisonfile, err):
    if comparisonfile == None:
        lines = []
    else:
        lines = comparisonfile.readlines()
    complines = err.readlines()
	
	# clean filenames from C:\dir\dir\file.txt -> file.txt
    comp = err.name.split("\\")
    comp = comp[len(comp)-1].split(".")
    comp = comp[len(comp)-2]
	
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
garbage, name = tempfile.mkstemp()

for dirpath, dirnames, filenames in os.walk(path):
    for filename in filenames:
        current = dirpath + filename
        if filename.endswith(".x"):
            outfilename = current.replace(".x", ".out")
            args = [sys.argv[1], '-stderr='+ outfilename, current]
            p = subprocess.Popen(args, shell=False, stderr=garbage, stdout=garbage)
            p.wait()
                
            tryopenname = current.replace(".x", ".txt")
            f = Path(tryopenname)
            if f.is_file():
                    with open(tryopenname) as comp:
                        if not compare_files(comp, open(outfilename)):
                            failure = failure + 1
            else:
                if not compare_files(None, open(outfilename)):
                    failure = failure + 1
                    
            os.unlink(outfilename) # remove .out, lets not be messy
           
os.close(garbage)
end = time.time()
time = '%.2f'%(end-start)
if failure > 0:
    print("Test bank failed: " + str(failure) + " failure(s) reported in " + time + " seconds")
    exit(1)
else:
    print("Test bank completed sucessfully in " + time + " seconds")

