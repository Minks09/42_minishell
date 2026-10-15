#!/usr/bin/env python3
"""Compare minishell's exit codes with bash.

Every test runs the same input in minishell and in bash, inside identical
sandbox directories, and compares:
  * the exit code          -> a difference is a FAILURE
  * stdout                 -> a difference is a WARNING (FAILURE with --strict)
  * files left in sandbox  -> a difference is a WARNING (FAILURE with --strict)

Parts:
  1. builtins / commands / expansions / syntax / pipes  (non-interactive)
  2. redirections and heredocs                           (non-interactive)
  3. signals (Ctrl-C, Ctrl-\\, Ctrl-D)                    (in a pseudo-terminal)

Usage:  python3 tests/exit_code_tester.py [-v] [--strict] [--only CATEGORY]
"""

import argparse
import os
import pty
import re
import select
import shutil
import signal
import stat
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT_MINISHELL = os.path.join(os.path.dirname(HERE), "minishell")

GREEN, RED, YELLOW, BLUE, GREY, BOLD, END = (
    "\033[32m", "\033[31m", "\033[33m", "\033[36m", "\033[90m", "\033[1m",
    "\033[0m")

# --------------------------------------------------------------------------- #
#  Test definitions                                                           #
# --------------------------------------------------------------------------- #
# Each test is (script, note). The script is fed to the shell on stdin, one
# command per line. The shell's exit code is the status of its last command.
# note: empty, or a reason why bash >= 5.3 gives another exit code than older
# bash (minishell follows bash <= 5.2, the version of most 42 machines).
#
# Rule: a syntax error must be the LAST line of a script, because bash in
# non-interactive mode aborts the whole script on a syntax error.

TOO_MANY = "bash 5.3 returns 2 for 'too many arguments', older bash 1"

