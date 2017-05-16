#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 Takuma Yagi <yagi@ks.cs.titech.ac.jp>
#
# Distributed under terms of the MIT license.

import os
import sys
import time
import cv2
import subprocess

"""
Calculate optical flow and output to file
"""
def video2flow(data_dir):
    start = time.time()
    cnt = 0
    for root, dirs, files in os.walk(data_dir):
        for rpath in files:
            basename, ext = os.path.splitext(rpath)
            if ext != ".avi":
                continue
            # aviファイルを発見
            cnt += 1
            file_name = os.path.join(root, rpath)
            target_dir = os.path.join(root, basename)

            print cnt
            sys.stdout.flush()
            if os.path.exists(target_dir+"/flow_00001.jpg"):
                continue

            if not os.path.exists(target_dir):
                os.mkdir(target_dir)
            # Brox
            cmd = "../build/denseFlow_gpu -f \"{0}\" -i \"{1}\" -b 20 -t 2 -d 0 -s 1".format(file_name, target_dir)
            print "{0} {1}".format(file_name, cnt)
            sys.stdout.flush()
            subprocess.call(cmd, shell=True)
    print "Elapsed time: {} s".format(time.time()-start)


def video2flow_single(filename):
    target_dir, _ = os.path.splitext(filename)
    if not os.path.exists(target_dir):
        os.mkdir(target_dir)
    cmd = "../build/denseFlow_gpu -f \"{0}\" -i \"{1}\" -b 20 -t 2 -d 0 -s 1".format(file_name, target_dir)
    print "{0}".format(filename)
    subprocess.call(cmd, shell=True)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Usage: python video2flow.py <data_dir>"

    video2flow(sys.argv[1])
