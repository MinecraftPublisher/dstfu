# .DS_Sto- SHUT THE FUCK UP!!!
dstfu is a nice macos utility that deletes `.DS_Store` files off of your systema and actively monitors file changes to find `.DS_Store` writes and delets any new `.DS_Store`s created.

## Building:
`make build`

## Installation:
`make install`

## WARNING: It does run in the background, But it does not insert itself into launch agents. You need to do that manually.
dstfu also kills every other instance after execution, and forks itself to run in the background.