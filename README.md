# microbian
The micro:bian embedded OS for BBC micro:bit and other
ARM-based microcontroller boards.

This repository exists primarily to store the full version of
micro:bian, including timeouts and ports to other boards: so that the
version distributed with my book, _Bare Metal micro:bit_, can be
simplified as much as possible.

To build, link make a symbolic link named `config.mk` to either `config.v1`
or `config.v2`, then use `make` or `make examples`.
