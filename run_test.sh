#!/bin/bash

rlwrap $QBIN $QHOME/qpackages/qspec/app/spec.q -q app/ib.q test/test_qib.q $*