TESTS = {
    "builtins": [
        ("echo hello world", ""),
        ("echo -n hello", ""),
        ("echo -nnn -n hello", ""),
        ("echo -na hello", ""),
        ("echo", ""),
        ("pwd", ""),
        ("pwd extra args", ""),
        ("cd", ""),
        ("cd /tmp\npwd", ""),
        ("cd ..\npwd", ""),
        ("cd /tmp\ncd -", ""),
        ("cd dir\npwd", ""),
        ("cd nonexistent_dir", ""),
        ("cd infile", ""),
        ("unset HOME\ncd", ""),
        ("cd /tmp /var", TOO_MANY),
        ("export A=1\necho $A", ""),
        ("export A\necho $A", ""),
        ("export A=\necho \"[$A]\"", ""),
        ("export A=1 B=2 C=3\necho $A$B$C", ""),
        ("export A=x\nexport A+=y\necho $A", ""),
        ("export 1A=x", ""),
        ("export A-=1", ""),
        ("export =", ""),
        ("export A=1 2B=2 C=3\necho $A $C", ""),
        ("export \"A B\"=1", ""),
        ("export A=1\nunset A\necho \"[$A]\"", ""),
        ("unset", ""),
        ("unset NOT_SET_VAR", ""),
        ("unset 1A", ""),
        ("unset A=B", ""),
        ("export A=1\nunset A 1B\necho \"[$A]\"", ""),
        ("unset PATH\nls", ""),
        ("unset PATH\n/bin/ls", ""),
        ("env | grep -q '^HOME='", ""),
        ("export ZZ=1\nenv | grep -c '^ZZ='", ""),
        ("export ZZ\nenv | grep -c '^ZZ='", ""),
        ("exit", ""),
        ("false\nexit", ""),
        ("exit 0", ""),
        ("exit 42", ""),
        ("exit 255", ""),
        ("exit 256", ""),
        ("exit 298", ""),
        ("exit -1", ""),
        ("exit -100", ""),
        ("exit +7", ""),
        ("exit \" 42 \"", ""),
        ("exit 9223372036854775807", ""),
        ("exit 9223372036854775808", ""),
        ("exit -9223372036854775808", ""),
        ("exit abc", ""),
        ("exit 4a", ""),
        ("exit \"\"", ""),
        ("exit 1 2", TOO_MANY),
        ("exit abc def", ""),
        ("exit 3\necho not reached", ""),
    ],
    "commands": [
        ("true", ""),
        ("false", ""),
        ("ls", ""),
        ("ls -la dir", ""),
        ("ls nonexistent_file", ""),
        ("/bin/echo absolute path", ""),
        ("/bin/ls nonexistent_file", ""),
        ("nonexistent_command", ""),
        ("./nonexistent_file", ""),
        ("/nonexistent/path/cmd", ""),
        ("./dir", ""),
        ("dir", ""),
        ("infile", ""),
        ("./noexec.sh", ""),
        ("./script.sh", ""),
        ("\"\"", ""),
        ("''", ""),
        ("$EMPTY_VAR", ""),
        ("$EMPTY_VAR echo hi", ""),
        ("$EMPTY_VAR false", ""),
        ("grep -q hello infile", ""),
        ("grep -q nothere infile", ""),
        ("grep hello missing_file", ""),
        ("test -f infile", ""),
        ("test -d infile", ""),
        ("head -n 1 infile", ""),
        ("sort infile", ""),
        ("wc -l infile", ""),
        ("expr 1 + 1", ""),
        ("expr 0", ""),
        ("sh -c 'exit 5'", ""),
        ("sh -c 'exit 300'", ""),
        ("sh -c 'kill -TERM $$'", ""),
        ("sh -c 'kill -KILL $$'", ""),
        ("sleep 0", ""),
        ("false\ntrue", ""),
        ("true\nfalse", ""),
        ("nonexistent_command\necho $?", ""),
        ("./dir\necho $?", ""),
    ],
    "expansion": [
        ("echo $HOME", ""),
        ("echo \"$HOME\"", ""),
        ("echo '$HOME'", ""),
        ("echo \"'$HOME'\"", ""),
        ("echo '\"$HOME\"'", ""),
        ("echo $?", ""),
        ("false\necho $?", ""),
        ("echo $?HELLO", ""),
        ("echo $", ""),
        ("echo \"$\"", ""),
        ("echo $1abc", ""),
        ("echo $NOT_SET_VAR", ""),
        ("echo \"[$NOT_SET_VAR]\"", ""),
        ("echo $\"HOME\"", ""),
        ("echo hello\"\"world", ""),
        ("echo \"\" a \"\" b", ""),
        ("e\"ch\"o quoted command", ""),
        ("'ls'", ""),
        ("\"ls\" dir", ""),
        ("export X=\"ls -a\"\n$X dir", ""),
        ("export X=\"ls -a\"\n\"$X\"", ""),
        ("export X=\"a   b\"\necho [$X]", ""),
        ("export X=\"a   b\"\necho [\"$X\"]", ""),
        ("export X=echo\n$X hi", ""),
        ("echo $SHLVL", ""),
    ],
    "syntax": [
        ("|", ""),
        ("| ls", ""),
        ("ls |", ""),
        ("ls | | wc", ""),
        ("ls ||| wc", ""),
        (">", ""),
        ("<", ""),
        (">>", ""),
        ("<<", ""),
        ("echo >", ""),
        ("echo > > out", ""),
        ("cat < | ls", ""),
        ("echo \"unclosed", ""),
        ("echo 'unclosed", ""),
        ("true\nls |", ""),
    ],
    "pipes": [
        ("ls | wc -l", ""),
        ("echo hi | cat | cat | cat", ""),
        ("cat infile | grep hello", ""),
        ("cat infile | grep nothere", ""),
        ("true | false", ""),
        ("false | true", ""),
        ("nonexistent_command | true", ""),
        ("true | nonexistent_command", ""),
        ("echo hi | ./dir", ""),
        ("echo hi | ./noexec.sh", ""),
        ("exit 5 | true", ""),
        ("true | exit 5", ""),
        ("sh -c 'exit 3' | sh -c 'exit 4'", ""),
        ("cat infile | sort | head -n 1", ""),
        ("export A=1 | true\necho \"[$A]\"", ""),
        ("cd /tmp | true\npwd", ""),
        ("echo hi | exit\necho still here", ""),
        ("ls | grep -q nothere", ""),
        ("cat | cat | ls", ""),
    ],
    "redirections": [
        ("echo hi > out", ""),
        ("echo hi > out\necho ho >> out", ""),
        ("echo hi >> out", ""),
        ("> out", ""),
        ("< infile", ""),
        ("cat < infile", ""),
        ("cat <infile >out", ""),
        ("wc -l < infile", ""),
        ("cat < missing", ""),
        ("cat < missing > out", ""),
        ("cat > out < missing", ""),
        ("cat < noperm", ""),
        ("echo hi > noperm", ""),
        ("echo hi >> noperm", ""),
        ("echo hi > dir", ""),
        ("echo a > o1 > o2", ""),
        ("echo a > o1 >> o2 > o3", ""),
        ("echo hi > o1 > noperm > o2", ""),
        ("cat < infile < missing", ""),
        ("cat < missing < infile", ""),
        ("echo hi > \"out file\"", ""),
        ("echo hi > $NOT_SET_VAR", ""),
        ("echo hi > \"$NOT_SET_VAR\"", ""),
        ("export F=\"a b\"\necho hi > $F", ""),
        ("export F=\"a b\"\necho hi > \"$F\"", ""),
        ("export F=out\necho hi > $F", ""),
        ("> out echo before", ""),
        ("echo a > out b c", ""),
        ("nonexistent_command > out", ""),
        ("exit 3 > out", ""),
        ("pwd > out", ""),
        ("cd dir > out\npwd", ""),
        ("echo hi > out | cat", ""),
        ("echo hi | cat > out", ""),
        ("cat < infile | wc -l > out", ""),
        ("cat < missing | echo still runs", ""),
        ("echo hi | cat < missing", ""),
        ("ls > out | cat < missing", ""),
    ],
    "heredoc": [
        ("cat << EOF\nhello\nworld\nEOF", ""),
        ("cat << EOF\nEOF", ""),
        ("cat << EOF\n$HOME\nEOF", ""),
        ("cat << EOF\n$NOT_SET_VAR.\nEOF", ""),
        ("cat << EOF\n'$HOME' \"$HOME\"\nEOF", ""),
        ("cat << 'EOF'\n$HOME\nEOF", ""),
        ("cat << \"EOF\"\n$HOME\nEOF", ""),
        ("cat << E\"O\"F\n$HOME\nEOF", ""),
        ("false\ncat << EOF\n$?\nEOF", ""),
        ("cat << $USER\nhi\n$USER", ""),
        ("cat << EOF | wc -l\na\nb\nc\nEOF", ""),
        ("cat << A << B\nfrom a\nA\nfrom b\nB", ""),
        ("cat << A | cat << B\nfrom a\nA\nfrom b\nB", ""),
        ("cat << EOF > out\nsaved\nEOF", ""),
        ("cat << EOF >> out\nline\nEOF", ""),
        ("cat << EOF > noperm\nx\nEOF", ""),
        ("cat << EOF < infile\nignored\nEOF", ""),
        ("cat < infile << EOF\nwins\nEOF", ""),
        ("grep hello << EOF\nhello there\nEOF", ""),
        ("grep hello << EOF\nnothing\nEOF", ""),
        ("nonexistent_command << EOF\nx\nEOF", ""),
        ("<< EOF\nno command\nEOF", ""),
        ("cat << EOF\n  indented\tand tabbed\nEOF", ""),
        ("cat << EOF\nEOF not alone\n EOF\nEOF", ""),
        ("cat << EOF\nunterminated", ""),
        ("cat << EOF\nfirst\nEOF\necho after", ""),
    ],
}

