#!/bin/sh

#  rsync.sh
#  Relays
#
#  Created by Jeroen Jonkman on 23-06-15.
#
MAC020="/Users/jeroenjonkman/Developer/Relays/"
MAC001="/Volumes/jeroenjonkman/Developer/Relays/"
rsync -taruvg ${MAC020} ${MAC001}
rsync -taruvg ${MAC001} ${MAC020}
