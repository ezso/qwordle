import textwrap
from shutil import which
import os

def is_installed(name):
    """Check whether `name` is on PATH and marked as executable."""
    return which(name) is not None

    
def style_test(tu, test_name):
    src_dir=tu.join_base("src")
    test_bin = ("splint")
    if not is_installed(test_bin):
        return tu.FAILURE("splint binary not available")
    
    args = ["-retvalother","-retvalint","-branchstate", "-exportlocal", "-usedef", "-compdef", "-temptrans", "+charint", "-predboolint", "-predboolothers", "-nullret", "-nullderef", "-nullpass", "-unrecog", "+matchanyintegral", "-mustfreefresh", "-formatcode", "-mustdefine", "-initallelements","-boolops", "-compdestroy", "+posixlib", "-unqualifiedtrans", "-shiftnegative", "-I", "include"]
    for file in os.listdir(src_dir):
        if file.endswith(".c") and file != "unit_tests.c" and file != "test_main.c":
            args.append(src_dir+"/"+file)
    
    run_res = tu.run(test_bin, args, timeout_secs=20)
    rc,outs,errs=run_res

    msg = "splint found warnings."
    ret_text=""
    if (len(outs) > 0):
        ret_text += "\nstdout:\n" + textwrap.indent(outs, "  ")
    if (len(errs) > 0):
        ret_text += "\nstderr:\n" + textwrap.indent(errs, "  ")
    if "no warning" not in ret_text:
        return tu.FAILURE(msg+ret_text)
    return tu.check(run_res, "splint test failed")

def style_test_cppcheck(tu, test_name):
    test_bin = ("cppcheck")
    if not is_installed(test_bin):
        return tu.FAILURE("cppcheck binary not available")

    run_res = tu.run("sh", ["-c", "cppcheck ."])
    rc,outs,errs=run_res

    msg = "cppcheck found warnings."
    ret_text=""
    if (len(outs) > 0):
        ret_text += "\nstdout:\n" + textwrap.indent(outs, "  ")
    if (len(errs) > 0):
        ret_text += "\nstderr:\n" + textwrap.indent(errs, "  ")
    if "error:" in ret_text:
        return tu.FAILURE(msg+ret_text)
    return tu.check(run_res, "cppcheck test failed")

def style_test_cpplint(tu, test_name):
    test_bin = ("cpplint")
    if not is_installed(test_bin):
        return tu.FAILURE("cpplint binary not available")

    run_res = tu.run("sh", ["-c", "cpplint --filter=-legal/copyright,-build/include_subdir,-readability/casting ./include/*.h ./src/*.c"])
    rc,outs,errs=run_res

    msg = "cpplint found warnings."
    ret_text=""
    if (len(outs) > 0):
        ret_text += "\nstdout:\n" + textwrap.indent(outs, "  ")
    if (len(errs) > 0):
        ret_text += "\nstderr:\n" + textwrap.indent(errs, "  ")
    if "Total errors found" in ret_text:
        return tu.FAILURE(msg+ret_text)
    return tu.check(run_res, "cpplint test failed")

def unit_test(tu, test_name):
    test_bin = tu.join_base("bin/testrunner")
    if not os.path.exists(test_bin):
        return tu.FAILURE("testrunner binary not available")
    run_res = tu.run(test_bin, [test_name])
    return tu.check(run_res, "unit test failed")

def integration_test(tu, test_name, args):
    ref_name, arg = args
    local_timeout_secs = 15

    wordle_bin = tu.join_base("bin/wordle_opt")
    if not os.path.exists(wordle_bin):
        return tu.FAILURE("wordle binary not available")
    
    ref_in_file = tu.join_base("test/ref_input/{}.txt".format(ref_name))
    # wordle_bin = "{} {} < {}".format(wordle_bin, arg, ref_in_file)
    with open(ref_in_file, "r") as f:
        wordle_input = f.read()

    (rc, outs, errs) = tu.run(wordle_bin,arg, timeout_secs=local_timeout_secs, input=wordle_input)

    check_res = tu.check((rc, outs, errs), "failed to validate")
    if not check_res:
        return check_res

    ref_file = tu.join_base("test/ref_output/{}.txt".format(ref_name))

    with open(ref_file, "r") as ref_src:
        ref_str = ref_src.read()
        if ref_str == outs:
            return tu.SUCCESS()
        else:
            return tu.FAILURE("ouput string differed from reference")


def specialize(fun, arg):
    return lambda tu, tn, x=arg: fun(tu, tn, x)

all_tests = {
    'public.style.splint': style_test,
    'public.style.cppcheck': style_test_cppcheck,
    # 'public.style.cpplint': style_test_cpplint, # for more linting
    
    'public.trie.create': unit_test,             
    'public.wordle.feedback_example1': unit_test,
    'public.wordle.feedback_example2': unit_test,
    'public.trie.simple_lookup': unit_test,      
    'public.wordle.simple_load': unit_test,      
    'public.wordle.simple_win': unit_test,       
    'public.wordle.functions': unit_test,        
}


timeout_secs = 5