# Signal tests run in a pseudo-terminal (interactive mode).
# Actions: ("send", text) types text, ("key", "c" | "\\" | "d") sends
# Ctrl-C / Ctrl-\ / Ctrl-D, ("wait", seconds).
# "check": "status" -> compare $? afterwards; "exit" -> compare the shell's
# own exit code (the shell must have left).
SIGNAL_TESTS = [
    ("Ctrl-C at empty prompt -> $? = 130",
     [("wait", 0.3), ("key", "c"), ("wait", 0.5)], "status"),
    ("Ctrl-C discards the typed line",
     [("send", "echo SHOULD_NOT_RUN"), ("wait", 0.3), ("key", "c"),
      ("wait", 0.5)], "status"),
    ("$? resets after Ctrl-C + new command",
     [("key", "c"), ("wait", 0.4), ("send", "true\n"), ("wait", 0.4)],
     "status"),
    ("Ctrl-\\ at prompt does nothing",
     [("send", "true\n"), ("wait", 0.3), ("key", "\\"), ("wait", 0.4)],
     "status"),
    ("Ctrl-C during sleep -> 130",
     [("send", "sleep 10\n"), ("wait", 0.8), ("key", "c"), ("wait", 0.6)],
     "status"),
    ("Ctrl-\\ during sleep -> 131",
     [("send", "sleep 10\n"), ("wait", 0.8), ("key", "\\"), ("wait", 0.6)],
     "status"),
    ("Ctrl-C during a pipeline -> 130",
     [("send", "sleep 10 | sleep 10\n"), ("wait", 0.8), ("key", "c"),
      ("wait", 0.6)], "status"),
    ("Ctrl-D ends cat -> 0",
     [("send", "cat\n"), ("wait", 0.6), ("key", "d"), ("wait", 0.5)],
     "status"),
    ("Ctrl-C during heredoc -> 130, command not run",
     [("send", "cat << EOF\n"), ("wait", 0.5), ("send", "SHOULD_NOT_RUN\n"),
      ("wait", 0.3), ("key", "c"), ("wait", 0.6)], "status"),
    ("Ctrl-D during heredoc -> warning, command runs",
     [("send", "cat << EOF\n"), ("wait", 0.5), ("send", "line\n"),
      ("wait", 0.3), ("key", "d"), ("wait", 0.6)], "status"),
    ("Ctrl-D at prompt exits with last status",
     [("send", "false\n"), ("wait", 0.4), ("key", "d")], "exit"),
    ("exit 3 in interactive mode",
     [("send", "exit 3\n")], "exit"),
]

