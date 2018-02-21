import glob, os
import shutil

import subprocess
def containsCmakeLists(dir):
    if(os.path.exists(dir)):
        for fname in os.listdir(dir):
            if(fname == 'CMakeLists.txt'):
                return True
    return False;

def listCMakeDirs(dir):
    r = []
    for root, dirs, files in os.walk(dir):
        for d in dirs:
            path = os.path.join(dir, d)
            if(containsCmakeLists(path)):
                r.append(path)
            r = r + listCMakeDirs(path)
    return r
def replaceWordInFile (filepath, target, word):
    print("replace " + target + " with " + word + " in " +  filepath)
    # Read in the file
    with open(filepath, 'r') as file :
      filedata = file.read()


    # Replace the target string
    filedata = filedata.replace(target, word)

    # Write the file out again
    with open(filepath, 'w') as file:
      file.write(filedata)


def createProjectVC10_64(dir):
    projPath = os.path.join(dir, 'vc10_win64')
    if(os.path.exists(projPath)):
        print(projPath)
        shutil.rmtree(projPath)
    os.makedirs(projPath)
    subprocess.call("cmake -G \"Visual Studio 10 2010 Win64\" .. -DCMAKE_USE_RELATIVE_PATHS=ON -DCMAKE_SUPPRESS_REGENERATION=ON",cwd=projPath)
#clean cmake
    for root, dirs, files in os.walk(dir) :
        for name in dirs:
            if("cmake" in name.lower()):
                print("delete " + os.path.join(root, name))
                shutil.rmtree(os.path.join(root, name))
        for name in files:
            if("cmake" in name.lower() and os.path.exists(os.path.join(root, name)) and name != "CMakeLists.txt"):
                print("delete " + os.path.join(root, name))
                os.remove(os.path.join(root, name))
#clean absolute path
    sisodir32 = os.environ["sisodir5_32"]
    sisodir64 = os.environ["sisodir5_64"]
    for root, dirs, files in os.walk(projPath) :
        for file in files:
            replaceWordInFile(os.path.join(root, file), "D:\\Runtime_Build_VS10-64\\install", "%sisodir5_64%")
            replaceWordInFile(os.path.join(root, file), "D:/Runtime_Build_VS10-64/install", "%sisodir5_64%")
            replaceWordInFile(os.path.join(root, file), sisodir64, "%sisodir5_64%")
            replaceWordInFile(os.path.join(root, file), sisodir32, "%sisodir5_64%")
            replaceWordInFile(os.path.join(root, file), dir.replace("\\","/") + "/", "..\\")
            replaceWordInFile(os.path.join(root, file), dir.replace("/","\\") + "\\", "..\\")

def createProjectVC10_32(dir):
    projPath = os.path.join(dir, 'vc10_win32')
    if(os.path.exists(projPath)):
        print(projPath)
        shutil.rmtree(projPath)
    os.makedirs(projPath)
    subprocess.call("cmake -G \"Visual Studio 10 2010\" .. -DCMAKE_USE_RELATIVE_PATHS=ON -DCMAKE_SUPPRESS_REGENERATION=ON",cwd=projPath)
#clean cmake
    for root, dirs, files in os.walk(dir) :
        for name in dirs:
            if("cmake" in name.lower()):
                print("delete " + os.path.join(root, name))
                shutil.rmtree(os.path.join(root, name))
        for name in files:
            if("cmake" in name.lower() and os.path.exists(os.path.join(root, name)) and name != "CMakeLists.txt"):
                print("delete " + os.path.join(root, name))
                os.remove(os.path.join(root, name))
#clean absolute path
    sisodir32 = os.environ["sisodir5_32"]
    sisodir64 = os.environ["sisodir5_64"]
    for root, dirs, files in os.walk(projPath) :
        for file in files:
            replaceWordInFile(os.path.join(root, file), "D:\\Runtime_Build_VS10-64\\install", "%sisodir5_32%")
            replaceWordInFile(os.path.join(root, file), "D:/Runtime_Build_VS10-64/install", "%sisodir5_32%")
            replaceWordInFile(os.path.join(root, file), sisodir64, "%sisodir5_32%")
            replaceWordInFile(os.path.join(root, file), sisodir32, "%sisodir5_32%")
            replaceWordInFile(os.path.join(root, file), dir.replace("\\","/") + "/", "..\\")
            replaceWordInFile(os.path.join(root, file), dir.replace("/","\\") + "\\", "..\\")

projectdirs = listCMakeDirs(os.getcwd())
for dir in projectdirs:
    print(dir)
    createProjectVC10_64(dir)
    createProjectVC10_32(dir)
