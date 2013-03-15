<?php

$toBeConverted = <<<EOF
00000000 b2209191 03e92f99 59a36580 2cb52503 03afe93c 421307c2 5e151b21
099e31cb efaf19a7 da52fc8c 012b534e ee532d31 a6e53c26 bc619e38 8674c144
f9b19c87 189f583d 0c005883 aa5c77e1 d25d048c 00b62491 66156ad9 c1aa3e02
99b511ad f6b47377 91da4ab2 b6a53cda cd2faac9 475120f2 87e3be3b f58f1563
b4c1b0b0 2251f04b d41c5091 a85b8f09 555d4665 41a74eab 5f4cf186 0555df83
EOF;

echo strtoupper(str_replace(PHP_EOL, '', str_replace(' ', '', $toBeConverted)));
