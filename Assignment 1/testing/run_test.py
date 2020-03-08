#! /usr/bin/env python3
# grieg only has python3.4

import subprocess
import glob
import filecmp
import os
import sys
import signal
import socket
import shutil
import tempfile

if socket.gethostname() != "grieg":
    print("""\
Error: Please run on grieg
run:
\t `ssh grieg`
\t `python3 {}`
""".format(' '.join(sys.argv)), flush=True)
    sys.exit(1)

# grab some useful information

user = os.getenv("USER")

SRVR = os.path.join("/", "srvr", user)
os.putenv("SRVR",   SRVR)

# Mimic `source env`

PGHOME = os.path.join(SRVR, "pgsql")
os.putenv("PGHOME", PGHOME)

signal.signal(signal.SIGINT, signal.SIG_IGN)

# PGDATA = os.path.join(PGHOME, "data")
PGDATA = tempfile.mkdtemp()
os.putenv("PGDATA", PGDATA)

signal.signal(signal.SIGINT, lambda x, y: shutil.rmtree(PGDATA) and exit(1))

PGHOST = PGDATA
os.putenv("PGHOST", PGHOST)

PGPORT = 5432
os.putenv("PGPORT", str(PGPORT))

LD_LIBRARY_PATH = os.path.join(PGHOME, "lib")
os.putenv("LD_LIBRARY_PATH", LD_LIBRARY_PATH)

PATH = "{}/bin:/home/cs9315:".format(PGHOME) + os.getenv("PATH")
os.putenv("PATH", PATH)

# Set some extra variables

SRCDIR = os.path.join(SRVR, "postgresql-12.1", "src")
os.putenv("SRCDIR", SRCDIR)

TUTDIR = os.path.join(SRCDIR, "tutorial")
os.putenv("TUTDIR", TUTDIR)

# TSTDIR = os.getcwd()
TSTDIR = os.path.dirname(os.path.realpath(__file__))
os.putenv("TSTDIR", TSTDIR)

DB = "pname-test"
os.putenv("DB", DB)

# Couldn't find user code

if not os.path.isfile(os.path.join(TSTDIR, "pname.c")):
    print("Error: No pname.c file in {}".format(TSTDIR), flush=True)
    sys.exit(1)

if not os.path.isfile(os.path.join(TSTDIR, "pname.source")):
    print("Error: No pname.source file in {}".format(TSTDIR), flush=True)
    sys.exit(1)

# Clean any old logs

if os.path.isfile(os.path.join(TSTDIR, "pg.log")):
    print("Info: removing old \"pg.log\" from {}".format(TSTDIR), flush=True)
    os.remove(os.path.join(TSTDIR, "pg.log"))

if os.path.isfile(os.path.join(TSTDIR, "test.log")):
    print("Info: removing old \"test.log\" from {}".format(TSTDIR), flush=True)
    os.remove(os.path.join(TSTDIR, "test.log"))

log = open(os.path.join(TSTDIR, "test.log"), "w")

# Kill any old servers

try:
    proc = subprocess.check_output(["pg_ctl", "status"], stderr=subprocess.STDOUT)
    if proc.decode("UTF-8").startswith("pg_ctl: server is running"):
        print("Info: Killing existing server", flush=True)
        print("Info: Killing existing server", file=log, flush=True)
        subprocess.call(["pg_ctl", "stop"], stdout=log, stderr=subprocess.STDOUT)
except subprocess.CalledProcessError:
    pass

# Remove any old files: pname.o, pname.so, pname, pname.sql, pname.bc

if os.path.isfile(os.path.join(TSTDIR, "pname.so")):
    print("Info: removing old pname.so from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname.so"))

if os.path.isfile(os.path.join(TSTDIR, "pname.o")):
    print("Info: removing old pname.o from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname.o"))

if os.path.isfile(os.path.join(TSTDIR, "pname")):
    print("Info: removing old pname from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname"))

if os.path.isfile(os.path.join(TSTDIR, "pname.sql")):
    print("Info: removing old pname.sql from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname.sql"))

# the Makefile removes this, so we do too
if os.path.isfile(os.path.join(TSTDIR, "pname.bc")):
    print("Info: removing old pname.bc from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname.bc"))

# Do some setup

