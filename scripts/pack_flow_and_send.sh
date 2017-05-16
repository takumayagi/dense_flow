#! /bin/sh
#
# pack_flow_and_send.sh
# Copyright (C) 2016 Takuma Yagi <yagi@ks.cs.titech.ac.jp>
#
# Distributed under terms of the MIT license.
#

find ./HMDB51 -name "flow_xym_?????.jpg" -print0 | tar cv -T - --null -f hmdb51_flow_xym.tar
scp hmdb51_flow_xym.tar tsubame:~/data0/

