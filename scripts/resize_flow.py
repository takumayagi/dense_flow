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
import glob
import re

"""
resize to (170, 128)
"""
def resize_flow(data_dir):
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
            search_dir = re.sub(r'([\[\]])', '[\\1]', os.path.join(root, basename))

            print search_dir
            for flow_path in sorted(glob.glob(search_dir+"/flow_xym_?????.jpg")):
                if os.path.exists(search_dir+"/flow_170_{0:05d}.jpg".format(int(flow_path[-9:-4]))):
                    continue

                flow = cv2.imread(flow_path)
                flow_half = cv2.resize(flow, (170, 128))
                cv2.imwrite(search_dir+"/flow_170_{0:05d}.jpg".format(int(flow_path[-9:-4])), flow_half)

            sys.stdout.flush()
    print "Elapsed time: {} s".format(time.time()-start)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Usage: python resize_flow.py <data_dir>"

    resize_flow(sys.argv[1])
