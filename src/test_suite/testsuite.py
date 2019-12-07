from argparse import ArgumentParser
import argparse
from pathlib import Path
import subprocess as sp
from difflib import unified_diff
from termcolor import colored
import yaml
import signal
from func_timeout import func_timeout,FunctionTimedOut
def run_shell(args,stdin):
    return sp.run(args,capture_output=True,text=True,input=stdin)

def differencies(reference,student):
    reference = reference.splitlines(keepends=True)
    student = student.splitlines(keepends=True)

    return ''.join(unified_diff(reference,student, \
                                fromfile="ref", tofile="student"))

def test(binary,testcase,i = 0):
    if (i == 1):
        student = run_shell(["valgrind","--error-exitcode=2","--leak-check=full","-s",binary], testcase["stdin"])
    else:
        student = run_shell([binary], testcase["stdin"])
        #student = run_shell(["echo", testcase["stdin"], "|", "./42sh"])


    reference = run_shell(["bash", "--posix"], testcase["stdin"])
    #reference = run_shell(["echo", testcase["stdin"], "|", "bash", "--posix"])
    for check in testcase.get("checks",["stdout","stderr","returncode","has_stderr"]):
        #print(check + "\n")
        if check == "stdout":
            assert reference.stdout == student.stdout, \
            f"stdout differs:\n{differencies(reference.stdout, student.stdout)}"
        if check == "stderr":
            if (i == 1 and student.returncode == 2):
                assert student.returncode == "a" \
                f"memory leaks"
            else:
                assert reference.stderr == student.stderr, \
                f"stderr differs:\n{differencies(reference.stderr, student.stderr)}"
        if check == "returncode":
            assert reference.returncode == student.returncode, \
            f"Exited with{student.returncode}, expected {reference.returncode}"
        if check == "has_stderr":
                #print("yoyo" + "\n")
                assert student.stderr != "" \
                f"Something was expected on stderr"


def lunch(categorie,i, t = 0):
    path = "categories/" + categorie + "/good.yml"
    with open(path,"r") as tests_files:
            content = yaml.safe_load(tests_files)
    print()
    total = 0
    passed = 0
    for testi in content:
        try:
            #test(binary,testi)
            func_timeout(t,test, args = (binary,testi,i))
        except AssertionError as err:
            print(f"[{colored('KO', 'red')}]",testi["name"])
            print(err)
        except FunctionTimedOut:
            print(f"[{colored('KO', 'red')}]",testi["name"],"--Timed Out")

        else:
            print(f"[{colored('OK', 'green')}]",testi["name"])
            passed = passed + 1
        #print("\n")
        total = total + 1
    return (passed,total)

if __name__ == "__main__":
    categories = ["command","buil_exec", "case" , "for"]
    parser = ArgumentParser(description="42sh Testsuite")
    parser.add_argument('bin', metavar='BIN')
    parser.add_argument('--list',"-l",help="list all categories",action = "store_true")

    parser.add_argument('--category',"-c",help="name of the category")
    parser.add_argument('--sanity','-s', help ='find memory leaks', action ="store_true")
    parser.add_argument('--timeout',"-t",help="timeout for each functions")
    args = parser.parse_args()
    if args.sanity:
        i = 1
    else:
        i = 0
    if args.timeout:
        t = int(args.timeout)
    else:
        t = 2
    binary = Path(args.bin).absolute()
    if args.list:
        for categorie in categories:
            print(categorie)
    elif args.category:
        print(args.category)
        nbr = lunch(args.category,i,t)
        print(str(nbr[0]) + " / " + str(nbr[1]) + "  passed for the category: " + args.category)
    else:
        for categorie in categories:
            print(categorie)
            nbr = lunch(categorie,i,t)
            print(str(nbr[0]) + " / " + str(nbr[1]) + \
                    "  passed for the category: " + categorie)
            print()