print("|---------- running `make` ----------|", file=log, flush=True)
try:
    proc = subprocess.check_output(["make"], stderr=subprocess.STDOUT).decode("UTF-8")
    if "error:" in proc:
        print("make produced a build error", flush=True)
        print(proc, flush=True)
        print(proc, file=log, flush=True)
        exit(1)
    print(proc, file=log, flush=True)
except subprocess.CalledProcessError as e:
    exit(1)

print("|---------- setting-up database server ----------|", file=log, flush=True)
subprocess.call(["initdb", "-A", "trust"], stdout=log, stderr=subprocess.STDOUT)
# Setup the config file
with open(os.path.join(PGDATA, "postgresql.conf"), "a") as conf:
    print("""\
listen_addresses = ''
port = 5432
max_connections = 10
max_wal_senders = 4
unix_socket_directories = '{}'
""".format(PGDATA), file=conf)
try:
    proc = subprocess.check_output(["pg_ctl", "-D", PGDATA, "start", "-l", os.path.join(TSTDIR, "pg.log")], stderr=subprocess.STDOUT).decode("UTF-8")
    if "server started" not in proc:
        print("server could not be started", flush=True)
        print(proc, flush=True)
        print(proc, file=log, flush=True)
        exit(1)
    print(proc, file=log, flush=True)
except subprocess.CalledProcessError as e:
    exit(1)

subprocess.call(["dropdb", "--if-exists", DB], stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)

# Do some testing