# --------------------------------------------------------------------------- #
#  Sandbox                                                                    #
# --------------------------------------------------------------------------- #


def make_sandbox(path):
    """(Re)create the sandbox directory with its fixture files."""
    remove_sandbox(path)
    os.makedirs(os.path.join(path, "dir"))
    with open(os.path.join(path, "infile"), "w") as f:
        f.write("hello\nworld\n42\n")
    with open(os.path.join(path, "noperm"), "w") as f:
        f.write("secret\n")
    os.chmod(os.path.join(path, "noperm"), 0)
    with open(os.path.join(path, "script.sh"), "w") as f:
        f.write("#!/bin/sh\nexit 7\n")
    os.chmod(os.path.join(path, "script.sh"), 0o755)
    with open(os.path.join(path, "noexec.sh"), "w") as f:
        f.write("#!/bin/sh\nexit 0\n")
    os.chmod(os.path.join(path, "noexec.sh"), 0o644)


def remove_sandbox(path):
    if not os.path.exists(path):
        return
    for root, dirs, files in os.walk(path):
        for name in dirs + files:
            try:
                os.chmod(os.path.join(root, name), 0o755)
            except OSError:
                pass
    shutil.rmtree(path, ignore_errors=True)


def snapshot(path):
    """Map every file of the sandbox to its content (or a marker)."""
    snap = {}
    for root, dirs, files in os.walk(path):
        for d in dirs:
            snap[os.path.relpath(os.path.join(root, d), path) + "/"] = "<dir>"
        for name in files:
            full = os.path.join(root, name)
            rel = os.path.relpath(full, path)
            try:
                with open(full, "rb") as f:
                    snap[rel] = f.read()
            except OSError:
                snap[rel] = "<unreadable mode %o>" % stat.S_IMODE(
                    os.stat(full).st_mode)
    return snap


