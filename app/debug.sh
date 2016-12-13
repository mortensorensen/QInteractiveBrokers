#!/bin/bash
DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
rlwrap ${QBIN} ${DIR}/debug.q -appdir ${DIR} -p 5000
