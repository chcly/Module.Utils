# -----------------------------------------------------------------------------
#
#   Copyright (c) Charles Carley.
#
#   This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
# ------------------------------------------------------------------------------
import codecs
import os, subprocess, re

PULL = "git pull"


def trim(line):
    line = line.replace('\t', '')
    line = line.replace('\n', '')
    line = line.replace(' ', '')
    return line


def makeAbsolute(cwd, relativeDir):
    return os.path.join(cwd, relativeDir).replace("\\", "/")


def checkUrl(url):
    matchObj = re.match("[a-zA-Z0-9:/.]+", url)
    if (matchObj != None):
        return url
    return None


def checkBranch(branch):
    matchObj = re.match("[a-zA-Z0-9.]+", branch)
    if (matchObj != None):
        return branch
    return None


def checkShallow(shallow):
    if (shallow == "true" or shallow == "false"):
        return shallow
    return None


def execProgram(args):
    subprocess.run(args.split(' '),
                   shell=False,
                   check=False,
                   env=os.environ,
                   capture_output=False)

def checkPrimaryBranch():
    # capture the output from git branch -a
    co = subprocess.run("git branch -a",
                   shell=True,
                   check=False,
                   env=os.environ,
                   capture_output=True)
    
    rv = None
    if (co != None and co.stdout != None):

        ot = codecs.decode(co.stdout);
        sl = ot.split('\n')
        if (len(sl) > 0):
            cb = sl[0]
            cb = cb.replace("*", '')
            cb = cb.strip(' ')
            rv = cb
    return rv



def changeDirectory(directory):

    try:
        os.chdir(directory)
        val = os.getcwd()
        directory = directory.replace('\\', '/')
        val = val.replace('\\', '/')
        return directory == val
    except OSError:
        pass
    return False


def initModules():
    # this is meant to be called from the same directory
    # as the module script.

    execProgram(PULL)
    execProgram("git submodule init")
    execProgram("git submodule update --init --merge")


def updateModules(dict):

    for key in dict.keys():
        module = dict[key]

        path = module.get("path", None)
        branch = module.get("branch", None)
        shallow = module.get("shallow", None)
        url = module.get("url", None)

        print("Updating", url)
        print("=>".rjust(11, ' '), module.get("name", None))

        if (path != None):
            if (not os.path.isdir(path)):
                print(
                    "could not determine the directory for the "
                    "supplied path:", path)
                continue

            if (url == None):
                continue

            if (not changeDirectory(path)):
                print(
                    "could not switch directory "
                    "to the module at the supplied path:", path)
                continue

            branchStr = checkPrimaryBranch()
            if (branchStr == None):
                branchStr = "master"

            if (branchStr != "master" and branchStr != "main"):
                print("Using nonstandard branch,", branchStr)

            if (branch != None):
                branchStr = branch

            shallowValue = False
            if (shallow != None):
                shallowValue = (shallow == 'true')

            if (shallowValue == False):
                execProgram("git checkout %s" % branchStr)
                execProgram(PULL)
            else:
                execProgram("git checkout -f -B %s" % branchStr)
                execProgram(PULL + " %s %s" % (url, branchStr))


def collectModules(cwd, dict):

    mp = makeAbsolute(cwd, dict)
    if (not os.path.exists(mp)):
        print("No .gitmodule found in", cwd, "\nNothing to update...")
        return None

    dict = {}
    fp = open(mp, mode='r')
    lines = fp.readlines()
    fp.close()


    name = None

    for line in lines:
        line = trim(line)

        opt = "[submodule\""
        if (line.startswith(opt)):
            name = line[len(opt):-2]

            dict[name] = {}
            var = dict[name]
            var['hasUrl'] = False

        elif (name != None):
            var = dict[name]
            if (line.find("url=") != -1):
                var["url"] = checkUrl(line.replace("url=", ''))
                var['hasUrl'] = True
            elif (line.find("branch=") != -1):
                var["branch"] = checkBranch(line.replace("branch=", ''))
            elif (line.find("shallow=") != -1):
                var["shallow"] = checkShallow(line.replace("shallow=", ''))
            elif (line.find("path=") != -1):
                loc = line.replace("path=", '')
                path = None
                if (os.path.isabs(loc)):
                    path = loc
                else:
                    path = makeAbsolute(cwd, loc)

                if (os.path.isdir(path)):
                    var["path"] = path
                    var["name"] = loc
            else:
                if (len(line) > 0):
                    print("unhandled line: ", line)

    for key in dict.keys():
        module = dict[key]
        if (module.get("hasUrl", False) == False):
            print("unable to determine the url for the module:", key)
    return dict


def main():
    cwd = os.getcwd()
    dict = collectModules(cwd, ".gitmodules")
    if (dict != None):
        initModules()
        updateModules(dict)
        changeDirectory(cwd)


if __name__ == '__main__':
    main()