def make_env():
    env = dict(os.environ)
    for var in ("BASH_ENV", "ENV", "PROMPT_COMMAND", "EMPTY_VAR",
                "NOT_SET_VAR", "A", "B", "C", "X", "F", "ZZ"):
        env.pop(var, None)
    env.setdefault("TERM", "xterm")
    env.setdefault("USER", "user")
    return env

# --------------------------------------------------------------------------- #
#  Non-interactive tests                                                      #
# --------------------------------------------------------------------------- #


def run_script(shell, script, sandbox, env):
    make_sandbox(sandbox)
    try:
        p = subprocess.run([shell], input=(script + "\n").encode(),
                           cwd=sandbox, env=env, stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE, timeout=10)
        code, out, err = p.returncode, p.stdout, p.stderr
    except subprocess.TimeoutExpired:
        code, out, err = "TIMEOUT", b"", b""
    if isinstance(code, int) and code < 0:
        code = "killed by signal %d" % -code
    files = snapshot(sandbox)
    remove_sandbox(sandbox)
    return code, out, err, files


def show(label, data):
    text = data.decode(errors="replace") if isinstance(data, bytes) else data
    text = text.rstrip("\n")
    if "\n" in text or len(text) > 60:
        print("        %s:" % label)
        for line in text.split("\n")[:12]:
            print("          | %s" % line)
    else:
        print("        %s: %r" % (label, text))


def files_diff(mini, bash):
    lines = []
    for name in sorted(set(mini) | set(bash)):
        if mini.get(name) != bash.get(name):
            lines.append("%s  mini=%r  bash=%r" % (
                name, short(mini.get(name)), short(bash.get(name))))
    return lines


def short(v):
    if v is None:
        return "(missing)"
    if isinstance(v, bytes):
        v = v.decode(errors="replace")
    return v if len(v) < 40 else v[:37] + "..."


def run_category(name, tests, args, ctx, stats):
    print("\n%s%s── %s %s%s" % (BOLD, BLUE, name, "─" * (60 - len(name)), END))
    for script, note in tests:
        stats["n"] += 1
        m_code, m_out, m_err, m_files = run_script(
            args.minishell, script, ctx["sandbox"], ctx["env"])
        b_code, b_out, b_err, b_files = run_script(
            args.bash, script, ctx["sandbox"], ctx["env"])
        out_ok = m_out == b_out
        fdiff = files_diff(m_files, b_files)
        known = bool(note) and ctx["bash53"] and m_code != b_code
        label = script.replace("\n", " ⏎ ")
        if m_code != b_code and not known:
            mark, kind = RED + "✗ FAIL" + END, "fail"
        elif known:
            mark, kind = YELLOW + "~ KNOWN" + END, "known"
        elif args.strict and (not out_ok or fdiff):
            mark, kind = RED + "✗ FAIL" + END, "fail"
        elif not out_ok or fdiff:
            mark, kind = YELLOW + "! WARN" + END, "warn"
        else:
            mark, kind = GREEN + "✓" + END, "ok"
        stats[kind] += 1
        if kind == "ok" and not args.verbose:
            continue
        print("  %s %s%s%s" % (mark, GREY if kind == "ok" else "", label, END))
        if kind == "ok" and args.verbose:
            print("        exit code: %s" % m_code)
            continue
        if kind == "known":
            print("        minishell=%s  bash=%s: %s" % (m_code, b_code, note))
            continue
        print("        exit code: minishell=%s  bash=%s" % (m_code, b_code))
        if not out_ok:
            show("minishell stdout", m_out)
            show("bash      stdout", b_out)
        for line in fdiff:
            print("        file %s" % line)
        if m_code != b_code:
            show("minishell stderr", m_err)
            show("bash      stderr", b_err)