print("|---------- running tests ----------|", file=log, flush=True)
# Hack to only get one level of directories
_, dirs, _ = os.walk(os.path.join(TSTDIR, "tests")).__next__()
for test_dir in sorted(dirs, key=lambda x: x.split("_")[0]):
    if test_dir.startswith("."):
        continue
    with open(os.path.join(TSTDIR, "tests", test_dir, "info.txt")) as info_file:
        print(info_file.read(), flush=True)
        print(info_file.read(), file=log, flush=True)
    schema = glob.glob(os.path.join(TSTDIR, "tests", test_dir, "schema.sql"))[0]
    for data in sorted(glob.glob(os.path.join(TSTDIR, "tests", test_dir, "data*.sql"))):
        for queries in sorted(glob.glob(os.path.join(TSTDIR, "tests", test_dir, "queries*.sql"))):
            try:
                os.makedirs(os.path.join(TSTDIR, "results", test_dir.split("_")[1]))
            except FileExistsError:
                pass
            output = open(os.path.join(TSTDIR, "results", test_dir.split("_")[1], "{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0])), "w")
            print("|---------- START TEST -- {}  -- {} -- {} ----------|".format(test_dir.split("_")[1], data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]), file=log, flush=True)
            print("---------- setting-up database", file=log, flush=True)
            subprocess.call(["createdb", DB], stdout=log, stderr=subprocess.STDOUT)  # TODO: check output?
            print("---------- Loading PersonName Type", file=log, flush=True)
            try:
                proc = subprocess.check_output(["psql", DB, "-f", os.path.join(TSTDIR, "pname.sql")], stderr=subprocess.STDOUT).decode("UTF-8")
                if "CREATE TABLE" in proc or "INSERT" in proc or "DROP TYPE" in proc:
                    print("Unexpected action done by {}".format(os.path.join(TSTDIR, "pname.sql")))
                    print(proc, flush=True)
                    print(proc, file=log, flush=True)
                    exit(1)
                if "ERROR:" in proc:
                    print("Error while executing {}".format(os.path.join(TSTDIR, "pname.sql")))
                    print(proc, flush=True)
                    print(proc, file=log, flush=True)
                    exit(1)
                print(proc, file=log, flush=True)
            except subprocess.CalledProcessError as e:
                print("Error while executing {}".format(os.path.join(TSTDIR, "pname.sql")))
                exit(1)
            print("---------- Loading Test Schema", file=log, flush=True)
            subprocess.call(["psql", DB, "-f", os.path.join(TSTDIR, "tests", test_dir, "schema.sql")], stdout=log, stderr=subprocess.STDOUT)  # TODO: check output?
            print("---------- Loading Test Data", file=log, flush=True)
            subprocess.call(["psql", DB, "-f", data], stdout=log, stderr=subprocess.STDOUT)  # TODO: check output?
            print("---------- Loading Test Queries", file=log, flush=True)
            subprocess.call(["psql", DB, "-f", queries], stdout=output, stderr=subprocess.STDOUT)
            print("---------- Running queries: see {} ----------".format(os.path.join(TSTDIR, "results", test_dir.split("_")[1], "{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]))), file=log, flush=True)
            print("---------- Dropping Test Data/Schema", file=log, flush=True)
            subprocess.call(["psql", DB, "-f", os.path.join(TSTDIR, "tests", test_dir, "schema_drop.sql")], stdout=log, stderr=subprocess.STDOUT)
            print("---------- Dropping PersonName Type", file=log, flush=True)
            subprocess.call(["psql", DB, "-f", os.path.join(TSTDIR, "pname_drop.sql")], stdout=log, stderr=subprocess.STDOUT)
            print("---------- tearing-down database", file=log, flush=True)
            subprocess.call(["dropdb", DB], stdout=log, stderr=subprocess.STDOUT)
            print("---------- TEST RESULTS", file=log, flush=True)
            output.close()

            # Do this horrible thing so that output is deterministic (remove timing data)
            subprocess.call(["sed", "-i", "-E", "s/(cost|actual time)=[^ ]*\s*//g", os.path.join(TSTDIR, "results", test_dir.split("_")[1], "{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]))])
            subprocess.call(["sed", "-i", "-E", "s/\s*(Planning|Execution)\s*Time:\s*[^ ]*\s*[^ ]*\s*//g", os.path.join(TSTDIR, "results", test_dir.split("_")[1], "{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]))])

            if os.path.isfile(os.path.join(TSTDIR, "tests", test_dir, "expected-{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]))):
                if filecmp.cmp(os.path.join(TSTDIR, "results", test_dir.split("_")[1], "{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0])),
                               os.path.join(TSTDIR, "tests", test_dir, "expected-{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0])), shallow=False):
                    print("PASS: {} -- {} -- {}".format(test_dir.split("_")[1], data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]), flush=True)
                    print("PASS: {} -- {} -- {}".format(test_dir.split("_")[1], data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]), file=log, flush=True)
                else:
                    print("FAIL: {} -- {} -- {}".format(test_dir.split("_")[1], data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]), flush=True)
                    print("FAIL: {} -- {} -- {}".format(test_dir.split("_")[1], data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]), file=log, flush=True)

                    print("\trun: `diff {} {}`".format(os.path.join(TSTDIR, "results", test_dir.split("_")[1], "{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0])),
                                                     os.path.join(TSTDIR, "tests", test_dir, "expected-{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]))), flush=True)
                    print("\trun: `diff {} {}`".format(os.path.join(TSTDIR, "results", test_dir.split("_")[1], "{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0])),
                                                     os.path.join(TSTDIR, "tests", test_dir, "expected-{}-{}.log".format(data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]))), file=log, flush=True)
            else:
                print("SKIP: {} -- {} -- {}: No expected output file".format(test_dir.split("_")[1], data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]), flush=True)
                print("SKIP: {} -- {} -- {}: No expected output file".format(test_dir.split("_")[1], data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]), file=log, flush=True)

            print("|---------- END TEST -- {}  -- {} -- {} ----------|".format(test_dir.split("_")[1], data.split('/')[-1].split('.')[0], queries.split('/')[-1].split('.')[0]), file=log, flush=True)

subprocess.call(["pg_ctl", "stop"], stdout=log, stderr=subprocess.STDOUT)

# Remove any new files: pname.o, pname.so, pname, pname.sql, pname.bc

if os.path.isfile(os.path.join(TSTDIR, "pname.so")):
    print("Info: removing pname.so from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname.so"))

if os.path.isfile(os.path.join(TSTDIR, "pname.o")):
    print("Info: removing pname.o from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname.o"))

if os.path.isfile(os.path.join(TSTDIR, "pname")):
    print("Info: removing pname from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname"))

if os.path.isfile(os.path.join(TSTDIR, "pname.sql")):
    print("Info: removing pname.sql from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname.sql"))

# the Makefile removes this, so we do too
if os.path.isfile(os.path.join(TSTDIR, "pname.bc")):
    print("Info: removing pname.bc from {}".format(TSTDIR), file=log, flush=True)
    os.remove(os.path.join(TSTDIR, "pname.bc"))

shutil.rmtree(PGDATA)

log.close()
