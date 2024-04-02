#!/usr/bin/env bash

/opt/SEGGER/JLink/JLinkGDBServerExe -select USB -device Cortex-A5 -endian little -if SWD -speed 4000 -noir -noLocalhostOnly