# --------------------------------------------------------------------------- #
#  Signal tests (pseudo-terminal)                                             #
# --------------------------------------------------------------------------- #

KEYS = {"c": b"\x03", "\\": b"\x1c", "d": b"\x04"}


class PtyShell:
    """A shell running in a pseudo-terminal, as if a user typed in it."""

    def __init__(self, argv, env, cwd):
        self.pid, self.fd = pty.fork()
        if self.pid == 0:
            try:
                os.chdir(cwd)
                os.execve(argv[0], argv, env)
            finally:
                os._exit(127)
        self.output = ""

    def read(self, duration):
        end = time.time() + duration
        while time.time() < end:
            ready, _, _ = select.select([self.fd], [], [], 0.05)
            if ready:
                try:
                    data = os.read(self.fd, 4096)
                except OSError:
                    return
                if not data:
                    return
                self.output += data.decode(errors="replace")

    def wait_for(self, regex, timeout):
        end = time.time() + timeout
        while time.time() < end:
            m = re.search(regex, self.output)
            if m:
                return m
            self.read(0.1)
        return re.search(regex, self.output)

    def send(self, text):
        os.write(self.fd, text.encode() if isinstance(text, str) else text)

    def exit_code(self, timeout):
        end = time.time() + timeout
        while time.time() < end:
            self.read(0.05)
            pid, st = os.waitpid(self.pid, os.WNOHANG)
            if pid:
                self.pid = 0
                if os.WIFSIGNALED(st):
                    return "killed by signal %d" % os.WTERMSIG(st)
                return os.WEXITSTATUS(st)
        return "still running"

    def close(self):
        if self.pid:
            try:
                os.kill(self.pid, signal.SIGKILL)
                os.waitpid(self.pid, 0)
            except OSError:
                pass
        try:
            os.close(self.fd)
        except OSError:
            pass


def run_signal_test(argv, actions, check, ctx):
    sh = PtyShell(argv, ctx["env"], ctx["tmp"])
    try:
        sh.send("echo __RE''ADY__\n")
        if not sh.wait_for(r"__READY__", 5):
            return "no prompt", ""
        for act, val in actions:
            if act == "send":
                sh.send(val)
            elif act == "key":
                sh.send(KEYS[val])
            elif act == "wait":
                sh.read(val)
        if check == "exit":
            return sh.exit_code(3), sh.output
        sh.send("echo __ST:$?__\n")
        m = sh.wait_for(r"__ST:(\d+)__", 5)
        result = int(m.group(1)) if m else "no answer"
        if re.search(r"(?m)^SHOULD_NOT_RUN\r?$", sh.output):
            result = "%s (but a cancelled line was executed)" % result
        return result, sh.output
    finally:
        sh.close()


def run_signals(args, ctx, stats):
    name = "signals"
    print("\n%s%s── %s %s%s" % (BOLD, BLUE, name, "─" * (60 - len(name)), END))
    mini_argv = [os.path.abspath(args.minishell)]
    bash_argv = [shutil.which(args.bash) or args.bash, "--norc",
                 "--noprofile", "-i"]
    for label, actions, check in SIGNAL_TESTS:
        stats["n"] += 1
        m_res, m_out = run_signal_test(mini_argv, actions, check, ctx)
        b_res, b_out = run_signal_test(bash_argv, actions, check, ctx)
        what = "shell exit code" if check == "exit" else "$?"
        if m_res == b_res and isinstance(m_res, int):
            stats["ok"] += 1
            if args.verbose:
                print("  %s✓%s %s%s%s  (%s = %s)" % (GREEN, END, GREY, label,
                                                    END, what, m_res))
            continue
        stats["fail"] += 1
        print("  %s✗ FAIL%s %s" % (RED, END, label))
        print("        %s: minishell=%s  bash=%s" % (what, m_res, b_res))
        if args.verbose:
            show("minishell terminal", clean_tty(m_out))
            show("bash      terminal", clean_tty(b_out))


