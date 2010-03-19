#!/usr/bin/python

import xmlrpclib, time, sys, subprocess, os, re, sys

address = "students.mimuw.edu.pl", 7553
proxy = xmlrpclib.ServerProxy("http://%s:%d/" % address)
gnugo = "'gnugo-3.8 --mode gtp --chinese-rules --capture-all-dead --positional-superko --level=0'"

def do_one_series ():
    task = proxy.get_game_setup ()
    (log_dir, series_id, game_count, black, white) = task
    print "TASK:\n", task, "\n"

    os.chdir (log_dir)

    sgf_prefix = "game-%d" % series_id
    args = [
        "gogui-twogtp",
        "-auto",
        "-size", "9",
        "-komi", "6.5",
        "-alternate",
        "-referee", gnugo,
        "-games", "%d" % game_count,
        "-black", black,
        "-white", white,
        "-sgffile", sgf_prefix,
        "-verbose",
        ]

    cmd = " ".join (args)
    print "New game series."
    print cmd
    print 
    process = subprocess.Popen (cmd,
                                stdout = subprocess.PIPE,
                                stderr = subprocess.STDOUT,
                                shell = True)
    print "Waiting for results."
    twogtp_log = process.communicate () [0]

    # print "LOG:" 
    # print twogtp_log
    # print

    result_list = []
    for game_no in range (game_count):
        file_name = sgf_prefix + "-" + str (game_no) + ".sgf"
        sgf_file = open (file_name, "r") # TODO r?
        sgf = sgf_file.read ()
        sgf_file.close ()
        winner_letter = re.search (r'RE\[([^]]*)\]', sgf).group(1) [0]
        assert (winner_letter in 'BW')
        result = (game_no % 2, 1 if winner_letter == 'B' else 0, file_name)
        result_list.append (result)

    
    print "RESULT ", series_id, ": ", result_list, "\n"
    proxy.report_game_result (series_id, result_list)

do_one_series ()