import sys
sys.path.append("/home/blicycle/BlicycleRetry/software/libbot2/trunk/bot2-procman/lcmtypes/Python/bot_procman")

import lcm
import info_t

m = lcm.LCM()

msg = info_t.info_t()

def setIt(x):
    msg = x
def msg_handler(channel, data):
    setIt(info_t.info_t.decode(data))

subscription = m.subscribe("PMD_INFO",msg_handler)


try:
    while True:
        m.handle()
        print msg.host

except KeyboardInterrupt:
    pass

m.unsubscribe(subscription)