def clean_tty(text):
    text = re.sub(r"\x1b\[[0-9;?]*[A-Za-z]", "", text)
    return text.replace("\r", "")

# --------------------------------------------------------------------------- #
#  Main                                                                       #
# --------------------------------------------------------------------------- #


def bash_version(bash):
    try:
        out = subprocess.run(
            [bash, "-c", "echo ${BASH_VERSINFO[0]}.${BASH_VERSINFO[1]}"],
            stdout=subprocess.PIPE, timeout=5).stdout.decode().strip()
        major, minor = out.split(".")
        return out, (int(major), int(minor))
    except (OSError, ValueError, subprocess.TimeoutExpired):
        return "?", (0, 0)


def main():
    categories = list(TESTS) + ["signals"]
    ap = argparse.ArgumentParser(
        description="Compare minishell exit codes with bash.")
    ap.add_argument("--minishell", default=DEFAULT_MINISHELL,
                    help="path to the minishell binary (default: %(default)s)")
    ap.add_argument("--bash", default="bash",
                    help="bash to compare with (default: first in PATH)")
    ap.add_argument("--only", action="append", choices=categories,
                    help="run only this category (can be repeated)")
    ap.add_argument("--strict", action="store_true",
                    help="stdout / file differences also count as failures")
    ap.add_argument("-v", "--verbose", action="store_true",
                    help="also list passing tests, show terminal output")
    args = ap.parse_args()

    if not os.access(args.minishell, os.X_OK):
        sys.exit("minishell not found at %s (run make first)" % args.minishell)
    if not shutil.which(args.bash) and not os.access(args.bash, os.X_OK):
        sys.exit("bash not found: %s" % args.bash)

    version, vtuple = bash_version(args.bash)
    tmp = tempfile.mkdtemp(prefix="ms_exit_tester_")
    ctx = {"env": make_env(), "tmp": tmp,
           "sandbox": os.path.join(tmp, "sandbox"),
           "bash_version": version, "bash53": vtuple >= (5, 3)}
    stats = {"n": 0, "ok": 0, "warn": 0, "known": 0, "fail": 0}

    print("%sminishell exit code tester%s" % (BOLD, END))
    print("minishell: %s" % os.path.abspath(args.minishell))
    print("bash:      %s (version %s)" % (shutil.which(args.bash) or args.bash,
                                         version))
    if os.geteuid() == 0:
        print("%swarning: running as root, permission tests are "
              "meaningless%s" % (YELLOW, END))

    selected = args.only or categories
    try:
        for name in categories:
            if name not in selected:
                continue
            if name == "signals":
                run_signals(args, ctx, stats)
            else:
                run_category(name, TESTS[name], args, ctx, stats)
    finally:
        remove_sandbox(tmp)

    print("\n%s%d tests%s: %s%d passed%s, %s%d warnings%s, "
          "%s%d known (bash version)%s, %s%d failed%s" % (
              BOLD, stats["n"], END, GREEN, stats["ok"], END, YELLOW,
              stats["warn"], END, YELLOW, stats["known"], END,
              RED if stats["fail"] else GREEN, stats["fail"], END))
    if stats["warn"]:
        print("%swarnings = same exit code, but different stdout or files "
              "(use --strict to fail on them)%s" % (GREY, END))
    sys.exit(1 if stats["fail"] else 0)


if __name__ == "__main__":
    main()
